#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "common.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

string loadFlightSchedule(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR: Could not open " << filename << endl;
        return "ERROR: Cannot open flight file.\n";
    }

    string line, content;
    while (getline(file, line)) {
        content += line + "\n";
    }

    cout << "[Server] Loaded flight data:\n" << content << endl;
    return content;
}

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

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    cout << "[Server] Server started. Waiting for client..." << endl;

    sockaddr_in clientAddr{};
    int clientLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
    cout << "[Server] Client connected." << endl;

    string flights = loadFlightSchedule("flights.txt");
    send(clientSocket, flights.c_str(), static_cast<int>(flights.size()), 0);
    cout << "[Server] Sent flight schedule to client." << endl;

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}