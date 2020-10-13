/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "netconnection.h"

#include <QHostAddress>
#include <QDebug>

using namespace NET;

NetConnection::NetConnection(QObject *parent) :
    QTcpSocket(parent),
    m_bCustomClient(false),
    m_bFirstByteReceived(false)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void NetConnection::init()
{
    m_strPeerAddress = peerAddress().toString();
    m_uiPeerPort = peerPort();
    m_uiLocalPort = localPort();
}

void NetConnection::send(QString str)
{
    write(str.toLocal8Bit());
}

void NetConnection::sendData(QByteArray data)
{
    if (m_bCustomClient)
        write(data);
}

bool NetConnection::isCustomClient() const
{
    return m_bCustomClient;
}

QString NetConnection::connectionAddress()
{
    return QString("%1:%2:%3").arg(m_strPeerAddress).arg(m_uiPeerPort).arg(m_uiLocalPort);
}

void NetConnection::onReadyRead()
{
    m_bufData.append(readAll());

    bool bCommandReceived = false;

    do
    {
        bCommandReceived = false;

        if (!m_bFirstByteReceived)
        {
            QByteArray key = QString("C641BA2B12D5569ABB7DFDE5136C9").toLatin1();

            if (m_bufData.size() >= key.size())
            {
                if (m_bufData.left(key.size()) == key)
                {
                    m_bFirstByteReceived = true;
                    m_bCustomClient = true;

                    m_bufData.remove(0, key.size());
                }
            }
        }

        for (int i = 0; i < m_bufData.size(); i++)
        {
            if (m_bufData[i] == '\n')
            {
                if (!m_bFirstByteReceived)
                {
                    m_bFirstByteReceived = true;
                    m_bCustomClient = false;
                }

                QByteArray cmd = m_bufData.left(i+1);
                m_bufData.remove(0, i+1);

                emit commandReceived(cmd.simplified());

                bCommandReceived = true;
            }
        }
    } while(bCommandReceived);
}
