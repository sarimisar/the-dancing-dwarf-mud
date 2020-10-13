/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "editexitdialog.h"
#include "ui_editexitdialog.h"
#include "selectfromlistdialog.h"
#include "editormap.h"

EditExitDialog::EditExitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditExitDialog)
{
    ui->setupUi(this);

    ui->comboBoxAction->addItem("APRI/CHIUDI",  static_cast<int>(QMUD::ActionType::OPEN));
    ui->comboBoxAction->addItem("SPOSTA",       static_cast<int>(QMUD::ActionType::MOVE));
    ui->comboBoxAction->addItem("NESSUNA",      static_cast<int>(QMUD::ActionType::UNKNOWN));

    setData(ExitData());
}

EditExitDialog::~EditExitDialog()
{
    delete ui;
}

void EditExitDialog::setData(const ExitData &data)
{
    ui->lineEditCode->setText(data.code);
    ui->lineEditDoor->setText(data.door);
    ui->lineEditDescOpen->setText(data.descOpen);
    ui->lineEditDescOpenOther->setText(data.descOpenOther);
    ui->lineEditDescClose->setText(data.descClose);
    ui->lineEditDescCloseOther->setText(data.descCloseOther);
    ui->checkBoxIsOpen->setChecked(data.isOpen);
    ui->checkBoxIsHidden->setChecked(data.isHidden);

    QMUD::ActionType action = data.action;
    if (action == QMUD::ActionType::CLOSE)
        action = QMUD::ActionType::OPEN;

    for (int i = 0; i < ui->comboBoxAction->count(); ++i)
        if (ui->comboBoxAction->itemData(i, Qt::UserRole).toInt() == static_cast<int>(action))
        {
            ui->comboBoxAction->setCurrentIndex(i);
            break;
        }
}

EditExitDialog::ExitData EditExitDialog::data() const
{
    ExitData data;

    data.code = ui->lineEditCode->text().simplified();
    data.door = ui->lineEditDoor->text().simplified();
    data.descOpen = ui->lineEditDescOpen->text().simplified();
    data.descOpenOther = ui->lineEditDescOpenOther->text().simplified();
    data.descClose = ui->lineEditDescClose->text().simplified();
    data.descCloseOther = ui->lineEditDescCloseOther->text().simplified();
    data.isOpen = ui->checkBoxIsOpen->isChecked();
    data.isHidden = ui->checkBoxIsHidden->isChecked();
    data.action = static_cast<QMUD::ActionType>(ui->comboBoxAction->currentData().toInt());

    return data;
}
