/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:06 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 14:55:21 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Logging.hpp"
	#include "Network/Socket.hpp"
	#include "Network/Client.hpp"
	// #include "Network/Epoll.hpp"

	#include <arpa/inet.h>
	#include <unistd.h>
	#include <cstring>
	#include <string>

#pragma endregion

#pragma region "Constructors"

    Socket::Socket() : sockfd(-1), port(4242) {}

    Socket::Socket(uint16_t _port) : sockfd(-1), port(_port) {}

    Socket::Socket(const Socket & src) : sockfd(src.sockfd), port(src.port) {}

	Socket::~Socket() { close(); }

#pragma endregion

#pragma region "Overloads"

	Socket & Socket::operator=(const Socket & rhs) {
        if (this != &rhs) { sockfd = rhs.sockfd; port = rhs.port; }
		return (*this);
    }

	bool Socket::operator==(const Socket & rhs) const {
		return (sockfd == rhs.sockfd);
	}

#pragma endregion

#pragma region "Create"

	int Socket::create() {
		if (sockfd != -1) return (1);

		//	Create socket
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			Log->critical("Socket creation failed");
			return (1);
		}

		//	Configure socket
		int options = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)) == -1) {
			::close(sockfd);
			Log->critical("Socket set option failed");
			return (1);
		}
		Log->debug("Socket created");

		//	Initialize the socket address structure
		sockaddr_in address; std::memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		address.sin_addr.s_addr = INADDR_ANY;

		//	Link the address to the socket
		if (bind(sockfd, (sockaddr *)&address, sizeof(address)) == -1) {
			::close(sockfd);
			Log->critical("Socket bind failed");
			return (1);
		}

		//	Listen on the address for incoming connections
		if (listen(sockfd, SOMAXCONN) == -1) {
			::close(sockfd);
			Log->critical("Socket listen failed");
			return (1);
		}

		//	Add the socket FD to EPOLL
		// if (Epoll::add(sockfd, true, false) == -1) {
		// 	Log->critical("Socket creation failed");
		// 	close(sockfd);
		// throw std::runtime_error("socket error");
		// }

		return (0);
	}

#pragma endregion

#pragma region "Close"

	void Socket::close() {
		if (sockfd != -1) {
			::close(sockfd);
			clients.clear();
		}
	}

#pragma endregion

#pragma region "Accept"

	int Socket::accept() {
		sockaddr_in Addr; socklen_t AddrLen = sizeof(Addr);
		int fd = ::accept(sockfd, (sockaddr *)&Addr, &AddrLen);
		if (fd < 0) {
			Log->error("Socket accept connection failed");
			return (1);
		}

		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(Addr.sin_addr), ip_str, INET_ADDRSTRLEN);
		std::string ip		= ip_str;
		uint16_t	port	= ntohs(Addr.sin_port);

		clients[fd] = Client(fd, ip, port);

		// if (Epoll::add(fd, true, false) == -1) { Event::events[fd].client->remove(); return; }

		return (0);
	}

#pragma endregion
