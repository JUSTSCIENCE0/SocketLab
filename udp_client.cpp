#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

int main()
{
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0)
    {
        printf("WSAStartup failed with error: %d\n", res);
        return -1;
    }

    sockaddr_in server, addr;
    server.sin_family = AF_INET;
    server.sin_port = htons(50002);
    server.sin_addr.s_addr = inet_addr("192.168.1.97");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(50003);
    addr.sin_addr.s_addr = inet_addr("192.168.1.8");

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (bind(sock, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        printf("Bind error\n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    while (true)
    {

        std::string message;
        std::cin >> message;
        message += "\n";

        int res = sendto(sock, message.c_str(), message.size(), 0, (sockaddr*)&server, sizeof(server));
        if (res != message.size())
        {
            printf("Send error\n");
            closesocket(sock);
            WSACleanup();
            return -1;
        }
    }
}
