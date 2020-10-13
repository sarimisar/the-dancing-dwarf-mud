/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>

namespace NET
{

class NetConnection;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = Q_NULLPTR);

signals:
    void newConnection(NET::NetConnection *conn);

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);
};

} // namespace NET

#endif // SERVER_H
