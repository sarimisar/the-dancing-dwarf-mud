/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QTimer>
#include <QTextBrowser>
#include <QGraphicsScene>
#include <QParallelAnimationGroup>
#include <QGraphicsItemAnimation>

#include "connection.h"
#define QMUD_CLIENT
#include "../common.h"

#include "formroomcontent.h"
#include "formdamageinfo.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void messageReceived(QMUD::ClientDataMessage::Message message);

private slots:
    void onDataReceived(QString data);
    void onJsonReceived(QByteArray json);
    void onTimerUpdateLagTimeout();
    void onTimerUpdateStatusTimeout();
    void echo(QString cmd);
    void onRoomContentClicked();
    void updateAttackInfo();
    void onCharacterConnectClicked(QString name);

    void on_lineEdit_returnPressed();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_textBrowserLoot_anchorClicked(const QUrl &arg1);
    void on_textBrowserLootTrash_anchorClicked(const QUrl &arg1);
    void on_textBrowserChat_anchorClicked(const QUrl &arg1);
    void on_lineEdit_textChanged(const QString &arg1);
    void on_textBrowserMain_anchorClicked(const QUrl &arg1);
    void on_pushButtonRequestYes_clicked();
    void on_pushButtonRequestNo_clicked();
    void on_pushButtonAttack_clicked();
    void on_lineEditLoginUsername_returnPressed();
    void on_lineEditLoginPassword_returnPressed();
    void on_pushButtonLoginLogin_clicked();
    void on_pushButtonLoginCreate_clicked();
    void on_pushButtonChListCreate_clicked();

protected:
    void keyPressEvent(QKeyEvent*);
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void parseEscapeSequence(int attribute, QListIterator<QString> &i, QTextCharFormat &textCharFormat, const QTextCharFormat &defaultTextCharFormat);
    void appendEscapeSequenceToTextEdit(QTextBrowser *te, QString data);
    void pushAttackInfo(const QMUD::ClientDataAttackInfo& attackInfo);

private:
    QMap<QMUD::TemporaryStatusType, int> m_mapStatus;

    Ui::MainWindow *ui;

    QVector<FormRoomContent*> m_vRoomContent;

    Connection m_connection;
    QTimer m_timerUpdateLag;
    QTimer m_timerUpdateStatus;

    QString m_strBuffer;

    QStringList m_lHistory;
    int m_iHistoryPos;
    QString m_strHistoryLastNonSendedCommand;

    bool m_bClientDataBasicInfoIsValid;
    QMUD::ClientDataBasicInfo m_clientDataBasicInfo;

    bool m_bClientDataTargetInfoIsValid;
    QMUD::ClientDataTargetInfo m_clientDataTargetInfo;

    bool m_bClientDataRoomContentIsValid;
    QMUD::ClientDataRoomContent m_clientDataRoomContent;

    FormDamageInfo* m_ptrDamageInfoPlayer;
    FormDamageInfo* m_ptrDamageInfoTarget;

    QMap<int, QMap<int, QImage>> m_mapMapsLevel0;
    QMap<int, QMap<int, QImage>> m_mapMapsLevel1;

    QGraphicsScene m_graphicScene;
    QGraphicsPixmapItem *m_ptrGraphicsItemMapLevel0;
    QGraphicsPixmapItem *m_ptrGraphicsItemMapLevel1;
    QGraphicsRectItem *m_ptrGraphicsItemRectShadowTop;
    QGraphicsRectItem *m_ptrGraphicsItemRectShadowLeft;
    QGraphicsRectItem *m_ptrGraphicsItemRectShadowRight;
    QGraphicsRectItem *m_ptrGraphicsItemRectShadowBottom;
    QGraphicsPixmapItem *m_ptrGraphicsItemPixmapPlayerBottom;
    QGraphicsPixmapItem *m_ptrGraphicsItemPixmapPlayerTop;
    QMap<QMUD::IdType, QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>> m_mapChs;
    QMUD::StaticIdType m_iMapIdOnScene;
    QMUD::StaticIdType m_iMapIndexOnScene;

    int m_iTilesSizePixels;
    int m_iVisibleMapRadius;
    int m_iVisibleMapDiameter;

    struct AttackInfo
    {
        AttackInfo() :
            chId(QMUD::IdInvalid),
            counter(0)
        { }

        QString suitorName;
        QString targetName;
        QMUD::IdType chId;

        QMUD::ClientDataAttackInfo clientData;
        int counter;
    };

    QVector<AttackInfo> m_vLastAttackInfo;
    QTimer m_timerUpdateAttackInfo;
};

#endif // MAINWINDOW_H
