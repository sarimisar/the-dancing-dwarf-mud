/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formcharacterinfo.h"
#include "formcreatecharacter.h"

#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QKeyEvent>
#include <QShortcut>
#include <QMessageBox>
#include <QDir>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QGraphicsEffect>
#include <QTimeLine>
#include <QOpenGLWidget>

// Calcolo le stringhe del danno
QString damageStringPlayer;
QString damageStringTarget;
QString damageStringOthers;

//switch (attackDamageType())
//{
//case QMUD::DamageType::PIERCING:
//    damageStringPlayer = tr("Trafiggi");
//    damageStringTarget = tr("ti trafigge");
//    damageStringOthers = tr("trafigge");
//    break;
//
//case QMUD::DamageType::BLUDGEONING:
//    damageStringPlayer = tr("Percuoti");
//    damageStringTarget = tr("ti percuote");
//    damageStringOthers = tr("percuote");
//    break;
//
//case QMUD::DamageType::SLASHING:
//    damageStringPlayer = tr("Tagli");
//    damageStringTarget = tr("ti taglia");
//    damageStringOthers = tr("taglia");
//    break;
//
//default:
//    damageStringPlayer = tr("Colpisci");
//    damageStringTarget = tr("ti colpisce");
//    damageStringOthers = tr("colpisce");
//}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_iHistoryPos(0),
    m_bClientDataBasicInfoIsValid(false),
    m_bClientDataTargetInfoIsValid(false),
    m_bClientDataRoomContentIsValid(false),
    m_ptrGraphicsItemMapLevel0(Q_NULLPTR),
    m_ptrGraphicsItemMapLevel1(Q_NULLPTR),
    m_ptrGraphicsItemRectShadowTop(Q_NULLPTR),
    m_ptrGraphicsItemRectShadowLeft(Q_NULLPTR),
    m_ptrGraphicsItemRectShadowRight(Q_NULLPTR),
    m_ptrGraphicsItemRectShadowBottom(Q_NULLPTR),
    m_ptrGraphicsItemPixmapPlayerBottom(Q_NULLPTR),
    m_ptrGraphicsItemPixmapPlayerTop(Q_NULLPTR),
    m_iMapIdOnScene(QMUD::StaticIdInvalid),
    m_iMapIndexOnScene(QMUD::StaticIdInvalid),
    m_iTilesSizePixels(80),
    m_iVisibleMapRadius(8),
    m_iVisibleMapDiameter(17)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->pageLogin);

    // Pagina di login
    ui->labelLoginLoginFailed->setVisible(false);
    ui->labelLoginUsernameFailed->setVisible(false);
    ui->labelLoginPasswordFailed->setVisible(false);
    ui->labelLoginContactAdmin->setVisible(false);
    ui->labelLoginSigninOk->setVisible(false);

    connect(&m_connection, &Connection::dataReceived, this, &MainWindow::onDataReceived);
    connect(&m_connection, &Connection::jsonReceived, this, &MainWindow::onJsonReceived);
    connect(&m_timerUpdateLag, &QTimer::timeout, this, &MainWindow::onTimerUpdateLagTimeout);
    connect(&m_timerUpdateStatus, &QTimer::timeout, this, &MainWindow::onTimerUpdateStatusTimeout);
    connect(ui->lineEdit, &CustomLineEdit::appendStr, this, &MainWindow::echo);

    m_timerUpdateLag.setInterval(100);
    m_timerUpdateLag.setSingleShot(false);

    m_timerUpdateStatus.setInterval(1000);
    m_timerUpdateStatus.setSingleShot(false);

    ui->textBrowserMain->ensureCursorVisible();

    ui->progressBarLag->setValue(0);
    ui->progressBarLag->setMaximum(1);

    ui->widgetCharInfo->setVisible(false);
    ui->widgetChat->setVisible(false);
    ui->widgetLoot->setVisible(false);
    ui->widgetTargetInfo->setVisible(false);
    ui->widgetRequest->setVisible(false);
    ui->widgetAggro->setVisible(false);
    ui->widgetMiniMap->setVisible(false);
    ui->widgetActionBar->setVisible(false);
    ui->widgetMain->setVisible(true);
    ui->widgetCombatInfo->setVisible(true);
    ui->lineEdit->setConnection(&m_connection);
    ui->lineEdit->installEventFilter(this);

    ui->graphicsViewMap->setScene(&m_graphicScene);
    ui->graphicsViewMiniMap->setScene(&m_graphicScene);
    ui->graphicsViewMiniMap->scale(0.1, 0.1);
    //ui->graphicsViewMiniMap->scale(0.2, 0.2);

    m_ptrDamageInfoPlayer = new FormDamageInfo(ui->pageInGame);
    m_ptrDamageInfoTarget = new FormDamageInfo(ui->pageInGame);
    m_ptrDamageInfoPlayer->raise();
    m_ptrDamageInfoTarget->raise();

    m_vLastAttackInfo.resize(10);

    connect(&m_timerUpdateAttackInfo, &QTimer::timeout, this, &MainWindow::updateAttackInfo);

    m_timerUpdateAttackInfo.setInterval(100);
    m_timerUpdateAttackInfo.setSingleShot(false);
    m_timerUpdateAttackInfo.start();

    // Carico le mappe
    QDir mapsDir = QDir::current();
    mapsDir.cd("maps");
    QStringList mapFileList = mapsDir.entryList(QDir::Files);

    for (QString file : mapFileList)
    {
        if (QFileInfo(file).suffix() == "png" &&
                QFileInfo(file).baseName().endsWith("_0"))
        {
            QStringList values = QFileInfo(file).baseName().split("_");
            m_mapMapsLevel0[values[values.size() - 3].toInt()][values[values.size() - 2].toInt()].load(mapsDir.path() + "/" + file);
        }
        else if (QFileInfo(file).suffix() == "png" &&
                 QFileInfo(file).baseName().endsWith("_1"))
        {
            QStringList values = QFileInfo(file).baseName().split("_");
            m_mapMapsLevel1[values[values.size() - 3].toInt()][values[values.size() - 2].toInt()].load(mapsDir.path() + "/" + file);
        }
    }

    if (!m_connection.connectToHost("localhost", 8060))
        m_connection.connectToHost("2.224.131.179", 8060);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static QColor XTERM_COLOR[] = {
    QColor(0,0,0),
    QColor(128,0,0),
    QColor(0,128,0),
    QColor(128,128,0),
    QColor(0,0,128),
    QColor(128,0,128),
    QColor(0,128,128),
    QColor(192,192,192),
    QColor(128,128,128),
    QColor(255,0,0),
    QColor(0,255,0),
    QColor(255,255,0),
    QColor(0,0,255),
    QColor(255,0,255),
    QColor(0,255,255),
    QColor(255,255,255),
    QColor(0,0,0),
    QColor(0,0,95),
    QColor(0,0,135),
    QColor(0,0,175),
    QColor(0,0,215),
    QColor(0,0,255),
    QColor(0,95,0),
    QColor(0,95,95),
    QColor(0,95,135),
    QColor(0,95,175),
    QColor(0,95,215),
    QColor(0,95,255),
    QColor(0,135,0),
    QColor(0,135,95),
    QColor(0,135,135),
    QColor(0,135,175),
    QColor(0,135,215),
    QColor(0,135,255),
    QColor(0,175,0),
    QColor(0,175,95),
    QColor(0,175,135),
    QColor(0,175,175),
    QColor(0,175,215),
    QColor(0,175,255),
    QColor(0,215,0),
    QColor(0,215,95),
    QColor(0,215,135),
    QColor(0,215,175),
    QColor(0,215,215),
    QColor(0,215,255),
    QColor(0,255,0),
    QColor(0,255,95),
    QColor(0,255,135),
    QColor(0,255,175),
    QColor(0,255,215),
    QColor(0,255,255),
    QColor(95,0,0),
    QColor(95,0,95),
    QColor(95,0,135),
    QColor(95,0,175),
    QColor(95,0,215),
    QColor(95,0,255),
    QColor(95,95,0),
    QColor(95,95,95),
    QColor(95,95,135),
    QColor(95,95,175),
    QColor(95,95,215),
    QColor(95,95,255),
    QColor(95,135,0),
    QColor(95,135,95),
    QColor(95,135,135),
    QColor(95,135,175),
    QColor(95,135,215),
    QColor(95,135,255),
    QColor(95,175,0),
    QColor(95,175,95),
    QColor(95,175,135),
    QColor(95,175,175),
    QColor(95,175,215),
    QColor(95,175,255),
    QColor(95,215,0),
    QColor(95,215,95),
    QColor(95,215,135),
    QColor(95,215,175),
    QColor(95,215,215),
    QColor(95,215,255),
    QColor(95,255,0),
    QColor(95,255,95),
    QColor(95,255,135),
    QColor(95,255,175),
    QColor(95,255,215),
    QColor(95,255,255),
    QColor(135,0,0),
    QColor(135,0,95),
    QColor(135,0,135),
    QColor(135,0,175),
    QColor(135,0,215),
    QColor(135,0,255),
    QColor(135,95,0),
    QColor(135,95,95),
    QColor(135,95,135),
    QColor(135,95,175),
    QColor(135,95,215),
    QColor(135,95,255),
    QColor(135,135,0),
    QColor(135,135,95),
    QColor(135,135,135),
    QColor(135,135,175),
    QColor(135,135,215),
    QColor(135,135,255),
    QColor(135,175,0),
    QColor(135,175,95),
    QColor(135,175,135),
    QColor(135,175,175),
    QColor(135,175,215),
    QColor(135,175,255),
    QColor(135,215,0),
    QColor(135,215,95),
    QColor(135,215,135),
    QColor(135,215,175),
    QColor(135,215,215),
    QColor(135,215,255),
    QColor(135,255,0),
    QColor(135,255,95),
    QColor(135,255,135),
    QColor(135,255,175),
    QColor(135,255,215),
    QColor(135,255,255),
    QColor(175,0,0),
    QColor(175,0,95),
    QColor(175,0,135),
    QColor(175,0,175),
    QColor(175,0,215),
    QColor(175,0,255),
    QColor(175,95,0),
    QColor(175,95,95),
    QColor(175,95,135),
    QColor(175,95,175),
    QColor(175,95,215),
    QColor(175,95,255),
    QColor(175,135,0),
    QColor(175,135,95),
    QColor(175,135,135),
    QColor(175,135,175),
    QColor(175,135,215),
    QColor(175,135,255),
    QColor(175,175,0),
    QColor(175,175,95),
    QColor(175,175,135),
    QColor(175,175,175),
    QColor(175,175,215),
    QColor(175,175,255),
    QColor(175,215,0),
    QColor(175,215,95),
    QColor(175,215,135),
    QColor(175,215,175),
    QColor(175,215,215),
    QColor(175,215,255),
    QColor(175,255,0),
    QColor(175,255,95),
    QColor(175,255,135),
    QColor(175,255,175),
    QColor(175,255,215),
    QColor(175,255,255),
    QColor(215,0,0),
    QColor(215,0,95),
    QColor(215,0,135),
    QColor(215,0,175),
    QColor(215,0,215),
    QColor(215,0,255),
    QColor(215,95,0),
    QColor(215,95,95),
    QColor(215,95,135),
    QColor(215,95,175),
    QColor(215,95,215),
    QColor(215,95,255),
    QColor(215,135,0),
    QColor(215,135,95),
    QColor(215,135,135),
    QColor(215,135,175),
    QColor(215,135,215),
    QColor(215,135,255),
    QColor(215,175,0),
    QColor(215,175,95),
    QColor(215,175,135),
    QColor(215,175,175),
    QColor(215,175,215),
    QColor(215,175,255),
    QColor(215,215,0),
    QColor(215,215,95),
    QColor(215,215,135),
    QColor(215,215,175),
    QColor(215,215,215),
    QColor(215,215,255),
    QColor(215,255,0),
    QColor(215,255,95),
    QColor(215,255,135),
    QColor(215,255,175),
    QColor(215,255,215),
    QColor(215,255,255),
    QColor(255,0,0),
    QColor(255,0,95),
    QColor(255,0,135),
    QColor(255,0,175),
    QColor(255,0,215),
    QColor(255,0,255),
    QColor(255,95,0),
    QColor(255,95,95),
    QColor(255,95,135),
    QColor(255,95,175),
    QColor(255,95,215),
    QColor(255,95,255),
    QColor(255,135,0),
    QColor(255,135,95),
    QColor(255,135,135),
    QColor(255,135,175),
    QColor(255,135,215),
    QColor(255,135,255),
    QColor(255,175,0),
    QColor(255,175,95),
    QColor(255,175,135),
    QColor(255,175,175),
    QColor(255,175,215),
    QColor(255,175,255),
    QColor(255,215,0),
    QColor(255,215,95),
    QColor(255,215,135),
    QColor(255,215,175),
    QColor(255,215,215),
    QColor(255,215,255),
    QColor(255,255,0),
    QColor(255,255,95),
    QColor(255,255,135),
    QColor(255,255,175),
    QColor(255,255,215),
    QColor(255,255,255),
    QColor(8,8,8),
    QColor(18,18,18),
    QColor(28,28,28),
    QColor(38,38,38),
    QColor(48,48,48),
    QColor(58,58,58),
    QColor(68,68,68),
    QColor(78,78,78),
    QColor(88,88,88),
    QColor(98,98,98),
    QColor(108,108,108),
    QColor(118,118,118),
    QColor(128,128,128),
    QColor(138,138,138),
    QColor(148,148,148),
    QColor(158,158,158),
    QColor(168,168,168),
    QColor(178,178,178),
    QColor(188,188,188),
    QColor(198,198,198),
    QColor(208,208,208),
    QColor(218,218,218),
    QColor(228,228,228),
    QColor(238,238,238)
};

