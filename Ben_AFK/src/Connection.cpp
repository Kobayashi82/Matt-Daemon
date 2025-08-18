/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 21:41:05 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 15:37:13 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "RawMode.hpp"
	#include "Password.hpp"
	#include "Encryption.hpp"
	#include "Connection.hpp"

	#include <arpa/inet.h>														// socket(), setsockopt(), bind(), listen(), accept(), inet_ntop(), htons(), ntohs(), sockaddr_in
	#include <iostream>															// std::cerr()
	#include <unistd.h>															// close()

#pragma endregion

#pragma region "Create Socket"

	int socket_create() {
		Options::sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (Options::sockfd < 0) { std::cerr << "Error: Socket creation failed\n"; return (1); }

		Options::sockaddr.sin_family = AF_INET;
		Options::sockaddr.sin_port = htons(Options::port);
		if (inet_pton(AF_INET, Options::host, &Options::sockaddr.sin_addr) <= 0) {
			std::cerr << "Error: Invalid address or address not supported: " << Options::hostname << "\n";
			close(Options::sockfd); return (1);
		}

		if (connect(Options::sockfd, (const sockaddr *)&Options::sockaddr, sizeof(sockaddr_in)) < 0) {
			std::cerr << "Error: Failed to connect to " << Options::hostname << ":" << Options::port << "\n";
			close(Options::sockfd); return (1);	
		}

		send_data("/CLIENT_SHELL_AUTH");

		return (0);
	}

#pragma endregion

#pragma region "Send Data"

	int send_data(const std::string & data) {
		if (send(Options::sockfd, data.c_str(), data.size(), 0) < 0) { std::cerr << "Error: Failed to send data\n"; return (1); }

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
				std::cerr << "Error: Message not encrypted\n"; return (1);
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
				else { std::cerr << "Error: Invalid response from server\n"; return (3); }

				if (!Options::insecure && !Options::encryption) { std::cerr << "Error: Unsecure connection\n"; return (3); }

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
