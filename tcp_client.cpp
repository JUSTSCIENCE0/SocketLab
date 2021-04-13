#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

int main()
{
    WSADATA wsaData;
    char messages[2048] = "";

    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0)
    {
        printf("WSAStartup failed with error: %d\n", res);
        return -1;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(50001);
    server.sin_addr.s_addr = inet_addr("192.168.1.97");

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    res = connect(sock, (sockaddr*)&server, sizeof(server));
    if (res == SOCKET_ERROR) {
        closesocket(sock);
        return 1;
    }

    while (true)
    {
        std::string message;
        std::cin >> message;

        if (!strcmp(message.c_str(), "recv_mes"))
        {
            send(sock, message.c_str(), message.size(), 0);
            int size = recv(sock, messages, 2048, 0);
            printf("%s\n", messages);
        }
        else
        {
            message += "\n";
            send(sock, message.c_str(), message.size(), 0);
        }
    }
}