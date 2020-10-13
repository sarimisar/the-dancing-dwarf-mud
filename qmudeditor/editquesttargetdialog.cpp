/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "editquesttargetdialog.h"
#include "ui_editquesttargetdialog.h"

#include "selectfromlistdialog.h"

EditQuestTargetDialog::EditQuestTargetDialog(const QVector<EditorMap *> &maps, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditQuestTargetDialog),
    m_vMaps(maps)
{
    ui->setupUi(this);
}

EditQuestTargetDialog::~EditQuestTargetDialog()
{
    delete ui;
}

void EditQuestTargetDialog::setData(const EditorMap::QuestData::TargetData &data)
{
    ui->lineEditDescription->setText(data.description);
    ui->plainTextEditCompleteDescription->setPlainText(data.completeDescription);

    ui->groupBoxNpcToTalkTo->setChecked(data.npcToTalkToEnabled);
    ui->spinBoxNpcToTalkToNpcID->setValue(data.npcToTalkToId);
    ui->spinBoxNpcToTalkToMapID->setValue(data.npcToTalkToMapId);

    ui->groupBoxNpcToKill->setChecked(data.npcToKillEnabled);
    ui->spinBoxNpcToKillNpcID->setValue(data.npcToKillId);
    ui->spinBoxNpcToKillMapID->setValue(data.npcToKillMapId);
    ui->spinBoxNpcToKillCount->setValue(data.npcToKillCount);

    ui->groupBoxNpcToKillGiveItem->setChecked(data.npcToKillGiveItemEnabled);
    ui->lineEditNpcToKillGiveItemName->setText(data.npcToKillGiveItemName);
    ui->spinBoxNpcToKillGiveItemProbability->setValue(data.npcToKillGiveItemProbability);
}

EditorMap::QuestData::TargetData EditQuestTargetDialog::data() const
{
    EditorMap::QuestData::TargetData data;

    data.description = ui->lineEditDescription->text();
    data.completeDescription = ui->plainTextEditCompleteDescription->toPlainText();

    data.npcToTalkToEnabled = ui->groupBoxNpcToTalkTo->isChecked();
    data.npcToTalkToId = ui->spinBoxNpcToTalkToNpcID->value();
    data.npcToTalkToMapId = ui->spinBoxNpcToTalkToMapID->value();

    data.npcToKillEnabled = ui->groupBoxNpcToKill->isChecked();
    data.npcToKillId = ui->spinBoxNpcToKillNpcID->value();
    data.npcToKillMapId = ui->spinBoxNpcToKillMapID->value();
    data.npcToKillCount = ui->spinBoxNpcToKillCount->value();

    data.npcToKillGiveItemEnabled = ui->groupBoxNpcToKillGiveItem->isChecked();
    data.npcToKillGiveItemName = ui->lineEditNpcToKillGiveItemName->text();
    data.npcToKillGiveItemProbability = ui->spinBoxNpcToKillGiveItemProbability->value();

    return data;
}

void EditQuestTargetDialog::on_pushButtonNpcToTalkToMapID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
        dlg.addElement(QString("%1 - %2").arg(map->id()).arg(map->name()), map->id());

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxNpcToTalkToMapID->setValue(dlg.selectedData());
}

void EditQuestTargetDialog::on_pushButtonNpcToTalkToNpcID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
    {
        if (map->id() == ui->spinBoxNpcToTalkToMapID->value())
        {
            for (auto npc : map->npcs())
                    dlg.addElement(QString("%1 - %2").arg(npc->id()).arg(npc->name()), npc->id());
            break;
        }
    }

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxNpcToTalkToNpcID->setValue(dlg.selectedData());
}

void EditQuestTargetDialog::on_pushButtonNpcToKillMapID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
        dlg.addElement(QString("%1 - %2").arg(map->id()).arg(map->name()), map->id());

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxNpcToKillMapID->setValue(dlg.selectedData());
}

void EditQuestTargetDialog::on_pushButtonNpcToKillNpcID_clicked()
{
    SelectFromListDialog dlg(this);

    for (auto map : m_vMaps)
    {
        if (map->id() == ui->spinBoxNpcToKillMapID->value())
        {
            for (auto npc : map->npcs())
                    dlg.addElement(QString("%1 - %2").arg(npc->id()).arg(npc->name()), npc->id());
            break;
        }
    }

    if (dlg.exec() == QDialog::Accepted)
        ui->spinBoxNpcToKillNpcID->setValue(dlg.selectedData());
}
