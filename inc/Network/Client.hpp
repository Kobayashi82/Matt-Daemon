/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/15 15:57:35 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <string>
	#include <vector>
	#include <map>
	#include <memory>

#pragma endregion

#pragma region "Enumarators"

	enum e_client_type { MSG, CLIENT, SHELL };

#pragma endregion

#pragma region "Client"

	class Client {

		public:

			// Variables
			int					fd;												// File descriptor associated with the client
			std::string			ip;												// IP address of the client
			int					port;											// Port number of the client
			int					type;											// Type of client (MSG, CLIENT, SHELL)
			time_t				last_activity;									// Timestamp of the last activity
			bool				diying;											// Flag indicating client is being removed
			std::string			user;											// Username for authentication
			std::string			pass;											// Password for authentication
			bool				authenticated;									// Flag indicating if client is authenticated
			bool				shell_running;									// Flag indicating if shell is running
			int					shell_pid;										// PID of the shell process
			int					master_fd;										// Master FD of the shell terminal
			int					slave_fd;										// Slave FD of the shell terminal
			std::vector <char>	write_buffer;									// Buffer for data to write to client
			std::vector <char>	write_sh_buffer;								// Buffer for data to write to shell

			// Constructors
			Client();															// Default constructor
			Client(int _fd, std::string _ip, int _port);						// Parameterized constructor
			Client(const Client & Cli);											// Copy constructor
			~Client();															// Destructor

			// Overloads
			Client &	operator=(const Client & rhs);							// Assignment operator
			bool		operator==(const Client & rhs) const;					// Equality operator

			// Methods
			void	check_timeout(int interval);								// Checks if the client has timed out
			void	update_last_activity();										// Updates the client last activity timestamp
			void	remove();													// Closes the connection, removes the client and cleans up resources
			void	schedule_removal();											// Schedule client for deferred removal (safe to call during epoll event processing)
	};
	
#pragma endregion

#pragma region "Variables"

	extern std::map <int, std::unique_ptr<Client>>	clients;					// FDs linked to their client
	extern std::map <int, Client *>					shells;						// FDs of shells linked to their client
	extern std::vector<int>							pending_removals;			// List of FDs scheduled for removal

#pragma endregion

#pragma region "Methods"

	void process_pending_removals();											// Process all pending client removals

#pragma endregion
