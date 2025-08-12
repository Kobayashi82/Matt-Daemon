/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shell.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 19:09:14 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 19:13:13 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

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
