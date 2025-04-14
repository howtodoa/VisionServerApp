#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class ModbusTcpClient {
private:
    SOCKET m_socket;
    struct sockaddr_in m_serverAddress;
    unsigned short m_port;
    std::string m_ipAddress;

public:
    ModbusTcpClient(const std::string& ipAddress, unsigned short port)
        : m_ipAddress(ipAddress), m_port(port) {
        m_socket = INVALID_SOCKET;
    }

    ~ModbusTcpClient() {
        if (m_socket != INVALID_SOCKET)
            closesocket(m_socket);
    }

    bool connect2() {
        // 初始化 Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock" << std::endl;
            return false;
        }

        // 创建 socket
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket == INVALID_SOCKET) {
            std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }

        // 设置服务器地址
        memset(&m_serverAddress, 0, sizeof(m_serverAddress));
        m_serverAddress.sin_family = AF_INET;
        m_serverAddress.sin_port = htons(m_port);
        m_serverAddress.sin_addr.s_addr = inet_addr(m_ipAddress.c_str());

        // 连接服务器
        if (connect(m_socket, (struct sockaddr*)&m_serverAddress, sizeof(m_serverAddress)) == SOCKET_ERROR) {
            std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
            return false;
        }

        return true;
    }

    bool readHoldingRegisters(unsigned short transactionId, unsigned char unitId,
        unsigned short startAddress, unsigned short numRegisters,
        unsigned char* dataBuffer) {
        // 构建 MODBUS 请求帧
        unsigned int pduLength = 6; // Function Code (1B), Start Address (2B), Num Registers (2B)
        unsigned int totalLength = pduLength + 6; // MODBUS TCP Header (6B)

        char* tcpBuffer = new char[totalLength];
        memset(tcpBuffer, 0, totalLength);

        // 填充 MODBUS TCP 头部
        *(unsigned short*)(tcpBuffer) = htons(transactionId);    // Transaction ID
        *(unsigned short*)(tcpBuffer + 2) = htons(0x0000);       // Protocol Identifier (always 0)
        *(unsigned short*)(tcpBuffer + 4) = htons(totalLength - 6); // Length
        *(unsigned char*)(tcpBuffer + 6) = unitId;              // Unit ID

        // 填充 MODBUS PDU 请求内容
        unsigned char* pduPtr = (unsigned char*)tcpBuffer + 6;
        *pduPtr++ = 0x03;   // Function Code for Read Holding Registers (0x03)
        *(unsigned short*)pduPtr = htons(startAddress);          // Start Address
        pduPtr += 2;
        *(unsigned short*)pduPtr = htons(numRegisters);          // Number of Registers

        // 发送数据
        if (send(m_socket, tcpBuffer, totalLength, 0) == SOCKET_ERROR) {
            delete[] tcpBuffer;
            return false;
        }

        // 接收响应
        int bytesRecv = recv(m_socket, (char*)dataBuffer, totalLength, 0);
        if (bytesRecv <= 0) {
            delete[] tcpBuffer;
            return false;
        }

        // 验证响应数据
        if (dataBuffer[7] != 0x03) { // 检查功能码是否正确
            delete[] tcpBuffer;
            return false;
        }

        delete[] tcpBuffer;
        return true;
    }

    bool writeSingleRegister(unsigned short transactionId, unsigned char unitId,
        unsigned short address, unsigned short value) {
        // 构建 MODBUS 请求帧
        unsigned int pduLength = 5; // Function Code (1B), Address (2B), Value (2B)
        unsigned int totalLength = pduLength + 6; // MODBUS TCP Header (6B)

        char* tcpBuffer = new char[totalLength];
        memset(tcpBuffer, 0, totalLength);

        // 填充 MODBUS TCP 头部
        *(unsigned short*)(tcpBuffer) = htons(transactionId);    // Transaction ID
        *(unsigned short*)(tcpBuffer + 2) = htons(0x0000);       // Protocol Identifier (always 0)
        *(unsigned short*)(tcpBuffer + 4) = htons(totalLength - 6); // Length
        *(unsigned char*)(tcpBuffer + 6) = unitId;              // Unit ID

        // 填充 MODBUS PDU 请求内容
        unsigned char* pduPtr = (unsigned char*)tcpBuffer + 6;
        *pduPtr++ = 0x06;   // Function Code for Write Single Register (0x06)
        *(unsigned short*)pduPtr = htons(address);               // Address
        pduPtr += 2;
        *(unsigned short*)pduPtr = htons(value);                 // Value

        // 发送数据
        if (send(m_socket, tcpBuffer, totalLength, 0) == SOCKET_ERROR) {
            delete[] tcpBuffer;
            return false;
        }

        // 接收响应
        char* responseBuffer = new char[totalLength];
        int bytesRecv = recv(m_socket, responseBuffer, totalLength, 0);
        if (bytesRecv <= 0) {
            delete[] tcpBuffer;
            delete[] responseBuffer;
            return false;
        }

        // 验证响应数据
        if (responseBuffer[7] != 0x06) { // 检查功能码是否正确
            delete[] tcpBuffer;
            delete[] responseBuffer;
            return false;
        }

        delete[] tcpBuffer;
        delete[] responseBuffer;
        return true;
    }
};
#if 0
int main() {
    ModbusTcpClient client("127.0.0.1", 502); // 示例地址和端口

    if (!client.connect2()) {
        printf("Connection failed!\n");
        return -1;
    }

    // 读取保持寄存器示例
    unsigned char readData[10];
    if (client.readHoldingRegisters(0x0001, 0x01, 0x0000, 5, readData)) {
        printf("Read registers successfully!\n");
    }

    // 写入单个寄存器示例
    if (client.writeSingleRegister(0x0002, 0x01, 0x0000, 0x1234)) {
        printf("Write register successfully!\n");
    }

    return 0;
}
#endif