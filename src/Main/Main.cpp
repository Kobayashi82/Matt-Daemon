/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 00:30:04 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/MattDaemon.hpp"
	#include "Log/Log.hpp"

	#include <iostream>
	#include <unistd.h>

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		(void) argc;
		(void) argv;

		// if (getuid()) {
		// 	std::cerr << "This program must be run as root" << std::endl;
		// 	return (1);
		// }

		try {
			Log	Log("mylog", Log.LOG);

			Log.warning("Probando");
			Log.error("una");
			Log.log("cosa");
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			return (2);
		}

		return (0);
	}

#pragma endregion
