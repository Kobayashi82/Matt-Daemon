/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:29:12 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 23:57:57 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "Crypto.hpp"
	#include "Shell.hpp"
	#include "Logging.hpp"

	#include <csignal>		// std::signal()
	#include <cstdlib>		// std::exit()

	#include <stdio.h>		// dprintf()
	#include <fcntl.h>		// open()
	#include <unistd.h>		// fork(), setsid(), chdir(), close()
	#include <sys/stat.h>	// umask()
	#include <sys/file.h>	// flock()

#pragma endregion

#pragma region "Variables"

	Tintin_reporter	*Log;

#pragma endregion

#pragma region "Daemonize"

	static int daemonize() {
		// 1. fork()
		int pid = fork();
		if (pid < 0) {
			Log->critical("First fork() failed");
			std::cerr << "First fork() failed\n";
			return (1);
		}
		if (pid > 0) std::exit(0);

		// 2. setsid()
		if (setsid() < 0) {
			Log->critical("setuid() failed");
			std::exit(1);
		}

		// 3. fork()
		pid = fork();
		if (pid < 0) {
			Log->critical("Second fork() failed");
			std::exit(1);
		}
		if (pid > 0) std::exit(0);

		// 4. signal()
		std::signal(SIGCHLD, SIG_IGN);
		std::signal(SIGHUP, SIG_IGN);

		// 5. umask()
		umask(022);

		// 6. chdir()
		chdir("/");

		// 7. close()
		close(0);
		close(1); 
		close(2);

		// 8. flock()
		int lockfd = open("/var/lock/matt_daemon.lock", O_RDWR|O_CREAT|O_TRUNC, 0640);
		if (lockfd < 0 || flock(lockfd, LOCK_EX|LOCK_NB)) {
			Log->error("Daemon locked");
			std::exit(1);
		}
		dprintf(lockfd, "%d\n", getpid());

		return (0);
	}

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		if (getuid()) {
			std::cerr << "This program must be run as root" << std::endl;
			return (1);
		}

		try {
			int result = 0;
			if ((result = Options::parse(argc, argv))) return (result - 1);
			std::cout << Options::logPath << " - " << static_cast<int>(Options::logLevel) << "\n";
			Tintin_reporter	Tintin_logger(Options::logPath, Options::logLevel);
			Log = &Tintin_logger;
			if (daemonize()) return (1);
			sleep(10);
			// connection
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			return (1);
		}

		return (0);
	}

#pragma endregion

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
