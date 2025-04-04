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

    // Reads the contents of the given file and returns it as a string
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
    bool errorOccurred = false;

    // Initialize WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[Server] WSAStartup failed!" << endl;
        return 1;
    }

    // Create TCP socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "[Server] Failed to create socket." << endl;
        WSACleanup();
        return 1;
    }

    // Configure server address (localhost:12345)
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to local address
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != 0) {
        cerr << "[Server] Bind failed!" << endl;
        errorOccurred = true;
    }
    // Start listening for incoming client connections
    else if (listen(serverSocket, 1) != 0) {
        cerr << "[Server] Listen failed!" << endl;
        errorOccurred = true;
    }
    else {
        cout << "[Server] Server started. Waiting for client..." << endl;

        // Accept client connection
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "[Server] Accept failed!" << endl;
            errorOccurred = true;
        }
        else {
            cout << "[Server] Client connected." << endl;

            // Load schedule and send to client
            std::string flights = flight::loadFlightSchedule("flights.txt");
            int sent = send(clientSocket, flights.c_str(), static_cast<int>(flights.size()), 0);
            if (sent == SOCKET_ERROR) {
                cerr << "[Server] Send failed!" << endl;
                errorOccurred = true;
            }
            else {
                cout << "[Server] Sent flight schedule to client." << endl;
            }

            // Close client socket
            closesocket(clientSocket);
        }
    }

    // Close server socket and cleanup WinSock
    closesocket(serverSocket);
    WSACleanup();

    // Return based on error flag
    return errorOccurred ? 1 : 0;
}