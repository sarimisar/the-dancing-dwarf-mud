/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "connection.h"

#include <QHostAddress>

Connection::Connection(QObject *parent) :
    QObject(parent),
    m_bFoundJson(false)
{
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

bool Connection::connectToHost(QString hostName, quint16 port)
{
    m_tcpSocket.connectToHost(hostName, port);
    m_tcpSocket.waitForConnected();

    if (m_tcpSocket.state() != QTcpSocket::ConnectedState)
    {
        m_tcpSocket.disconnectFromHost();
        return false;
    }

    m_tcpSocket.write("C641BA2B12D5569ABB7DFDE5136C9");

    return true;
}

void Connection::send(QString str)
{
    m_tcpSocket.write(str.toUtf8() + "\n");
}

bool Connection::isConnected() const
{
    return m_tcpSocket.state() == QTcpSocket::ConnectedState;
}

void Connection::onReadyRead()
{
    m_buffer.append(m_tcpSocket.readAll());

    while(true)
    {
        if (!m_bFoundJson)
        {
            int len = 0;

            for (int i=0; i<m_buffer.size(); i++)
            {
                if (m_buffer[i] == static_cast<char>(29))
                {
                    len = i;
                    m_bFoundJson = true;
                    break;
                }
            }

            if (!m_bFoundJson)
                len = m_buffer.length();

            if (len != 0)
            {
                emit dataReceived(QString::fromUtf8(m_buffer.left(len)));
                m_buffer.remove(0, len);
            }

            if (m_buffer.isEmpty())
                return;
        }
        else
        {
           int len = 0;

            for (int i=1; i<m_buffer.size(); i++)
            {
                if (m_buffer[i] == static_cast<char>(29))
                {
                    len = i - 1;
                    m_bFoundJson = false;
                    break;
                }
            }

            if (len != 0)
            {
                emit jsonReceived(m_buffer.mid(1, len));
                m_buffer.remove(0, len + 2);
            }
            else
                return;

            if (m_buffer.isEmpty())
                return;
        }
    }
}
