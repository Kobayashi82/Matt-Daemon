/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Communication.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:46:19 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 23:40:02 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Logging.hpp"
	#include "Network/Socket.hpp"
	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"
	#include "Network/Communication.hpp"

	#include <cstring>															// For std::memset() and std::strcmp()
	#include <unistd.h>															// For read() and write()
	#include <sys/socket.h>														// For recv() and send()
	
	#include <iomanip>															// 
	#include <shadow.h>															// 
	#include <crypt.h>															// 

#pragma endregion

#pragma region "Variables"

	const size_t Communication::CHUNK_SIZE	= 4096;								// Size of the buffer for read/recv and write/send operations

	const std::string Communication::KEY	= "Th1s_1s_n0t_4_s3cr3t_k3y_1t's_0nly_4_l0v3_m3ss4g3:_I_L0v3_U_λµяΔ!";

#pragma endregion

#pragma region "Communications"

	#pragma region "CLIENT"

		#pragma region "Read"

			int Communication::read_client(Client *client) {
				if (!client || client->fd < 0) return (0);

				char buffer[CHUNK_SIZE];	std::memset(buffer, 0, sizeof(buffer));
				ssize_t bytes_read = recv(client->fd, buffer, CHUNK_SIZE, 0);

				// Read some data
				if (bytes_read > 0) {
					client->update_last_activity();
					if (client->type == MSG)	Log->log(std::string(buffer, buffer + bytes_read));
					else						client->write_sh_buffer.insert(client->write_sh_buffer.end(), buffer, buffer + bytes_read);
				}
				// No data (usually means a client disconected)
				else if (bytes_read == 0)	{ client->remove();	return (1); }
				// Error reading
				else if (bytes_read == -1)	{ client->remove();	return (1); }

				return (0);
			}

		#pragma endregion

		#pragma region "Write"

			void Communication::write_client(Client *client) {
				if (!client || client->fd < 0) return;

				// There are data, send it
				if (!client->write_buffer.empty()) {
					client->update_last_activity();

					size_t buffer_size = client->write_buffer.size();
					size_t chunk = std::min(buffer_size, static_cast<size_t>(CHUNK_SIZE));
					ssize_t bytes_written = send(client->fd, client->write_buffer.data(), chunk, MSG_DONTWAIT);

					// Sent some data
					if (bytes_written > 0) {
						client->write_buffer.erase(client->write_buffer.begin(), client->write_buffer.begin() + bytes_written);
					}
					// No writing if 'write_buffer' is empty, so this must be an error
					else if (bytes_written == 0)	{ client->remove();	return; }
					// Error writing
					else if (bytes_written == -1)	{ client->remove();	return; }
				}
			}

		#pragma endregion

	#pragma endregion

	#pragma region "SHELL"

		#pragma region "Read"

			int Communication::read_shell(Client *client) {
				if (!client || client->master_fd < 0) return (0);

				char buffer[CHUNK_SIZE];	std::memset(buffer, 0, sizeof(buffer));

				ssize_t bytes_read = read(client->master_fd, buffer, CHUNK_SIZE);

				// Read some data
				if (bytes_read > 0) {
					client->update_last_activity();
					client->write_buffer.insert(client->write_buffer.end(), buffer, buffer + bytes_read);
				}
				// No data (shell ended or stdout closed)
				else if (bytes_read == 0) {
					// close shell and client
					return (1);
				}
				// Error reading
				else if (bytes_read == -1) {
					// close shell and client
					return (1);
				}

				return (0);
			}

		#pragma endregion

		#pragma region "Write"

			void Communication::write_shell(Client *client) {
				if (!client || client->master_fd < 0) return;

				// There are data, send it
				if (!client->write_sh_buffer.empty()) {
					client->update_last_activity();

					size_t buffer_size = client->write_sh_buffer.size();
					size_t chunk = std::min(buffer_size, static_cast<size_t>(CHUNK_SIZE));
					ssize_t bytes_written = write(client->master_fd, client->write_sh_buffer.data(), chunk);

					// Sent some data
					if (bytes_written > 0) {
						client->write_sh_buffer.erase(client->write_sh_buffer.begin(), client->write_sh_buffer.begin() + bytes_written);
					}
					// No writing if 'write_sh_buffer' is empty, so this must be an error
					else if (bytes_written == 0) {
						// close shell and client
						return;
					}
					// Error writing
					else if (bytes_written == -1) {
						// close shell and client
						return;
					}
				}
			}

		#pragma endregion

	#pragma endregion

#pragma endregion

#pragma region "Encryption"

	#pragma region "Authenticate"

		bool Communication::authenticate(const std::string& user, const std::string& pass) {
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

				std::string Communication::process(const std::string& data) {
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

				std::string Communication::toHex(const std::string& data) {
					std::stringstream ss;
					ss << std::hex << std::setfill('0');
					for (unsigned char c : data) ss << std::setw(2) << static_cast<int>(c);

					return (ss.str());
				}

			#pragma endregion

			#pragma region "From Hex"

				std::string Communication::fromHex(const std::string& hexData) {
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

			std::string Communication::encrypt(const std::string& plaintext)  {
				return (toHex(process(plaintext)));
			}

		#pragma endregion

		#pragma region "Decrypt"

			std::string Communication::decrypt(const std::string& ciphertext) {
				return (process(fromHex(ciphertext)));
			}

		#pragma endregion

	#pragma endregion

#pragma endregion
