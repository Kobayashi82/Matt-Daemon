/*
 * NetworkManager.h
 * Handles network connections, socket management, and data transmission.
 * Provides interface for connecting to server and sending/receiving messages.
 */

#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <glibmm/dispatcher.h>

class NetworkManager {
public:
    NetworkManager();
    virtual ~NetworkManager();

    // Connection management
    bool connectToServer(const std::string& ip, int port, const std::string& user);
    void disconnectFromServer();
    bool sendMessage(const std::string& message);

    // Connection status
    bool isConnected() const { return _sockfd > 0; }
    int getSocketFd() const { return _sockfd; }

    // Data reception
    void startReceiving();
    void stopReceiving();

    // Dispatcher for GUI communication
    Glib::Dispatcher& getDispatcher() { return _dispatcher; }
    Glib::Dispatcher& getDisconnectDispatcher() { return _disconnectDispatcher; }
    std::string getPendingData();

private:
    void receiveDataLoop();

    int _sockfd;
    std::thread _recvThread;
    std::atomic<bool> _running;
    Glib::Dispatcher _dispatcher;
    Glib::Dispatcher _disconnectDispatcher;
    std::mutex _dataMutex;
    std::string _pendingData;
};
