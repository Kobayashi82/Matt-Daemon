/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logging.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:28:17 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/13 14:57:02 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <iostream>
	#include <fstream>
	#include <cstdint>
	#include <mutex>

#pragma endregion

#pragma region "Enumarators"

	enum e_level { DEBUG, INFO, LOG, WARNING, ERROR, CRITICAL };

#pragma endregion

#pragma region "Tintin Reporter"

	class Tintin_reporter {

		public:

			//	Constructors
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
			void			critical(const std::string& msg);

		private:

			//	Variables
			std::string		_logPath;
    		std::ofstream	_logFile;
			uint8_t			_logLevel;
			std::mutex		_mutex;

			// Methods
			static void createDirectory(const std::string& filePath);
			static std::string getTimestamp();
	};

#pragma endregion

#pragma region "Variables"

	extern Tintin_reporter	*Log;

#pragma endregion
