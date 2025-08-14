/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Communication.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:46:18 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/14 21:12:37 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Communication"

	class Client;
	class Communication {

		public:

			//	Methods
			static int	read_client(Client *client);							// 
			static void	write_client(Client *client);							// 

			static int	read_shell(Client *client);								// 
			static void	write_shell(Client *client);							// 

			static bool	authenticate(const std::string& user, const std::string& pass);
			static int	get_userpass(const std::string msg, std::string & user, std::string & pass);

			static std::string encrypt(const std::string& plaintext);			// 
			static std::string decrypt(const std::string& ciphertext);			// 

		private:
	
			//	Variables
			static const size_t			CHUNK_SIZE;								// 
			static const std::string	KEY;									// 

			// Constructors
			Communication() {}													// Default constructor (no instantiable)
			~Communication() {}													// Destructor (no instantiable)

			// Methods
			static std::string	process(const std::string& data);				// 
			static std::string	toHex(const std::string& data);					// 
			static std::string	fromHex(const std::string& hexData);			// 
	};

#pragma endregion
