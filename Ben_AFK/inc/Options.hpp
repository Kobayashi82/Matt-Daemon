/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Options.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:33 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/15 23:48:28 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Include"

	#include <string>															// std::string
	#include <cstdint>															// uint16_t
	#include <arpa/inet.h>														// inet_ntop()

#pragma endregion

#pragma region "Defines"

	#define NAME	"Ben_AFK"													// Name of the program

#pragma endregion

#pragma region "Options"

	class Options {

		public:

			static bool					insecure;								// Allow non-encrypted communication
			static uint16_t				port;									// Port to connect
			static std::string			user;									// User used for login
			static bool					encryption;								// 
			static int					retries;								// 
			static bool					authenticated;							// 

			static int					sockfd;									// 
			static char					hostname[254];							// IP address or hostname
			static char					host[INET_ADDRSTRLEN];					// IP address resolved
			static struct sockaddr_in	sockaddr;								// Stores IPv4 socket address information

			static int	parse(int argc, char **argv);							// Parse options passed as arguments to the program

		private:

			static std::string	_fullName;										// Name and path used to execute the program (same as argv[0])

			Options() {}														// Default constructor (no instantiable)
			~Options() {}														// Destructor (no instantiable)
		
			template<typename T>
			static int	ft_strtoul(char **argv, const char *optarg, T *value, unsigned long max_value, bool allow_zero);
			static int	help();
			static int	usage();
			static int	version();
			static int	invalid();
			static int	validate_host(const char *hostname);
			static int	parseUserHost(const std::string & _host);
	};

#pragma endregion
