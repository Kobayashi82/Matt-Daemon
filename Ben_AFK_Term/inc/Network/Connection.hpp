/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 21:41:14 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 22:39:02 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <string>															// std::string

#pragma endregion

#pragma region "Methods"

	int			socket_create();												// Creates socket
	int			send_data(const std::string& data);								// Sends data through the socket
	int			receive_data();													// Receives data from the socket
	int			interactive_loop();												// Runs the main interactive loop
	std::string	get_terminal_size();											// Gets the terminal's current size

#pragma endregion
