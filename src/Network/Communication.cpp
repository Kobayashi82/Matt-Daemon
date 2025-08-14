/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Communication.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:46:19 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 14:33:51 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Options.hpp"
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

	void get_userpass(const std::string msg, std::string & user, std::string & pass) {	
		size_t userPos = msg.find("user=");
		size_t passPos = msg.find("pass=");

		if (userPos != std::string::npos && passPos != std::string::npos) {
			size_t userStart = userPos + 5;
			size_t userEnd   = passPos - 1;
			user = msg.substr(userStart, userEnd - userStart);		
			size_t passStart = passPos + 5;
			pass = msg.substr(passStart);
		}
	}

		#pragma region "Read"

			int Communication::read_client(Client *client) {
				if (!client || client->fd < 0 || client->diying) return (0);

				char buffer[CHUNK_SIZE];	std::memset(buffer, 0, sizeof(buffer));
				ssize_t bytes_read = recv(client->fd, buffer, CHUNK_SIZE, 0);

				// Read some data
				if (bytes_read > 0) {
					client->update_last_activity();

					if (client->type == MSG) {
						std::string msg = std::string(buffer, buffer + bytes_read);
						if (!msg.empty() && msg.back() == '\n') msg.pop_back();
						if (msg == "quit") {
							Epoll::Running = false;
						} if (msg == "/CLIENT_SHELL_AUTH") {
							std::string response;
							if (Options::disabledShell) {
								response = "/SHELL_DISABLED";
								client->diying = true;
							} else {
								response = "/AUTHORIZE ENCRYPTION=" + std::string(!Options::disabledEncryption ? "true" : "false");
								client->type = CLIENT;
							}
							client->write_buffer.insert(client->write_buffer.end(), response.begin(), response.end());
						} else {
							Log->log(msg);
						}
					} else if (client->type == CLIENT) {
						std::string msg = std::string(buffer, buffer + bytes_read);
						if (!msg.empty() && msg.back() == '\n') msg.pop_back(); // temp
						try {
							if (!Options::disabledEncryption) msg = decrypt(msg);
						} catch (const std::exception& e) {
							Log->critical("Message not encrypted");
							client->remove(); return (1);
						}
						if (msg.substr(0, 14) == "/AUTHORIZATION") {
							Log->log("AUTHORIZATION received");
							std::string user, pass, response;
							get_userpass(msg.substr(15), user, pass);
							Log->info(user + " - " + pass);
							if (authenticate(user, pass)) {
								response = "/AUTHORIZATION OK";
							} else {
								response = "/AUTHORIZATION FAIL";
							}
							if (!Options::disabledEncryption) response = encrypt(response);
							client->write_buffer.insert(client->write_buffer.end(), response.begin(), response.end());
						} else {
							client->write_sh_buffer.insert(client->write_sh_buffer.end(), msg.begin(), msg.end());
						}
					}
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

					if (client->diying && client->write_buffer.empty()) { client->remove();	return; }
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
