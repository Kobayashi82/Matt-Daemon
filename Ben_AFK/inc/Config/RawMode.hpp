/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawMode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 11:25:45 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/18 15:36:58 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Methods"

	int			raw_mode_enable(bool passmode);
	void		raw_mode_disable(bool passmode);
	std::string	get_terminal_size();

#pragma endregion
