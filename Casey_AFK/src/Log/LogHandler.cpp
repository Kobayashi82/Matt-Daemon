/*
 * LogHandler.cpp
 * Handles log processing and formatting for display in GUI.
 * Processes incoming log data and formats it for presentation.
 */

#include "Log/LogHandler.hpp"
#include <iostream>

LogHandler::LogHandler() {
}

LogHandler::~LogHandler() {
}

std::string LogHandler::processLogs(const std::string& rawLogs) {
    std::string formatted;
    size_t start = 0, end;

    while ((end = rawLogs.find('\n', start)) != std::string::npos) {
        std::string line = rawLogs.substr(start, end - start);
        if (!line.empty()) {
            formatted += formatLogLine(line) + "\n";
        }
        start = end + 1;
    }

    // Handle remaining text without newline
    if (start < rawLogs.size()) {
        formatted += formatLogLine(rawLogs.substr(start));
    }

    return formatted;
}

std::string LogHandler::formatLogLine(const std::string& line) {
    // Server messages without prefix
    return line;
}

void LogHandler::appendToDisplay(const std::string& text) {
    _displayText += text;
}

std::string LogHandler::getDisplayText() const {
    return _displayText;
}

void LogHandler::clearDisplay() {
    _displayText.clear();
}
