#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "common.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[Client] WSAStartup failed!" << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        cerr << "[Client] Failed to connect to server." << endl;
        return 1;
    }

    cout << "[Client] Connected to server. Waiting for data..." << endl;

    char buffer[2048] = {};
    int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytes > 0) {
        cout << "[Client] Received flight schedule:\n";
        cout << string(buffer, bytes) << endl;
    }
    else {
        cerr << "[Client] No data received or connection error." << endl;
    }

    cout << "[Client] Press Enter to exit...";
    cin.ignore();
    cin.get();

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}