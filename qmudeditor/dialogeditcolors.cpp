/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "dialogeditcolors.h"
#include "ui_dialogeditcolors.h"

#include <QComboBox>
#include <QObjectUserData>
#include <QLineEdit>
#include <QMenu>

#include "formcenteredcheckbox.h"

#define SELECTED_ITEM_COLOR QColor(100, 100, 100)

DialogEditColors::DialogEditColors(QMap<QString, CharInfo> colors, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditColors),
    m_mapChars(colors),
    m_iSelectedRow(-1)
{
    ui->setupUi(this);

    m_validChars.resize(93);
    std::iota(m_validChars.begin(), m_validChars.end(), '!');
    m_validChars.push_back('~');
    m_validChars.removeOne('!');
    m_validChars.removeOne('\'');
    m_validChars.removeOne(' ');

    // Room flags
    ui->tableWidgetFlags->setVisible(false);
    ui->tableWidgetFlags->setRowCount(7);
    ui->tableWidgetFlags->setColumnCount(2);

    ui->tableWidgetFlags->setItem(0, 1, new QTableWidgetItem(QObject::tr("PACE - Non si puo' combattere")));
    ui->tableWidgetFlags->setItem(1, 1, new QTableWidgetItem(QObject::tr("DIFFICILE - Si spende un po' di movimento in piu'")));
    ui->tableWidgetFlags->setItem(2, 1, new QTableWidgetItem(QObject::tr("IMPERVIA - Si spende molto movimento in piu'")));
    ui->tableWidgetFlags->setItem(3, 1, new QTableWidgetItem(QObject::tr("SOLO VOLO - Richiede volo per essere oltrepassata")));
    ui->tableWidgetFlags->setItem(4, 1, new QTableWidgetItem(QObject::tr("ACQUA - Stanza di mare, fiume o allagata")));
    ui->tableWidgetFlags->setItem(5, 1, new QTableWidgetItem(QObject::tr("NO RISORSE - Non contiene risorse raccoglibili")));
    ui->tableWidgetFlags->setItem(6, 1, new QTableWidgetItem(QObject::tr("ESTERNA - Stanza all'aria aperta")));

    ui->tableWidgetFlags->item(0, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::PACE));
    ui->tableWidgetFlags->item(1, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::DIFFICULT));
    ui->tableWidgetFlags->item(2, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::IMPERVIOUS));
    ui->tableWidgetFlags->item(3, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::IMPOSSIBLE));
    ui->tableWidgetFlags->item(4, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::WATER));
    ui->tableWidgetFlags->item(5, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::NO_RESOURCES));
    ui->tableWidgetFlags->item(6, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::RoomFlagType::OUTSIDE));

    for (int i = 0; i < ui->tableWidgetFlags->rowCount(); ++i)
    {
        auto check = new FormCenteredCheckBox();
        connect(check->checkBox(), SIGNAL(stateChanged(int)), this, SLOT(onFlagStateChanged(int)));

        ui->tableWidgetFlags->setCellWidget(i, 0, check);
    }

    ui->tableWidgetFlags->resizeColumnToContents(0);

    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Azioni" << "Codice" << "Carattere" << "Colore" << "Anteprima" << "Nome" << "Stanza");

    for (auto it = m_mapChars.begin(); it != m_mapChars.end(); ++it)
        addRow(*it);

    updateRow();

    ui->tableWidget->resizeColumnsToContents();

    connect(ui->tableWidget, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onItemClicked(const QModelIndex &)));
}

DialogEditColors::~DialogEditColors()
{
    delete ui;
}

QMap<QString, CharInfo> DialogEditColors::chars()
{
    QMap<QString, CharInfo> mapChars;

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        CharInfo charInfo = rowToCharInfo(row);
        mapChars[charInfo.key] = charInfo;
    }

    return mapChars;
}

void DialogEditColors::onCurrentIndexChanged(int index)
{
    Q_UNUSED(index)

    updateRow();
}

