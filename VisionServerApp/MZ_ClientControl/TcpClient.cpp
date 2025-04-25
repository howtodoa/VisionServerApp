#include "pch.h"
#include "TcpClient.h"
#include <cstring>
#include <iostream>

CAsyncTCPClient::CAsyncTCPClient(const std::string& host, int port)
    : m_socket(INVALID_SOCKET), m_host(host), m_port(port), m_connected(false) 
{
    // 初始化 Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

}

void CAsyncTCPClient::Connect() {
    if (!m_connected) 
    {
        // 创建客户端套接字
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket == INVALID_SOCKET) {
            throw SocketException(WSAGetLastError(), "Failed to create client socket");
        }

        // 设置服务器地址信息
        SOCKADDR_IN sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(m_host.c_str());
        sin.sin_port = htons(m_port);
        std::cout << "connect start" << std::endl;
        // 连接服务器
        if (connect(m_socket, (SOCKADDR*)&sin, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
            throw SocketException(WSAGetLastError(), "Failed to connect to server");
        }

        m_connected = true;

        // 启动数据处理线程
        std::thread processDataThread(&CAsyncTCPClient::ProcessData, this);
        processDataThread.detach();
        m_onConnectCallback(m_socket,m_connected);
    }
}

void CAsyncTCPClient::Disconnect() {
    std::cout << "CAsyncTCPClient::Disconnect: Start, m_connected=" << m_connected << std::endl;
    std::cout << "m_onConnectCallback empty: " << !m_onConnectCallback << std::endl;
    if (m_connected)
    {
        try {
            if (m_onConnectCallback) {
                m_onConnectCallback(m_socket, m_connected); // 行 50
            }
            else {
                std::cerr << "CAsyncTCPClient::Disconnect: m_onConnectCallback empty!" << std::endl;
            }
        }
        catch (const std::bad_function_call& e) {
            std::cerr << "CAsyncTCPClient::Disconnect: " << e.what() << std::endl;
        }
        m_connected = false;
        closesocket(m_socket);
        m_socket = INVALID_SOCKET; // 标记无效 add by tjq
        WSACleanup();// add by tjq

    }
    std::cout << "CAsyncTCPClient::Disconnect: End" << std::endl;
}
bool CAsyncTCPClient::IsConnected()
{
    return m_connected;
}

bool CAsyncTCPClient::Send(const char* buffer, int length) 
{
    std::lock_guard<std::mutex> lock(m_socketMutex);

    if (send(m_socket, buffer, length, 0) == SOCKET_ERROR) 
    {
        return false;
    }
    return true;
}

void CAsyncTCPClient::SetOnReceiveCallback(const OnReceiveCallback& callback)
{
    m_onReceiveCallback = callback;
}

void CAsyncTCPClient::SetOnConnectCallback(const OnConnectCallback& callback)
{
    m_onConnectCallback = callback;
}

void CAsyncTCPClient::ProcessData() {
    char buffer[1024];
    std::cout << "CAsyncTCPClient::ProcessData: Start" << std::endl;
    while (m_connected) {
        if (m_socket == INVALID_SOCKET) {//add by tjq
            std::cout << "Socket invalid, exiting ProcessData" << std::endl;
            break;
        }
        int bytesRecv = recv(m_socket, buffer, sizeof(buffer), 0);
        std::cout << "bytesRecv: " << bytesRecv << ", WSAGetLastError: " << WSAGetLastError() << std::endl;
        if (bytesRecv > 0) {
            if (m_onReceiveCallback) {
                m_onReceiveCallback(m_socket, buffer, bytesRecv);
            }
        }
        else if (bytesRecv == 0 || bytesRecv == SOCKET_ERROR) {
            std::cout << "Disconnectstart1111111111111" << std::endl;
            Disconnect(); // 行 97
            std::cout << "Disconnectendl" << std::endl;
            break;
        }
    }
    std::cout << "CAsyncTCPClient::ProcessData: End" << std::endl;
}