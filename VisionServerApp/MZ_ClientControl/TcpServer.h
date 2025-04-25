#ifndef ASYNCTCP_SERVER_H
#define ASYNCTCP_SERVER_H

#include "SocketBase.h"
#include <thread>
#include <mutex>

class CAsyncTCPServer {
private:
    int m_port;
    SOCKET m_serverSocket;
    std::vector<SOCKET> m_clientSockets;
    bool m_running;

    std::mutex m_socketMutex;

public:
    // 服务端构造函数
    CAsyncTCPServer(int port);

    // 启动服务器
    void Start();

    // 停止服务器
    void Stop();

    // 发送数据给指定客户端
    bool SendToClient(SOCKET clientSocket, const char* buffer, int length);

    // 接收数据回调函数类型
    using OnReceiveCallback = std::function<void(SOCKET socket, const char* data, int len)>;

    // 设置接收数据的回调函数
    void SetOnReceiveCallback(const OnReceiveCallback& callback);

    // 客户端连接回调函数类型
    using OnConnectCallback = std::function<void(SOCKET socket)>;

    // 设置客户端连接的回调函数
    void SetOnConnectCallback(const OnConnectCallback& callback);

    // 客户端断开回调函数类型
    using OnDisconnectCallback = std::function<void(SOCKET socket)>;

    // 设置客户端断开的回调函数
    void SetOnDisconnectCallback(const OnDisconnectCallback& callback);
    // 关闭客户端连接
    void CloseClient(SOCKET clientSocket);
    // 关闭所有客户端连接
    void CloseAllClients();
    // 获取客户端连接数
    int GetClientCount();
    //获取客户端接连IP地址
    std::string GetClientIP(SOCKET clientSocket);
    //获取客户端接连端口
    int GetClientPort(SOCKET clientSocket);

private:
    // 接受新连接的线程函数
    void AcceptThread();

    // 处理客户端数据的线程函数
    void ProcessClient(SOCKET clientSocket);
    OnDisconnectCallback m_onDisconnectCallback;
    OnConnectCallback m_onConnectCallback;
    OnReceiveCallback m_onReceiveCallback;
};

#endif // ASYNCTCP_SERVER_H