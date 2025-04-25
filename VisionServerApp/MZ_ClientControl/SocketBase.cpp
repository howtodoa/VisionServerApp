#include "pch.h"
#include "SocketBase.h"
//从已经准备好的socket中读取数据。
//sockfd：已经准备好的socket连接。
//buffer：接收数据缓冲区的地址。
//n：本次接收数据的字节数。
//返回值：成功接收到n字节的数据后返回true，socket连接不可用返回false。
bool recvn_T(const int sockfd, char *buffer, int bufferLength,int& RecvLength)
{
	int remain, len, idx;
	remain = bufferLength;
	idx = 0;
	//while (remain > 0) {
	//	if ((len = recv(sockfd, buffer + idx, remain, 0)) <= 0)
	//		return false;
	//	idx += len;
	//	remain -= len;
	//}

	if ((RecvLength = recv(sockfd, buffer + idx, remain, 0)) <= 0)
		return false;

	return true;
}
//向已经准备好的socket中写入数据。
//sockfd：已经准备好的socket连接。
//buffer：待发送数据缓冲区的地址。
//n：待发送数据的字节数，缺省值是0时默认为buffer的大小。
//返回值：成功发送完n字节的数据后返回true，socket连接不可用返回false。
bool sendn_T(const int sockfd, const char *buffer, const size_t n) 
{
	int remain, len, idx;
	remain = (n == 0) ? strlen(buffer) : n;
	idx = 0;
	while (remain > 0) {
		if ((len = send(sockfd, buffer + idx, remain, 0)) <= 0)
			return false;
		idx += len;
		remain -= len;
	}
	return true;
}
//接收socket的对端发送过来的数据。
//sockfd：可用的socket连接。
//buffer：接收数据缓冲区的地址。
//ibuflen：本次成功接收数据的字节数。解决TCP网络传输「粘包」
//itimeout：接收等待超时的时间，单位：秒，缺省值是0-无限等待。
//返回值：true-成功；false-失败，失败有两种情况：1）等待超时；2）socket连接已不可用。
bool TcpRecv_T(const int sockfd, char *buffer, int bufferLength,int *ibuflen, const int itimeout) 
{
	if (sockfd == -1) 
		return false;
	if (itimeout > 0) 
	{  //延时操作 
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(sockfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		//如果在itimeout时间内没有可用资源的文件描述符的话就退出 
		if (select(sockfd + 1, &tmpfd, 0, 0, &timeout) <= 0) return false;
	}

	//数据包 = 数据大小 + 数据
	//(*ibuflen) = 0;

	//if (!recvn_T(sockfd, (char*)ibuflen, 4)) 
	//	return false;

	//(*ibuflen) = ntohl(*ibuflen);  //把网络字节序转换为主机字节序。

	return recvn_T(sockfd, buffer, bufferLength ,*ibuflen);
}
//向socket的对端发送数据。
//sockfd：可用的socket连接。
//buffer：待发送数据缓冲区的地址。
//ibuflen：待发送数据的字节数，如果发送的是ascii字符串，ibuflen取0，如果是二进制流数据，ibuflen为二进制数据块的大小。
//itimeout：接收等待超时的时间，单位：秒，值是0-无限等待。
//返回值：true-成功；false-失败，如果失败，表示socket连接已不可用。
bool TcpSend_T(const int sockfd, const char *buffer, const int ibuflen, const int itimeout) {
	if (sockfd == -1) return false;
	if (itimeout > 0) {  //延时操作 
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(sockfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		//如果在itimeout时间内没有可用资源的文件描述符的话就退出 
		if (select(sockfd + 1, 0, &tmpfd, 0, &timeout) <= 0) 
			return false;
	}

	////如果长度为0，就采用字符串的长度
	int ilen = (ibuflen == 0) ? strlen(buffer) : ibuflen;
	return sendn_T(sockfd, buffer, ilen);
}

bool close_T(int socket)
{
	closesocket(socket);
	return true;
}