void escapeSequenceParse(QVector<QString> values, QTextCharFormat & textCharFormat)
{
    if (values.size() == 1)
    {
        if (values[0] == "40") textCharFormat.setBackground(QColor(0, 0, 0));
        else if (values[0] == "41") textCharFormat.setBackground(QColor(128, 0, 0));
        else if (values[0] == "42") textCharFormat.setBackground(QColor(0, 128, 0));
        else if (values[0] == "43") textCharFormat.setBackground(QColor(128, 128, 0));
        else if (values[0] == "44") textCharFormat.setBackground(QColor(0, 0, 128));
        else if (values[0] == "45") textCharFormat.setBackground(QColor(128, 0, 128));
        else if (values[0] == "46") textCharFormat.setBackground(QColor(0, 128, 128));
        else if (values[0] == "47") textCharFormat.setBackground(QColor(192, 192, 192));
        else if (values[0] == "0")
        {
            textCharFormat.setFontWeight(QFont::Light);
            textCharFormat.setFontItalic(false);
            textCharFormat.setFontUnderline(false);
            textCharFormat.setFontStrikeOut(false);
            textCharFormat.setForeground(QColor(192, 192, 192));
            textCharFormat.setBackground(Qt::transparent);
        }
        else if (values[0] == "1") textCharFormat.setFontWeight(QFont::Bold);
        else if (values[0] == "3") textCharFormat.setFontItalic(true);
        else if (values[0] == "4") textCharFormat.setFontUnderline(true);
        else if (values[0] == "7")
        {
            QBrush foregroundBrush = textCharFormat.foreground();
            textCharFormat.setForeground(textCharFormat.background());
            textCharFormat.setBackground(foregroundBrush);
        }
        else if (values[0] == "9") textCharFormat.setFontStrikeOut(true);
        else
        {
            textCharFormat.setAnchorHref(values[0]);
        }
    }
    else if (values.size() == 2)
    {
        if (values[0] == "0")
        {
            switch (values[1].toInt())
            {
            case 30: textCharFormat.setForeground(QColor(0, 0, 0)); break;
            case 31: textCharFormat.setForeground(QColor(128, 0, 0)); break;
            case 32: textCharFormat.setForeground(QColor(0, 128, 0)); break;
            case 33: textCharFormat.setForeground(QColor(128, 128, 0)); break;
            case 34: textCharFormat.setForeground(QColor(0, 0, 128)); break;
            case 35: textCharFormat.setForeground(QColor(128, 0, 128)); break;
            case 36: textCharFormat.setForeground(QColor(0, 128, 128)); break;
            case 37: textCharFormat.setForeground(QColor(192, 192, 192)); break;
            }
        }
        else if (values[0] == "1")
        {
            switch (values[1].toInt())
            {
            case 30: textCharFormat.setForeground(QColor(128, 128, 128)); break;
            case 31: textCharFormat.setForeground(QColor(255, 0, 0)); break;
            case 32: textCharFormat.setForeground(QColor(0, 255, 0)); break;
            case 33: textCharFormat.setForeground(QColor(255, 255, 0)); break;
            case 34: textCharFormat.setForeground(QColor(0, 0, 255)); break;
            case 35: textCharFormat.setForeground(QColor(255, 0, 255)); break;
            case 36: textCharFormat.setForeground(QColor(0, 255, 255)); break;
            case 37: textCharFormat.setForeground(QColor(255, 255, 255)); break;
            }
        }
    }
    else if (values.size() == 3)
    {
        if (values[0] == "38")
        {
            if (values[1] == "5")
            {
                if (values[2].toInt() >= 0 && values[2].toInt() < 256)
                    textCharFormat.setForeground(XTERM_COLOR[values[2].toInt()]);
            }
        }
        else if (values[0] == "48")
        {
            if (values[1] == "5")
            {
                if (values[2].toInt() >= 0 && values[2].toInt() < 256)
                    textCharFormat.setBackground(XTERM_COLOR[values[2].toInt()]);
            }
        }
    }
}

