/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:01 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/15 16:02:09 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Logging.hpp"
	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"
	#include "Network/Communication.hpp"

	#include <unistd.h>															// For close()
	#include <ctime>															// For time() and difftime()
	#include <csignal>															// For kill()
	#include <algorithm>														// For std::find()

#pragma endregion

#pragma region "Variables"

	std::map <int, std::unique_ptr<Client>>	clients;							// FDs linked to their client
	std::map <int, Client *>				shells;								// FDs of shells linked to their client
	std::vector<int> 						pending_removals = {};				// List of FDs scheduled for removal

#pragma endregion

#pragma region "Constructors"

    Client::Client() : fd(-1), ip(""), port(0), type(MSG), last_activity(std::time(NULL)) {}

    Client::Client(int _fd, std::string _ip, int _port) : fd(_fd), ip(_ip), port(_port), type(MSG) {
		last_activity = std::time(NULL); diying = false; user = ""; pass = ""; authenticated = false;
		shell_running = false; shell_pid = 0; master_fd = -1; slave_fd = -1;
	}

    Client::Client(const Client & src) : fd(src.fd), ip(src.ip), port(src.port), type(MSG) {
		last_activity = src.last_activity; diying = src.diying; user = src.user; pass = src.pass; authenticated = src.authenticated;
		shell_running = src.shell_running; shell_pid = src.shell_pid; master_fd = src.master_fd; slave_fd = src.slave_fd;
		write_buffer = src.write_buffer; write_sh_buffer = src.write_sh_buffer;
	}

	Client::~Client() {
		if (fd >= 0) {
			Epoll::remove(fd);
			close(fd); fd = -1;
			Log->info("Client [" + ip + ":" + std::to_string(port) + "] disconnected");
		}
	}

#pragma endregion

#pragma region "Overloads"

	Client & Client::operator=(const Client & rhs) {
        if (this != &rhs) {
			fd = rhs.fd; ip = rhs.ip; port = rhs.port;
			last_activity = rhs.last_activity; diying = rhs.diying; user = rhs.user; pass = rhs.pass; authenticated = rhs.authenticated;
			shell_running = rhs.shell_running; shell_pid = rhs.shell_pid; master_fd = rhs.master_fd; slave_fd = rhs.slave_fd;
			write_buffer = rhs.write_buffer; write_sh_buffer = rhs.write_sh_buffer;
		}
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

			std::string response = "Connection time-out\n";
			write_buffer.insert(write_buffer.end(), response.begin(), response.end());
			Communication::write_client(this);
			schedule_removal();
		}
	}

    void Client::update_last_activity() { last_activity = std::time(NULL); }

#pragma endregion

#pragma region "Remove"

	void Client::remove() {
		clients.erase(fd);
	}

#pragma endregion

#pragma region "Schedule Removal"

	void Client::schedule_removal() {
		Log->debug("Client [" + ip + ":" + std::to_string(port) + "] scheduled for deferred removal");
		if (std::find(pending_removals.begin(), pending_removals.end(), fd) == pending_removals.end())
			pending_removals.push_back(fd);
	}

#pragma endregion

#pragma region "Process Pending Removals"

	void process_pending_removals() {
		for (int fd : pending_removals) {
			auto it = clients.find(fd);
			if (it != clients.end()) {
				Log->debug("Processing deferred removal of client: " + it->second->ip + ":" + std::to_string(it->second->port));
				it->second->remove();
			}
		}
		pending_removals.clear();
	}

#pragma endregion
