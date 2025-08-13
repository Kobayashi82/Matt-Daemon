/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:01 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 14:55:43 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Variables"

	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"

	#include <ctime>
	#include <csignal>

#pragma endregion

#pragma region "Variables"

	std::map <int, Client> clients;

#pragma endregion

#pragma region "Constructors"

    Client::Client() : fd(-1), ip(""), port(0) {
		last_activity = std::time(NULL);
	}

    Client::Client(int _fd, std::string _ip, int _port) : fd(_fd), ip(_ip), port(_port) {
		last_activity = std::time(NULL);
	}

    Client::Client(const Client & src) : fd(src.fd), ip(src.ip), port(src.port), last_activity(src.last_activity) {}

	Client::~Client() {
		if (fd != -1) {
			if (shell_running && shell_pid != 0) {
				shell_running = shell_pid = 0;
				kill(shell_pid, SIGKILL);
			}
			close(fd);
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
		if (difftime(current_time, last_activity) > interval)  remove();
	}

    void Client::update_last_activity() { last_activity = std::time(NULL); }

#pragma endregion

#pragma region "Remove"

	void Client::remove() {
		clients.erase(fd);
	}

#pragma endregion