void MainWindow::appendEscapeSequenceToTextEdit(QTextBrowser *te, QString data)
{  
    QRegExp const escapeSequenceExpression(R"(\033\[([\d;]+)m|\034\[([\da-zA-Z.9 '\"]*)\])");
    int offset = escapeSequenceExpression.indexIn(data);

    te->moveCursor(QTextCursor::End);
    te->insertPlainText(data.mid(0, offset));

    QTextCharFormat textCharFormat;// = te->currentCharFormat();

    while (!(offset < 0))
    {
        int previousOffset = offset + escapeSequenceExpression.matchedLength();
        QStringList capturedTexts = escapeSequenceExpression.capturedTexts().first().split(';');
        capturedTexts[0] = capturedTexts[0].remove(0, 2);
        capturedTexts.last() = capturedTexts.last().remove(capturedTexts.last().size()-1, 1);

        QVector<QString> escapeCodes;

        for (auto val : capturedTexts)
            escapeCodes.push_back(val);

        escapeSequenceParse(escapeCodes, textCharFormat);

        offset = escapeSequenceExpression.indexIn(data, previousOffset);

        if (offset < 0)
        {
            te->setCurrentCharFormat(textCharFormat);
            te->insertPlainText(data.mid(previousOffset));
        }
        else
        {
            te->setCurrentCharFormat(textCharFormat);
            te->insertPlainText(data.mid(previousOffset, offset - previousOffset));
        }
    }

    QTextCursor cursor = te->textCursor();
    cursor.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::MoveMode::MoveAnchor);
    te->setTextCursor(cursor);

    te->ensureCursorVisible();
}

void MainWindow::updateAttackInfo()
{
    int fontSize = 14;

    QStringList text;

    for (auto it = m_vLastAttackInfo.rbegin(); it != m_vLastAttackInfo.rend(); ++it)
    {
        if (it->counter > 0)
        {
            if (it->clientData.isDamage())
            {
                QString targetName = it->targetName;
                QString suitorName = it->suitorName;
                int alpha = it->counter;

                if (it->chId == it->clientData.suitorId())
                {
                    if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_DODGE)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">%2 schiva il tuo attacco</span>").arg(alpha).arg(targetName));
                    else if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_PARRY)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">%2 para il tuo attacco</span>").arg(alpha).arg(targetName));
                    else if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_ILLUSORY_IMAGE)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">Colpisci una delle immagini illusorie di %2</span>").arg(alpha).arg(targetName));
                    else
                        text.prepend(tr("<span style=\"color:rgba(255, 255, 0, %1);\">Colpisci %2 infliggendogli %3 danni</span>").arg(alpha).arg(targetName).arg(it->clientData.value()));
                }
                else if (it->chId == it->clientData.targetId())
                {
                    if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_DODGE)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">Schivi l'attacco di %2</span>").arg(alpha).arg(suitorName));
                    else if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_PARRY)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">Pari l'attacco di %2</span>").arg(alpha).arg(suitorName));
                    else if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_ILLUSORY_IMAGE)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">%2 colpisce una delle tue immagini illusorie</span>").arg(alpha).arg(suitorName));
                    else
                        text.prepend(tr("<span style=\"color:rgba(255, 255, 0, %1);\">%2 ti colpisce infliggendoti %3 danni</span>").arg(alpha).arg(suitorName).arg(it->clientData.value()));
                }
                else
                {
                    if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_DODGE)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">%2 schiva l'attacco di %3</span>").arg(alpha).arg(targetName).arg(suitorName));
                    else if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_PARRY)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">%2 para l'attacco di %3</span>").arg(alpha).arg(targetName).arg(suitorName));
                    else if (it->clientData.value() == QMUD::ClientDataAttackInfo::VALUE_ILLUSORY_IMAGE)
                        text.prepend(tr("<span style=\"color:rgba(167, 162, 160, %1);\">%2 colpisce una delle immagini illusorie di %3</span>").arg(alpha).arg(targetName).arg(suitorName));
                    else
                        text.prepend(tr("%2 colpisce %3 infliggendogli <span style=\"color:rgba(255, 255, 0, %1);\">%4</span> danni").arg(alpha).arg(targetName).arg(suitorName).arg(it->clientData.value()));
                }
            }

            if (fontSize > 10)
                fontSize--;

            it->counter -= 5;

            if (it->counter < 0)
                it->counter = 0;
        }
    }

    ui->textBrowserCombatInfo->clear();

    for (auto line : text)
        ui->textBrowserCombatInfo->append(line);
}

void MainWindow::onCharacterConnectClicked(QString name)
{
    m_connection.send(QString("ch_connect %1").arg(name));
}

