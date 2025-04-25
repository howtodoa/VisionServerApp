#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#ifndef SOCKETCOMMON_H
#define SOCKETCOMMON_H

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#include <vector>
#include <functional>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

// »ù´¡Ì×½Ó×Ö´íÎóÀà
class SocketException {
public:
    int error_code;
    std::string message;

    SocketException(int code, const std::string& msg) : error_code(code), message(msg) {}
};

#endif // SOCKETCOMMON_H