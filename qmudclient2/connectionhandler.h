#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlContext>

#include "connection.h"
#include "characterlistitemmodel.h"

class ConnectionHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStatusChange)

    Q_PROPERTY(PageType currentPage READ currentPage NOTIFY currentPageChange)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY lastMessageChange)
    Q_PROPERTY(QString lastInfoMessage READ lastInfoMessage NOTIFY lastMessageChange)

    Q_PROPERTY(int playerHpCurrent READ playerHpCurrent NOTIFY playerLifeChange)
    Q_PROPERTY(int playerHpMaximum READ playerHpMaximum NOTIFY playerLifeChange)

public:
    enum class PageType
    {
        LOGIN,
        CHARACTERS_LIST,
        IN_GAME
    };
    Q_ENUMS(PageType)

    static void declareQML();
    void declareQMLContext(QQmlEngine* engine);

    explicit ConnectionHandler(QObject *parent = nullptr);

    Q_INVOKABLE void clearMessages();
    Q_INVOKABLE void cmdLogin(QString username, QString password);
    Q_INVOKABLE void cmdSignin(QString username, QString password);
    Q_INVOKABLE void cmdConnect(QString chName);

public slots:
    bool connectToHost();

signals:
    void connectionStatusChange();
    void currentPageChange();
    void lastMessageChange();
    void playerLifeChange();

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(QByteArray message);

    bool isConnected() const;

    PageType currentPage() const;
    QString lastErrorMessage() const;
    QString lastInfoMessage() const;

    int playerHpCurrent() const;
    int playerHpMaximum() const;

private:
    Connection *m_ptrConnection;

    CharacterListItemModel m_charactersModel;

    bool m_bIsConnected;

    PageType m_eCurrentPage;
    QString m_strLastErrorMessage;
    QString m_strLastInfoMessage;

    int m_iPlayerHpCurrent;
    int m_iPlayerHpMaximum;
};

#endif // CONNECTIONHANDLER_H
