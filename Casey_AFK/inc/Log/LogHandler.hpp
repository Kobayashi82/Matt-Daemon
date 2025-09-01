/*
 * LogHandler.h
 * Handles log processing and formatting for display in GUI.
 * Processes incoming log data and formats it for presentation.
 */

#pragma once
#include <string>
#include <glibmm/dispatcher.h>

class LogHandler {
public:
    LogHandler();
    virtual ~LogHandler();

    // Log processing
    std::string processLogs(const std::string& rawLogs);
    std::string formatLogLine(const std::string& line);

    // Log display
    void appendToDisplay(const std::string& text);
    std::string getDisplayText() const;
    void clearDisplay();  // Nuevo método para limpiar los logs

    // User message handling
    std::string formatUserMessage(const std::string& message);

private:
    std::string _displayText;
};
