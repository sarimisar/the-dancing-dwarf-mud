/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "server.h"
#include "netconnection.h"
#include "globaldebug.h"

using namespace NET;

Server::Server(QObject *parent) :
    QTcpServer(parent)
{

}

void Server::incomingConnection(qintptr socketDescriptor)
{
    NetConnection *conn = new NetConnection(this);

    if (conn->setSocketDescriptor(socketDescriptor))
    {
        conn->init();

        GLOBAL_INFO(tr("Nuova connessione da %1").arg(conn->connectionAddress()));

        emit newConnection(conn);
    }
    else
    {
        GLOBAL_CRITICAL(tr("Connessione fallita (setSocketDescriptor)"));

        conn->deleteLater();
    }
}
