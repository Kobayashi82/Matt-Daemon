/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:44:57 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 22:32:25 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Config/Options.hpp"
	#include "Config/Signals.hpp"
	#include "Config/RawMode.hpp"

	#include <iostream>															// std::getline(), std::cerr(), std::exit()
	#include <unistd.h>															// close()
	#include <csignal>															// std::signal()

#pragma endregion

#pragma region "Variables"

	volatile bool window_resized = false;

#pragma endregion

#pragma region "Handlers"

	#pragma region "SIGINT"

		static void sigint_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			std::cerr << "\nDisconnected\n";
			std::exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGTERM"

		static void sigterm_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			std::cerr << "\nDisconnected\n";
			std::exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGUP"

		static void sigup_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			std::cerr << "\nDisconnected\n";
			std::exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGQUIT"

		static void sigquit_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			std::cerr << "\nDisconnected\n";
			std::exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGWINCH"

		static void sigwinch_handler(int sig) { (void) sig;
			window_resized = true;
		}

	#pragma endregion

	#pragma region "SIGPIPE"

		static void sigpipe_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			std::cerr << "\nDisconnected\n";
			std::exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGSEV"

		static void sigsev_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			std::cerr << "\nDisconnected\n";
			signal(SIGSEGV, SIG_DFL);
			raise(SIGSEGV);
			std::exit(128 + sig);
		}

	#pragma endregion

#pragma endregion

#pragma region "Set"

	int signal_set() {
		int result = 0;

		if (std::signal(SIGINT,   sigint_handler)	== SIG_ERR) result++;	// Interrupt from keyboard (Ctrl+C)
		if (std::signal(SIGTERM,  sigterm_handler)	== SIG_ERR) result++;	// Request to terminate the program gracefully (sent by 'kill' or system shutdown)
		if (std::signal(SIGHUP,   sigup_handler)	== SIG_ERR) result++;	// Terminal hangup or controlling process terminated (often used to reload config)
		if (std::signal(SIGQUIT,  sigquit_handler)	== SIG_ERR) result++;	// Quit from keyboard (Ctrl+\)
		if (std::signal(SIGPIPE,  sigpipe_handler)	== SIG_ERR) result++;	// Broken pipe (write to pipe with no readers)
		if (std::signal(SIGSEGV,  sigsev_handler)	== SIG_ERR) result++;	// Invalid memory reference (segmentation fault)
		if (std::signal(SIGWINCH, sigwinch_handler)	== SIG_ERR) result++;	// Window size change

		return (result);
	}

#pragma endregion
