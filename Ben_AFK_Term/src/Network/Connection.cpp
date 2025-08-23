/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 21:41:05 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 22:40:28 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Config/Options.hpp"
	#include "Config/RawMode.hpp"
	#include "Config/Signals.hpp"

	#include "Security/Password.hpp"
	#include "Security/Encryption.hpp"

	#include "Network/Connection.hpp"

	#include <arpa/inet.h>														// socket(), setsockopt(), bind(), listen(), accept(), inet_ntop(), htons(), ntohs(), sockaddr_in
	#include <iostream>															// std::cerr()
	#include <unistd.h>															// close()
	#include <errno.h>															// errno, EINTR
	#include <sys/select.h>														// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET

#pragma endregion

#pragma region "Create Socket"

	int socket_create() {
		Options::sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (Options::sockfd < 0) { std::cerr << "Socket creation failed\n"; return (1); }

		Options::sockaddr.sin_family = AF_INET;
		Options::sockaddr.sin_port = htons(Options::port);
		if (inet_pton(AF_INET, Options::host, &Options::sockaddr.sin_addr) <= 0) {
			std::cerr << "Invalid address or address not supported: " << Options::hostname << "\n";
			close(Options::sockfd); return (1);
		}

		if (connect(Options::sockfd, (const sockaddr *)&Options::sockaddr, sizeof(sockaddr_in)) < 0) {
			std::cerr << "Failed to connect to " << Options::hostname << ":" << Options::port << "\n";
			close(Options::sockfd); return (1);	
		}

		send_data("/CLIENT_SHELL_AUTH");

		return (0);
	}

#pragma endregion

#pragma region "Send Data"

	int send_data(const std::string& data) {
		if (send(Options::sockfd, data.c_str(), data.size(), 0) < 0) { std::cerr << "Failed to send data\n"; return (1); }

		return (0);
	}

#pragma endregion

#pragma region "Receive Data"

	int receive_data() {
		char buffer[4096];

		ssize_t bytes = recv(Options::sockfd, buffer, sizeof(buffer), 0);

		if (bytes > 0) {
			std::string msg = std::string(buffer, bytes);
			try {
				if (Options::encryption) msg = decrypt(msg);
			} catch (const std::exception& e) {
				std::cerr << "Message not encrypted\n"; return (1);
			}

			if (!msg.find("Maximum connections reached")) { std::cerr << "Maximum connections reached\n"; return (3); }

			if (!msg.find("/SHELL_DISABLED")) { std::cerr << "Remote shell access is disabled on the server\n"; return (3); }

			if (!msg.find("/SHELL_FAIL")) { std::cerr << "Failed to open remote shell\n"; return (3); }

			if (!msg.find("/DISCONNECT")) { std::cerr << "Connection rejected by the server\n"; return (3); }

			if (!msg.find("/AUTHORIZE ENCRYPTION=")) {
				std::string value = msg.substr(22);

				value.erase(value.find_last_not_of(" \t\n\r") + 1);
				if		(value == "true")	Options::encryption = true;
				else if	(value == "false")	Options::encryption = false;
				else { std::cerr << "Invalid response from server\n"; return (3); }

				if (!Options::insecure && !Options::encryption) { std::cerr << "Unsecure connection\n"; return (3); }

				std::string password = getPassword();
				if (!Options::retries) return (1);

				std::string response = "/AUTHORIZATION user=" + Options::user + " pass=" + password;
				if (Options::encryption) response = encrypt(response);
				send_data(response);

				return (0);
			}

			if (!Options::authenticated && !msg.find("/AUTHORIZATION_OK")) {
				Options::authenticated = true;

				std::string terminal_size = get_terminal_size();
				std::string size_msg = "/TERMINAL_SIZE " + terminal_size;
				if (Options::encryption) size_msg = encrypt(size_msg);
				send_data(size_msg);

				if (raw_mode_enable(false)) return (1); else return (2);
			}

			if (!Options::authenticated && !msg.find("/AUTHORIZATION_FAIL")) {
				Options::retries--;
				if (!Options::retries)	{ std::cerr << "Authentication failure, giving up\n\n"; return (3); }
				else					  std::cerr << "Authentication failure, please try again\n";

				std::string password = getPassword();
				if (!Options::retries) return (1);

				std::string response = "/AUTHORIZATION user=" + Options::user + " pass=" + password;
				if (Options::encryption) response = encrypt(response);
				send_data(response);

				return (0);
			}
		}

		if (bytes <= 0) return (3);

		return (0);
	}

#pragma endregion

#pragma region "Interactive Loop"

	int interactive_loop() {
		fd_set readfds;
		char buffer[4096];

		while (true) {
			if (window_resized) {
				window_resized = false;
				std::string terminal_size = get_terminal_size();
				std::string resize_msg = "/TERMINAL_RESIZE " + terminal_size;
				if (Options::encryption) resize_msg = encrypt(resize_msg);
				send_data(resize_msg);
			}

			FD_ZERO(&readfds);
			FD_SET(Options::sockfd, &readfds);
			FD_SET(STDIN_FILENO, &readfds);

			int max_fd = (Options::sockfd > STDIN_FILENO) ? Options::sockfd : STDIN_FILENO;

			int select_result = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
			if (select_result < 0) { 
				if (errno == EINTR) continue;
				std::cerr << "Multiplexing failed\n\n"; return (3); 
			}

			// Shell Output
			if (FD_ISSET(Options::sockfd, &readfds)) {
				ssize_t bytes = recv(Options::sockfd, buffer, sizeof(buffer), 0);
				if (bytes <= 0) return (3);

				std::string msg = std::string(buffer, bytes);
				if (Options::encryption) {
					try { msg = decrypt_with_index(msg, Options::decryption_index); }
					catch (const std::exception& e) { continue; }
				}
				write(STDOUT_FILENO, msg.c_str(), msg.length());
			}

			// User Input
			if (FD_ISSET(STDIN_FILENO, &readfds)) {
				ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
				if (bytes <= 0) return (3);

				std::string input = std::string(buffer, bytes);
				if (Options::encryption) input = encrypt(input);

				if (send(Options::sockfd, input.c_str(), input.length(), 0) < 0) { std::cerr << "Failed to send data\n"; return (1); }
			}
		}

		return (0);
	}

#pragma endregion