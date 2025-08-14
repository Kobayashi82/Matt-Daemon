/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Daemon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 23:07:15 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 23:11:01 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Logging.hpp"
	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"

	#include <iostream>															// std::cerr()
	#include <csignal>															// std::signal()
	#include <sys/wait.h>														// waitpid()
	#include <fcntl.h>															// open()
	#include <unistd.h>															// fork(), setsid(), chdir(), close()
	#include <sys/stat.h>														// umask()
	#include <sys/file.h>														// flock()

#pragma endregion

#pragma region "Signals Handlers"

	#pragma region "SIGTERM"

		static void sigterm_handler(int signum) { (void) signum;
			Log->info("Signal SIGTERM received");
			Epoll::Running = false;
		}

	#pragma endregion
		
	#pragma region "SIGCHLD"

		static void sigchld_handler(int signum) { (void) signum;
			int pid, status;

			while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
				for (auto& client_pair : clients) {
					Client *client = client_pair.second.get();
					if (client && client->shell_pid == pid && client->shell_running) {
						Log->info("Client [" + client->ip + ":" + std::to_string(client->port) + "] shell process " + std::to_string(pid) + " terminated, disconnecting client");
						client->shell_running = false;
						client->shell_pid = 0;
						client->schedule_removal();
						break;
					}
				}
			}
		}

	#pragma endregion

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
		int signals = 0;
		if (std::signal(SIGTERM, sigterm_handler) == SIG_ERR)	{ signals++; Log->warning("Signal SIGTERM failed"); }
		if (std::signal(SIGCHLD, sigchld_handler) == SIG_ERR)	{ signals++; Log->warning("Signal SIGCHLD failed"); }
		if (std::signal(SIGHUP, SIG_IGN) == SIG_ERR)			{ signals++; Log->warning("Signal SIGHUP failed");  }
		if (std::signal(SIGPIPE, SIG_IGN) == SIG_ERR)			{ signals++; Log->warning("Signal SIGPIPE failed"); }
		if (signals == 0) Log->debug("Signals set");

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