void DialogEditColors::onItemClicked(const QModelIndex &index)
{
    auto *clickedItem = ui->tableWidget->item(index.row(), index.column());
    int selectedRow = clickedItem->row();

    for (int row=0; row < ui->tableWidget->rowCount(); ++row)
    {
        auto *itemToChange0 = ui->tableWidget->item(row, 4);

        if (row == selectedRow)
        {
            itemToChange0->setBackgroundColor(SELECTED_ITEM_COLOR);
        }
        else
        {
            itemToChange0->setBackgroundColor(QColor(0, 0, 0));
        }
    }

    m_iSelectedRow = selectedRow;

    if (m_iSelectedRow >= 0)
    {
        ui->tableWidgetFlags->setVisible(true);

        m_iSelectedRow = -1;
        for (int i=0; i<ui->tableWidgetFlags->rowCount(); ++i)
            (static_cast<FormCenteredCheckBox*>(ui->tableWidgetFlags->cellWidget(i, 0)))->checkBox()->setChecked(false);
        m_iSelectedRow = selectedRow;

        CharInfo charInfo = rowToCharInfo(m_iSelectedRow);

        for (auto flag : charInfo.flags)
        {
            for (int i=0; i < ui->tableWidgetFlags->rowCount(); ++i)
            {
                if (ui->tableWidgetFlags->item(i, 1)->data(Qt::UserRole).toInt() == static_cast<int>(flag))
                {
                    static_cast<FormCenteredCheckBox*>(ui->tableWidgetFlags->cellWidget(i, 0))->checkBox()->setChecked(true);
                    break;
                }
            }
        }
    }
    else
        ui->tableWidgetFlags->setVisible(false);
}

void DialogEditColors::onFlagStateChanged(int)
{
    updateFlags();
}

void DialogEditColors::updateRow()
{
    for (int i=0; i<ui->tableWidget->rowCount(); ++i)
    {
        CharInfo charInfo = rowToCharInfo(i);

        QTableWidgetItem *item = new QTableWidgetItem(charInfo.value);
        Qt::ItemFlags eFlags = item->flags();
        eFlags &= ~Qt::ItemIsEditable;
        item->setFlags(eFlags);
        item->setForeground(charInfo.color);
        if (i == m_iSelectedRow)
            item->setBackgroundColor(SELECTED_ITEM_COLOR);
        else
            item->setBackgroundColor(QColor(0, 0, 0));
        ui->tableWidget->setItem(i, 4, item);
    }
}

void DialogEditColors::updateFlags()
{
    if (m_iSelectedRow >= 0)
    {
        QVector<int> flags;

        for (int i=0; i < ui->tableWidgetFlags->rowCount(); ++i)
            if ((static_cast<FormCenteredCheckBox*>(ui->tableWidgetFlags->cellWidget(i, 0)))->checkBox()->isChecked())
                flags.push_back(ui->tableWidgetFlags->item(i, 1)->data(Qt::UserRole).toInt());

        ui->tableWidget->item(m_iSelectedRow, 5)->setData(Qt::UserRole, QVariant::fromValue<QVector<int>>(flags));
    }
}

