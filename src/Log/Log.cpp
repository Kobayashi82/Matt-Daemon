/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:28:53 by vzurera-          #+#    #+#             */
/*   Updated: 2025/08/12 00:58:06 by vzurera-         ###   ########.fr       */
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

			Tintin_reporter::Tintin_reporter() : _logPath("/var/log/matt_daemon/matt_daemon.log"), _logLevel(LOG) {
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

			Tintin_reporter::Tintin_reporter(const std::string& logPath) : _logPath(logPath), _logLevel(LOG) {
				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Level"

			Tintin_reporter::Tintin_reporter(uint8_t logLevel) : _logPath("/var/log/matt_daemon/matt_daemon.log"), _logLevel(logLevel) {
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

			Tintin_reporter::Tintin_reporter(const std::string& logPath, uint8_t logLevel) : _logPath(logPath), _logLevel(logLevel) {
				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Copy"

			Tintin_reporter::Tintin_reporter(const Tintin_reporter& src) : _logPath(src._logPath), _logLevel(src._logLevel) {
				_logFile.open(_logPath, std::ios::app);
				if (!_logFile.is_open()) {
					std::string errorMsg = "Cannot open log file: " + _logPath + " - " + strerror(errno);
					throw std::runtime_error(errorMsg);
				}

				_logFile << std::unitbuf;
			}

		#pragma endregion

		#pragma region "Destructror"

			Tintin_reporter::~Tintin_reporter() {
				if (_logFile.is_open()) _logFile.close();
			}

		#pragma endregion

	#pragma endregion

	#pragma region "Overloads"

		#pragma region "Copy"

			Tintin_reporter& Tintin_reporter::operator=(const Tintin_reporter& rhs) {
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

			bool Tintin_reporter::operator==(const Tintin_reporter& rhs) const {
				return (_logPath == rhs._logPath && _logLevel == rhs._logLevel);
			}

		#pragma endregion

	#pragma endregion

	#pragma region "File Operations"

		#pragma region "Open"

			void Tintin_reporter::open() {
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

			void Tintin_reporter::close() {
				if (_logFile.is_open()) _logFile.close();
			}

		#pragma endregion

		#pragma region "Clear"

			void Tintin_reporter::clear() {
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
	
		void Tintin_reporter::set_logLevel(uint8_t logLevel) { _logLevel = logLevel; }
		void Tintin_reporter::set_logPath(const std::string& logPath) { _logPath = logPath; open(); }

		uint8_t Tintin_reporter::get_logLevel() const { return (_logLevel); }
		std::string Tintin_reporter::get_logPath() const { return (_logPath); }

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

			void Tintin_reporter::debug(const std::string& msg) {
				if (_logLevel > DEBUG) return;
				_logFile << "[" << getTimestamp() << "]   [ DEBUG ] - " << msg << std::endl;
			}

			void Tintin_reporter::info(const std::string& msg) {
				if (_logLevel > INFO) return;
				_logFile << "[" << getTimestamp() << "]    [ INFO ] - " << msg << std::endl;
			}

			void Tintin_reporter::log(const std::string& msg) {
				if (_logLevel > LOG) return;
				_logFile << "[" << getTimestamp() << "]     [ LOG ] - " << msg << std::endl;
			}

			void Tintin_reporter::warning(const std::string& msg) {
				if (_logLevel > WARNING) return;
				_logFile << "[" << getTimestamp() << "] [ WARNING ] - " << msg << std::endl;
			}

			void Tintin_reporter::error(const std::string& msg) {
				if (_logLevel > ERROR) return;
				_logFile << "[" << getTimestamp() << "]   [ ERROR ] - " << msg << std::endl;
			}

			void Tintin_reporter::fatal(const std::string& msg) {
				if (_logLevel > FATAL) return;
				_logFile << "[" << getTimestamp() << "]   [ FATAL ] - " << msg << std::endl;
			}

		#pragma endregion

	#pragma endregion

#pragma endregion
