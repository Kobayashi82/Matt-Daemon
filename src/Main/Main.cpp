/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 23:13:14 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Options.hpp"
	#include "Main/Logging.hpp"
	#include "Main/Daemon.hpp"
	#include "Network/Socket.hpp"
	#include "Network/Epoll.hpp"

	#include <iostream>															// std::cerr()
	#include <unistd.h>															// getuid()

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		int result = 0;

		try {
			if (getuid()) { std::cerr << "This program must be run as root" << std::endl; return (1); }
			if ((result = Options::parse(argc, argv))) return (result - 1);

			Tintin_reporter	Tintin_logger(Options::logPath, Options::logLevel);
	
			Log->debug("Initiating daemon");
			if (daemonize()) return (1);
			Log->info("Daemon started");

			if (Epoll::create()) result = 1;
			else {
				Log->debug("Epoll initialized");
				Socket socket(Options::portNumber);
				if (socket.create()) result = 1;
				else {
					Log->info("Daemon listening on port " + std::to_string(Options::portNumber));

					Epoll::Running = true;
					while (Epoll::Running) {
						if (Epoll::events(&socket))	break;
					}
				}
			}

			Log->info("Daemon closed");
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			return (1);
		}

		return (result);
	}

#pragma endregion
