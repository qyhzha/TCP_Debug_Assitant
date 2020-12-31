#ifndef TCPSOCKER_H
#define TCPSOCKER_H

#include <QTcpServer>
#include <QTcpSocket>

class TcpSocker : public QTcpSocket
{
    public:
        TcpSocker();
};

#endif // TCPSOCKER_H
