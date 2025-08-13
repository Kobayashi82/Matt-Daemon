/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 14:48:01 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Socket"

	class Socket {

		public:

			//	Variables
			int				sockfd;																		//	File descriptor associated with the client
			uint16_t		port;																		//	Port number of the client

			//	Constructors
			Socket();
			Socket(uint16_t _port);										//	Parameterized constructor
			Socket(const Socket & src);									//	Copy constructor
			~Socket();

			//	Overloads
			Socket &	operator=(const Socket & rhs);													//	Overload for assignation
			bool		operator==(const Socket & rhs) const;											//	Overload for comparison

			//	Methods
			int		create();											//	Creates socket
			int		accept();											//	Accept a new connection (client)
			void	close();											//	Accept a new connection (client)
	};

#pragma endregion
