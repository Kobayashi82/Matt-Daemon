/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Options.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:04 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 22:33:54 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Config/Options.hpp"

	#include <algorithm>														// std::transform()
	#include <iostream>															// std::cerr()
	#include <cstring>															// std::strlen(), std::memset()
	#include <netdb.h>															// getaddrinfo(), freeaddrinfo(), addrinfo
	#include <unistd.h>															// getuid()
	#include <pwd.h>															// getpwuid()
	#include <getopt.h>															// getopt_long()

#pragma endregion

#pragma region "Variables"

	bool				Options::insecure			= false;					// Allow unencrypted communications
	uint16_t			Options::port				= 4242;						// Port to listen for incoming connections
	std::string			Options::user				= "";						// Username to use for login
	bool				Options::encryption			= false;					// Server requires encryption (true = encrypted, false = plain)
	int					Options::retries			= 3;						// Remaining password retry attempts
	bool				Options::authenticated		= false;					// Authentication state (true if login successful)
	size_t				Options::decryption_index	= 0;						// Index for continuous decryption key rotation
	bool				Options::gui_mode			= false;					// Run in GUI mode (launch Gtk interface)

	int					Options::sockfd				= -1;						// Socket file descriptor
	char				Options::hostname[254];									// IP address or hostname
	char				Options::host[INET_ADDRSTRLEN];							// Resolved IPv4 address in string form
	struct sockaddr_in	Options::sockaddr;										// Stores IPv4 socket address information

	std::string	Options::_fullName		= "Ben__AFK";							// Name and path used to execute the program (same as argv[0])

#pragma endregion

#pragma region "Messages"

	#pragma region "Help"

		int Options::help() {
			std::cerr << "Usage: " << PROGRAM_NAME << " [ OPTION... ] HOST...\n";
			std::cerr << "\n";
			std::cerr << " Options:\n";
			std::cerr << "\n";
			std::cerr << "  -k,  --insecure             Allow unencrypted communication\n";
			std::cerr << "  -l,  --login=USERNAME       Login username                   (default: current user)\n";
			std::cerr << "  -p,  --port=PORT            Port number to connect to        (default: 4242)\n";
			std::cerr << "\n";
			std::cerr << "  -g,  --gui                  Run in graphical (GUI) mode\n";
			std::cerr << "  -h?, --help                 Display this help message\n";
      		std::cerr << "  -u,  --usage                Display short usage message\n";
  			std::cerr << "  -V,  --version              Show program version\n";
			std::cerr << "\n";
			std::cerr << " HOST                         Hostname or IP address to connect to\n";
			std::cerr << "\n";
			std::cerr << "Report bugs to <kobayashi82@outlook.com>\n";

			return (1);
		}

	#pragma endregion

	#pragma region "Usage"

		int Options::usage() {
			std::cerr << "Usage: " << PROGRAM_NAME << " [-k, --insecure] [-g, --gui] [-l USERNAME, --login=USERNAME] [-p PORT, --port=PORT]\n";
			std::cerr << "               HOST ...\n";

			return (1);
		}

	#pragma endregion

	#pragma region "Version"

		int Options::version() {
			std::cerr << PROGRAM_NAME << " 1.0\n";
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

#pragma endregion

#pragma region "Parsing"

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

	#pragma region "Validate Host"

		int Options::validate_host(const char *_hostname) {
			struct addrinfo hints, *res;

			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_RAW;
			hints.ai_flags = AI_CANONNAME;

			if (getaddrinfo(_hostname, NULL, &hints, &res)) return (2);

			std::memcpy(&sockaddr, res->ai_addr, res->ai_addrlen);
			snprintf(hostname, sizeof(hostname), "%s", res->ai_canonname ? res->ai_canonname : _hostname);

			inet_ntop(AF_INET, &sockaddr.sin_addr, host, INET_ADDRSTRLEN);
			freeaddrinfo(res);

			return (0);
		}

	#pragma endregion

	#pragma region "User@Host"

		int Options::parseUserHost(const std::string & _host) {
			size_t atPos = _host.find('@');

			if (atPos == std::string::npos) {
				std::strncpy(host, _host.c_str(), sizeof(host));
			} else {
				std::string newUser = _host.substr(0, atPos);
				std::string newHost = _host.substr(atPos + 1);

				if (!user.empty() && user != newUser) {
					std::cerr << "User conflict: '" + user + "' vs '" + newUser + "'\n";
					return (1);
				}

				if (user.empty()) user = newUser;
				std::strncpy(host, newHost.c_str(), sizeof(host));
			}
			
			return (0);
		}

	#pragma endregion

#pragma endregion

#pragma region "Parse"

	int Options::parse(int argc, char **argv) {
		_fullName = argv[0];

		struct option long_options[] = {
			{"insecure",			no_argument,		0, 'k'},	// [-k, --insecure]
			{"login",				required_argument,	0, 'l'},	// [-l, --login=USER]
			{"port",				required_argument,	0, 'p'},	// [-p, --port=NUM]
			{"gui",				no_argument,		0, 'g'},	// [-g, --gui]

			{"help",				no_argument,		0, 'h'},	// [-h?, --help]
			{"usage",				no_argument,		0, 'u'},	// [	--usage]
			{"version",				no_argument,		0, 'V'},	// [-V, --version]
			{0, 0, 0, 0}
		};

		int opt;
		while ((opt = getopt_long(argc, argv, "kgl:p:h?uV", long_options, NULL)) != -1) {
			switch (opt) {
				case 'k':	insecure = true;										break;
				case 'g':	gui_mode = true;										break;
				case 'l':	user = std::string(optarg);								break;
				case 'p':	if (!ft_strtoul(argv, optarg, &port, 65535, false))		break;					return (2);

				case '?':	if (std::string(argv[optind - 1]) == "-?")				return (help());		return (invalid());
				case 'h':															return (help());
				case 'u':															return (usage());
				case 'V':															return (version());
			}
		}

		if (optind >= argc) { std::cerr << PROGRAM_NAME << ": Missing host\n";										return (invalid()); }

		if (parseUserHost(std::string(argv[optind])))														return (2);

		if (user.empty()) {
			struct passwd* pw = getpwuid(getuid());
			user = pw ? std::string(pw->pw_name) : "";
			if (user.empty()) { std::cerr << "Unable to determine current user\n";							return (2); }
		}

		if (validate_host(host)) { std::cerr << "Unknown host\n";											return (2); }

		return (0);
	}

#pragma endregion
