#pragma once
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>


bool recvn_T(const int sockfd, char *buffer, int bufferlength,int& RecvLength);
bool sendn_T(const int sockfd, const char *buffer, const size_t n = 0);
bool TcpRecv_T(const int sockfd, char *buffer, int bufferlength,int *ibuflen, const int itimeout = 0);
bool TcpSend_T(const int sockfd, const char *buffer, const int ibuflen = 0, const int itimeout = 5);
bool close_T(int socket);