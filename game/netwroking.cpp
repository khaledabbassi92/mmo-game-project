#include "core.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

bool NetworkManager::init()
{
    if (winsockInitialized)
        return true;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "[NetworkManager] WSAStartup failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    winsockInitialized = true;
    return true;
}

void NetworkManager::shutdown()
{
    disconnectTCP();
    disconnectUDP();

    if (winsockInitialized)
    {
        WSACleanup();
        winsockInitialized = false;
        std::cout << "[NetworkManager] Winsock cleaned up.\n";
    }
}

bool NetworkManager::connectTCP(const char* ip, unsigned short port)
{
    if (!init()) return false;

    tcpSocket = static_cast<uintptr_t>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (tcpSocket == ~0ULL)
    {
        std::cerr << "[TCP] socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);

    if (connect(static_cast<SOCKET>(tcpSocket), (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cerr << "[TCP] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(static_cast<SOCKET>(tcpSocket));
        tcpSocket = ~0ULL;
        return false;
    }

    u_long mode = 1;
    ioctlsocket(static_cast<SOCKET>(tcpSocket), FIONBIO, &mode);

    std::cout << "[TCP] Connected to " << ip << ":" << port << "\n";
    return true;
}

bool NetworkManager::sendTCP(const char* data, int size)
{
    if (tcpSocket == ~0ULL) return false;

    int sent = send(static_cast<SOCKET>(tcpSocket), data, size, 0);
    return (sent == size);
}

int NetworkManager::receiveTCP(char* buffer, int size)
{
    if (tcpSocket == ~0ULL) return -1;
    return recv(static_cast<SOCKET>(tcpSocket), buffer, size, 0);
}

void NetworkManager::disconnectTCP()
{
    if (tcpSocket != ~0ULL)
    {
        closesocket(static_cast<SOCKET>(tcpSocket));
        tcpSocket = ~0ULL;
    }
}

bool NetworkManager::connectUDP(const char* ip, unsigned short port)
{
    if (!init()) return false;

    udpSocket = static_cast<uintptr_t>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
    if (udpSocket == ~0ULL)
    {
        std::cerr << "[UDP] socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);

    if (connect(static_cast<SOCKET>(udpSocket), (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cerr << "[UDP] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(static_cast<SOCKET>(udpSocket));
        udpSocket = ~0ULL;
        return false;
    }

    u_long mode = 1;
    ioctlsocket(static_cast<SOCKET>(udpSocket), FIONBIO, &mode);

    std::cout << "[UDP] Target set to " << ip << ":" << port << "\n";
    return true;
}

bool NetworkManager::sendUDP(const char* data, int size)
{
    if (udpSocket == ~0ULL) return false;
    int sent = send(static_cast<SOCKET>(udpSocket), data, size, 0);
    return (sent == size);
}

int NetworkManager::receiveUDP(char* buffer, int size)
{
    if (udpSocket == ~0ULL) return -1;
    return recv(static_cast<SOCKET>(udpSocket), buffer, size, 0);
}

void NetworkManager::disconnectUDP()
{
    if (udpSocket != ~0ULL)
    {
        closesocket(static_cast<SOCKET>(udpSocket));
        udpSocket = ~0ULL;
    }
}

bool NetworkManager::sendLoginRequest(const UIManager& ui)
{
    std::vector<char> packet;

    auto write = [&](const void* data, size_t size) {
        const char* bytes = static_cast<const char*>(data);
        packet.insert(packet.end(), bytes, bytes + size);
    };

    uint32_t userLen = static_cast<uint32_t>(std::strlen(ui.username));
    write(&userLen, sizeof(userLen));
    if (userLen > 0)
        write(ui.username, userLen);

    uint32_t passLen = static_cast<uint32_t>(std::strlen(ui.password));
    write(&passLen, sizeof(passLen));
    if (passLen > 0)
        write(ui.password, passLen);

    uint32_t packetSize = static_cast<uint32_t>(packet.size());

    if (!sendTCP(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize)))
        return false;

    return sendTCP(packet.data(), static_cast<int>(packet.size()));
}

void NetworkManager::sendPlayerPosition(const MainPlayer& player, float deltaTime)
{
    static float accumulator = 0.0f;
    accumulator += deltaTime;

    if (accumulator < 1.0f / 60.0f)
        return;

    accumulator -= 1.0f / 60.0f;

    std::vector<char> packet;
    auto write = [&](const void* data, size_t size) {
        const char* bytes = static_cast<const char*>(data);
        packet.insert(packet.end(), bytes, bytes + size);
    };

    write(&player.id, sizeof(player.id));
    write(&player.position.x, sizeof(player.position.x));
    write(&player.position.y, sizeof(player.position.y));

    sendUDP(packet.data(), static_cast<int>(packet.size()));
}