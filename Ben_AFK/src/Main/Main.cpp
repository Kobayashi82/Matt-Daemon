/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:00 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 22:41:03 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Config/Options.hpp"
	#include "Config/Signals.hpp"
	#include "Config/RawMode.hpp"

	#include "Network/Connection.hpp"

	#include <iostream>															// std::cerr()
	#include <unistd.h>															// close()
	#include <errno.h>															// errno, EINTR
	#include <sys/select.h>														// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET

#pragma endregion

#pragma region "Main Loop"

	static int main_loop() {
		fd_set readfds;

		while (!Options::authenticated) {
			FD_ZERO(&readfds);
			FD_SET(Options::sockfd, &readfds);

			int select_result = select(Options::sockfd + 1, &readfds, nullptr, nullptr, nullptr);
			if (select_result < 0) { 
				if (errno == EINTR) continue;
				std::cerr << "Multiplexing failed\n"; return (1); 
			}

			if (FD_ISSET(Options::sockfd, &readfds)) {
				int result = receive_data();
				if (result == 1) return (1);	// Error
				if (result == 2) break;			// Authenticated
				if (result == 3) return (3);	// Disconnected
			}
		}

		return (interactive_loop());
	}

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		if (signal_set()) { std::cerr << "Signal set failed\n"; return (1); }

		int result = 0;
		if ((result = Options::parse(argc, argv))) return (result - 1);

		if (socket_create()) return (1);
		result = main_loop();

		raw_mode_disable(false);
		if (Options::sockfd >= 0) close(Options::sockfd);

		if (!Options::retries) std::cerr << "Disconnected" << std::endl;
		else if (result == 3) std::cerr << "Disconnected" << std::endl;;

		return (result);
	}

#pragma endregion
