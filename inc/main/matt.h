/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matt.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:11:10 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/11 19:57:07 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "main/options.h"

#pragma endregion

#pragma region "Structures"

	typedef struct s_matt {
		bool			running;
		char			*fullname;
		t_options		options;
	}	t_matt;

#pragma endregion

#pragma region "Variables"

	extern t_matt g_matt;

#pragma endregion

#pragma region "Methods"



#pragma endregion
