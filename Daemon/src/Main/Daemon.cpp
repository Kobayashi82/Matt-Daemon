/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Daemon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 23:07:15 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 15:20:34 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Signals.hpp"
	#include "Main/Logging.hpp"
	#include "Network/Client.hpp"

	#include <iostream>															// std::cerr(), std::exit()
	#include <fcntl.h>															// open()
	#include <unistd.h>															// fork(), setsid(), chdir(), close()
	#include <sys/stat.h>														// umask()
	#include <sys/file.h>														// flock()

#pragma endregion

#pragma region "Daemonize"

	int daemonize() {
		// 1. fork()
		int pid = fork();
		if (pid < 0) {
			Log->critical("Daemon: first fork() failed");
			std::cerr << "Daemon: first fork() failed\n";
			return (1);
		}
		if (pid > 0) std::exit(0);
		Log->debug("Daemon: first fork() completed");

		// 2. setsid()
		if (setsid() < 0) { Log->critical("Daemon: setuid() failed"); std::exit(1); }
		Log->debug("Daemon: setsid() completed");

		// 3. fork()
		pid = fork();
		if (pid < 0) { Log->critical("Daemon: second fork() failed"); std::exit(1); }
		if (pid > 0) std::exit(0);
		Log->debug("Daemon: second fork() completed");

		// 4. signal()
		if (!signal_set())	Log->debug("Daemon: all signal handlers successfully installed");
		else				Log->debug("Daemon: failed to register one or more signal handlers");

		// 5. umask()
		umask(022);
		Log->debug("umask() set");

		// 6. chdir()
		if (chdir("/"))	Log->warning("Daemon: chdir() failed");
		else			Log->debug("Daemon: working directory changed");

		// 7. close()
		close(0); close(1); close(2);
		Log->debug("Daemon: standard file descriptors closed");

		// 8. flock()
		int lockfd = open("/var/lock/matt_daemon.lock", O_RDWR|O_CREAT|O_TRUNC, 0640);
		if (lockfd < 0 || flock(lockfd, LOCK_EX|LOCK_NB)) {
			Log->critical("Daemon: already running");
			std::exit(1);
		}
		dprintf(lockfd, "%d\n", getpid());
		Log->debug("Daemon: lock set");

		return (0);
	}

#pragma endregion
