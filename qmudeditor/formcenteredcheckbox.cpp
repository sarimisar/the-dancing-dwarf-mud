/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "formcenteredcheckbox.h"
#include "ui_formcenteredcheckbox.h"

FormCenteredCheckBox::FormCenteredCheckBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCenteredCheckBox)
{
    ui->setupUi(this);
}

FormCenteredCheckBox::~FormCenteredCheckBox()
{
    delete ui;
}

QCheckBox *FormCenteredCheckBox::checkBox() const
{
    return ui->checkBox;
}
