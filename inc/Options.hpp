/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Options.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 12:15:15 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 23:33:23 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Include"

	#include <string>
	#include <cstdint>
	#include <unistd.h>

#pragma endregion

#pragma region "Defines"

	#define NAME	"MattDaemon"

#pragma endregion

#pragma region "Options"

	class Options {

		public:

			static bool			disabledEncryption;
			static bool			disabledShell;
			static uint16_t		maxClients;
			static uint16_t		portNumber;
			static uint8_t		logLevel;
			static std::string	logPath;

			static int	parse(int argc, char **argv);

		private:

			enum e_level { DEBUG, INFO, LOG, WARNING, ERROR, CRITICAL };

			static std::string	_fullName;

			Options() {}
			~Options() {}
		
			template<typename T>
			static int	ft_strtoul(char **argv, const char *optarg, T *value, unsigned long max_value, bool allow_zero);
			static int	log_level(const std::string &level);
			static int	help();
			static int	usage();
			static int	version();
			static int	invalid();
	};

#pragma endregion
