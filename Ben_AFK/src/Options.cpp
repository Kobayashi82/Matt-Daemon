/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Options.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:04 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/15 23:48:44 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"

	#include <algorithm>														// std::transform()
	#include <iostream>															// std::cerr()
	#include <cstring>															// std::strlen(), std::memset()
	#include <netdb.h>															// getaddrinfo(), freeaddrinfo(), addrinfo
	#include <unistd.h>															// getuid()
	#include <pwd.h>															// getpwuid()
	#include <getopt.h>															// getopt_long()

#pragma endregion

#pragma region "Variables"

	bool				Options::insecure		= false;						// Disable encrypted communication 
	uint16_t			Options::port			= 4242;							// Port to listen for incoming connections
	std::string			Options::user			= "";							// Path to use for logging
	bool				Options::encryption		= false;						// 
	int					Options::retries		= 3;							// 
	bool				Options::authenticated	= false;						// 

	int					Options::sockfd			= -1;							// 
	char				Options::hostname[254];									// IP address or hostname
	char				Options::host[INET_ADDRSTRLEN];							// IP address resolved
	struct sockaddr_in	Options::sockaddr;										// Stores IPv4 socket address information

	std::string	Options::_fullName		= "Ben__AFK";							// Name and path used to execute the program (same as argv[0])

#pragma endregion

#pragma region "Strtoul"

	template<typename T>
	int Options::ft_strtoul(char **argv, const char *optarg, T *value, unsigned long max_value, bool allow_zero) {
		try {
			size_t			idx;
			unsigned long	tmp = std::stoul(optarg, &idx, 0);

			if (idx != std::strlen(optarg)) {
				std::cerr << argv[0] << ": invalid value (`" << optarg << "' near `" << (optarg + idx) << "')\n";
				return (1);
			}

			if (!tmp && !allow_zero) {
				std::cerr << argv[0] << ": option value too small: " << optarg << "\n";
				return (1);
			}

			if (max_value && tmp > max_value) {
				std::cerr << argv[0] << ": option value too big: " << optarg << "\n";
				return (1);
			}

			*value = static_cast<T>(tmp);
			return (0);
		} catch (const std::exception &) {
			std::cerr << argv[0] << ": invalid number: " << optarg << "\n";
			return (1);
		}
	}

#pragma endregion

#pragma region "Messages"

	#pragma region "Help"

		int Options::help() {
			std::cerr << "Usage: " << NAME << " [ -esh?uV ] [ -c max_clients ]  [ -p port ] [ -f log_file ] [ -l log_level ]\n";
			std::cerr << "\n";
			std::cerr << " Options:\n";
			std::cerr << "\n";
			std::cerr << "  -e,  --disable-encryption   disable encryption for remote shell\n";
			std::cerr << "  -s,  --disable-shell        disable remote shell access\n";
			std::cerr << "  -c,  --max-clients=NUM      maximum number of simultaneous clients    (default: 3, unlimited = 0)\n";
			std::cerr << "  -p,  --port=NUM             port number to listen on                  (default: 4242)\n";
			std::cerr << "  -f,  --log-file=PATH        path to the log file                      (default: /var/log/matt_daemon/matt_daemon.log)\n";
			std::cerr << "  -l,  --log-level=LEVEL      logging verbosity level                   (default: INFO)\n";
			std::cerr << "\n";
			std::cerr << "  -h?, --help                 give this help list\n";
      		std::cerr << "  -u,  --usage                give a short usage message\n";
  			std::cerr << "  -V,  --version              print program version\n";
			std::cerr << "\n";
			std::cerr << "Report bugs to <kobayashi82@outlook.com>\n";

			return (1);
		}

	#pragma endregion

	#pragma region "Usage"

		int Options::usage() {
			std::cerr << "Usage: " << NAME << " [-e, --disable-encryption] [-s, --disable-shell] [-c NUM, --max-clients=NUM]\n";
			std::cerr << "                  [-p NUM, --port=NUM] [-f PATH, --log-file=PATH] [-l LEVEL, --log-level=LEVEL]\n";
			std::cerr << "                  [-h? --help] [-u --usage] [-V --version]\n";

			return (1);
		}

	#pragma endregion

	#pragma region "Version"

		int Options::version() {
			std::cerr << NAME << " 1.0\n";
			std::cerr << "Copyright (C) 2025 Kobayashi Corp ⓒ.\n";
			std::cerr << "License WTFPL: DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE.\n";
			std::cerr << "This is free software: you are free to change and redistribute it.\n";
			std::cerr << "There is NO WARRANTY, to the extent permitted by law.\n";
			std::cerr << "\n";
			std::cerr << "Written by Kobayashi82 (vzurera-).\n";

			return (1);
		}

	#pragma endregion

	#pragma region "Invalid"

		int Options::invalid() {
			std::cerr << "Try '" << _fullName << " --help' or '" << _fullName << " --usage' for more information.\n";
			return (2);
		}

	#pragma endregion

	#pragma region "Validate Host"

		int Options::validate_host(const char *_hostname) {
			struct addrinfo hints, *res;

			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_RAW;
			hints.ai_flags = AI_CANONNAME;

			if (getaddrinfo(_hostname, NULL, &hints, &res)) return (1);

			std::memcpy(&sockaddr, res->ai_addr, res->ai_addrlen);
			snprintf(hostname, sizeof(hostname), "%s", res->ai_canonname ? res->ai_canonname : _hostname);

			inet_ntop(AF_INET, &sockaddr.sin_addr, host, INET_ADDRSTRLEN);
			freeaddrinfo(res);

			return (0);
		}

	#pragma endregion

