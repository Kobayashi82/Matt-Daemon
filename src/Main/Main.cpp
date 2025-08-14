/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 20:38:17 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Options.hpp"
	#include "Main/Logging.hpp"
	#include "Main/Shell.hpp"
	#include "Network/Socket.hpp"
	#include "Network/Epoll.hpp"

	#include <csignal>															// For std::signal()
	#include <cstdlib>															// For std::exit()
	#include <sys/wait.h>														// For waitpid()

	#include <stdio.h>															// For dprintf()
	#include <fcntl.h>															// For open()
	#include <unistd.h>															// For fork(), setsid(), chdir() and close()
	#include <sys/stat.h>														// For umask()
	#include <sys/file.h>														// For flock()

#pragma endregion

#pragma region "Daemonize"

	static void sigterm_handler(int signum) {
		(void) signum;
		Log->info("Signal SIGTERM received");
		Epoll::Running = false;
	}

	static void sigchld_handler(int signum) {
		(void) signum;
		// Clean up zombie child processes
		pid_t pid;
		int status;
		while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
			Log->debug("Child process " + std::to_string(pid) + " terminated with status " + std::to_string(status));
			
			// Find the client associated with this shell process and disconnect it
			bool client_found = false;
			for (auto& client_pair : clients) {
				Client* client = client_pair.second.get();
				if (client && client->shell_pid == pid && client->shell_running) {
					Log->info("Client [" + client->ip + ":" + std::to_string(client->port) + "] shell process " + std::to_string(pid) + " terminated, disconnecting client");
					client->shell_running = false; // Mark shell as not running
					client->shell_pid = 0; // Clear PID to avoid double cleanup
					client->remove();
					client_found = true;
					break;
				}
			}
			
			if (!client_found) {
				Log->debug("No client found for terminated process " + std::to_string(pid));
			}
		}
	}

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
		if (std::signal(SIGTERM, sigterm_handler) == SIG_ERR)	{ signals++; Log->warning("Signal SIGTERM failed"); }
		if (std::signal(SIGCHLD, sigchld_handler) == SIG_ERR)	{ signals++; Log->warning("Signal SIGCHLD failed"); }
		if (std::signal(SIGHUP, SIG_IGN) == SIG_ERR)	{ signals++; Log->warning("Signal SIGHUP failed");  }
		if (std::signal(SIGPIPE, SIG_IGN) == SIG_ERR)	{ signals++; Log->warning("Signal SIGPIPE failed");  }
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
