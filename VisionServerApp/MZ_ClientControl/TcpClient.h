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
    // �ͻ��˹��캯��
    CAsyncTCPClient(const std::string& host, int port);

    // ���ӷ�����
    void Connect();
    // �Ͽ�����
    void Disconnect();
    // �Ƿ�������
    bool IsConnected();
    // ��������
    bool Send(const char* buffer, int length);

    // �������ݻص���������
    using OnReceiveCallback = std::function<void(SOCKET pSocket, const char* data, int len)>;

    // ���ý������ݵĻص�����
    void SetOnReceiveCallback(const OnReceiveCallback& callback);

    //����״̬�ص���������
    using OnConnectCallback = std::function<void(SOCKET pSocket, const bool connected)>;
    //��������״̬�ı�Ļص�����
    void SetOnConnectCallback(const OnConnectCallback& callback);

private:
    // ���ݴ����̺߳���
    void ProcessData();
    OnReceiveCallback m_onReceiveCallback;
    OnConnectCallback m_onConnectCallback;
};

#endif // ASYNCTCP_CLIENT_H