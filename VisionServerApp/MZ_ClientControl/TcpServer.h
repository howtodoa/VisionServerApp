#pragma once
#include "SocketBase.h"
#include <string>
class CTcpServer
{
public:
private:
	int m_socklen;                    // �ṹ��struct sockaddr_in�Ĵ�С��
	struct sockaddr_in m_clientaddr;  // �ͻ��˵ĵ�ַ��Ϣ��
	struct sockaddr_in m_servaddr;    // ����˵ĵ�ַ��Ϣ��
public:
	SOCKET  m_listenfd;   // ��������ڼ�����socket��
	SOCKET  m_connfd;     // �ͻ�������������socket��
	bool m_btimeout;   // ����Recv��Send����ʱ��ʧ�ܵ�ԭ���Ƿ��ǳ�ʱ��true-��ʱ��false-δ��ʱ��
	int  m_buflen;     // ����Recv�����󣬽��յ��ı��ĵĴ�С����λ���ֽڡ�

	CTcpServer();  // ���캯����

	bool InitServer(std::string localServerIP, const unsigned int port);

				   // ����˳�ʼ����
				   // port��ָ����������ڼ����Ķ˿ڡ�
				   // ����ֵ��true-�ɹ���false-ʧ�ܣ�һ������£�ֻҪport������ȷ��û�б�ռ�ã���ʼ������ɹ���
	//bool InitServer(const unsigned int port);

	// �����ȴ��ͻ��˵���������
	// ����ֵ��true-���µĿͻ���������������false-ʧ�ܣ�Accept���жϣ����Acceptʧ�ܣ���������Accept��
	bool Accept();

	

	// ��ȡ�ͻ��˵�ip��ַ��
	// ����ֵ���ͻ��˵�ip��ַ����"192.168.1.100"��
	void GetIP(std::string & IPdotdec);
	
	//char *GetIP();

	// ���տͻ��˷��͹��������ݡ�
	// buffer���������ݻ������ĵ�ַ�����ݵĳ��ȴ����m_buflen��Ա�����С�
	// itimeout���ȴ����ݵĳ�ʱʱ�䣬��λ���룬ȱʡֵ��0-���޵ȴ���
	// ����ֵ��true-�ɹ���false-ʧ�ܣ�ʧ�������������1���ȴ���ʱ����Ա����m_btimeout��ֵ������Ϊtrue��2��socket�����Ѳ����á�
	//bool Recv(char *buffer, const int itimeout = 0);
	bool Recv(char* buffer, int bufferLength, const int itimeout);
	// ��ͻ��˷������ݡ�
	// buffer�����������ݻ������ĵ�ַ��
	// ibuflen�����������ݵĴ�С����λ���ֽڣ�ȱʡֵΪ0��������͵���ascii�ַ�����ibuflenȡ0������Ƕ����������ݣ�ibuflenΪ���������ݿ�Ĵ�С��
	//itimeout�����յȴ���ʱ��ʱ�䣬��λ���룬ֵ��0-���޵ȴ���
	// ����ֵ��true-�ɹ���false-ʧ�ܣ����ʧ�ܣ���ʾsocket�����Ѳ����á�
	bool Send(const char *buffer, const int ibuflen = 0, const int itimeout = 5);

	// �رռ�����socket����m_listenfd�������ڶ���̷��������ӽ��̴����С�
	void CloseListen();

	// �رտͻ��˵�socket����m_connfd�������ڶ���̷������ĸ����̴����С�
	void CloseClient();

	~CTcpServer();  // ���������Զ��ر�socket���ͷ���Դ��
};