void MainWindow::pushAttackInfo(const QMUD::ClientDataAttackInfo &attackInfo)
{
    if (!m_bClientDataBasicInfoIsValid ||
            !m_bClientDataTargetInfoIsValid ||
            !m_bClientDataRoomContentIsValid)
        return;

    AttackInfo info;

    if (attackInfo.isDamage())
    {
        QString targetName = tr("qualcuno");
        QString suitorName = tr("qualcuno");

        if (m_clientDataBasicInfo.id() == attackInfo.suitorId())
            suitorName = m_clientDataBasicInfo.name();
        else if (m_clientDataTargetInfo.id() == attackInfo.suitorId())
            suitorName = m_clientDataTargetInfo.name();
        else
        {
            for (auto ch : m_clientDataRoomContent.characters())
            {
                if (ch.id == attackInfo.suitorId())
                {
                    suitorName = ch.name;
                    break;
                }
            }
        }

        if (m_clientDataBasicInfo.id() == attackInfo.targetId())
            targetName = m_clientDataBasicInfo.name();
        else if (m_clientDataTargetInfo.id() == attackInfo.targetId())
            targetName = m_clientDataTargetInfo.name();
        else
        {
            for (auto ch : m_clientDataRoomContent.characters())
            {
                if (ch.id == attackInfo.targetId())
                {
                    targetName = ch.name;
                    break;
                }
            }
        }

        info.chId = m_clientDataBasicInfo.id();
        info.counter = 255;
        info.clientData = attackInfo;
        info.suitorName = suitorName;
        info.targetName = targetName;

        m_vLastAttackInfo.push_back(info);
        m_vLastAttackInfo.pop_front();
    }
}

void MainWindow::echo(QString cmd)
{
    onDataReceived("\033[1;33m" + cmd + "\033[0;37m\n");
}

void MainWindow::onRoomContentClicked()
{
    FormRoomContent* content = qobject_cast<FormRoomContent*>(sender());

    if (content->isNpc())
        m_connection.send(QString("target %1").arg(content->npcId()));
}

void MainWindow::onDataReceived(QString data)
{
    data = data.replace("\r", "");
    m_strBuffer += data;

    // Controllo se ci sono escape sequence lasciate a meta'
    bool escape33Found = false;
    bool escape34Found = false;

    for (int i = 0; i < m_strBuffer.size(); ++i)
    {
        if (!escape33Found && m_strBuffer[i] == '\033')
            escape33Found = true;
        if (escape33Found && m_strBuffer[i] ==  'm')
            escape33Found = false;

        if (!escape34Found && m_strBuffer[i] == '\034')
            escape34Found = true;
        if (escape34Found && m_strBuffer[i] ==  ']')
            escape34Found = false;
    }

    if (!escape33Found && !escape34Found)
    {
        appendEscapeSequenceToTextEdit(ui->textBrowserMain, m_strBuffer);
        m_strBuffer.clear();
    }
}

