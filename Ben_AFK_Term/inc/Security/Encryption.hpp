/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Encryption.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:54:52 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 12:47:45 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <string>

#pragma endregion

#pragma region "Methods"

	std::string encrypt(const std::string& plaintext);
	std::string decrypt(const std::string& ciphertext);
	std::string	encrypt_with_index(const std::string& plaintext, size_t& index);
	std::string decrypt_with_index(const std::string& ciphertext, size_t& index);

#pragma endregion
