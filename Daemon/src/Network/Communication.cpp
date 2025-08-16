/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Communication.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:46:19 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/16 16:43:10 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Main/Options.hpp"
	#include "Main/Logging.hpp"
	#include "Main/Shell.hpp"
	#include "Network/Socket.hpp"
	#include "Network/Client.hpp"
	#include "Network/Epoll.hpp"
	#include "Network/Communication.hpp"

	#include <cstring>															// std::memset(), std::strcmp()
	#include <unistd.h>															// read() and write(), crypt()
	#include <sys/socket.h>														// recv(), send()
	#include <sys/ioctl.h>														// ioctl()
	#include <termios.h>														// struct winsize
	#include <iomanip>															// std::stringstream, std::setfill(), std::setw()
	#include <shadow.h>															// getspnam()

#pragma endregion

#pragma region "Variables"

	const size_t Communication::CHUNK_SIZE	= 4096;								// Size of the buffer for read/recv and write/send operations
	const std::string Communication::KEY	= "Th1s_1s_n0t_4_s3cr3t_k3y";		// Encryption key

#pragma endregion

#pragma region "Communications"

	#pragma region "CLIENT"

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
							Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] wants to close the daemon");
							Epoll::Running = false;
						} else if (msg == "/CLIENT_SHELL_AUTH") {
							Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] wants to open a shell");
							std::string response;
							if (Options::disabledShell) {
								response = "/SHELL_DISABLED";
								Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] shell is disabled, so reject connection");
								client->diying = true;
							} else {
								response = "/AUTHORIZE ENCRYPTION=" + std::string(!Options::disabledEncryption ? "true" : "false");
								client->authenticated = false;
								client->type = CLIENT;
							}
							client->write_buffer.insert(client->write_buffer.end(), response.begin(), response.end());
							Epoll::set(client->fd, true, true);
						} else {
							Log->log(msg);
						}
					} else if (client->type == CLIENT) {
						std::string msg = std::string(buffer, buffer + bytes_read);
						try {
							if (!Options::disabledEncryption) msg = decrypt(msg);
						} catch (const std::exception& e) {
							Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] message not encrypted, so reject connection");
							client->schedule_removal(); 
							return (1);
						}
						if (!client->shell_running && msg.substr(0, 14) == "/AUTHORIZATION") {
							std::string user, pass, response;

							if (get_userpass(msg.substr(15), user, pass)) {
								Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] authorization failed - invalid format");
								response = "/AUTHORIZATION_FAIL";
								if (!Options::disabledEncryption) response = encrypt(response);
								client->write_buffer.insert(client->write_buffer.end(), response.begin(), response.end());
								Epoll::set(client->fd, true, true);
								return (0);
							}
							Log->info("Authentication attempt - User: " + user);
							if (authenticate(user, pass)) {
								response = "/AUTHORIZATION_OK";
								Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] authorization successful");
								client->authenticated = true;
								client->user = user;
								client->pass = pass;
							} else {
								response = "/AUTHORIZATION_FAIL";
								Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] authorization failed");
							}
							if (!Options::disabledEncryption) response = encrypt(response);
							client->write_buffer.insert(client->write_buffer.end(), response.begin(), response.end());
							Epoll::set(client->fd, true, true);
						} else if (msg.substr(0, 14) == "/TERMINAL_SIZE") {
							std::string size_info = msg.substr(15);
							size_t x_pos = size_info.find('x');
							if (x_pos != std::string::npos) {
								try {
									client->terminal_cols = std::stoi(size_info.substr(0, x_pos));
									client->terminal_rows = std::stoi(size_info.substr(x_pos + 1));
									Log->info("Client [" + client->ip + ":" + std::to_string(client->port) + "] terminal size: " + std::to_string(client->terminal_cols) + "x" + std::to_string(client->terminal_rows));
									
									// If authenticated, shell not running and client not dying, start shell
									if (client->type == CLIENT && client->authenticated && !client->shell_running && !client->diying) {
										if (shell_start(client)) {
											Log->debug("Client [" + client->ip + ":" + std::to_string(client->port) + "] shell failed");
											std::string response = "/SHELL_FAIL";
											if (!Options::disabledEncryption) response = encrypt(response);
											client->write_buffer.insert(client->write_buffer.end(), response.begin(), response.end());
											client->diying = true;
											Epoll::set(client->fd, true, true);
										}
									}
								} catch (const std::exception& e) {
									Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] invalid terminal size format");
								}
							}
						} else if (msg.substr(0, 16) == "/TERMINAL_RESIZE") {
							std::string	size_info = msg.substr(17);
							size_t		x_pos = size_info.find('x');

							if (x_pos != std::string::npos) {
								try {
									client->terminal_cols = std::stoi(size_info.substr(0, x_pos));
									client->terminal_rows = std::stoi(size_info.substr(x_pos + 1));
									
									if (client->shell_running && client->master_fd >= 0) {
										struct winsize ws;
										ws.ws_row = client->terminal_rows;
										ws.ws_col = client->terminal_cols;
										ws.ws_xpixel = 0;
										ws.ws_ypixel = 0;
										if (ioctl(client->master_fd, TIOCSWINSZ, &ws) == 0) {
											Log->info("Client [" + client->ip + ":" + std::to_string(client->port) + "] terminal resized to: " + std::to_string(client->terminal_cols) + "x" + std::to_string(client->terminal_rows));
										} else {
											Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] failed to resize PTY");
										}
									}
								} catch (const std::exception& e) {
									Log->warning("Client [" + client->ip + ":" + std::to_string(client->port) + "] invalid terminal resize format");
								}
							}
						} else {
							client->write_sh_buffer.insert(client->write_sh_buffer.end(), msg.begin(), msg.end());
							if (client->shell_running && client->master_fd >= 0) Epoll::set(client->master_fd, true, true);
						}
					}
				}
				// No data (usually means a client disconected)
				else if (bytes_read == 0) { client->schedule_removal(); return (1); }
				// Error reading
				else if (bytes_read == -1) { client->schedule_removal(); return (1); }

				return (0);
			}

		#pragma endregion

		#pragma region "Write"

			void Communication::write_client(Client *client) {
				if (!client || client->fd < 0) return;

				// There are data, send it
				if (!client->write_buffer.empty()) {
					client->update_last_activity();
				
					// No iniciar el shell automáticamente aquí
					// El shell se iniciará cuando se reciba /TERMINAL_SIZE

					size_t buffer_size = client->write_buffer.size();
					size_t chunk = std::min(buffer_size, static_cast<size_t>(CHUNK_SIZE));
					ssize_t bytes_written = send(client->fd, client->write_buffer.data(), chunk, MSG_DONTWAIT);

					// Sent some data
					if (bytes_written > 0) {
						client->write_buffer.erase(client->write_buffer.begin(), client->write_buffer.begin() + bytes_written);
						
						if (client->write_buffer.empty()) Epoll::set(client->fd, true, false);
					}

					if (client->diying && client->write_buffer.empty()) { 
						client->schedule_removal();
						return; 
					}

					// No writing
					else if (bytes_written == 0) { client->schedule_removal(); return; }
					// Error writing
					else if (bytes_written == -1) { client->schedule_removal(); return; }
				}
			}

		#pragma endregion

	#pragma endregion

	#pragma region "SHELL"

		#pragma region "Read"

			int Communication::read_shell(Client *client) {
				if (!client || client->master_fd < 0  || !client->shell_running) return (0);

				char buffer[CHUNK_SIZE];	std::memset(buffer, 0, sizeof(buffer));
				ssize_t bytes_read = read(client->master_fd, buffer, CHUNK_SIZE);

				// Read some data
				if (bytes_read > 0) {
					client->update_last_activity();
					std::string output(buffer, bytes_read);
					if (!Options::disabledEncryption) output = encrypt_with_index(output, client->encryption_index);				
					client->write_buffer.insert(client->write_buffer.end(), output.begin(), output.end());
					Epoll::set(client->fd, true, true);
				}

				// No data
				else if (bytes_read == 0) return (0);
				// Error reading
				else if (bytes_read == -1) return (0);

				return (0);
			}

		#pragma endregion

		#pragma region "Write"

			void Communication::write_shell(Client *client) {
				if (!client || client->master_fd < 0  || !client->shell_running) return;

				// There are data, send it
				if (!client->write_sh_buffer.empty()) {
					client->update_last_activity();

					size_t buffer_size = client->write_sh_buffer.size();
					size_t chunk = std::min(buffer_size, static_cast<size_t>(CHUNK_SIZE));

					std::string command(client->write_sh_buffer.begin(), client->write_sh_buffer.begin() + chunk);
					ssize_t bytes_written = write(client->master_fd, command.c_str(), command.length());

					// Sent some data
					if (bytes_written > 0) {
						size_t to_remove = std::min(static_cast<size_t>(bytes_written), chunk);
						client->write_sh_buffer.erase(client->write_sh_buffer.begin(), client->write_sh_buffer.begin() + to_remove);

						if (client->write_sh_buffer.empty()) Epoll::set(client->master_fd, true, false);
					}
					// No writing
					else if (bytes_written == 0) return;
					// Error writing
					else if (bytes_written == -1) return;
				}
			}

		#pragma endregion

	#pragma endregion

