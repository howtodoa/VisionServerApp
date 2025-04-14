#include "pch.h"
#include <atlstr.h>
#include "TcpClient.h"

CTcpClient::CTcpClient()
{
	m_sockfd = -1;
	memset(m_ip, 0, sizeof(m_ip));
	m_port = 0;
	m_btimeout = false;

	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	int wsResult = WSAStartup(version, &wsaData);

}
CTcpClient::~CTcpClient()
{
	Close();
	WSACleanup();
}

bool CTcpClient::ConnectToServer(const char *ip, const int port) 
{
	if (m_sockfd != -1) {
		close_T(m_sockfd);
		m_sockfd = -1;
	}

	strcpy_s(m_ip, ip);
	m_port = port;

	//struct hostent* h;
	struct sockaddr_in servaddr;

	if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		return false;

	/*
	if (!(h = gethostbyname(m_ip)))
	{
		close(m_sockfd);
		m_sockfd = -1;
		return false;
	}
	*/
	addrinfoW hints, *result = NULL, *ptr = NULL;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // 指定IPv4或IPv6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 将主机名转换为IP地址
	int iResult = GetAddrInfo(CA2W(m_ip), NULL, &hints, &result);
	if (iResult != 0) 
		return false;


	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(m_port);  // 指定服务端的通讯端口
	memcpy(&servaddr.sin_addr, result->ai_addr, result->ai_addrlen);

	if (connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		close_T(m_sockfd);
		m_sockfd = -1;
		return false;
	}

	return true;
}

bool CTcpClient::Recv(char *buffer, int bufferLength, const int itimeout)
{
	if (m_sockfd == -1) return false;

	if (itimeout>0) {
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(m_sockfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		m_btimeout = false;

		int i;
		if ((i = select(m_sockfd + 1, &tmpfd, 0, 0, &timeout)) <= 0) {
			if (i == 0) m_btimeout = true;
			return false;
		}
	}

	m_buflen = 0;
	return (TcpRecv_T(m_sockfd, buffer, bufferLength ,&m_buflen));
}

bool CTcpClient::Send(const char *buffer, const int ibuflen, const int itimeout) 
{
	if (m_sockfd == -1) return false;

	if (itimeout>0) {
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(m_sockfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		m_btimeout = false;

		int i;
		if ((i = select(m_sockfd + 1, &tmpfd, 0, 0, &timeout)) <= 0) {
			if (i == 0) m_btimeout = true;
			return false;
		}
	}

	int ilen = ibuflen;

	if (ibuflen == 0) ilen = strlen(buffer);

	return(TcpSend_T(m_sockfd, buffer, ilen));
}

void CTcpClient::Close() 
{
	if (m_sockfd > 0) 
		close_T(m_sockfd);

	m_sockfd = -1;
	memset(m_ip, 0, sizeof(m_ip));
	m_port = 0;
	m_btimeout = false;
}


