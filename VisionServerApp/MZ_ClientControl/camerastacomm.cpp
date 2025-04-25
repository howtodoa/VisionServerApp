#include "camerastacomm.h"
#include <QHostAddress>
#include <QTimer>

CameraStaComm::CameraStaComm()
    : m_bManClosed(false)
    , m_bConnFlag(false)
    , m_bReConnectFlag(false)
{
    m_pSock = new QTcpSocket();
    m_pThread = new QThread();
    m_pSock->moveToThread(m_pThread);
    this->moveToThread(m_pThread);

    connect( m_pSock, SIGNAL(readyRead()), this, SLOT(OnReadyRead()) );
    connect( this, SIGNAL(sig_connect()), this, SLOT(slot_connect()));
    connect( this, SIGNAL(sig_disconnect()), this, SLOT(slot_disconnect()));
    connect( this, SIGNAL(sig_writeData( const char *, int  )), this, SLOT(slot_writeData( const char *, int  )));
    connect( this, SIGNAL(sig_writeData( QByteArray  )), this, SLOT(slot_writeData( QByteArray  )));
    connect( m_pSock, SIGNAL(connected()), this, SLOT(OnConnected()) );
    connect( m_pSock, SIGNAL(disconnected()), this, SLOT(OnDisConnected()) );
    connect( m_pSock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)) );

    m_pThread->start();
}

CameraStaComm::~CameraStaComm()
{
    Close();
}

void CameraStaComm::InitSock(const char * ip, int port)
{
    QString strip( ip );
    m_addr.setAddress( strip );
    m_nPort = port;

}

void CameraStaComm::ConnectToServer()
{
    emit sig_connect();
}


void CameraStaComm::Close(bool bManClosed)
{
    m_bManClosed = bManClosed;
    emit sig_disconnect();
}

void CameraStaComm::slot_connect()
{
    if (m_pSock)
    {
        m_pSock->connectToHost( m_addr, m_nPort );
    }
}

void CameraStaComm::slot_disconnect()
{
    if (m_pSock)
    {
        if (m_pSock->isOpen())
        {
            m_pSock->disconnectFromHost();
        }
    }
}

void CameraStaComm::slot_writeData( const char *data, int len )
{
    if( NULL == data || len <= 0 )
        return ;
    if (!m_pSock)
        return ;

    qint64 ret = m_pSock->write(data, len);

    if( 0 >= ret )
        return ;

    return ;
}

void CameraStaComm::slot_writeData( QByteArray data )
{
    if( data.length() <= 0 )
        return ;
    if (!m_pSock)
        return ;

    qint64 ret = m_pSock->write(data);

    if( 0 >= ret )
        return ;

    return ;
}

bool CameraStaComm::WriteData( const char *data, int len )
{
    if( NULL == data || len <= 0 )
        return false;

    if (!m_pSock)
        return false;

    if (!m_bConnFlag)
        return false;

    QByteArray s_data(data, len);

    //emit writeData(data, len);
    emit sig_writeData(s_data);

    return true;
}

int CameraStaComm::PickCameraSta(const char *data, int /*len*/)
{
    CameraStatus * pSta = (CameraStatus *)(data+sizeof(SC_PackHead));
    CameraStatus newPack;
    memcpy(&newPack, pSta, sizeof(CameraStatus));
    PushPack(newPack);
    return 1;
}

int CameraStaComm::PushPack(const CameraStatus & pNewPack)
{
    int retSz = 0;
    m_lockCameraSta.lock();
    if (m_vecCameraSta.size() >= MAX_CSTA_VEC_LEN)
    {
        std::vector<CameraStatus>::iterator iter = m_vecCameraSta.begin();
        m_vecCameraSta.erase(iter);
    }
    m_vecCameraSta.push_back(pNewPack);
    retSz = m_vecCameraSta.size();
    m_lockCameraSta.unlock();
    return retSz;
}

