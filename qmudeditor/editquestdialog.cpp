/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "editquestdialog.h"
#include "ui_editquestdialog.h"

#include "editormap.h"
#include "editquesttargetdialog.h"
#include "selectfromlistdialog.h"

#include <QMessageBox>

EditQuestDialog::EditQuestDialog(const QVector<EditorMap *> &maps, EditorMap* currentMap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditQuestDialog),
    m_vMaps(maps),
    m_ptrCurrentMap(currentMap)
{
    ui->setupUi(this);

    ui->comboBoxDifficulty->addItem(tr("Normale"),         static_cast<int>(QMUD::NpcType::NORMAL));
    ui->comboBoxDifficulty->addItem(tr("Elite"),           static_cast<int>(QMUD::NpcType::ELITE));
    ui->comboBoxDifficulty->addItem(tr("Epica"),           static_cast<int>(QMUD::NpcType::RARE));
    ui->comboBoxDifficulty->addItem(tr("Leggendaria"),     static_cast<int>(QMUD::NpcType::LEGENDARY));
    ui->comboBoxDifficulty->addItem(tr("Boss livello 1"),  static_cast<int>(QMUD::NpcType::BOSS_A));
    ui->comboBoxDifficulty->addItem(tr("Boss livello 2"),  static_cast<int>(QMUD::NpcType::BOSS_B));
}

EditQuestDialog::~EditQuestDialog()
{
    delete ui;
}

void EditQuestDialog::setData(const EditorMap::QuestData &data)
{
    ui->labelQuestId->setText(QString::number(data.id));
    ui->spinBoxGiveNpcID->setValue(data.giveNpcId);
    ui->spinBoxGiveNpcMapID->setValue(data.giveNpcMapId);
    ui->spinBoxCompleteNpcID->setValue(data.completeNpcId);
    ui->spinBoxCompleteNpcMapID->setValue(data.completeNpcMapId);
    ui->spinBoxQuestLevel->setValue(data.level);
    ui->lineEditQuestName->setText(data.name);
    for (int i = 0; i < ui->comboBoxDifficulty->count(); ++i)
        if (ui->comboBoxDifficulty->itemData(i, Qt::UserRole).toInt() == static_cast<int>(data.difficulty))
        {
            ui->comboBoxDifficulty->setCurrentIndex(i);
            break;
        }
    ui->spinBoxGold->setValue(data.gold);
    ui->groupBoxGiftItem->setChecked(data.giftItemEnabled);
    ui->groupBoxSpecificGiftItem->setChecked(data.specificGiftItemEnabled);
    ui->spinBoxGiftItem->setValue(data.specificGiftItemId);
    ui->spinBoxGiftItemMap->setValue(data.specificGiftItemMapId);
    ui->plainTextEditQuestDescription->setPlainText(data.giveDescription);
    ui->plainTextEditQuestCompleteDescription->setPlainText(data.completeDescription);

    ui->tableWidgetQuestTargets->setRowCount(0);

    for (const auto& target : data.targetData)
        addTarget(target);
}

EditorMap::QuestData EditQuestDialog::data() const
{
    EditorMap::QuestData data;

    data.id = ui->labelQuestId->text().toInt();
    data.giveNpcId = ui->spinBoxGiveNpcID->value();
    data.giveNpcMapId = ui->spinBoxGiveNpcMapID->value();
    data.completeNpcId = ui->spinBoxCompleteNpcID->value();
    data.completeNpcMapId = ui->spinBoxCompleteNpcMapID->value();
    data.level = ui->spinBoxQuestLevel->value();
    data.name = ui->lineEditQuestName->text();
    data.difficulty = static_cast<QMUD::NpcType>(ui->comboBoxDifficulty->currentData(Qt::UserRole).toInt());
    data.gold = ui->spinBoxGold->value();
    data.giftItemEnabled = ui->groupBoxGiftItem->isChecked();
    data.specificGiftItemEnabled = ui->groupBoxSpecificGiftItem->isChecked();
    data.specificGiftItemId = ui->spinBoxGiftItem->value();
    data.specificGiftItemMapId = ui->spinBoxGiftItemMap->value();
    data.giveDescription = ui->plainTextEditQuestDescription->toPlainText();
    data.completeDescription = ui->plainTextEditQuestCompleteDescription->toPlainText();

    for (int row=0; row<ui->tableWidgetQuestTargets->rowCount(); ++row)
    {
        EditorMap::QuestData::TargetData target = ui->tableWidgetQuestTargets->item(row, 1)->data(Qt::UserRole).value<EditorMap::QuestData::TargetData>();

        data.targetData.push_back(target);
    }

    return data;
}