void MainWindow::onJsonReceived(QByteArray json)
{
    QMUD::ClientData* pkt = QMUD::ClientData::deserialize(json);

    if (pkt)
    {
        if (pkt->type() == QMUD::ClientDataType::LIFE)
        {
            QMUD::ClientDataLife *data = static_cast<QMUD::ClientDataLife*>(pkt);

            ui->progressBarHitPoints->setMaximum(data->hpCurrentMaximum());
            ui->progressBarHitPoints->setValue(data->hpCurrent());
            ui->progressBarHitPoints->setFormat(QString::number(data->hpCurrent()) + "/" + QString::number(data->hpCurrentMaximum()));
            ui->progressBarManaPoints->setMaximum(data->mpCurrentMaximum());
            ui->progressBarManaPoints->setValue(data->mpCurrent());
            ui->progressBarActionPoints->setMaximum(data->apCurrentMaximum());
            ui->progressBarActionPoints->setValue(data->apCurrent());

            ui->widgetCharInfo->setVisible(true);
        }
        else if (pkt->type() == QMUD::ClientDataType::BASIC_INFO)
        {
            QMUD::ClientDataBasicInfo *data = static_cast<QMUD::ClientDataBasicInfo*>(pkt);

            ui->labelChName->setText(data->name());

            //ui->labelChClass->setText(QMUD::classTypeToReadableString(data->classType()));
            ui->labelChLevel->setText(QString::number(data->level()));
            ui->progressBarChClassExp->setMaximum(static_cast<int>(data->experienceToLevelUp()));
            ui->progressBarChClassExp->setValue(static_cast<int>(data->experience()));
            ui->progressBarChClassExp->setVisible(true);

            m_bClientDataBasicInfoIsValid = true;
            m_clientDataBasicInfo = *data;

            ui->widgetCharInfo->setVisible(true);
            ui->widgetActionBar->setVisible(true);
            ui->widgetChat->setVisible(true);
        }
        else if (pkt->type() == QMUD::ClientDataType::TARGET_INFO)
        {
            QMUD::ClientDataTargetInfo *data = static_cast<QMUD::ClientDataTargetInfo*>(pkt);

            if (data->name().isEmpty())
            {
                ui->widgetTargetInfo->setVisible(false);
                m_bClientDataTargetInfoIsValid = false;
                m_clientDataTargetInfo = QMUD::ClientDataTargetInfo();
            }
            else
            {
                ui->labelTargetName->setText(data->name());
                ui->labelTargetLevel->setText(QString::number(data->level()));
                ui->progressBarTargetHitPoints->setMaximum(data->hpCurrentMaximum());
                ui->progressBarTargetHitPoints->setValue(data->hpCurrent());
                ui->progressBarTargetHitPoints->setFormat(QString::number(data->hpCurrent()) + "/" + QString::number(data->hpCurrentMaximum()));
                ui->progressBarTargetManaPoints->setMaximum(data->mpCurrentMaximum());
                ui->progressBarTargetManaPoints->setValue(data->mpCurrent());
                ui->progressBarTargetActionPoints->setMaximum(data->apCurrentMaximum());
                ui->progressBarTargetActionPoints->setValue(data->apCurrent());

                m_bClientDataTargetInfoIsValid = true;
                m_clientDataTargetInfo = *data;

                ui->widgetTargetInfo->setVisible(true);
            }
        }
        else if (pkt->type() == QMUD::ClientDataType::ACTION_LAG)
        {
            QMUD::ClientDataActionLag *data = static_cast<QMUD::ClientDataActionLag*>(pkt);

            if (!data->update())
            {
                ui->progressBarLag->setMaximum(data->actionLag());
                ui->progressBarLag->setValue(data->actionLag());
            }
            else
                ui->progressBarLag->setValue(data->actionLag());

            if (ui->progressBarLag->value() > 0)
                m_timerUpdateLag.start();
        }
        else if (pkt->type() == QMUD::ClientDataType::LOOT)
        {
            QMUD::ClientDataLoot *data = static_cast<QMUD::ClientDataLoot*>(pkt);

            ui->textBrowserLoot->clear();
            ui->textBrowserLootTrash->clear();

            auto loot = data->lootList();
            int index = 0;

            for (auto item : loot)
            {
                if (!item.toSell)
                {
                    ui->textBrowserLoot->append(QString("<a href=\"bottino tieni %1\"><font color=\"green\">tieni</font></a> ").arg(index) +
                                             QString("<a href=\"bottino scarta %1\"><font color=\"red\">scarta</font></a> ").arg(index)  +
                                             QString("<font color=\"%1\">").arg(QMUD::itemQualityToHTMLColor(item.quality)) +
                                                item.name + "</font>");
                }
                else
                {
                    ui->textBrowserLootTrash->append(QString("<a href=\"bottino recupera %1\"><font color=\"green\">recupera</font></a> ").arg(index) +
                                             QString("<font color=\"%1\">").arg(QMUD::itemQualityToHTMLColor(item.quality)) + item.name + "</font>");
                }

                index++;
            }
        }
        else if (pkt->type() == QMUD::ClientDataType::STATUS)
        {
            QMUD::ClientDataStatus *data = static_cast<QMUD::ClientDataStatus*>(pkt);

            m_mapStatus.clear();

            auto statusList = data->statusList();

            for (auto elem : statusList)
                m_mapStatus[elem.status] = elem.time * 5000;

            m_timerUpdateStatus.start();
        }
        else if (pkt->type() == QMUD::ClientDataType::AGGRO)
        {
            QMUD::ClientDataTargetAggro *data = static_cast<QMUD::ClientDataTargetAggro*>(pkt);

            if (data->aggroList().isEmpty())
            {
                ui->textBrowserAggro->clear();
            }
            else
            {
                ui->textBrowserAggro->clear();

                auto aggroList = data->aggroList();

                std::sort(aggroList.begin(), aggroList.end(), [](const QMUD::ClientDataTargetAggro::AggroData& a, const QMUD::ClientDataTargetAggro::AggroData& b)
                { return a.aggro > b.aggro; });

                qint64 maxAggro = 1;
                for (auto elem : aggroList)
                    if (elem.aggro > maxAggro)
                        maxAggro = elem.aggro;

                for (auto elem : aggroList)
                {
                    QString bar;
                    int count = static_cast<int>((double(elem.aggro) / maxAggro) * 12) + 1;
                    int percent = static_cast<int>((double(elem.aggro) / maxAggro) * 100);
                    ui->textBrowserAggro->append(QString("%1: %2% %3").arg(elem.name, 10).arg(percent, 3).arg("", count, '='));
                }
            }
        }
        else if (pkt->type() == QMUD::ClientDataType::ROOM_INFO)
        {
            QMUD::ClientDataRoomInfo *data = static_cast<QMUD::ClientDataRoomInfo*>(pkt);

            appendEscapeSequenceToTextEdit(ui->textBrowserMain, QString("[LOAD MAP %1.%2 (%3, %4)]").arg(data->map()).arg(data->index()).arg(data->x()).arg(data->y()));

            if (data->map() != m_iMapIdOnScene || data->index() != m_iMapIndexOnScene)
            {
                auto itLevel0Maps = m_mapMapsLevel0.find(data->map());
                auto itLevel1Maps = m_mapMapsLevel1.find(data->map());

                if (itLevel0Maps != m_mapMapsLevel0.end() &&
                    itLevel1Maps != m_mapMapsLevel1.end())
                {
                    m_iMapIdOnScene = data->map();
                    m_iMapIndexOnScene = data->map();

                    auto itLevel0 = itLevel0Maps->find(data->index());
                    auto itLevel1 = itLevel1Maps->find(data->index());

                    m_graphicScene.setSceneRect(itLevel0->rect());

                    if (!m_ptrGraphicsItemMapLevel0)
                    {
                        m_graphicScene.setBackgroundBrush(QColor::fromRgba(qRgba(50, 50, 50, 255)));
                        m_ptrGraphicsItemMapLevel0 = new QGraphicsPixmapItem(QPixmap::fromImage(*itLevel0));
                        m_ptrGraphicsItemMapLevel1 = new QGraphicsPixmapItem(QPixmap::fromImage(*itLevel1));
                        m_ptrGraphicsItemRectShadowTop = new QGraphicsRectItem();
                        m_ptrGraphicsItemRectShadowLeft = new QGraphicsRectItem();
                        m_ptrGraphicsItemRectShadowRight = new QGraphicsRectItem();
                        m_ptrGraphicsItemRectShadowBottom = new QGraphicsRectItem();
                        m_ptrGraphicsItemPixmapPlayerBottom = new QGraphicsPixmapItem();
                        m_ptrGraphicsItemPixmapPlayerBottom->setPixmap(QPixmap(":/images/images/player.svg")
                                                                 .scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        m_ptrGraphicsItemPixmapPlayerTop = new QGraphicsPixmapItem();
                        m_ptrGraphicsItemPixmapPlayerTop->setPixmap(QPixmap(":/images/images/player.svg")
                                                                 .scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        m_ptrGraphicsItemPixmapPlayerTop->setOpacity(0.5);

                        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
                        effect->setBlurRadius(20);
                        effect->setOffset(0);
                        effect->setColor(QColor(0, 0, 0, 255));
                        m_ptrGraphicsItemPixmapPlayerTop->setGraphicsEffect(effect);

                        m_graphicScene.addItem(m_ptrGraphicsItemMapLevel0);
                        m_graphicScene.addItem(m_ptrGraphicsItemMapLevel1);
                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowTop);
                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowLeft);
                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowRight);
                        m_graphicScene.addItem(m_ptrGraphicsItemRectShadowBottom);
                        m_graphicScene.addItem(m_ptrGraphicsItemPixmapPlayerBottom);
                        m_graphicScene.addItem(m_ptrGraphicsItemPixmapPlayerTop);

                        m_ptrGraphicsItemMapLevel1->setOpacity(0.8);

                        ui->graphicsViewMap->setSceneRect(itLevel0->rect().adjusted(- itLevel0->rect().width(),
                                                                                 - itLevel0->rect().height(),
                                                                                 itLevel0->rect().width(),
                                                                                 itLevel0->rect().height()));
                        ui->graphicsViewMiniMap->setSceneRect(itLevel0->rect().adjusted(- itLevel0->rect().width(),
                                                                                 - itLevel0->rect().height(),
                                                                                 itLevel0->rect().width(),
                                                                                 itLevel0->rect().height()));
                    }
                    else
                    {
                        m_ptrGraphicsItemMapLevel0->setPixmap(QPixmap::fromImage(*itLevel0));
                        m_ptrGraphicsItemMapLevel1->setPixmap(QPixmap::fromImage(*itLevel1));
                    }

                    m_ptrGraphicsItemMapLevel0->setZValue(1);
                    m_ptrGraphicsItemPixmapPlayerBottom->setZValue(2);
                    m_ptrGraphicsItemMapLevel1->setZValue(3);
                    m_ptrGraphicsItemPixmapPlayerTop->setZValue(4);

                    m_ptrGraphicsItemMapLevel0->setX(0);
                    m_ptrGraphicsItemMapLevel0->setY(0);
                    m_ptrGraphicsItemMapLevel1->setX(0);
                    m_ptrGraphicsItemMapLevel1->setY(0);

                    ui->widgetMiniMap->setVisible(true);
                }
            }

            QMap<QMUD::IdType, QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>> newChs;

            for (auto& elem : data->chs())
            {
                auto it = m_mapChs.find(elem.id);

                if (it == m_mapChs.end())
                {
                    QGraphicsPixmapItem* item = new QGraphicsPixmapItem();
                    //if (elem.race == QMUD::RaceType::KOBOLD)
                    //    item->setPixmap(QPixmap(":/images/images/kobold.png").scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    //else if (elem.race == QMUD::RaceType::CAT)
                    //    item->setPixmap(QPixmap(":/images/images/cat.png").scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    //else
                        item->setPixmap(QPixmap(":/images/images/battle.svg").scaled(m_iTilesSizePixels*0.8, m_iTilesSizePixels*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation));

                    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
                    effect->setBlurRadius(10);
                    effect->setOffset(0);
                    effect->setColor(QColor(0, 0, 0, 255));

                    item->setGraphicsEffect(effect);

                    newChs[elem.id] = QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>(item, elem);

                    m_graphicScene.addItem(item);
                    item->setZValue(2);
                }
                else
                {
                    newChs[elem.id] = QPair<QGraphicsPixmapItem*, QMUD::ClientDataRoomInfo::ChInfo>(it.value().first, elem);
                }
            }

            for (auto& elem : m_mapChs.keys())
            {
                if (!newChs.contains(elem))
                {
                    delete m_mapChs[elem].first;
                }
            }

            m_mapChs = newChs;

            for (auto& elem : m_mapChs)
            {
                elem.first->setX(elem.second.x * m_iTilesSizePixels + (m_iTilesSizePixels - elem.first->pixmap().width())/2);
                elem.first->setY(elem.second.y * m_iTilesSizePixels + (m_iTilesSizePixels - elem.first->pixmap().height())/2);
            }

            QRect sceneRect((data->x() - m_iVisibleMapRadius) * m_iTilesSizePixels,
                            (data->y() - m_iVisibleMapRadius) * m_iTilesSizePixels,
                            m_iVisibleMapDiameter * m_iTilesSizePixels,
                            m_iVisibleMapDiameter * m_iTilesSizePixels);

            m_ptrGraphicsItemRectShadowTop->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
            m_ptrGraphicsItemRectShadowBottom->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
            m_ptrGraphicsItemRectShadowLeft->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
            m_ptrGraphicsItemRectShadowRight->setBrush(QColor::fromRgba(qRgba(0, 0, 0, 50)));
            m_ptrGraphicsItemRectShadowTop->setPen(Qt::NoPen);
            m_ptrGraphicsItemRectShadowBottom->setPen(Qt::NoPen);
            m_ptrGraphicsItemRectShadowLeft->setPen(Qt::NoPen);
            m_ptrGraphicsItemRectShadowRight->setPen(Qt::NoPen);
            m_ptrGraphicsItemRectShadowTop->setZValue(1000);
            m_ptrGraphicsItemRectShadowBottom->setZValue(1000);
            m_ptrGraphicsItemRectShadowLeft->setZValue(1000);
            m_ptrGraphicsItemRectShadowRight->setZValue(1000);

            m_ptrGraphicsItemRectShadowTop->setRect(sceneRect.left(), sceneRect.top(), sceneRect.width()-1, -sceneRect.height()*1000);
            m_ptrGraphicsItemRectShadowBottom->setRect(sceneRect.left(), sceneRect.bottom(), sceneRect.width()-1, sceneRect.height()*1000);
            m_ptrGraphicsItemRectShadowLeft->setRect(sceneRect.left(), sceneRect.top()-sceneRect.height()*1000, -sceneRect.width()*1000, sceneRect.height()*3*1000);
            m_ptrGraphicsItemRectShadowRight->setRect(sceneRect.right(), sceneRect.top()-sceneRect.height()*1000, sceneRect.width()*1000, sceneRect.height()*3*1000);

            ui->graphicsViewMap->centerOn(sceneRect.center());


            m_ptrGraphicsItemPixmapPlayerBottom->setPos(QPointF(sceneRect.center().x() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().width()/2,
                                                                sceneRect.center().y() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().height()/2));
            m_ptrGraphicsItemPixmapPlayerTop->setPos(QPointF(sceneRect.center().x() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().width()/2,
                                                                sceneRect.center().y() - m_ptrGraphicsItemPixmapPlayerBottom->pixmap().height()/2));

            ui->graphicsViewMiniMap->centerOn(sceneRect.center());
        }
        else if (pkt->type() == QMUD::ClientDataType::ROOM_CONTENT)
        {
            QMUD::ClientDataRoomContent *data = static_cast<QMUD::ClientDataRoomContent*>(pkt);

            for (auto obj : ui->widgetRoomContent->children())
                delete obj;

            QVBoxLayout* layout = new QVBoxLayout();
            for (auto ch : data->characters())
            {
                FormRoomContent* content = new FormRoomContent();
                content->setMaximumHeight(60);
                content->setNpcData(ch.name, ch.level, ch.hpCurrent, ch.hpMax, ch.mpCurrent, ch.mpMax, ch.id);
                connect(content, SIGNAL(clicked()), this, SLOT(onRoomContentClicked()));
                layout->addWidget(content);
            }

            layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

            m_bClientDataRoomContentIsValid = true;
            m_clientDataRoomContent = *data;

            ui->widgetRoomContent->setLayout(layout);
        }
        else if (pkt->type() == QMUD::ClientDataType::NEW_COMBAT_ROUND)
        {

        }
        else if (pkt->type() == QMUD::ClientDataType::ATTACK_INFO)
        {
            QMUD::ClientDataAttackInfo *data = static_cast<QMUD::ClientDataAttackInfo*>(pkt);

            if (!m_bClientDataBasicInfoIsValid ||
                    !m_bClientDataTargetInfoIsValid ||
                    !m_bClientDataRoomContentIsValid)
                return;

            if (m_clientDataBasicInfo.id() == data->targetId())
            {
                if (data->isDamage())
                    m_ptrDamageInfoPlayer->pushDamage(data->value());
                else
                    m_ptrDamageInfoPlayer->pushCure(data->value());
            }
            else if (m_clientDataTargetInfo.id() == data->targetId())
            {
                if (data->isDamage())
                    m_ptrDamageInfoTarget->pushDamage(data->value());
                else
                    m_ptrDamageInfoTarget->pushCure(data->value());
            }

            pushAttackInfo(*data);
        }
        else if (pkt->type() == QMUD::ClientDataType::REQUEST)
        {
            QMUD::ClientDataRequest *data = static_cast<QMUD::ClientDataRequest*>(pkt);

            ui->lineEdit->setEnabled(false);
            ui->textBrowserRequest->clear();
            appendEscapeSequenceToTextEdit(ui->textBrowserRequest, data->request());
            ui->widgetRequest->setVisible(true);
        }
        else if (pkt->type() == QMUD::ClientDataType::CDT_MESSAGE)
        {
            QMUD::ClientDataMessage *data = static_cast<QMUD::ClientDataMessage*>(pkt);

            switch (data->message())
            {
            case QMUD::ClientDataMessage::Message::ERROR_GENERIC:

                break;

            case QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED:
            case QMUD::ClientDataMessage::Message::ERROR_SIGNIN_FAILED:
                ui->labelLoginLoginFailed->setVisible(true);
                break;

            case QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED_CONTACT_ADMINISTRATOR:
                ui->labelLoginLoginFailed->setVisible(true);
                ui->labelLoginContactAdmin->setVisible(true);
                break;

            case QMUD::ClientDataMessage::Message::ERROR_USERNAME_FAILED:
            case QMUD::ClientDataMessage::Message::ERROR_USERNAME_EXISTS:
                ui->labelLoginUsernameFailed->setVisible(true);
                break;

            case QMUD::ClientDataMessage::Message::ERROR_PASSWORD_FAILED:
                ui->labelLoginPasswordFailed->setVisible(true);
                break;

            case QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED: break;
            case QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED_CONTACT_ADMINISTRATOR: break;
            case QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR: break;
            case QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED:
                break;

            case QMUD::ClientDataMessage::Message::INFO_SIGNIN_OK:
                ui->labelLoginSigninOk->setVisible(true);
                break;

            case QMUD::ClientDataMessage::Message::INFO_CH_CREATION_OK: break;

            case QMUD::ClientDataMessage::Message::CONN_STATUS_IDLE:
            case QMUD::ClientDataMessage::Message::CONN_STATUS_LOGIN:
                ui->stackedWidget->setCurrentWidget(ui->pageLogin);
                ui->stackedWidget->setCurrentWidget(ui->pageLogin);
                break;

            case QMUD::ClientDataMessage::Message::CONN_STATUS_CHARACTERS_LIST:
                ui->stackedWidget->setCurrentWidget(ui->pageCharactersList);
                break;

            case QMUD::ClientDataMessage::Message::CONN_STATUS_IN_GAME:
                ui->stackedWidget->setCurrentWidget(ui->pageInGame);
                break;
            }

            emit messageReceived(data->message());
        }
        else if (pkt->type() == QMUD::ClientDataType::CHARACTERS_LIST)
        {
            QMUD::ClientDataCharactersList *data = static_cast<QMUD::ClientDataCharactersList*>(pkt);

            ui->listWidgetChList->clear();

            auto list = data->chsList();

            for (auto elem : list)
            {
                QListWidgetItem* item = new QListWidgetItem();
                ui->listWidgetChList->addItem(item);

                FormCharacterInfo* wdg = new FormCharacterInfo();
                wdg->setData(elem);
                connect(wdg, &FormCharacterInfo::connect, this, &MainWindow::onCharacterConnectClicked);

                ui->listWidgetChList->setItemWidget(item, wdg);

                wdg->adjustSize();
                item->setSizeHint(wdg->sizeHint() + QSize(0, 15));
            }
        }

        delete pkt;
    }
    else
    {
        QJsonObject obj = QJsonDocument::fromJson(json).object();

        QString type = obj["t"].toString();

        if (type == "5") // Chat
        {
            enum class SayType
            {
                SAY_CHARACTER,
                SAY_GROUP,
                SAY_ROOM,
                SAY_AREA
            };

            SayType sayType = static_cast<SayType>(obj["s"].toInt());
            QString from = obj["fn"].toString();
            QString to = obj["tn"].toString();
            QString message = obj["m"].toString();

            if (sayType == SayType::SAY_CHARACTER)
            {
                if (from.toLower() != ui->labelChName->text().toLower())
                    ui->textBrowserChat->append(QString("<font color=\"pink\">[</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"pink\">]:%2</font>")
                                                .arg(from)
                                                .arg(message));
                else
                    ui->textBrowserChat->append(QString("<font color=\"pink\">[</font><font color=\"white\">tu a <a href=\"dico a '%1'\" style=\"color:white\">%1</a></font><font color=\"pink\">]:%2</font>")
                                                .arg(to)
                                                .arg(message));
            }
            else if (sayType == SayType::SAY_GROUP)
            {
                if (from.toLower() != ui->labelChName->text().toLower())
                    ui->textBrowserChat->append(QString("<font color=\"#2ECCFA\">[<a href=\"gdico\" style=\"color:white\">GRUPPO</a>:</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"#2ECCFA\">]:%2</font>")
                                                .arg(from)
                                                .arg(message));
                else
                    ui->textBrowserChat->append(QString("<font color=\"#2ECCFA\">[<a href=\"gdico\" style=\"color:white\">GRUPPO</a>:</font><font color=\"white\">tu</font><font color=\"#2ECCFA\">]:%1</font>")
                                                .arg(message));
            }
            else if (sayType == SayType::SAY_ROOM)
            {
                if (from.toLower() != ui->labelChName->text().toLower())
                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"sdico\" style=\"color:white\">STANZA</a>:</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"white\">]:%2</font>")
                                                .arg(from)
                                                .arg(message));
                else
                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"sdico\" style=\"color:white\">STANZA</a>:</font><font color=\"white\">tu</font><font color=\"white\">]:%1</font>")
                                                .arg(message));
            }
            else if (sayType == SayType::SAY_AREA)
            {
                if (from.toLower() != ui->labelChName->text().toLower())
                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"adico\" style=\"color:white\">AREA</a>:</font><a href=\"dico a '%1'\"><font color=\"white\">%1</font></a><font color=\"white\">]:%2</font>")
                                                .arg(from)
                                                .arg(message));
                else
                    ui->textBrowserChat->append(QString("<font color=\"white\">[<a href=\"adico\" style=\"color:white\">AREA</a>:</font><font color=\"white\">tu</font><font color=\"white\">]:%1</font>")
                                                .arg(message));
            }
        }
    }
}

