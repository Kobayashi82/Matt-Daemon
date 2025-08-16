/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Password.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 22:29:03 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 12:24:11 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "RawMode.hpp"
	#include "Password.hpp"

	#include <iostream>															// std::getline(), std::cerr()

#pragma endregion

#pragma region "Get Password Input"

	static std::string getPasswordInput() {
		std::string password;

		raw_mode_enable(true);

		std::getline(std::cin, password);

		raw_mode_disable(true);

		return (password);
	}

#pragma endregion

#pragma region "Get Password"

	std::string getPassword() {
		std::string password;

		while (Options::retries > 0) {
			password = getPasswordInput();
			if (password.empty()) {
				Options::retries--;
				if (!Options::retries)	std::cerr << "Authentication failure, giving up\n";
				else					std::cerr << "Authentication failure, please try again\n";
			} else break;
		}

		return (password);
	}

#pragma endregion
