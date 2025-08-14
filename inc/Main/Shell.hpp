/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shell.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 18:02:22 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 20:47:59 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "Network/Client.hpp"

#pragma endregion

#pragma region "Methods"

	int	shell_start(Client *client);
	int	shell_close(Client *client);

#pragma endregion
