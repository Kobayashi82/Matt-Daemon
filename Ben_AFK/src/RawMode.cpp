/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:24:35 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 12:25:09 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "RawMode.hpp"

	#include <iostream>															// std::cerr()
	#include <unistd.h>															// STDIN_FILENO
	#include <termios.h>														// tcgetattr(), tcsetattr(), termios

#pragma endregion

#pragma region "Variables"

	static struct termios	original_termios;
	static bool				raw_enabled = false;
	static bool				in_passmode = false;

#pragma endregion

#pragma region "Enable"

	int raw_mode_enable(bool passmode) {
		if (passmode) {
			struct termios	newTermios;

			std::cout << "Password for " + Options::user + ": "; std::cout.flush();

			tcgetattr(STDIN_FILENO, &original_termios);
			newTermios = original_termios;

			newTermios.c_lflag &= ~ECHO;
			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

			in_passmode = true;
		} else if (!raw_enabled) {
			if (tcgetattr(STDIN_FILENO, &original_termios) == -1) { std::cerr << "Error: Failed to get terminal attributes\n"; return (1); }

			struct termios raw = original_termios;
			raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
			raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
			raw.c_cflag |= (CS8);
			raw.c_oflag &= ~(OPOST);
			raw.c_cc[VMIN] = 1;
			raw.c_cc[VTIME] = 0;

			if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { std::cerr << "Error: Failed to set raw mode\n"; return (1); }

			raw_enabled = true;
		}

		return (0);
	}

#pragma endregion

#pragma region "Disable"

	void raw_mode_disable(bool passmode) {
		if (passmode || in_passmode) {
			tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
			std::cout << std::endl;
			in_passmode = false;
		} else if (raw_enabled) {
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
			raw_enabled = false;
		}
	}

#pragma endregion