void MainWindow::onTimerUpdateLagTimeout()
{
    int val = ui->progressBarLag->value();
    if (val > 0)
    {
        val -= 100;
        if (val < 0)
            val = 0;

        ui->progressBarLag->setValue(val);
    }
    else
        m_timerUpdateLag.stop();
}

void MainWindow::onTimerUpdateStatusTimeout()
{
    //ui->textBrowserStatus->clear();
    //for (auto it = m_mapStatus.begin(); it != m_mapStatus.end(); ++it)
    //{
    //    it.value() -= m_timerUpdateStatus.interval();
    //
    //    if (it.value() > 0)
    //    {
    //        if (it.key() != QMUD::TemporaryStatusType::ACTIONLAG)
    //        {
    //            int minutes = it.value()/1000/60;
    //            ui->textBrowserStatus->append(QString("%1 (%2)").arg(QMUD::statusToReadableString(it.key()))
    //                                          .arg(minutes == 0 ? tr("meno di un minuto") : tr("%1 minuti").arg(minutes)));
    //        }
    //    }
    //}
}

void MainWindow::on_lineEdit_returnPressed()
{
    if (!m_connection.isConnected())
    {
        ui->widgetCharInfo->setVisible(false);
        ui->widgetChat->setVisible(false);
        ui->widgetLoot->setVisible(false);
        ui->widgetTargetInfo->setVisible(false);
        ui->widgetMain->setVisible(true);

        if (!m_connection.connectToHost("localhost", 8060))
            m_connection.connectToHost("2.224.131.179", 8060);
    }
    else
    {
        echo(ui->lineEdit->text());
        ui->textBrowserMain->ensureCursorVisible();
        ui->lineEdit->selectAll();
        m_connection.send(ui->lineEdit->text());

        if (m_lHistory.isEmpty() || m_lHistory[0] != ui->lineEdit->text())
            m_lHistory.push_front(ui->lineEdit->text());

        m_iHistoryPos = -1;
    }
}

