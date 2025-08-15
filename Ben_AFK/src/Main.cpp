/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:00 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 00:58:36 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "Ben_AFK.hpp"

	#include <csignal>

#pragma endregion

void signal_handler(int sig) {
	disable_raw_mode();
	exit(128 + sig);
}

#pragma region "Main"

	int main(int argc, char **argv) {
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
		
		int result = 0;
		if ((result = Options::parse(argc, argv)))	return (result - 1);

		if (socket_create())						return (1);
		if (main_loop()) { disable_raw_mode();		return (1); }

		disable_raw_mode();

		return (0);
	}

#pragma endregion
