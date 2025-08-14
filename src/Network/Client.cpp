/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:01 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 18:38:31 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Logging.hpp"
	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"

	#include <unistd.h>															// For close()
	#include <ctime>															// For time() and difftime()
	#include <csignal>															// For kill()

#pragma endregion

#pragma region "Variables"

	std::map <int, std::unique_ptr<Client>> clients;							// 
	std::map <int, Client *> shells;											// 

#pragma endregion

#pragma region "Constructors"

    Client::Client() : fd(-1), ip(""), port(0), type(MSG), last_activity(std::time(NULL)) {}

    Client::Client(int _fd, std::string _ip, int _port) : fd(_fd), ip(_ip), port(_port), type(MSG),  last_activity(std::time(NULL)) {}

    Client::Client(const Client & src) : fd(src.fd), ip(src.ip), port(src.port), type(MSG), last_activity(src.last_activity) {}

	Client::~Client() {
		if (fd != -1) {
			if (shell_running && shell_pid != 0) {
				shell_running = shell_pid = 0;
				kill(shell_pid, SIGKILL);
			}
			close(fd);
			Log->info("Client [" + ip + ":" + std::to_string(port) + "] disconnected");
		}
	}

#pragma endregion

#pragma region "Overloads"

	Client & Client::operator=(const Client & rhs) {
        if (this != &rhs) { fd = rhs.fd; ip = rhs.ip; port = rhs.port; last_activity = rhs.last_activity; }
		return (*this);
    }

	bool Client::operator==(const Client & rhs) const {
		return (fd == rhs.fd);
	}

#pragma endregion

#pragma region "Time-Out"

	void Client::check_timeout(int interval) {
		time_t current_time = std::time(NULL);
		if (difftime(current_time, last_activity) > interval) {
			Log->info("Client [" + ip + ":" + std::to_string(port) + "] connection time-out");
			remove();
		}
	}

    void Client::update_last_activity() { last_activity = std::time(NULL); }

#pragma endregion

#pragma region "Remove"

	void Client::remove() {
		Log->info("Client [" + ip + ":" + std::to_string(port) + "] disconnecting");
		
		// Store original fd before modifying it
		int original_fd = fd;
		
		// Clean up shell resources if shell is running
		if (shell_running && shell_pid > 0) {
			Log->debug("Client [" + ip + ":" + std::to_string(port) + "] terminating shell process " + std::to_string(shell_pid));
			kill(shell_pid, SIGTERM);
			shell_running = false;
			shell_pid = 0;
		}
		
		// Clean up shell descriptors and epoll entries
		if (master_fd >= 0) {
			shells.erase(master_fd);
			Epoll::remove(master_fd);
			close(master_fd);
			master_fd = -1;
		}
		
		if (slave_fd >= 0) {
			close(slave_fd);
			slave_fd = -1;
		}
		
		// Clean up client socket
		if (fd >= 0) {
			Epoll::remove(fd);
			close(fd);
			fd = -1;
		}
		
		// Remove from clients map using original fd
		clients.erase(original_fd);
	}

#pragma endregion