#pragma endregion

	int Options::parseUserHost(const std::string & _host) {
		size_t atPos = _host.find('@');

		if (atPos == std::string::npos) {
			std::strncpy(host, _host.c_str(), sizeof(host));
		} else {
			std::string newUser = _host.substr(0, atPos);
			std::string newHost = _host.substr(atPos + 1);

			if (!user.empty() && user != newUser) {
				std::cerr << "Error: user conflict: '" + user + "' vs '" + newUser + "'\n";
				return (1);
			}

			if (user.empty()) user = newUser;
			std::strncpy(host, newHost.c_str(), sizeof(host));
		}
		
		return (0);
	}

#pragma region "Parse"

	int Options::parse(int argc, char **argv) {
		_fullName = argv[0];

		struct option long_options[] = {
			{"insecure",			no_argument,		0, 'k'},	// [-k, --insecure]
			{"port",				required_argument,	0, 'p'},	// [-p, --port=NUM]
			{"login",				required_argument,	0, 'l'},	// [-l, --login=USER]

			{"help",				no_argument,		0, 'h'},	// [-h?, --help]
			{"usage",				no_argument,		0, 'u'},	// [	--usage]
			{"version",				no_argument,		0, 'V'},	// [-V, --version]
			{0, 0, 0, 0}
		};

		int opt;
		while ((opt = getopt_long(argc, argv, "kp:l:h?uV", long_options, NULL)) != -1) {
			switch (opt) {
				case 'k':	insecure = true;																break;
				case 'p':	if (ft_strtoul(argv, optarg, &port, 65535, false))		return (2);				break;
				case 'l':	user = std::string(optarg);														break;

				case '?':	if (std::string(argv[optind - 1]) == "-?")				return (help());		return (invalid());
				case 'h':															return (help());
				case 'u':															return (usage());
				case 'V':															return (version());
			}
		}

		if (optind >= argc) {
			std::cerr << NAME << ": Missing host\n";
			invalid(); return (2);
		}

		if (parseUserHost(std::string(argv[optind]))) return (2);

		if (user.empty()) {
			struct passwd* pw = getpwuid(getuid());
			user = pw ? std::string(pw->pw_name) : "";
		}

		if (user.empty()) { std::cerr << "Error: unable to determine current user\n"; return (2); }

		if (validate_host(host)) { std::cerr << "Error: unknown host\n"; return (2); }

		return (0);
	}

#pragma endregion