void MainWindow::on_pushButton_clicked()
{
    onDataReceived("\n");
    m_connection.send("bottino SCARTA-TUTTO");
}

void MainWindow::on_pushButton_2_clicked()
{
    onDataReceived("\n");
    m_connection.send("bottino SACRIFICA-CESTINO");
}

void MainWindow::on_textBrowserLoot_anchorClicked(const QUrl &arg1)
{
    onDataReceived("\n");
    m_connection.send(arg1.toString());
}

void MainWindow::on_textBrowserLootTrash_anchorClicked(const QUrl &arg1)
{
    onDataReceived("\n");
    m_connection.send(arg1.toString());
}

void MainWindow::on_textBrowserChat_anchorClicked(const QUrl &arg1)
{
    ui->lineEdit->setText(arg1.toString() + " ");
    ui->lineEdit->setCursorPosition(ui->lineEdit->text().size() + 1);
}

void MainWindow::keyPressEvent(QKeyEvent *key)
{
    if (key->key() == Qt::Key_Up)
    {
        if (m_iHistoryPos < m_lHistory.size() -1)
        {
            if (m_iHistoryPos == -1)
                m_strHistoryLastNonSendedCommand = ui->lineEdit->text();

            m_iHistoryPos++;

            ui->lineEdit->blockSignals(true);
            ui->lineEdit->setText(m_lHistory[m_iHistoryPos]);
            ui->lineEdit->blockSignals(false);
        }
    }
    else if (key->key() == Qt::Key_Down)
    {
        if (m_iHistoryPos > 0)
        {
            m_iHistoryPos--;

            ui->lineEdit->blockSignals(true);
            ui->lineEdit->setText(m_lHistory[m_iHistoryPos]);
            ui->lineEdit->blockSignals(false);
        }
        else if (m_iHistoryPos == 0)
        {
            ui->lineEdit->blockSignals(true);
            ui->lineEdit->setText(m_strHistoryLastNonSendedCommand);
            m_iHistoryPos = -1;
            ui->lineEdit->blockSignals(false);
        }
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && watched == ui->lineEdit)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        if (key->key() == Qt::Key_B && (QApplication::keyboardModifiers() & Qt::AltModifier))
        {
            ui->widgetLoot->setVisible(!ui->widgetLoot->isVisible());
            event->accept();
            return true;
        }
        else if (key->key() == Qt::Key_S && (QApplication::keyboardModifiers() & Qt::AltModifier))
        {
            ui->widgetChat->setVisible(!ui->widgetChat->isVisible());
            event->accept();
            return true;
        }
        else if (key->key() == Qt::Key_D && (QApplication::keyboardModifiers() & Qt::AltModifier))
        {
            onDataReceived("\n");
            m_connection.send("diario");
            event->accept();
            return true;
        }
        else if (key->key() == Qt::Key_C && (QApplication::keyboardModifiers() & Qt::AltModifier))
        {
            onDataReceived("\n");
            m_connection.send("condizione");
            event->accept();
            return true;
        }
        else if (key->key() == Qt::Key_I && (QApplication::keyboardModifiers() & Qt::AltModifier))
        {
            onDataReceived("\n");
            m_connection.send("inventario");
            event->accept();
            return true;
        }
        else if (key->key() == Qt::Key_E && (QApplication::keyboardModifiers() & Qt::AltModifier))
        {
            onDataReceived("\n");
            m_connection.send("equipaggiamento");
            event->accept();
            return true;
        }
    }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->widgetMain->setGeometry(0, 0, width(), height());
    ui->widgetMain->layout()->setContentsMargins(0,0,0,0);

    //ui->widgetLogin->setGeometry((width() - 450) / 2, (height() - 250) / 2, 450, 250);

    ui->widgetRequest->setGeometry(0, 0, width(), height());
    ui->widgetChat->setGeometry((width() - 500) / 2,
                                5,
                                500,
                                150);

    ui->widgetCommandLine->setGeometry(width() - 400 - 10,
                                       height() - 150 - 10,
                                       400,
                                       150);

    ui->widgetLoot->setGeometry(width() - 500 - 5,
                                5 + 200 + 5,
                                500,
                                400);

    ui->widgetActionBar->setGeometry(width() * 0.1,
                                     height() - 10 - ui->widgetActionBar->height(),
                                     width() * 0.8,
                                     ui->widgetActionBar->height());

    ui->widgetTargetInfo->setGeometry((width() - ui->widgetTargetInfo->width() - ui->widgetCharInfo->width() - 20) / 2 + ui->widgetCharInfo->width() + 20,
                                  height() - ui->widgetActionBar->height() - 20 - qMax(ui->widgetCharInfo->height(), ui->widgetTargetInfo->height()),
                                  ui->widgetTargetInfo->width(),
                                  ui->widgetTargetInfo->height());

    ui->widgetCharInfo->setGeometry((width() - ui->widgetTargetInfo->width() - ui->widgetCharInfo->width() - 20) / 2,
                                  height() - ui->widgetActionBar->height() - 20 - qMax(ui->widgetCharInfo->height(), ui->widgetTargetInfo->height()),
                                  ui->widgetCharInfo->width(),
                                  ui->widgetCharInfo->height());

    m_ptrDamageInfoPlayer->setGeometry(ui->widgetCharInfo->geometry().x(),
                                       ui->widgetCharInfo->geometry().y() + 20 - m_ptrDamageInfoPlayer->height(),
                                       m_ptrDamageInfoPlayer->width(),
                                       m_ptrDamageInfoPlayer->height());

    m_ptrDamageInfoTarget->setGeometry(ui->widgetTargetInfo->geometry().x(),
                                       ui->widgetTargetInfo->geometry().y() + 20 - m_ptrDamageInfoTarget->height(),
                                       m_ptrDamageInfoTarget->width(),
                                       m_ptrDamageInfoTarget->height());

    ui->widgetMiniMap->setGeometry(width() - 10 - 200,
                                10,
                                200,
                                200);

    ui->widgetRoomContent->setGeometry(10,
                                10,
                                200,
                                height() - 20);

    int widgetCombatInfoWidth = (width() - ui->widgetRoomContent->width() - ui->widgetMiniMap->width() - 20) * 0.8;
    int widgetCombatInfoHeight = (ui->widgetCharInfo->y() - ui->widgetChat->y() - ui->widgetChat->height()) * 0.8;

    ui->widgetCombatInfo->setGeometry((width() - widgetCombatInfoWidth) / 2,
                                      (height() - widgetCombatInfoHeight) / 2,
                                      widgetCombatInfoWidth,
                                      widgetCombatInfoHeight);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    m_iHistoryPos = -1;
}

