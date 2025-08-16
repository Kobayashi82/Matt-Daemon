/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Daemon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 23:07:15 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 12:59:11 by vzurera-         ###   ########.fr       */
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
		if (!signal_set())	Log->debug("All signal handlers successfully installed");
		else				Log->debug("Failed to register one or more signal handlers");

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
