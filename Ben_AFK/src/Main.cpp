/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:49:00 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 00:25:47 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Options.hpp"
	#include "Connection.hpp"

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		int result = 0;

		if ((result = Options::parse(argc, argv))) return (result - 1);

		if (socket_create()) return (1);
		if (main_loop()) return (1);

		// HACER help, usage y version
		// Conectar y enviar /CLIENT_SHELL_AUTH
		// Si recibe respuesta: preguntar por pass
		// Enviar credentials
		// Si recibe OK:
		// Terminal en modo raw
		// Enviar pulsaciones al servidor
		// Imprimir datos recividos en la terminal
		// Interceptar señales y enviarlas (no se como aun)

		return (result);
	}

#pragma endregion
