/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:28:53 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 00:53:22 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "Log/Log.hpp"

	#include <cstring>
	#include <sys/stat.h>
	#include <ctime>

#pragma endregion

#pragma region "Log"

	#pragma region "Constructors"

		#pragma region "Default"

			Log::Log() : _logPath("/var/log/matt_daemon/matt_daemon.log"), _logLevel(LOG) {
				mkdir(_logPath.c_str(), 0755);

				_logFile.open("/var/log/matt_daemon", std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;	// Auto-Flush
			}

		#pragma endregion

		#pragma region "Path"

			Log::Log(const std::string& logPath) : _logPath(logPath), _logLevel(LOG) {
				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Level"

			Log::Log(uint8_t logLevel) : _logPath("/var/log/matt_daemon/matt_daemon.log"), _logLevel(logLevel) {
				mkdir(_logPath.c_str(), 0755);

				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Path & Level"

			Log::Log(const std::string& logPath, uint8_t logLevel) : _logPath(logPath), _logLevel(logLevel) {
				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Copy"

			Log::Log(const Log& src) : _logPath(src._logPath), _logLevel(src._logLevel) {
				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Destructror"

			Log::~Log() {
				if (_logFile.is_open()) _logFile.close();
			}

		#pragma endregion

	#pragma endregion

	#pragma region "Overloads"

		#pragma region "Copy"

			Log& Log::operator=(const Log& rhs) {
				if (this != &rhs) {
					if (_logFile.is_open()) _logFile.close();

					_logPath = rhs._logPath;
					_logLevel = rhs._logLevel;
					_logFile.open(_logPath, std::ios::app);
					if (!_logFile.is_open()) {
						std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
						throw std::runtime_error(errorMsg);
					}

					_logFile << std::unitbuf;
				}

				return (*this);
			}

		#pragma endregion

		#pragma region "Equality"

			bool Log::operator==(const Log& rhs) const {
				return (_logPath == rhs._logPath && _logLevel == rhs._logLevel);
			}

		#pragma endregion

	#pragma endregion

	#pragma region "File Operations"

		#pragma region "Open"

			void Log::open() {
				if (_logFile.is_open()) _logFile.close();

				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Close"

			void Log::close() {
				if (_logFile.is_open()) _logFile.close();
			}

		#pragma endregion

		#pragma region "Clear"

			void Log::clear() {
				if (_logFile.is_open()) _logFile.close();

				_logFile.open(_logPath, std::ios::trunc);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

	#pragma endregion

	#pragma region "Setter & Getters"
	
		void Log::set_logLevel(uint8_t logLevel) { _logLevel = logLevel; }
		void Log::set_logPath(std::string& logPath) { _logPath = logPath; open(); }

		uint8_t Log::get_logLevel() const { return (_logLevel); }
		std::string Log::get_logPath() const { return (_logPath); }

	#pragma endregion

	#pragma region "Logging"

		#pragma region "Get Time Stamp"

			static std::string getTimestamp() {
				time_t		now = time(0);
				struct tm*	timeinfo = localtime(&now);

				char buffer[20];
				strftime(buffer, sizeof(buffer), "%d/%m/%Y-%H:%M:%S", timeinfo);

				return (std::string(buffer));
			}

		#pragma endregion

		#pragma region "Log by Level"

			void Log::debug(const std::string& msg) {
				if (_logLevel > DEBUG) return;
				_logFile << "[" << getTimestamp() << "]   [ DEBUG ] - " << msg << std::endl;
			}

			void Log::info(const std::string& msg) {
				if (_logLevel > INFO) return;
				_logFile << "[" << getTimestamp() << "]    [ INFO ] - " << msg << std::endl;
			}

			void Log::log(const std::string& msg) {
				if (_logLevel > LOG) return;
				_logFile << "[" << getTimestamp() << "]     [ LOG ] - " << msg << std::endl;
			}

			void Log::warning(const std::string& msg) {
				if (_logLevel > WARNING) return;
				_logFile << "[" << getTimestamp() << "] [ WARNING ] - " << msg << std::endl;
			}

			void Log::error(const std::string& msg) {
				if (_logLevel > ERROR) return;
				_logFile << "[" << getTimestamp() << "]   [ ERROR ] - " << msg << std::endl;
			}

			void Log::fatal(const std::string& msg) {
				if (_logLevel > FATAL) return;
				_logFile << "[" << getTimestamp() << "]   [ FATAL ] - " << msg << std::endl;
			}

		#pragma endregion

	#pragma endregion

#pragma endregion