bool CameraStaComm::GetPack(CameraStatus & dataPack)
{
    bool bRet = false;
    m_lockCameraSta.lock();
    std::vector<CameraStatus>::iterator iter = m_vecCameraSta.begin();
    if (iter != m_vecCameraSta.end())
    {
        dataPack = *iter;
        m_vecCameraSta.erase(iter);
        bRet = true;
    }
    m_lockCameraSta.unlock();
    return bRet;
}


void CameraStaComm::ParseData( const char *data, qint64 len )
{
    static char s_pack_buf[BUFFERSIZE_MAX] = {0};
    static int s_offset = 0;
    memcpy(s_pack_buf+s_offset, data, len);
    s_offset += len;

    while (s_offset >= (int)sizeof(SC_PackHead))
    {
        int indexStart = -1;
        for (int i = 0; i < s_offset-1; ++i)
        {
            if ( (unsigned char)s_pack_buf[i] == (unsigned char)HEAD_LABEL_SC_1 && (unsigned char)s_pack_buf[i+1] == (unsigned char)HEAD_LABEL_SC_2)
            {
                indexStart = i;break;
            }
        }

        if (indexStart < 0)
        {
            s_offset = 0;
            indexStart = 0;
            memset(s_pack_buf, 0, sizeof(s_pack_buf));
            return;
        }
        else if (indexStart > 0)
        {
            s_offset -= indexStart;
            memcpy(s_pack_buf, s_pack_buf+indexStart, s_offset);
            indexStart = 0;
        }

        //
        SC_PackHead * pHead = (SC_PackHead *)s_pack_buf;
        int data_len = NTOHL((unsigned int)(pHead->data_len));
        data_len = data_len + sizeof(SC_PackHead);
        if (data_len > (int)(sizeof(CameraStatus)+sizeof(SC_PackHead)))
        {
            s_offset = 0;
            indexStart = 0;
            memset(s_pack_buf, 0, sizeof(s_pack_buf));
            return;
        }
        if (s_offset < data_len)
        {
            return;
        }

        if (pHead->operate_type == SC_CAMERA_STATUS)
        {
            PickCameraSta(s_pack_buf, data_len);
        }

        //移动剩下的包数据
        s_offset -= data_len;
        memcpy(s_pack_buf, s_pack_buf+data_len, s_offset);
    }
}

void CameraStaComm::OnReadyRead()
{
    char data[BUFFERSIZE_MAX] = { 0 };
    qint64 len = m_pSock->read( data, BUFFERSIZE_MAX );
    if( len <= 0 )
        return;
    else
        ParseData( data, len );
}

void CameraStaComm::OnConnected()
{
    SocketConnectedSignal(m_nPort);
    m_bConnFlag = true;
    m_bManClosed = false;
    m_bReConnectFlag = false;
    printf( "CameraStaComm connected: %d !\n", m_pSock->socketDescriptor() );
}

void CameraStaComm::OnDisConnected()
{
    m_bConnFlag = false;
    SocketDisconnectedSignal(m_nPort);
    if (!m_bManClosed)
    {
        if (!m_bReConnectFlag)
        {
            m_bReConnectFlag = true;
            //重连...
            ConnectToServer();
        }
    }
    printf( "disconnected: %d !\n", m_pSock->socketDescriptor() );
}

void CameraStaComm::OnError(QAbstractSocket::SocketError err)
{
    SocketErrorSignal(m_nPort);
    if (err == QAbstractSocket::ConnectionRefusedError)
    {
        if (!m_bManClosed)
        {
            m_bReConnectFlag = true;
            //重连...
            QTimer::singleShot(0, this, SLOT(slot_connect()));
        }
    }
    else
    {
        if (!m_bManClosed)
        {
            if (!m_bReConnectFlag)
            {
                m_bReConnectFlag = true;
                QTimer::singleShot(0, this, SLOT(slot_disconnect()));
                //重连...
                QTimer::singleShot(0, this, SLOT(slot_connect()));
                qDebug()<<"RemoteHostClosedError    reconnect";
            }
        }
    }
}
