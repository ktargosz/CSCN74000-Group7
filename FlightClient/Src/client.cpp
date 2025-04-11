#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "common.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Handles the file transfer from server to client
void receiveFile(SOCKET socket, const string& outFilename) {
    ofstream outFile(outFilename, ios::binary);
    if (!outFile.is_open()) {
        cerr << "[Client] Failed to open file for writing\n";
        return;
    }

    int64_t fileSize = 0;
    recv(socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
    cout << "[Client] Expecting file of size " << fileSize << " bytes...\n";

    char buffer[4096];
    int64_t received = 0;

    while (received < fileSize) {
        int bytes = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            cerr << "[Client] Error or connection closed\n";
            break;
        }
        outFile.write(buffer, bytes);
        received += bytes;
    }

    outFile.close();
    cout << "[Client] File received and saved as '" << outFilename << "'\n";
}

int main() {
    WSADATA wsaData;
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
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect to server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != 0) {
        cerr << "[Client] Failed to connect to server." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "[Client] Connected to server.\n";

    while (true) {
        // Prompt user for command
        string command;
        cout << "Enter command (GET_SCHEDULE / GET_BIG_FILE / QUIT): ";
        getline(cin, command);

        // Send the command
        send(clientSocket, command.c_str(), static_cast<int>(command.size()), 0);

        // Handle response
        if (command == "GET_SCHEDULE") {
            std::array<char, 2048> buffer{};
            int bytes = recv(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
            if (bytes > 0) {
                cout << "[Client] Received flight schedule:\n";
                cout << string(buffer.data(), bytes) << endl;
            }
            else {
                cerr << "[Client] Failed to receive schedule.\n";
            }
        }
        else if (command == "GET_BIG_FILE") {
            receiveFile(clientSocket, "received_file.txt");
        }
        else if (command == "QUIT") {
            cout << "[Client] Disconnecting from server.\n";
            break;
        }
        else {
            cerr << "[Client] Unknown command or no response.\n";
        }
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
