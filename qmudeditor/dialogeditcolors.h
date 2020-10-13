/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef DIALOGEDITCOLORS_H
#define DIALOGEDITCOLORS_H

#include "../common.h"

#include <QDialog>
#include <QMap>
#include <QTableWidgetItem>

namespace Ui {
class DialogEditColors;
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

static QString XTERM_COLOR_STRING[] = {
    "&x",
    "&r",
    "&g",
    "&O",
    "&b",
    "&p",
    "&c",
    "&w",
    "&z",
    "&R",
    "&G",
    "&Y",
    "&B",
    "&P",
    "&C",
    "&W"
};


struct CharInfo
{
    CharInfo();

    QChar key;
    QChar value;
    QColor color;
    QString name;
    bool is_room;
    QVector<QMUD::RoomFlagType> flags;
    QPushButton* button;
    QTableWidgetItem *item;
};

class DialogEditColors : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditColors(QMap<QString, CharInfo> colors, QWidget *parent = nullptr);
    ~DialogEditColors();

    QMap<QString, CharInfo> chars();

private slots:
    void onCurrentIndexChanged(int index);
    void onItemClicked(const QModelIndex &index);
    void onFlagStateChanged(int);
    void on_pushButtonAdd_clicked();

private:
    bool eventFilter(QObject *obj, QEvent *event);

    CharInfo rowToCharInfo(int index);
    void updateRow();
    void updateFlags();
    void addRow(const CharInfo& charInfo);

    Ui::DialogEditColors *ui;

    QMap<QString, CharInfo> m_mapChars;

    int m_iSelectedRow;

    QVector<QChar> m_validChars;
};

#endif // DIALOGEDITCOLORS_H
