#include "pch.h"
#include "TcpServer.h"


CTcpServer::CTcpServer() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
	}
	else
		printf("The Winsock 2.2 dll was found okay\n");

	m_listenfd = -1;
	m_connfd = -1;
	m_socklen = 0;
	m_btimeout = false;
}

bool CTcpServer::InitServer(std::string localServerIP,const unsigned int port) 
{
	CloseListen();

	if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) 
		return false;

	//设置SO_REUSEADDR选项，解决关闭程序端口还在占用，再运行socket bind不成功的问题
	// WINDOWS平台如下
	char b_opt='1';
	setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&b_opt,sizeof(b_opt));

	// Linux如下
	//int opt = 1;
	//unsigned int len = sizeof(opt);
	//setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, len);

	memset(&m_servaddr, 0, sizeof(m_servaddr));
	m_servaddr.sin_family = AF_INET;
	//m_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, localServerIP.c_str(), &m_servaddr.sin_addr);
	m_servaddr.sin_port = htons(port);
	if (bind(m_listenfd, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr)) != 0) {
		CloseListen();
		return false;
	}

	if (listen(m_listenfd, 5) != 0) {
		CloseListen();
		return false;
	}

	m_socklen = sizeof(struct sockaddr_in);

	return true;
}

bool CTcpServer::Accept() 
{
	if (m_listenfd == -1) 
		return false;

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	fd_set tmpfd;

	FD_ZERO(&tmpfd);
	FD_SET(m_listenfd, &tmpfd);
	if (m_connfd == -1)
	{
		FD_SET(m_connfd, &tmpfd);
	}
	select(FD_SETSIZE, &tmpfd, NULL, NULL, &timeout);

	if(FD_ISSET(m_listenfd,&tmpfd))
	{
		if ((m_connfd = accept(m_listenfd, (struct sockaddr *)&m_clientaddr, (int*)&m_socklen)) < 0)
			return false;
	}
	return true;
}

void CTcpServer::GetIP(std::string & IPdotdec)
{
	//return(inet_ntoa(m_clientaddr.sin_addr));
	char IPdotdectt[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &m_clientaddr.sin_addr, IPdotdectt, sizeof(IPdotdectt));
	//return IPdotdec;
	IPdotdec = IPdotdectt;
}
bool CTcpServer::Recv(char *buffer, int bufferLength,const int itimeout) 
{
	if (m_connfd == -1) 
		return false;

	if (itimeout>0) 
	{
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(m_connfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		m_btimeout = false;
		int i;
		if ((i = select(m_connfd + 1, &tmpfd, 0, 0, &timeout)) <= 0)
		{
			if (i == 0)
				m_btimeout = true;
			return false;
		}
	}
	m_buflen = 0;
	return(TcpRecv_T(m_connfd, buffer, bufferLength ,&m_buflen));
}

bool CTcpServer::Send(const char *buffer, const int ibuflen, const int itimeout)
{
	if (m_connfd == -1) 
		return false;

	if (itimeout>0)
	{
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(m_connfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		m_btimeout = false;


		int i;
		if ((i = select(m_connfd + 1, NULL,&tmpfd, 0, &timeout)) <= 0)
		{
			if (i == 0)
				m_btimeout = true;
			return false;
		}
	}
	int ilen = ibuflen;
	if (ilen == 0) 
		ilen = strlen(buffer);
	return(TcpSend_T(m_connfd, buffer, ilen));
}

void CTcpServer::CloseListen() {
	if (m_listenfd > 0) {
		close_T(m_listenfd);
		m_listenfd = -1;
	}
}

void CTcpServer::CloseClient() {
	if (m_connfd > 0) {
		close_T(m_connfd);
		m_connfd = -1;
	}
}

CTcpServer::~CTcpServer() {
	CloseListen();
	CloseClient();
}