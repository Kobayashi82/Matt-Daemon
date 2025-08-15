/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 21:41:14 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/15 23:50:33 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

	#include <string>

std::string getUserPassword();
std::string getPassword();

std::string encrypt(const std::string& plaintext);
std::string decrypt(const std::string& ciphertext);

int socket_create();
int send_data(const std::string & data);
int main_loop();
