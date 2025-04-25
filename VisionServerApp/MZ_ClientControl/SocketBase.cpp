#include "pch.h"
#include "SocketBase.h"
//���Ѿ�׼���õ�socket�ж�ȡ���ݡ�
//sockfd���Ѿ�׼���õ�socket���ӡ�
//buffer���������ݻ������ĵ�ַ��
//n�����ν������ݵ��ֽ�����
//����ֵ���ɹ����յ�n�ֽڵ����ݺ󷵻�true��socket���Ӳ����÷���false��
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
//���Ѿ�׼���õ�socket��д�����ݡ�
//sockfd���Ѿ�׼���õ�socket���ӡ�
//buffer�����������ݻ������ĵ�ַ��
//n�����������ݵ��ֽ�����ȱʡֵ��0ʱĬ��Ϊbuffer�Ĵ�С��
//����ֵ���ɹ�������n�ֽڵ����ݺ󷵻�true��socket���Ӳ����÷���false��
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
//����socket�ĶԶ˷��͹��������ݡ�
//sockfd�����õ�socket���ӡ�
//buffer���������ݻ������ĵ�ַ��
//ibuflen�����γɹ��������ݵ��ֽ��������TCP���紫�䡸ճ����
//itimeout�����յȴ���ʱ��ʱ�䣬��λ���룬ȱʡֵ��0-���޵ȴ���
//����ֵ��true-�ɹ���false-ʧ�ܣ�ʧ�������������1���ȴ���ʱ��2��socket�����Ѳ����á�
bool TcpRecv_T(const int sockfd, char *buffer, int bufferLength,int *ibuflen, const int itimeout) 
{
	if (sockfd == -1) 
		return false;
	if (itimeout > 0) 
	{  //��ʱ���� 
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(sockfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		//�����itimeoutʱ����û�п�����Դ���ļ��������Ļ����˳� 
		if (select(sockfd + 1, &tmpfd, 0, 0, &timeout) <= 0) return false;
	}

	//���ݰ� = ���ݴ�С + ����
	//(*ibuflen) = 0;

	//if (!recvn_T(sockfd, (char*)ibuflen, 4)) 
	//	return false;

	//(*ibuflen) = ntohl(*ibuflen);  //�������ֽ���ת��Ϊ�����ֽ���

	return recvn_T(sockfd, buffer, bufferLength ,*ibuflen);
}
//��socket�ĶԶ˷������ݡ�
//sockfd�����õ�socket���ӡ�
//buffer�����������ݻ������ĵ�ַ��
//ibuflen�����������ݵ��ֽ�����������͵���ascii�ַ�����ibuflenȡ0������Ƕ����������ݣ�ibuflenΪ���������ݿ�Ĵ�С��
//itimeout�����յȴ���ʱ��ʱ�䣬��λ���룬ֵ��0-���޵ȴ���
//����ֵ��true-�ɹ���false-ʧ�ܣ����ʧ�ܣ���ʾsocket�����Ѳ����á�
bool TcpSend_T(const int sockfd, const char *buffer, const int ibuflen, const int itimeout) {
	if (sockfd == -1) return false;
	if (itimeout > 0) {  //��ʱ���� 
		fd_set tmpfd;

		FD_ZERO(&tmpfd);
		FD_SET(sockfd, &tmpfd);

		struct timeval timeout;
		timeout.tv_sec = itimeout;
		timeout.tv_usec = 0;

		//�����itimeoutʱ����û�п�����Դ���ļ��������Ļ����˳� 
		if (select(sockfd + 1, 0, &tmpfd, 0, &timeout) <= 0) 
			return false;
	}

	////�������Ϊ0���Ͳ����ַ����ĳ���
	int ilen = (ibuflen == 0) ? strlen(buffer) : ibuflen;
	return sendn_T(sockfd, buffer, ilen);
}

bool close_T(int socket)
{
	closesocket(socket);
	return true;
}