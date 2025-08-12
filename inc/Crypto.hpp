/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Crypto.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 18:02:22 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 18:19:15 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <string>

#pragma endregion

#pragma region "Methods"

	bool		authenticate(const std::string& user, const std::string& pass);

	std::string encrypt(const std::string& plaintext);
	std::string decrypt(const std::string& ciphertext);

#pragma endregion
