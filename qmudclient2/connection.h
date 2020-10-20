/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(QObject* parent = Q_NULLPTR);

    bool connectToHost(QString hostName, quint16 port);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(QByteArray message);

public slots:
    void send(QString str);

private slots:
    void onReadyRead();

private:
    QTcpSocket m_tcpSocket;
    QByteArray m_buffer;
    bool m_bFoundJson;
};

#endif // CONNECTION_H
