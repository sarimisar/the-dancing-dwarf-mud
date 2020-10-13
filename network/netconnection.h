/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>

namespace NET
{

class NetConnection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit NetConnection(QObject *parent = Q_NULLPTR);

    void init();
    void send(QString str);
    void sendData(QByteArray data);

    bool isCustomClient() const;

public:
    QString connectionAddress();

signals:
    void commandReceived(QString str);

public slots:

private slots:
    void onReadyRead();

private:
    bool m_bCustomClient;

    QString m_strPeerAddress;
    quint16 m_uiPeerPort;
    quint16 m_uiLocalPort;

    QByteArray m_bufData;

    bool m_bFirstByteReceived;
};

} // namespace NET

#endif // CONNECTION_H
