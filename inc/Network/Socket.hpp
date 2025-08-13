/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 11:17:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 23:08:17 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Socket"

	class Socket {

		public:

			// Variables
			int		sockfd;														// File descriptor associated with the client
			int		port;														// Port number of the client

			// Constructors
			Socket();															// Default constructor
			Socket(int _port);													// Parameterized constructor
			Socket(const Socket & src);											// Copy constructor
			~Socket();															// Destructor

			// Overloads
			Socket &	operator=(const Socket & rhs);							// Overload for assignation
			bool		operator==(const Socket & rhs) const;					// Overload for equality

			// Methods
			int		create();													// Creates socket
			int		accept();													// Accepts a new connection (client)
			void	close();													// Closes socket
	};

#pragma endregion
