#include <iostream>
#include <string>
#include <array>              // For std::array��V2578 fix)
#include <winsock2.h>
#include <ws2tcpip.h>
#include "common.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    bool errorOccurred = false;

    // Initialize WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[Client] WSAStartup failed!" << endl;
        return 1;
    }

    // Create client socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "[Client] Failed to create socket." << endl;
        WSACleanup();
        return 1;
    }

    // Configure server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);

    // Convert IP string to binary format
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        cerr << "[Client] Invalid address or inet_pton failed." << endl;
        errorOccurred = true;
    }
    else if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != 0) {
        cerr << "[Client] Failed to connect to server." << endl;
        errorOccurred = true;
    }
    else {
        cout << "[Client] Connected to server. Waiting for data..." << endl;

        // Receive flight schedule from server
        std::array<char, 2048> buffer{};      // safer than raw array
        int bytes = recv(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
        if (bytes == SOCKET_ERROR) {
            cerr << "[Client] Socket error while receiving data." << endl;
            errorOccurred = true;
        }
        else if (bytes == 0) {
            cerr << "[Client] Connection closed by server." << endl;
        }
        else {
            cout << "[Client] Received flight schedule:\n";
            cout << string(buffer.data(), bytes) << endl;
        }
    }

    cout << "[Client] Press Enter to exit...";
    cin.ignore();
    cin.get();

    closesocket(clientSocket);
    WSACleanup();

    return errorOccurred ? 1 : 0;
}