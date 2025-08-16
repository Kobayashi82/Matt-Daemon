/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Encryption.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:54:52 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 11:55:19 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <string>

#pragma endregion

#pragma region "Methods"

	std::string encrypt(const std::string& plaintext);
	std::string decrypt(const std::string& ciphertext);

#pragma endregion
