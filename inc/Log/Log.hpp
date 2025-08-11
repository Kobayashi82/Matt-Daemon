/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:28:17 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 00:56:52 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <fstream>
	#include <cstdint>

#pragma endregion

#pragma region "Tintin_reporter"

	class Tintin_reporter {

		public:

			//	Enumarators
			enum e_level { DEBUG, INFO, LOG, WARNING, ERROR, FATAL };

			//	Constructors
			Tintin_reporter();
			Tintin_reporter(uint8_t logLevel);
			Tintin_reporter(const std::string& logPath);
			Tintin_reporter(const std::string& logPath, uint8_t logLevel);
			Tintin_reporter(const Tintin_reporter& src);
			~Tintin_reporter();

			//	Overloads
			Tintin_reporter&	operator=(const Tintin_reporter& rhs);
			bool				operator==(const Tintin_reporter& rhs) const;

			// Methods
			void			open();
			void			close();
			void			clear();

			void			set_logLevel(uint8_t logLevel);
			void			set_logPath(const std::string& logPath);
			uint8_t			get_logLevel() const;
			std::string		get_logPath() const;

			void			debug(const std::string& msg);
    		void			info(const std::string& msg);
			void			log(const std::string& msg);
			void			warning(const std::string& msg);
			void			error(const std::string& msg);
			void			fatal(const std::string& msg);

		private:

			//	Variables
			std::string		_logPath;
    		std::ofstream	_logFile;
			uint8_t			_logLevel;
	};

#pragma endregion
