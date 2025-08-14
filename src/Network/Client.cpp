/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:01 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 12:37:52 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"

	#include <unistd.h>															// For close()
	#include <ctime>															// For time() and difftime()
	#include <csignal>															// For kill()

#pragma endregion

#pragma region "Variables"

	std::map <int, std::unique_ptr<Client>> clients;							// 
	std::map <int, Client> shells;												// 

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