void MainWindow::on_textBrowserMain_anchorClicked(const QUrl &arg1)
{
    onDataReceived("\n");
    m_connection.send(arg1.toString());
}

void MainWindow::on_pushButtonRequestYes_clicked()
{
    ui->lineEdit->setEnabled(true);
    ui->widgetRequest->setVisible(false);
    m_connection.send("SI");
}

void MainWindow::on_pushButtonRequestNo_clicked()
{
    ui->lineEdit->setEnabled(true);
    ui->widgetRequest->setVisible(false);
    m_connection.send("NO");
}

void MainWindow::on_pushButtonAttack_clicked()
{
    m_connection.send("attack");
}

void MainWindow::on_lineEditLoginUsername_returnPressed()
{
    ui->labelLoginLoginFailed->setVisible(false);
    ui->labelLoginUsernameFailed->setVisible(false);
    ui->labelLoginPasswordFailed->setVisible(false);
    ui->labelLoginContactAdmin->setVisible(false);
    ui->labelLoginSigninOk->setVisible(false);

    m_connection.send(QString("login %1 %2").arg(ui->lineEditLoginUsername->text()).arg(ui->lineEditLoginPassword->text()));
}

void MainWindow::on_lineEditLoginPassword_returnPressed()
{
    ui->labelLoginLoginFailed->setVisible(false);
    ui->labelLoginUsernameFailed->setVisible(false);
    ui->labelLoginPasswordFailed->setVisible(false);
    ui->labelLoginContactAdmin->setVisible(false);
    ui->labelLoginSigninOk->setVisible(false);

    m_connection.send(QString("login %1 %2").arg(ui->lineEditLoginUsername->text()).arg(ui->lineEditLoginPassword->text()));
}

void MainWindow::on_pushButtonLoginLogin_clicked()
{
    ui->labelLoginLoginFailed->setVisible(false);
    ui->labelLoginUsernameFailed->setVisible(false);
    ui->labelLoginPasswordFailed->setVisible(false);
    ui->labelLoginContactAdmin->setVisible(false);
    ui->labelLoginSigninOk->setVisible(false);

    m_connection.send(QString("login %1 %2").arg(ui->lineEditLoginUsername->text()).arg(ui->lineEditLoginPassword->text()));
}

void MainWindow::on_pushButtonLoginCreate_clicked()
{
    ui->labelLoginLoginFailed->setVisible(false);
    ui->labelLoginUsernameFailed->setVisible(false);
    ui->labelLoginPasswordFailed->setVisible(false);
    ui->labelLoginContactAdmin->setVisible(false);
    ui->labelLoginSigninOk->setVisible(false);

    m_connection.send(QString("signin %1 %2").arg(ui->lineEditLoginUsername->text()).arg(ui->lineEditLoginPassword->text()));
}

void MainWindow::on_pushButtonChListCreate_clicked()
{
    FormCreateCharacter* wnd = new FormCreateCharacter();

    connect(this, &MainWindow::messageReceived, wnd, &FormCreateCharacter::onMessage);
    connect(wnd, &FormCreateCharacter::sendCommand, &m_connection, &Connection::send);
    wnd->show();
}