void DialogEditColors::addRow(const CharInfo &charInfo)
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
    int row = ui->tableWidget->rowCount() - 1;

    QComboBox *cbKey = new QComboBox(this);
    for (auto elem : m_validChars)
        cbKey->addItem(elem, elem);
    cbKey->setEditable(true);
    cbKey->lineEdit()->setReadOnly(true);
    cbKey->lineEdit()->setAlignment(Qt::AlignCenter);
    cbKey->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbKey->installEventFilter(this);

    for (int i=0; i<cbKey->count(); ++i)
    {
        if (cbKey->itemData(i).toChar() == charInfo.key)
        {
            cbKey->setCurrentIndex(i);
            break;
        }
    }

    ui->tableWidget->setCellWidget(row, 1, cbKey);

    QComboBox *cbValue = new QComboBox(this);
    for (auto elem : m_validChars)
        cbValue->addItem(elem, elem);
    //cbValue->setFrame(false);
    cbValue->setEditable(true);
    cbValue->lineEdit()->setReadOnly(true);
    cbValue->lineEdit()->setAlignment(Qt::AlignCenter);
    cbValue->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbValue->installEventFilter(this);

    for (int i=0; i<cbValue->count(); ++i)
    {
        if (cbValue->itemData(i).toChar() == charInfo.value)
        {
            cbValue->setCurrentIndex(i);
            break;
        }
    }

    ui->tableWidget->setCellWidget(row, 2, cbValue);

    QComboBox *cbColor = new QComboBox(this);

    for (int i=0; i<16; ++i)
    {
        cbColor->addItem(XTERM_COLOR_STRING[i], XTERM_COLOR[i]);
        cbColor->setItemData(i, XTERM_COLOR[i], Qt::BackgroundColorRole);
    }

    for (int i=16; i<256; ++i)
    {
        cbColor->addItem(QString("&[%1]").arg(i), XTERM_COLOR[i]);
        cbColor->setItemData(i, XTERM_COLOR[i], Qt::BackgroundColorRole);
    }

    cbColor->setEditable(true);
    cbColor->lineEdit()->setReadOnly(true);
    cbColor->lineEdit()->setAlignment(Qt::AlignCenter);
    cbColor->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbColor->installEventFilter(this);

    for (int i=0; i<cbColor->count(); ++i)
    {
        if (cbColor->itemData(i).value<QColor>() == charInfo.color)
        {
            cbColor->setCurrentIndex(i);
            break;
        }
    }

    ui->tableWidget->setCellWidget(row, 3, cbColor);

    ui->tableWidget->setItem(row, 5, new QTableWidgetItem(charInfo.name));
    QVector<int> flags;
    for (auto flag : charInfo.flags)
        flags.push_back(static_cast<int>(flag));

    ui->tableWidget->item(row, 5)->setData(Qt::UserRole, QVariant::fromValue<QVector<int>>(flags));

    QComboBox *cbIsRoom = new QComboBox(this);
    cbIsRoom->addItem("SI", true);
    cbIsRoom->addItem("NO", false);
    cbIsRoom->setEditable(true);
    cbIsRoom->lineEdit()->setReadOnly(true);
    cbIsRoom->lineEdit()->setAlignment(Qt::AlignCenter);
    cbIsRoom->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbIsRoom->installEventFilter(this);

    for (int i=0; i<cbIsRoom->count(); ++i)
    {
        if (cbIsRoom->itemData(i).toBool() == charInfo.is_room)
        {
            cbIsRoom->setCurrentIndex(i);
            break;
        }
    }

    ui->tableWidget->setCellWidget(row, 6, cbIsRoom);

    QPushButton *pbItemRemove = new QPushButton("Rimuovi", this);

    connect(pbItemRemove, &QPushButton::clicked, [this, pbItemRemove]() {
        for (auto i = 0; i<ui->tableWidget->rowCount(); ++i)
        {
            if (qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, 0)) == pbItemRemove)
            {
                if (i == m_iSelectedRow)
                {
                    m_iSelectedRow = -1;
                    ui->tableWidgetFlags->setVisible(false);
                }

                ui->tableWidget->removeRow(i);
                return;
            }
        }
    });

    ui->tableWidget->setCellWidget(row, 0, pbItemRemove);

    connect(cbKey, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    connect(cbValue, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    connect(cbColor, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void DialogEditColors::on_pushButtonAdd_clicked()
{
    CharInfo charInfo;
    charInfo.key = '1';

    for (auto elem : m_validChars)
    {
        bool found = false;
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
            if (qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))->currentData().toChar() == elem)
            {
                found = true;
                break;
            }

        if (!found)
        {
            charInfo.key = elem;
            break;
        }
    }

    charInfo.value = '.';
    charInfo.color = QColor(255, 255, 255);
    charInfo.name = "Una stanza";
    charInfo.is_room = true;

    addRow(charInfo);
    updateRow();
}

bool DialogEditColors::eventFilter(QObject *obj, QEvent *event)
{
    if (qobject_cast<QComboBox*>(obj) && event->type() == QEvent::Wheel)
    {
        return true;
    }

    return QObject::eventFilter(obj, event);
}

CharInfo DialogEditColors::rowToCharInfo(int index)
{
    CharInfo charInfo;
    charInfo.key = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(index, 1))->currentData().toChar();
    charInfo.value = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(index, 2))->currentData().toChar();
    charInfo.color = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(index, 3))->currentData().value<QColor>();
    charInfo.name = ui->tableWidget->item(index, 5)->text();
    auto flags = ui->tableWidget->item(index, 5)->data(Qt::UserRole).value<QVector<int>>();
    for (auto flag : flags)
        charInfo.flags.push_back(static_cast<QMUD::RoomFlagType>(flag));
    charInfo.is_room = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(index, 6))->currentData().toBool();

    return charInfo;
}

CharInfo::CharInfo() :
    is_room(false),
    button(Q_NULLPTR),
    item(Q_NULLPTR)
{

}
