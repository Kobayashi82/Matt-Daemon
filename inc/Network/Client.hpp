/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 14:56:19 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <string>																						//	For standard input/output stream objects like std::cin, std::cout
	#include <map>

#pragma endregion

#pragma region "Client"

	class Client {

		public:

			//	Variables
			int				fd;																			//	File descriptor associated with the client
			std::string		ip;																			//	IP address of the client
			int				port;																		//	Port number of the client
			time_t			last_activity;																//	Last activity time point	(for keep-alive)
			bool			shell_running;
			int				shell_pid;
			int				master_fd;
			int				slave_fd;

			//	Constructors
			Client();
			Client(int _fd, std::string _ip, int _port);												//	Parameterized constructor
			Client(const Client & Cli);																	//	Copy constructor
			~Client();

			//	Overloads
			Client &	operator=(const Client & rhs);													//	Overload for assignation
			bool		operator==(const Client & rhs) const;											//	Overload for comparison

			//	Methods
			void	check_timeout(int interval);														//	Checks if the client has timed out
			void	update_last_activity();																//	Updates the client last activity timestamp
			void	remove();																		//	Closes the connection, removes the client and cleans up resources

	};
	
#pragma endregion

#pragma region "Variables"

	extern std::map <int, Client> clients;

#pragma endregion
