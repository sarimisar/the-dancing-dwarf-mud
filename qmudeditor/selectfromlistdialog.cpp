/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "selectfromlistdialog.h"
#include "ui_selectfromlistdialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

SelectFromListDialog::SelectFromListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectFromListDialog)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

SelectFromListDialog::~SelectFromListDialog()
{
    delete ui;
}

void SelectFromListDialog::addElement(QString name, int index)
{
    QListWidgetItem *item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, index);
    ui->listWidget->addItem(item);
}

int SelectFromListDialog::selectedData() const
{
    auto selection = ui->listWidget->selectedItems();
    if (selection.isEmpty())
        return -1;
    else
        return selection[0]->data(Qt::UserRole).toInt();
}

void SelectFromListDialog::on_listWidget_itemSelectionChanged()
{
    auto selection = ui->listWidget->selectedItems();

    if (selection.isEmpty())
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
