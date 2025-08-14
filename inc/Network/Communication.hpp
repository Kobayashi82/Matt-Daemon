/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Communication.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:46:18 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 22:53:23 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Communication"

	class Client;
	class Communication {

		public:

			//	Methods
			static int	read_client(Client *client);							// Reads data from the client socket
			static void	write_client(Client *client);							// Writes data to the client socket

			static int	read_shell(Client *client);								// Reads data from the shell associated with the client
			static void	write_shell(Client *client);							// Writes data to the shell associated with the client

			static bool	authenticate(const std::string& user, const std::string& pass);					// Authenticates a user with username and password
			static int	get_userpass(const std::string msg, std::string & user, std::string & pass);	// Extracts username and password from a message

			static std::string encrypt(const std::string& plaintext);			// Encrypts a plaintext string
			static std::string decrypt(const std::string& ciphertext);			// Decrypts a ciphertext string

		private:
	
			//	Variables
			static const size_t			CHUNK_SIZE;								// Size of the buffer for read/recv and write/send operations
			static const std::string	KEY;									// Encryption key

			// Constructors
			Communication() {}													// Default constructor (no instantiable)
			~Communication() {}													// Destructor (no instantiable)

			// Methods
			static std::string	process(const std::string& data);				// 
			static std::string	toHex(const std::string& data);					// Converts binary data to hexadecimal string
			static std::string	fromHex(const std::string& hexData);			// Converts hexadecimal string back to binary
	};

#pragma endregion
