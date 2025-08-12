/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 22:00:06 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "MattDaemon.hpp"
	#include "Options.hpp"
	#include "Crypto.hpp"
	#include "Tintin_reporter.hpp"
	#include "Shell.hpp"

	#include <unistd.h>

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		if (getuid()) {
			std::cerr << "This program must be run as root" << std::endl;
			return (1);
		}

		// try {
		// 	if (authenticate(argv[1], argv[2])) {
		// 		std::cout << "Autenticación OK\n";
		// 	} else {
		// 		std::cout << "Autenticación fallida\n";
		// 	}
		// } catch(const std::exception& e) {
		// 	std::cerr << e.what() << '\n';
		// }
		

		// std::string popo1 = encrypt("Hola, como estás #~@{<`}");
		// std::cout << popo1 << "\n";
		// std::string popo2 = decrypt(popo1);
		// std::cout << popo2 << "\n";

		// popo(std::string(argv[1]));

		try {
			int result = 0;
			if ((result = Options::parse(argc, argv))) return (result);
			Tintin_reporter	Log(Options::logPath, Options::logLevel);
			// daemon
			// connection
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			return (2);
		}

		return (0);
	}

#pragma endregion
