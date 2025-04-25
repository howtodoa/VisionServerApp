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
    // ����˹��캯��
    CAsyncTCPServer(int port);

    // ����������
    void Start();

    // ֹͣ������
    void Stop();

    // �������ݸ�ָ���ͻ���
    bool SendToClient(SOCKET clientSocket, const char* buffer, int length);

    // �������ݻص���������
    using OnReceiveCallback = std::function<void(SOCKET socket, const char* data, int len)>;

    // ���ý������ݵĻص�����
    void SetOnReceiveCallback(const OnReceiveCallback& callback);

    // �ͻ������ӻص���������
    using OnConnectCallback = std::function<void(SOCKET socket)>;

    // ���ÿͻ������ӵĻص�����
    void SetOnConnectCallback(const OnConnectCallback& callback);

    // �ͻ��˶Ͽ��ص���������
    using OnDisconnectCallback = std::function<void(SOCKET socket)>;

    // ���ÿͻ��˶Ͽ��Ļص�����
    void SetOnDisconnectCallback(const OnDisconnectCallback& callback);
    // �رտͻ�������
    void CloseClient(SOCKET clientSocket);
    // �ر����пͻ�������
    void CloseAllClients();
    // ��ȡ�ͻ���������
    int GetClientCount();
    //��ȡ�ͻ��˽���IP��ַ
    std::string GetClientIP(SOCKET clientSocket);
    //��ȡ�ͻ��˽����˿�
    int GetClientPort(SOCKET clientSocket);

private:
    // ���������ӵ��̺߳���
    void AcceptThread();

    // ����ͻ������ݵ��̺߳���
    void ProcessClient(SOCKET clientSocket);
    OnDisconnectCallback m_onDisconnectCallback;
    OnConnectCallback m_onConnectCallback;
    OnReceiveCallback m_onReceiveCallback;
};

#endif // ASYNCTCP_SERVER_H