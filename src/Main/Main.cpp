/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/15 15:01:35 by vzurera-         ###   ########.fr       */
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
	#include <fcntl.h>															// open()
	#include <sys/file.h>														// flock()

#pragma endregion

int validate_input(int argc, char **argv) {
	int result = 0;

	if (getuid()) { std::cerr << "This program must be run as root" << std::endl;															return (1); }

	if ((result = Options::parse(argc, argv))) return (result - 1);
	
	int lockfd = open("/var/lock/matt_daemon.lock", O_RDWR|O_CREAT|O_TRUNC, 0640);
	if (lockfd < 0 || flock(lockfd, LOCK_EX|LOCK_NB)) { std::cerr << "Daemon already running\n";											return (1); }
	close(lockfd);

	if (!Socket::is_port_free(Options::portNumber)) { std::cerr << "Port " << Options::portNumber << " is not available\n";					return (1); }

	if (!Options::shellPath.empty()) {
		if (access(Options::shellPath.c_str(), F_OK)) { std::cerr << "Shell not found at " << Options::shellPath << "\n";					return (1); }
		if (access(Options::shellPath.c_str(), X_OK)) { std::cerr << "No execute permission for shell at " << Options::shellPath << "\n";	return (1); }
	}

	return (0);
}

#pragma region "Main"

	int main(int argc, char **argv) {
		int result = 0;

		try {
			if ((result = validate_input(argc, argv))) return (result);
		
			Tintin_reporter	Tintin_logger(Options::logFile, Options::logLevel);

			if (!Options::disabledShell && Options::shellPath.empty() && access("/bin/bash", X_OK) && access("/bin/zsh", X_OK) && access("/bin/sh", X_OK)) {
				Options::disabledShell = true;
				std::cerr << "Warning: No shell available. Remote shell disabled\n";
				Log->warning("No shell available. Remote shell disabled");
			}

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

// sudo pkill MattDaemon; sudo ./MattDaemon -f lala -l debug -e
