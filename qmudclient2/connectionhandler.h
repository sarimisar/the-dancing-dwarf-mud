#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPoint>

#include "connection.h"
#include "characterlistitemmodel.h"
#include "../common.h"

class ConnectionHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStatusChange)

    Q_PROPERTY(PageType currentPage READ currentPage NOTIFY currentPageChange)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY lastMessageChange)
    Q_PROPERTY(QString lastInfoMessage READ lastInfoMessage NOTIFY lastMessageChange)

    Q_PROPERTY(QPoint playerPosition READ playerPosition NOTIFY playerPositionChange)
    Q_PROPERTY(QPoint playerMapId READ playerMapId NOTIFY playerMapIdChange)

    Q_PROPERTY(QString playerName READ playerName NOTIFY playerBasicInfoChange)
    Q_PROPERTY(int playerLevel READ playerLevel NOTIFY playerBasicInfoChange)
    Q_PROPERTY(QString playerClass READ playerClass NOTIFY playerBasicInfoChange)
    Q_PROPERTY(QString playerRace READ playerRace NOTIFY playerBasicInfoChange)
    Q_PROPERTY(int playerExperience READ playerExperience NOTIFY playerBasicInfoChange)
    Q_PROPERTY(int playerExperienceToLevelUp READ playerExperienceToLevelUp NOTIFY playerBasicInfoChange)

    Q_PROPERTY(int playerHpCurrent READ playerHpCurrent NOTIFY playerLifeChange)
    Q_PROPERTY(int playerHpMaximum READ playerHpMaximum NOTIFY playerLifeChange)
    Q_PROPERTY(int playerMpCurrent READ playerMpCurrent NOTIFY playerLifeChange)
    Q_PROPERTY(int playerMpMaximum READ playerMpMaximum NOTIFY playerLifeChange)
    Q_PROPERTY(int playerApCurrent READ playerApCurrent NOTIFY playerLifeChange)
    Q_PROPERTY(int playerApMaximum READ playerApMaximum NOTIFY playerLifeChange)

    Q_PROPERTY(QMUD::IdType selectedPcId READ selectedPcId NOTIFY selectedPcChange)
    Q_PROPERTY(QString selectedPcName READ selectedPcName NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcLevel READ selectedPcLevel NOTIFY selectedPcChange)
    Q_PROPERTY(QString selectedPcClass READ selectedPcClass NOTIFY selectedPcChange)
    Q_PROPERTY(QString selectedPcRace READ selectedPcRace NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcHpCurrent READ selectedPcHpCurrent NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcHpMaximum READ selectedPcHpMaximum NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcMpCurrent READ selectedPcMpCurrent NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcMpMaximum READ selectedPcMpMaximum NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcApCurrent READ selectedPcApCurrent NOTIFY selectedPcChange)
    Q_PROPERTY(int selectedPcApMaximum READ selectedPcApMaximum NOTIFY selectedPcChange)

    Q_PROPERTY(QStringList npcs READ npcs NOTIFY npcsChange)

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
    Q_INVOKABLE void cmdChCreate(QString name, int race, int chClass, int sex);
    Q_INVOKABLE void cmdConnect(QString chName);
    Q_INVOKABLE void cmdMoveNorth();
    Q_INVOKABLE void cmdMoveSouth();
    Q_INVOKABLE void cmdMoveWeast();
    Q_INVOKABLE void cmdMoveEast();
    Q_INVOKABLE void cmdMoveUp();
    Q_INVOKABLE void cmdMoveDown();
    Q_INVOKABLE void cmdTarget(QMUD::StaticIdType id);
    Q_INVOKABLE void cmdAction(QString action);

public slots:
    bool connectToHost();

signals:
    void connectionStatusChange();
    void currentPageChange();
    void lastMessageChange();
    void playerBasicInfoChange();
    void playerLifeChange();
    void playerPositionChange();
    void playerMapIdChange();
    void selectedPcChange();
    void npcsChange();

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(QByteArray message);

    bool isConnected() const;

    PageType currentPage() const;
    QString lastErrorMessage() const;
    QString lastInfoMessage() const;

    QPoint playerPosition() const;
    QPoint playerMapId() const;

    QString playerName() const;
    int playerLevel() const;
    QString playerClass() const;
    QString playerRace() const;
    int playerExperience() const;
    int playerExperienceToLevelUp() const;

    int playerHpCurrent() const;
    int playerHpMaximum() const;
    int playerMpCurrent() const;
    int playerMpMaximum() const;
    int playerApCurrent() const;
    int playerApMaximum() const;

    QMUD::IdType selectedPcId() const;
    QString selectedPcName() const;
    int selectedPcLevel() const;
    QString selectedPcClass() const;
    QString selectedPcRace() const;
    int selectedPcHpCurrent() const;
    int selectedPcHpMaximum() const;
    int selectedPcMpCurrent() const;
    int selectedPcMpMaximum() const;
    int selectedPcApCurrent() const;
    int selectedPcApMaximum() const;

    QStringList npcs() const;

private:
    Connection *m_ptrConnection;

    CharacterListItemModel m_charactersModel;

    bool m_bIsConnected;

    PageType m_eCurrentPage;
    QString m_strLastErrorMessage;
    QString m_strLastInfoMessage;

    // Mappa e posizione del player
    QPoint m_ptPlayerPosition;
    QPoint m_ptPlayerMapId;

    // Player basic info
    QString m_strPlayerName;
    int m_iPlayerLevel;
    QString m_strPlayerClass;
    QString m_strPlayerRace;
    int m_iPlayerExperience;
    int m_iPlayerExperienceToLevelUp;

    // Player info
    int m_iPlayerHpCurrent;
    int m_iPlayerHpMaximum;
    int m_iPlayerMpCurrent;
    int m_iPlayerMpMaximum;
    int m_iPlayerApCurrent;
    int m_iPlayerApMaximum;

    // Selected PC info
    QMUD::IdType m_iSelectedPcId;
    QString m_strSelectedPcName;
    int m_iSelectedPcLevel;
    QString m_strSelectedPcClass;
    QString m_strSelectedPcRace;
    int m_iSelectedPcHpCurrent;
    int m_iSelectedPcHpMaximum;
    int m_iSelectedPcMpCurrent;
    int m_iSelectedPcMpMaximum;
    int m_iSelectedPcApCurrent;
    int m_iSelectedPcApMaximum;

    // Npcs info
    QStringList m_vNpcs;
};

#endif // CONNECTIONHANDLER_H
