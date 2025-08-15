/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Password.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 22:29:03 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 00:05:26 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

	#include "Options.hpp"

	#include <iostream>
	#include <termios.h>
	#include <unistd.h>

	std::string getUserPassword() {
		struct termios	oldTermios, newTermios;
		std::string		password;

		std::cout << "Password for " + Options::user + ": "; std::cout.flush();

		tcgetattr(STDIN_FILENO, &oldTermios);
		newTermios = oldTermios;

		newTermios.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

		std::getline(std::cin, password);

		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);

		std::cout << std::endl;

		return (password);
	}

	std::string getPassword() {
		std::string password;

		while (Options::retries > 0) {
			password = getUserPassword();
			if (password.empty()) {
				Options::retries--;
				if (!Options::retries)	std::cerr << "Authentication failure, giving up\n";
				else					std::cerr << "Authentication failure, please try again\n";
			} else break;
		}

		return (password);
	}
