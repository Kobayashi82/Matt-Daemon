/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:44:57 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 12:23:20 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "RawMode.hpp"

	#include <iostream>															// std::getline(), std::cerr()
	#include <unistd.h>															// close()
	#include <csignal>															// signal(s)

#pragma endregion

#pragma region "Handlers"

	#pragma region "SIGINT"

		static void sigint_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGTERM"

		static void sigterm_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGUP"

		static void sigup_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGQUIT"

		static void sigquit_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGPIPE"

		static void sigpipe_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGSEV"

		static void sigsev_handler(int sig) {
			raw_mode_disable(false);
			if (Options::sockfd >= 0) close(Options::sockfd);
			signal(SIGSEGV, SIG_DFL);
			raise(SIGSEGV);
			exit(128 + sig);
		}

	#pragma endregion

#pragma endregion

#pragma region "Set"

	int signal_set() {
		int result = 0;

		if (signal(SIGINT,  sigint_handler)		== SIG_ERR) result++;	// Interrupt from keyboard (Ctrl+C)
		if (signal(SIGTERM, sigterm_handler)	== SIG_ERR) result++;	// Request to terminate the program gracefully (sent by 'kill' or system shutdown)
		if (signal(SIGHUP,  sigup_handler)		== SIG_ERR) result++;	// Terminal hangup or controlling process terminated (often used to reload config)
		if (signal(SIGQUIT, sigquit_handler)	== SIG_ERR) result++;	// Quit from keyboard (Ctrl+\)
		if (signal(SIGPIPE, sigpipe_handler)	== SIG_ERR) result++;	// Broken pipe (write to pipe with no readers)
		if (signal(SIGSEGV, sigsev_handler)		== SIG_ERR) result++;	// Invalid memory reference (segmentation fault)

		return (result);
	}

#pragma endregion
