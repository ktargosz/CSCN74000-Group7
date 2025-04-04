#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "common.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

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

}  // namespace flight

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[Server] WSAStartup failed!" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        cerr << "[Server] Bind failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) != 0) {
        cerr << "[Server] Listen failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "[Server] Server started. Waiting for client..." << endl;

    sockaddr_in clientAddr{};
    int clientLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "[Server] Accept failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "[Server] Client connected." << endl;

    // Read schedule and send to client
    std::string flights = flight::loadFlightSchedule("flights.txt");
    int sent = send(clientSocket, flights.c_str(), static_cast<int>(flights.size()), 0);
    if (sent == SOCKET_ERROR) {
        cerr << "[Server] Send failed!" << endl;
    }

    cout << "[Server] Sent flight schedule to client." << endl;

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}