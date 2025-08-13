/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 15:17:04 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "Crypto.hpp"
	#include "Shell.hpp"
	#include "Logging.hpp"
	#include "Network/Socket.hpp"

	#include <csignal>		// std::signal()
	#include <cstdlib>		// std::exit()

	#include <stdio.h>		// dprintf()
	#include <fcntl.h>		// open()
	#include <unistd.h>		// fork(), setsid(), chdir(), close()
	#include <sys/stat.h>	// umask()
	#include <sys/file.h>	// flock()

#pragma endregion

#pragma region "Daemonize"

	static int daemonize() {
		// 1. fork()
		int pid = fork();
		if (pid < 0) {
			Log->critical("First fork() failed");
			std::cerr << "First fork() failed\n";
			return (1);
		}
		if (pid > 0) std::exit(0);
		Log->debug("First fork() completed");

		// 2. setsid()
		if (setsid() < 0) { Log->critical("setuid() failed"); std::exit(1); }
		Log->debug("setsid() completed");

		// 3. fork()
		pid = fork();
		if (pid < 0) { Log->critical("Second fork() failed"); std::exit(1); }
		if (pid > 0) std::exit(0);
		Log->debug("Second fork() completed");

		// 4. signal()
		int signals = 0;
		if (std::signal(SIGCHLD, SIG_IGN) == SIG_ERR)	{ signals++; Log->warning("Signal SIGCHLD failed"); }
		if (std::signal(SIGHUP, SIG_IGN) == SIG_ERR)	{ signals++; Log->warning("Signal SIGHUP failed");  }
		if (signals != 2) Log->debug("Signals set");

		// 5. umask()
		umask(022);
		Log->debug("umask() set");

		// 6. chdir()
		if (chdir("/"))	Log->warning("chdir() failed");
		else			Log->debug("Working directory changed");

		// 7. close()
		close(0); close(1); close(2);
		Log->debug("Standard file descriptors closed");

		// 8. flock()
		int lockfd = open("/var/lock/matt_daemon.lock", O_RDWR|O_CREAT|O_TRUNC, 0640);
		if (lockfd < 0 || flock(lockfd, LOCK_EX|LOCK_NB)) {
			Log->critical("Daemon already running");
			std::exit(1);
		}
		dprintf(lockfd, "%d\n", getpid());
		Log->debug("Daemon lock set");

		return (0);
	}

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		try {
			if (getuid()) { std::cerr << "This program must be run as root" << std::endl; return (1); }
		
			int result = 0;
			if ((result = Options::parse(argc, argv))) return (result - 1);

			Tintin_reporter	Tintin_logger(Options::logPath, Options::logLevel);
	
			Log->debug("Initiating daemon");
			if (daemonize()) return (1);
			Log->info("Daemon started");

			Log->debug("Initializing network");
			Socket Socket(Options::portNumber);
			if (Socket.create()) return (1);
			Log->info("Daemon listening on port " + std::to_string(Options::portNumber));

			// epoll

			sleep(10);
			Log->info("Daemon closed");
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			return (1);
		}

		return (0);
	}

#pragma endregion
