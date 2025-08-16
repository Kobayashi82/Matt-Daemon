/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shell.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 19:09:14 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 13:43:33 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Logging.hpp"
	#include "Main/Shell.hpp"
	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"

	#include <pwd.h>															// getpwnam()
	#include <sys/wait.h>														// close(), fork(), setsid(), setgid(), setuid(), dup2(), chdir(), execvp()
	#include <sys/ioctl.h>														// ioctl()
	#include <fcntl.h>															// open()
	#include <unistd.h>															// access()

#pragma endregion

#pragma region "Start"

	int shell_start(Client *client) {
		if (!client) return (1);

		struct passwd *pw = getpwnam(client->user.c_str());

		if (!pw) {
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] user not found");
			return (1);
		}

		client->master_fd = posix_openpt(O_RDWR | O_NOCTTY);
		if (client->master_fd == -1) {
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] posix_openpt() failed");
			return (1);
		}
		
		if (grantpt(client->master_fd) == -1 || unlockpt(client->master_fd) == -1) {
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] grantpt()/unlockpt() failed");
			close(client->master_fd);
			return (1);
		}
		
		char pty_name[256];
		if (ptsname_r(client->master_fd, pty_name, sizeof(pty_name)) != 0) {
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] ptsname_r() failed");
			close(client->master_fd);
			return (1);
		}

		pid_t pid = fork();
		if (pid < 0) {
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] fork() failed");
			close(client->master_fd);
			return (1);
		}

		if (pid == 0) {
			client->slave_fd = open(pty_name, O_RDWR);
			if (client->slave_fd == -1) {
				Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] open slave failed");
				std::exit(1);
			}

			if (setsid() == -1) std::exit(1);
			
			ioctl(client->slave_fd, TIOCSCTTY, 0);

			dup2(client->slave_fd, STDIN_FILENO);
			dup2(client->slave_fd, STDOUT_FILENO);
			dup2(client->slave_fd, STDERR_FILENO);
			
			close(client->master_fd);
			if (client->slave_fd > 2) {
				close(client->slave_fd);
			}

			if (setgid(pw->pw_gid) != 0) {
				Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] setgid() failed");
				std::exit(1);
			}
			if (setuid(pw->pw_uid) != 0) {
				Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] setuid() failed");
				std::exit(1);
			}

			setenv("HOME", pw->pw_dir, 1);
			setenv("USER", pw->pw_name, 1);
			setenv("LOGNAME", pw->pw_name, 1);

			if (chdir(pw->pw_dir) != 0) {
				Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] chdir() failed");
				std::exit(1);
			}

			const char* shell_path = nullptr;
			if		(!access("/bin/bash", X_OK))	shell_path = "/bin/bash";
			else if (!access("/bin/zsh", X_OK))		shell_path = "/bin/zsh";
			else if (!access("/bin/sh", X_OK))		shell_path = "/bin/sh";
			else {
				Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] no shell found");
				std::exit(1);
			}

			char *args[] = { (char *)shell_path, nullptr };
			execvp(args[0], args);
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] execve() failed");
			std::exit(1);
		}

		client->shell_pid = pid;
		client->shell_running = true;
		shells[client->master_fd] = client;
		Epoll::add(client->master_fd, true, true);

		Log->info("Client [" + client->ip + ":" + std::to_string(client->port) + "] shell started with PTY: " + std::string(pty_name));
		return (0);
	}

#pragma endregion

#pragma region "Close"

	int shell_close(Client *client) {
		if (!client) return (1);

		if (client->shell_running && client->shell_pid > 0) {
			Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] terminating shell process " + std::to_string(client->shell_pid));
			kill(client->shell_pid, SIGTERM);
			client->shell_running = false;
			client->shell_pid = 0;
		}

		if (client->master_fd >= 0) {
			shells.erase(client->master_fd);
			Epoll::remove(client->master_fd);
			close(client->master_fd);
			client->master_fd = -1;
		}

		if (client->slave_fd >= 0) {
			close(client->slave_fd);
			client->slave_fd = -1;
		}

		return (0);
	}

#pragma endregion
