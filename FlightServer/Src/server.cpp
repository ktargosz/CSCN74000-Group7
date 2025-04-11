#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "common.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Namespace for shared project-related functions and structures
namespace flight {
    std::string loadFlightSchedule(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not open " << filename << std::endl;
            return "ERROR: Cannot open flight file.\n";
        }

        std::string line, content;
        while (std::getline(file, line)) {
            content += line + "\n";
        }

        std::cout << "[Server] Loaded flight data:\n" << content << std::endl;
        return content;
    }

    bool sendFile(SOCKET socket, const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[Server] Could not open " << filename << std::endl;
            return false;
        }

        file.seekg(0, std::ios::end);
        int64_t size = static_cast<int64_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        std::cout << "[Server] Sending file of size: " << size << " bytes\n";
        send(socket, reinterpret_cast<const char*>(&size), sizeof(size), 0);

        char buffer[4096];
        while (!file.eof()) {
            file.read(buffer, sizeof(buffer));
            int bytesRead = static_cast<int>(file.gcount());
            send(socket, buffer, bytesRead, 0);
        }

        file.close();
        return true;
    }
}

int main() {
    WSADATA wsaData;

    // Initialize WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[Server] WSAStartup failed!" << std::endl;
        return 1;
    }

    // Create TCP socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "[Server] Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Allow reuse of the address (for faster restarts)
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Configure server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != 0) {
        std::cerr << "[Server] Bind failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Start listening
    if (listen(serverSocket, SOMAXCONN) != 0) {
        std::cerr << "[Server] Listen failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "[Server] Server started. Listening on port 12345...\n";

    // Accept and handle clients in a loop
    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "[Server] Accept failed!" << std::endl;
            continue;
        }

        std::cout << "[Server] Client connected.\n";

        // Loop to receive commands from this client
        while (true) {
            char commandBuffer[256] = {};
            int cmdLen = recv(clientSocket, commandBuffer, sizeof(commandBuffer), 0);
            if (cmdLen <= 0) {
                std::cerr << "[Server] Connection lost or client closed socket.\n";
                break;
            }

            std::string command(commandBuffer, cmdLen);
            std::cout << "[Server] Command received: " << command << std::endl;

            if (command == "GET_SCHEDULE") {
                std::string flights = flight::loadFlightSchedule("flights.txt");
                send(clientSocket, flights.c_str(), static_cast<int>(flights.size()), 0);
            }
            else if (command == "GET_BIG_FILE") {
                flight::sendFile(clientSocket, "telemetry_log.txt");
            }
            else if (command == "QUIT") {
                std::cout << "[Server] Client requested disconnect.\n";
                break;
            }
            else {
                std::cerr << "[Server] Unknown command received.\n";
            }
        }

        closesocket(clientSocket);
        std::cout << "[Server] Client disconnected.\n\n";
    }

    // Never reached in current version
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