void EditQuestDialog::addTarget(const EditorMap::QuestData::TargetData &data)
{
    ui->tableWidgetQuestTargets->setRowCount(ui->tableWidgetQuestTargets->rowCount() + 1);

    int row = ui->tableWidgetQuestTargets->rowCount() - 1;

    // Azioni
    QPushButton *pbRemove = new QPushButton("", this);
    pbRemove->setIcon(QIcon(":/icons/icons/remove.png"));
    pbRemove->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    pbRemove->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(pbRemove, &QPushButton::clicked, [this, pbRemove, data]() {
        if (QMessageBox::question(this, tr("Attenzione"),
                                  tr("Vuoi rimuovere l'obbiettivo %1").arg(data.description), QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes)
        {
            for (auto i = 0; i<ui->tableWidgetQuestTargets->rowCount(); ++i)
            {
                if (qobject_cast<QPushButton*>(ui->tableWidgetQuestTargets->cellWidget(i, 0)) == pbRemove)
                {
                    ui->tableWidgetQuestTargets->removeRow(i);
                    return;
                }
            }
        }
    });

    ui->tableWidgetQuestTargets->setCellWidget(row, 0, pbRemove);

    ui->tableWidgetQuestTargets->setItem(row, 1, new QTableWidgetItem(data.description));
    ui->tableWidgetQuestTargets->item(row, 1)->setData(Qt::UserRole, QVariant::fromValue(data));
    ui->tableWidgetQuestTargets->resizeColumnToContents(0);
}

void EditQuestDialog::on_pushButtonAddQuestTarget_clicked()
{
    EditQuestTargetDialog dialog(m_vMaps, this);

    EditorMap::QuestData::TargetData data;
    dialog.setData(data);

    if (dialog.exec() == QDialog::Accepted)
        addTarget(dialog.data());
}

void EditQuestDialog::on_tableWidgetQuestTargets_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)

    EditorMap::QuestData::TargetData data = ui->tableWidgetQuestTargets->item(row, 1)->data(Qt::UserRole).value<EditorMap::QuestData::TargetData>();

    EditQuestTargetDialog dialog(m_vMaps, this);
    dialog.setData(data);

    if (dialog.exec() == QDialog::Accepted)
    {
        ui->tableWidgetQuestTargets->item(row, 1)->setData(Qt::UserRole, QVariant::fromValue(dialog.data()));
        ui->tableWidgetQuestTargets->item(row, 1)->setText(dialog.data().description);
    }
}

void EditQuestDialog::on_pushButtonGiveNpcSelectMapID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
        dlg.addElement(QString("%1 - %2").arg(map->id()).arg(map->name()), map->id());

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxGiveNpcMapID->setValue(dlg.selectedData());
}

void EditQuestDialog::on_pushButtonGiveNpcSelectNpcID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
    {
        if (map->id() == ui->spinBoxGiveNpcMapID->value())
        {
            for (auto npc : map->npcs())
                    dlg.addElement(QString("%1 - %2").arg(npc->id()).arg(npc->name()), npc->id());
            break;
        }
    }

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxGiveNpcID->setValue(dlg.selectedData());
}

void EditQuestDialog::on_pushButtonCompleteNpcSelectMapID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
        dlg.addElement(QString("%1 - %2").arg(map->id()).arg(map->name()), map->id());

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxCompleteNpcMapID->setValue(dlg.selectedData());
}

void EditQuestDialog::on_pushButtonCompleteNpcSelectNpcID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
    {
        if (map->id() == ui->spinBoxCompleteNpcMapID->value())
        {
            for (auto npc : map->npcs())
                    dlg.addElement(QString("%1 - %2").arg(npc->id()).arg(npc->name()), npc->id());
            break;
        }
    }

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxCompleteNpcID->setValue(dlg.selectedData());
}

void EditQuestDialog::on_pushButtonGiftItem_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
    {
        if (map->id() == ui->spinBoxGiftItemMap->value())
        {
            for (auto item : map->items())
                    dlg.addElement(QString("%1 - %2").arg(item->id()).arg(item->name()), item->id());
            break;
        }
    }

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxGiftItem->setValue(dlg.selectedData());
}

void EditQuestDialog::on_pushButtonGiftItemMap_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
        dlg.addElement(QString("%1 - %2").arg(map->id()).arg(map->name()), map->id());

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxGiftItemMap->setValue(dlg.selectedData());
}
