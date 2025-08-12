/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Crypto.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:49:40 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 21:05:09 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Crypto.hpp"

	#include <shadow.h>
	#include <crypt.h>
	#include <cstring>
	#include <iomanip>

#pragma endregion

#pragma region "Variables"

	static const std::string KEY = "Th1s_1s_n0t_4_s3cr3t_k3y_1t's_0nly_4_l0v3_m3ss4g3:_I_L0v3_U_λµяΔ!";

#pragma endregion

#pragma region "Authenticate"

	bool authenticate(const std::string& user, const std::string& pass) {
		auto spwd_entry = getspnam(user.c_str());
		if (!spwd_entry) return (false);

		const char* hash = spwd_entry->sp_pwdp;
		char* res = crypt(pass.c_str(), hash);
		if (!res) return (false);

		return (std::strcmp(res, hash) == 0);
	}

#pragma endregion

#pragma region "Encryptation"

	#pragma region "Internal"

		#pragma region "Process"

			static std::string process(const std::string& data) {
				std::string result;
				result.reserve(data.length());

				for (size_t i = 0; i < data.length(); ++i) {
					char encrypted_char = data[i] ^ KEY[i % KEY.length()];
					result.push_back(encrypted_char);
				}

				return (result);
			}

		#pragma endregion

		#pragma region "To Hex"

			static std::string toHex(const std::string& data) {
				std::stringstream ss;
				ss << std::hex << std::setfill('0');
				for (unsigned char c : data) ss << std::setw(2) << static_cast<int>(c);

				return (ss.str());
			}

		#pragma endregion

		#pragma region "From Hex"

			static std::string fromHex(const std::string& hexData) {
				if (hexData.length() % 2 != 0) throw std::invalid_argument("Invalid hex string length");

				std::string result;
				result.reserve(hexData.length() / 2);

				for (size_t i = 0; i < hexData.length(); i += 2) {
					std::string byte = hexData.substr(i, 2);
					char c = static_cast<char>(std::strtol(byte.c_str(), nullptr, 16));
					result.push_back(c);
				}

				return (result);
			}

		#pragma endregion

	#pragma endregion

	#pragma region "Encrypt"

		std::string encrypt(const std::string& plaintext)  {
			return (toHex(process(plaintext)));
		}

	#pragma endregion

	#pragma region "Decrypt"

		std::string decrypt(const std::string& ciphertext) {
			return (process(fromHex(ciphertext)));
		}

	#pragma endregion

#pragma endregion
