/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Password.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 22:29:03 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 22:33:20 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Config/Options.hpp"
	#include "Config/RawMode.hpp"

	#include "Security/Password.hpp"

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

		if (Options::retries > 0) {
			password = getPasswordInput();
		}

		return (password);
	}

#pragma endregion
