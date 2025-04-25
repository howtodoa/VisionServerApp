#include "pch.h"
#include "TcpServer.h"
#include <cstring>
#include <iostream>

CAsyncTCPServer::CAsyncTCPServer(int port)
    : m_port(port), m_serverSocket(INVALID_SOCKET), m_running(false) {
}

void CAsyncTCPServer::Start() {
    if (!m_running) {
        // ��ʼ�� Winsock
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // �����������׽���
        m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_serverSocket == INVALID_SOCKET) {
            throw SocketException(WSAGetLastError(), "Failed to create server socket");
        }

        // ���÷�������ַ��Ϣ
        SOCKADDR_IN sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(m_port);

        // ���׽��ֵ�ָ���˿�
        if (bind(m_serverSocket, (SOCKADDR*)&sin, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
            throw SocketException(WSAGetLastError(), "Failed to bind server socket");
        }

        // ������������
        listen(m_serverSocket, SOMAXCONN);

        m_running = true;

        // �������������ӵ��߳�
        std::thread acceptThread(&CAsyncTCPServer::AcceptThread, this);
        acceptThread.detach();
    }
}

void CAsyncTCPServer::Stop() {
    if (m_running) {
        m_running = false;

        // �ر����пͻ����׽���
        for (SOCKET socket : m_clientSockets) {
            closesocket(socket);
        }

        // ����������׽���
        closesocket(m_serverSocket);

        WSACleanup();
    }
}

bool CAsyncTCPServer::SendToClient(SOCKET clientSocket, const char* buffer, int length) {
    std::lock_guard<std::mutex> lock(m_socketMutex);

    if (send(clientSocket, buffer, length, 0) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

void CAsyncTCPServer::SetOnReceiveCallback(const OnReceiveCallback& callback)
{
    m_onReceiveCallback = callback;
}

void CAsyncTCPServer::SetOnConnectCallback(const OnConnectCallback& callback)
{
    m_onConnectCallback = callback;
}

void CAsyncTCPServer::SetOnDisconnectCallback(const OnDisconnectCallback& callback)
{
    m_onDisconnectCallback = callback;
}

void CAsyncTCPServer::CloseClient(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(m_socketMutex);

    auto it = std::remove(m_clientSockets.begin(), m_clientSockets.end(), clientSocket);
    m_clientSockets.erase(it);

    closesocket(clientSocket);
    if (m_onDisconnectCallback) 
    {
        m_onDisconnectCallback(clientSocket);   
    }
}

void CAsyncTCPServer::CloseAllClients()
{
    std::lock_guard<std::mutex> lock(m_socketMutex);

    for (SOCKET socket : m_clientSockets) {
        closesocket(socket);
        if (m_onDisconnectCallback)
        {
            m_onDisconnectCallback(socket);
        }
    }

    m_clientSockets.clear();   
}

int CAsyncTCPServer::GetClientCount()
{
     std::lock_guard<std::mutex> lock(m_socketMutex);
     return m_clientSockets.size();
}

std::string CAsyncTCPServer::GetClientIP(SOCKET clientSocket)
{
    SOCKADDR_IN clientAddr;
    int addrLen = sizeof(SOCKADDR_IN);

    getpeername(clientSocket, (SOCKADDR*)&clientAddr, &addrLen);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);

    return std::string(ip);
}

int CAsyncTCPServer::GetClientPort(SOCKET clientSocket)
{
    SOCKADDR_IN clientAddr;
    int addrLen = sizeof(SOCKADDR_IN);

    getpeername(clientSocket, (SOCKADDR*)&clientAddr, &addrLen);

    return ntohs(clientAddr.sin_port);
}

void CAsyncTCPServer::AcceptThread() {
    while (m_running) {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(SOCKADDR_IN);

        SOCKET newSocket = accept(m_serverSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (newSocket == INVALID_SOCKET) {
            continue;
        }

        // ֪ͨ�ͻ������ӻص�
        if (m_onConnectCallback) {
            m_onConnectCallback(newSocket);
        }

        std::lock_guard<std::mutex> lock(m_socketMutex);
        m_clientSockets.push_back(newSocket);

        // ��������ͻ������ݵ��߳�
        std::thread processThread(&CAsyncTCPServer::ProcessClient, this, newSocket);
        processThread.detach();
    }
}

void CAsyncTCPServer::ProcessClient(SOCKET clientSocket) {
    char buffer[1024];

    while (m_running) {
        int bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRecv > 0) {
            // ���ý������ݻص�
            if (m_onReceiveCallback) {
                m_onReceiveCallback(clientSocket, buffer, bytesRecv);
            }
        }
        else if (bytesRecv == 0) {
            // �ͻ��˶Ͽ�����
            if (m_onDisconnectCallback) {
                m_onDisconnectCallback(clientSocket);
            }

            std::lock_guard<std::mutex> lock(m_socketMutex);
            auto it = std::remove(m_clientSockets.begin(), m_clientSockets.end(), clientSocket);
            m_clientSockets.erase(it);

            closesocket(clientSocket);
            break;
        }
    }
}