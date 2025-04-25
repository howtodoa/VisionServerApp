#ifndef ASYNCTCP_CLIENT_H
#define ASYNCTCP_CLIENT_H

#include "SocketBase.h"
#include <thread>
#include <mutex>
#include <atomic>

class CAsyncTCPClient {
private:
    SOCKET m_socket;
    std::string m_host;
    int m_port;
   // std::atomic<bool> m_connected{ false };
    bool m_connected;

    std::mutex m_socketMutex;

public:
    // 客户端构造函数
    CAsyncTCPClient(const std::string& host, int port);

    // 连接服务器
    void Connect();
    // 断开连接
    void Disconnect();
    // 是否已连接
    bool IsConnected();
    // 发送数据
    bool Send(const char* buffer, int length);

    // 接收数据回调函数类型
    using OnReceiveCallback = std::function<void(SOCKET pSocket, const char* data, int len)>;

    // 设置接收数据的回调函数
    void SetOnReceiveCallback(const OnReceiveCallback& callback);

    //接连状态回调函数类型
    using OnConnectCallback = std::function<void(SOCKET pSocket, const bool connected)>;
    //设置连接状态改变的回调函数
    void SetOnConnectCallback(const OnConnectCallback& callback);

private:
    // 数据处理线程函数
    void ProcessData();
    OnReceiveCallback m_onReceiveCallback;
    OnConnectCallback m_onConnectCallback;
};

#endif // ASYNCTCP_CLIENT_H