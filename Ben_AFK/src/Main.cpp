/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:00 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 15:00:02 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "RawMode.hpp"
	#include "Signals.hpp"
	#include "Encryption.hpp"
	#include "Connection.hpp"

	#include <iostream>															// std::cerr()
	#include <unistd.h>															// close()
	#include <sys/select.h>														// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET

#pragma endregion

#pragma region "Loop"

	#pragma region "Interactive"

		static int interactive_loop() {
			fd_set readfds;
			char buffer[4096];

			while (true) {
				FD_ZERO(&readfds);
				FD_SET(Options::sockfd, &readfds);
				FD_SET(STDIN_FILENO, &readfds);

				int max_fd = (Options::sockfd > STDIN_FILENO) ? Options::sockfd : STDIN_FILENO;

				if (select(max_fd + 1, &readfds, nullptr, nullptr, nullptr) < 0) { std::cerr << "Error: Multiplexing failed\n"; return (1); }

				// Shell Output
				if (FD_ISSET(Options::sockfd, &readfds)) {
					ssize_t bytes = recv(Options::sockfd, buffer, sizeof(buffer), 0);
					if (bytes <= 0) return (1);

					std::string msg = std::string(buffer, bytes);
					if (Options::encryption) {
						try {
							msg = decrypt(msg);
						} catch (const std::exception& e) { continue; }
					}
					write(STDOUT_FILENO, msg.c_str(), msg.length());
				}

				// User Input
				if (FD_ISSET(STDIN_FILENO, &readfds)) {
					ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
					if (bytes <= 0) { return (1); }

					std::string input = std::string(buffer, bytes);
					if (Options::encryption) input = encrypt(input);

					if (send(Options::sockfd, input.c_str(), input.length(), 0) < 0) { std::cerr << "Error: Failed to send data\n"; return (1); }
				}
			}

			return (0);
		}

	#pragma endregion

	#pragma region "Authentication"

		static int main_loop() {
			fd_set readfds;

			while (!Options::authenticated) {
				FD_ZERO(&readfds);
				FD_SET(Options::sockfd, &readfds);

				if (select(Options::sockfd + 1, &readfds, nullptr, nullptr, nullptr) < 0) { std::cerr << "Error: Multiplexing failed\n"; return (1); }

				if (FD_ISSET(Options::sockfd, &readfds)) {
					int result = receive_data();
					if (result == 1) return (1);	// Error
					if (result == 2) break;			// Authenticated
				}
			}

			return (interactive_loop());
		}

	#pragma endregion

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		if (signal_set()) { std::cerr << "Error: Signal set failed\n"; return (1); }

		int result = 0;
		if ((result = Options::parse(argc, argv))) return (result - 1);

		if (socket_create()) return (1);
		if (main_loop()) result = 1;

		raw_mode_disable(false);
		if (Options::sockfd >= 0) close(Options::sockfd);

		return (result);
	}

#pragma endregion
