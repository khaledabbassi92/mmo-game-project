#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <cstdint>
#include <iostream>

#include "core.h"

#pragma comment(lib, "ws2_32.lib")

// ==================================================
// Winsock init / cleanup
// ==================================================

static bool winsockInitialized = false;

static bool InitWinsock()
{
    if (winsockInitialized)
        return true;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "[Net] WSAStartup failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    winsockInitialized = true;
    return true;
}

static void CleanupWinsock()
{
    if (winsockInitialized)
    {
        WSACleanup();
        winsockInitialized = false;
    }
}

// ==================================================
// TCP
// ==================================================

static SOCKET tcpSocket = INVALID_SOCKET;

bool TCP_Connect()
{
    if (!InitWinsock())
        return false;

    tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (tcpSocket == INVALID_SOCKET)
    {
        std::cerr << "[TCP] socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(4444);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(tcpSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cerr << "[TCP] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(tcpSocket);
        tcpSocket = INVALID_SOCKET;
        return false;
    }

    return true;
}

bool TCP_Send(const char* data, int size)
{
    if (tcpSocket == INVALID_SOCKET)
        return false;

    int sent = send(tcpSocket, data, size, 0);
    if (sent != size)
    {
        std::cerr << "[TCP] send() failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

int TCP_Receive(char* buffer, int size)
{
    if (tcpSocket == INVALID_SOCKET)
        return -1;

    return recv(tcpSocket, buffer, size, 0);
}

void TCP_Disconnect()
{
    if (tcpSocket != INVALID_SOCKET)
    {
        closesocket(tcpSocket);
        tcpSocket = INVALID_SOCKET;
    }
}

// ==================================================
// UDP
// ==================================================

static SOCKET udpSocket = INVALID_SOCKET;

bool UDP_Connect()
{
    if (!InitWinsock())
        return false;

    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (udpSocket == INVALID_SOCKET)
    {
        std::cerr << "[UDP] socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(4445);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(udpSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cerr << "[UDP] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
        return false;
    }

    return true;
}

bool UDP_Send(const char* data, int size)
{
    if (udpSocket == INVALID_SOCKET)
        return false;

    int sent = send(udpSocket, data, size, 0);
    if (sent != size)
    {
        // Don't spam every frame, only log occasionally if needed
        return false;
    }
    return true;
}

int UDP_Receive(char* buffer, int size)
{
    if (udpSocket == INVALID_SOCKET)
        return -1;

    return recv(udpSocket, buffer, size, 0);
}

void UDP_Disconnect()
{
    if (udpSocket != INVALID_SOCKET)
    {
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
    }
}

// ==================================================
// High-level helpers
// ==================================================

bool Addplayertoserver()
{
    std::vector<char> packet;

    auto write = [&](const void* data, size_t size)
    {
        const char* bytes = static_cast<const char*>(data);
        packet.insert(packet.end(), bytes, bytes + size);
    };

    write(&mainPlayer.id, sizeof(mainPlayer.id));

    uint32_t nameLength = static_cast<uint32_t>(mainPlayer.username.size());
    write(&nameLength, sizeof(nameLength));
    write(mainPlayer.username.data(), nameLength);

    write(&mainPlayer.position.x, sizeof(mainPlayer.position.x));
    write(&mainPlayer.position.y, sizeof(mainPlayer.position.y));

    write(&mainPlayer.level, sizeof(mainPlayer.level));
    write(&mainPlayer.health, sizeof(mainPlayer.health));
    write(&mainPlayer.maxHealth, sizeof(mainPlayer.maxHealth));
    write(&mainPlayer.experience, sizeof(mainPlayer.experience));
    write(&mainPlayer.animation, sizeof(mainPlayer.animation));

    uint8_t connected = mainPlayer.connected ? 1 : 0;
    write(&connected, sizeof(connected));

    uint32_t packetSize = static_cast<uint32_t>(packet.size());

    if (!TCP_Send(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize)))
        return false;

    return TCP_Send(packet.data(), static_cast<int>(packet.size()));
}

void UpdatePlayerPosition(float deltaTime)
{
    static float accumulator = 0.0f;

    accumulator += deltaTime;

    if (accumulator < 1.0f / 60.0f)
        return;

    accumulator -= 1.0f / 60.0f;

    std::vector<char> packet;

    auto write = [&](const void* data, size_t size)
    {
        const char* bytes = static_cast<const char*>(data);
        packet.insert(packet.end(), bytes, bytes + size);
    };

    write(&mainPlayer.id, sizeof(mainPlayer.id));
    write(&mainPlayer.position.x, sizeof(mainPlayer.position.x));
    write(&mainPlayer.position.y, sizeof(mainPlayer.position.y));

    UDP_Send(packet.data(), static_cast<int>(packet.size()));
}