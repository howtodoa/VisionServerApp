#ifndef CAMERASTACOMM_H
#define CAMERASTACOMM_H
#include <QTcpSocket>
#include <QHostAddress>
#include <QMutex>
#include "commdatadef.h"
#include <QThread>

//class CameraStaComm : public QTcpSocket
class CameraStaComm : public QObject
{
    Q_OBJECT
public:
    CameraStaComm();
    ~CameraStaComm();

    void    ConnectToServer ();
    void    Close           (bool bManClosed = true);
    bool    WriteData       ( const char *data, int len );
    void    InitSock(const char * IP, int port);
    bool    GetPack(CameraStatus &);

    signals:
    void SocketConnectedSignal(int port);
    void SocketErrorSignal(int port);
    void SocketDisconnectedSignal(int port);
protected:
    void    ParseData   ( const char *data, qint64 len );
    int     PickCameraSta(const char *data, int len);

    int PushPack(const CameraStatus &);

    bool    m_bManClosed;
    bool    m_bConnFlag;
    bool    m_bReConnectFlag;
    QHostAddress m_addr;
    int     m_nPort;

    QMutex  m_lockCameraSta;
    std::vector<CameraStatus> m_vecCameraSta;

    QTcpSocket * m_pSock;
    QThread *m_pThread;

signals:
    void    sig_connect();
    void    sig_disconnect();
    void    sig_writeData       ( const char *data, int len );
    void    sig_writeData       ( QByteArray data );

private slots:
    void    slot_connect();
    void    slot_disconnect();
    void    slot_writeData       ( const char *data, int len );
    void    slot_writeData       ( QByteArray data );
    void    OnConnected ();
    void    OnDisConnected ();
    void    OnReadyRead ();
    void    OnError (QAbstractSocket::SocketError);

private:
    enum { BUFFERSIZE_MAX = 8192 };
    enum { MAX_CSTA_VEC_LEN = 20 };
};

#endif // CAMERASTACOMM_H
