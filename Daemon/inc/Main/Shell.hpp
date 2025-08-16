/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shell.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 18:02:22 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 22:49:21 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "Network/Client.hpp"

#pragma endregion

#pragma region "Methods"

	int	shell_start(Client *client);											// Start a new shell session
	int	shell_close(Client *client);											// Close a shell session

#pragma endregion
