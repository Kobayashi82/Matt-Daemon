/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 21:41:05 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 00:58:26 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "Ben_AFK.hpp"

	#include <arpa/inet.h>														// socket(), setsockopt(), bind(), listen(), accept(), inet_ntop(), htons(), ntohs(), sockaddr_in
	#include <iostream>															// std::cerr()
	#include <unistd.h>															// close()
	#include <termios.h>														// tcgetattr(), tcsetattr(), termios
	#include <sys/select.h>														// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET

#pragma endregion

struct termios	original_termios;
bool			raw_mode_enabled = false;

int enable_raw_mode() {
	if (tcgetattr(STDIN_FILENO, &original_termios) == -1) { std::cerr << "Error: Failed to get terminal attributes\n"; return (1); }
	
	struct termios raw = original_termios;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_cflag |= (CS8);
	raw.c_oflag &= ~(OPOST);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;
	
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { std::cerr << "Error: Failed to set raw mode\n"; return (1); }
	
	raw_mode_enabled = true;

	return (0);
}

void disable_raw_mode() {
	if (raw_mode_enabled) {
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
		raw_mode_enabled = false;
	}
}

int socket_create() {
	Options::sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (Options::sockfd < 0) {
		std::cerr << "Error: Socket creation failed\n";
		return (1);
	}

	Options::sockaddr.sin_family = AF_INET;
	Options::sockaddr.sin_port = htons(Options::port);
	if (inet_pton(AF_INET, Options::host, &Options::sockaddr.sin_addr) <= 0) {
		std::cerr << "Error: Invalid address or address not supported: " << Options::hostname << "\n";
		close(Options::sockfd);
		return (1);
	}

	if (connect(Options::sockfd, (const sockaddr *)&Options::sockaddr, sizeof(sockaddr_in)) < 0) {
		std::cerr << "Error: Failed to connect to " << Options::hostname << ":" << Options::port << "\n";
		close(Options::sockfd);
		return (1);	
	}

	send_data("/CLIENT_SHELL_AUTH");

	return (0);
}

int send_data(const std::string & data) {
	if (send(Options::sockfd, data.c_str(), data.size(), 0) < 0) {
		std::cerr << "Error: Failed to send data\n";
		return (1);
	}

	return (0);
}

int receive_data() {
	char buffer[4096];

	ssize_t bytes = recv(Options::sockfd, buffer, sizeof(buffer), 0);

	if (bytes > 0) {
		std::string msg = std::string(buffer, bytes);
		try {
			if (Options::encryption) msg = decrypt(msg);
		} catch (const std::exception& e) {
			std::cerr << "Error: Message not encrypted\n"; return (1);
		}

		if (!msg.find("/AUTHORIZE ENCRYPTION=")) {
			std::string value = msg.substr(22);

			value.erase(value.find_last_not_of(" \t\n\r") + 1);
			if		(value == "true")	Options::encryption = true;
			else if	(value == "false")	Options::encryption = false;
			else {
				std::cerr << "Error: Invalid response from server\n"; return (1);
			}

			if (!Options::insecure && !Options::encryption) {
				std::cerr << "Error: Unsecure connection\n"; return (1);
			}

			std::string password = getPassword();
			if (!Options::retries) return (1);
			std::string response = "/AUTHORIZATION user=" + Options::user + " pass=" + password;
			if (Options::encryption) response = encrypt(response);
			send_data(response);
			return (0);
		}

		if (!Options::authenticated && !msg.find("/AUTHORIZATION_OK")) {
			Options::authenticated = true;
			if (enable_raw_mode()) return (1); else return (2);
		}
		if (!Options::authenticated && !msg.find("/AUTHORIZATION_FAIL")) {
			Options::retries--;
			if (!Options::retries)	{ std::cerr << "Authentication failure, giving up\n"; return (1); }
			else					  std::cerr << "Authentication failure, please try again\n";
			std::string password = getPassword();
			if (!Options::retries) return (1);
			std::string response = "/AUTHORIZATION user=" + Options::user + " pass=" + password;
			if (Options::encryption) response = encrypt(response);
			send_data(response);
			return (0);
		}
	}

	if (bytes <= 0) return (1);

	return (0);
}

int interactive_loop() {
	fd_set readfds;
	char buffer[4096];

	while (true) {
		FD_ZERO(&readfds);
		FD_SET(Options::sockfd, &readfds);
		FD_SET(STDIN_FILENO, &readfds);

		int max_fd = (Options::sockfd > STDIN_FILENO) ? Options::sockfd : STDIN_FILENO;

		if (select(max_fd + 1, &readfds, nullptr, nullptr, nullptr) < 0) {
			std::cerr << "Error: Multiplexing failed\n"; disable_raw_mode(); return (1);
		}

		// Shell Output
		if (FD_ISSET(Options::sockfd, &readfds)) {
			ssize_t bytes = recv(Options::sockfd, buffer, sizeof(buffer), 0);
			if (bytes <= 0) { disable_raw_mode(); return (1); }

			std::string msg = std::string(buffer, bytes);
			try {
				if (Options::encryption) msg = decrypt(msg);
			} catch (const std::exception& e) {
				std::cerr << "Error: Message not encrypted\n"; return (1);
			}

			write(STDOUT_FILENO, msg.c_str(), msg.length());
		}

		// User Input
		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
			if (bytes <= 0) { disable_raw_mode(); return (1); }

			std::string input = std::string(buffer, bytes);
			if (Options::encryption) input = encrypt(input);

			if (send(Options::sockfd, input.c_str(), input.length(), 0) < 0) {
				std::cerr << "Error: Failed to send data\n";
				disable_raw_mode(); return (1);
			}
		}
	}
	
	return (0);
}

int main_loop() {
	fd_set readfds;

	while (!Options::authenticated) {
		FD_ZERO(&readfds);
		FD_SET(Options::sockfd, &readfds);

		if (select(Options::sockfd + 1, &readfds, nullptr, nullptr, nullptr) < 0) {
			std::cerr << "Error: Multiplexing failed\n"; return (1);
		}

		if (FD_ISSET(Options::sockfd, &readfds)) {
			int result = receive_data();
			if (result == 1) return (1);	// Error
			if (result == 2) break;			// Authenticated
		}
	}
	
	return (interactive_loop());
}