#pragma endregion

#pragma region "Authentication"

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

	#pragma region "Get User/Pass"

		int Communication::get_userpass(const std::string msg, std::string & user, std::string & pass) {	
			size_t userPos = msg.find("user=");
			size_t passPos = msg.find("pass=");

			if (userPos != std::string::npos && passPos != std::string::npos) {
				size_t userStart = userPos + 5;
				size_t userEnd   = passPos - 1;
				user = msg.substr(userStart, userEnd - userStart);		
				size_t passStart = passPos + 5;
				pass = msg.substr(passStart);

				return (0);
			}

			return (1);
		}

	#pragma endregion

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

	#pragma region "Encrypt with continuous index"

		std::string Communication::encrypt_with_index(const std::string& plaintext, size_t& index) {
			std::string result;
			result.reserve(plaintext.length());

			for (size_t i = 0; i < plaintext.length(); ++i) {
				char encrypted_char = plaintext[i] ^ KEY[index % KEY.length()];
				result.push_back(encrypted_char);
				index++;
			}

			return (toHex(result));
		}

	#pragma endregion

	#pragma region "Decrypt with continuous index"

		std::string Communication::decrypt_with_index(const std::string& ciphertext, size_t& index) {
			std::string binary_data = fromHex(ciphertext);
			std::string result;
			result.reserve(binary_data.length());

			for (size_t i = 0; i < binary_data.length(); ++i) {
				char decrypted_char = binary_data[i] ^ KEY[index % KEY.length()];
				result.push_back(decrypted_char);
				index++;
			}

			return (result);
		}

	#pragma endregion

#pragma endregion
