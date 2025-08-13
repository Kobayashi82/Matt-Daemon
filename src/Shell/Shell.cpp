/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shell.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 19:09:14 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 14:57:41 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include <unistd.h>
	#include <pwd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <iostream>

#pragma endregion

#pragma region "Popo"

	int popo(const std::string user) {
		struct passwd *pw = getpwnam(user.c_str());

		if (!pw) {
			std::cerr << "Usuario no encontrado\n";
			return 1;
		}

		pid_t pid = fork();

		if (pid == 0) {
			// --- Hijo ---
			if (setgid(pw->pw_gid) != 0) {
				perror("setgid");
				_exit(1);
			}
			if (setuid(pw->pw_uid) != 0) {
				perror("setuid");
				_exit(1);
			}

			setenv("HOME", pw->pw_dir, 1);
			setenv("USER", pw->pw_name, 1);
			setenv("LOGNAME", pw->pw_name, 1);
			if (chdir(pw->pw_dir) != 0) {
				perror("chdir");
				_exit(1);
			}
			char *args[] = { (char*)"/bin/bash", nullptr };
			execvp(args[0], args);
			perror("execvp");
			_exit(1);
			return (0);
		}
		else if (pid > 0) {
			// --- Padre ---
			int status;
			waitpid(pid, &status, 0);
			std::cout << "Proceso hijo terminó con código " << WEXITSTATUS(status) << "\n";
		}
		else {
			perror("fork");
			return 1;
		}
		return (0);
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

#pragma endregion
