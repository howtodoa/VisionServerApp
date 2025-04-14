#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SocketBase.h"
class CTcpClient
{
	// socketͨ�ŵĿͻ�����
	public:
		int  m_sockfd;    // �ͻ��˵�socket.
		char m_ip[21];    // ����˵�ip��ַ��
		int  m_port;      // ������ͨ�ŵĶ˿ڡ�
		bool m_btimeout;  // ����Recv��Send����ʱ��ʧ�ܵ�ԭ���Ƿ��ǳ�ʱ��true-��ʱ��false-δ��ʱ��
		int  m_buflen;    // ����Recv�����󣬽��յ��ı��ĵĴ�С����λ���ֽڡ�

		CTcpClient();  // ���캯����

		// �����˷�����������
					   // ip������˵�ip��ַ��
					   // port������˼����Ķ˿ڡ�
					   // ����ֵ��true-�ɹ���false-ʧ�ܡ�
		bool ConnectToServer(const char *ip, const int port);

		// ���շ���˷��͹��������ݡ�
		// buffer���������ݻ������ĵ�ַ�����ݵĳ��ȴ����m_buflen��Ա�����С�
		// itimeout���ȴ����ݵĳ�ʱʱ�䣬��λ���룬ȱʡֵ��0-���޵ȴ���
		// ����ֵ��true-�ɹ���false-ʧ�ܣ�ʧ�������������1���ȴ���ʱ����Ա����m_btimeout��ֵ������Ϊtrue��2��socket�����Ѳ����á�
		bool Recv(char *buffer, int bufferLength, const int itimeout = 0);

		// �����˷������ݡ�
		// buffer�����������ݻ������ĵ�ַ��
		// ibuflen�����������ݵĴ�С����λ���ֽڣ�ȱʡֵΪ0��������͵���ascii�ַ�����ibuflenȡ0������Ƕ����������ݣ�ibuflenΪ���������ݿ�Ĵ�С��
		//itimeout�����յȴ���ʱ��ʱ�䣬��λ���룬ֵ��0-���޵ȴ���
		// ����ֵ��true-�ɹ���false-ʧ�ܣ����ʧ�ܣ���ʾsocket�����Ѳ����á�
		bool Send(const char *buffer, const int ibuflen = 0, const int itimeout = 5);

		// �Ͽ������˵�����
		void Close();

		~CTcpClient();  // ���������Զ��ر�socket���ͷ���Դ��
};

