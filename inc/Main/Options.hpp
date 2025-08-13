/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Options.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 12:15:15 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 23:22:21 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Include"

	#include <string>
	#include <cstdint>

#pragma endregion

#pragma region "Defines"

	#define NAME	"MattDaemon"

#pragma endregion

#pragma region "Options"

	class Options {

		public:

			static bool			disabledEncryption;								// Disable encrypted communication 
			static bool			disabledShell;									// Disable remote shell
			static uint16_t		maxClients;										// Maximum number of clients connected simultaneously
			static uint16_t		portNumber;										// Port to listen for incoming connections
			static uint8_t		logLevel;										// Level of logging
			static std::string	logPath;										// Path to use for logging

			static int	parse(int argc, char **argv);							// Parse options passed as arguments to the program

		private:

			enum e_level { DEBUG, INFO, LOG, WARNING, ERROR, CRITICAL };

			static std::string	_fullName;										// Name and path used to execute the program (same as argv[0])

			Options() {}														// Default constructor (no instantiable)
			~Options() {}														// Destructor (no instantiable)
		
			template<typename T>
			static int	ft_strtoul(char **argv, const char *optarg, T *value, unsigned long max_value, bool allow_zero);
			static int	log_level(const std::string &level);
			static int	help();
			static int	usage();
			static int	version();
			static int	invalid();
	};

#pragma endregion
