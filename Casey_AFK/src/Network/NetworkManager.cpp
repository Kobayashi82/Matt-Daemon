/*
 * NetworkManager.cpp
 * Handles network connections, socket management, and data transmission.
 * Provides interface for connecting to server and sending/receiving messages.
 */

#include "Network/NetworkManager.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <errno.h>

NetworkManager::NetworkManager() :
    _sockfd(-1),
    _running(false)
{
}

NetworkManager::~NetworkManager() {
    disconnectFromServer();
}

bool NetworkManager::connectToServer(const std::string& ip, int port, const std::string& user, int numLogs) {
    // Create socket
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        ::close(_sockfd);
        _sockfd = -1;
        return false;
    }

    // Connect to server
    if (connect(_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        ::close(_sockfd);
        _sockfd = -1;
        return false;
    }

    // Send initial message
    std::string msg = "/CASEY user=" + user + " " + std::to_string(numLogs) + "\n";
    
    if (send(_sockfd, msg.c_str(), msg.size(), 0) < 0) {
        std::cerr << "Failed to send initial message" << std::endl;
        ::close(_sockfd);
        _sockfd = -1;
        return false;
    }

    return true;
}

void NetworkManager::disconnectFromServer() {
    // First shutdown the socket to unblock recv() in the receiving thread
    if (_sockfd > 0) {
        shutdown(_sockfd, SHUT_RDWR);
    }
    // Then stop receiving thread
    stopReceiving();
    // Finally close the socket
    if (_sockfd > 0) {
        ::close(_sockfd);
        _sockfd = -1;
    }
}

bool NetworkManager::sendMessage(const std::string& message) {
    if (_sockfd <= 0) return false;

    std::string msg = message + "\n";
    if (send(_sockfd, msg.c_str(), msg.size(), 0) < 0) {
        std::cerr << "Failed to send message" << std::endl;
        return false;
    }
    return true;
}

void NetworkManager::startReceiving() {
    if (_running || _sockfd <= 0) return;

    _running = true;
    _recvThread = std::thread([this]() {
        receiveDataLoop();
    });
}

void NetworkManager::stopReceiving() {
    _running = false;
    if (_recvThread.joinable()) {
        _recvThread.join();
    }
}

void NetworkManager::receiveDataLoop() {
    char buffer[1024];
    while (_running) {
        ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            {
                std::lock_guard<std::mutex> lock(_dataMutex);
                _pendingData += buffer;
            }
            _dispatcher.emit();
        } else if (n == 0) {
            _running = false;
            _disconnectDispatcher.emit();
            break;
        } else {
            std::cerr << "Receive error: " << strerror(errno) << std::endl;
            _running = false;
            _disconnectDispatcher.emit();
            break;
        }
    }
}

std::string NetworkManager::getPendingData() {
    std::lock_guard<std::mutex> lock(_dataMutex);
    std::string data;
    data.swap(_pendingData);
    return data;
}
