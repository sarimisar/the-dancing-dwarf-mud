/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef FORMCENTEREDCHECKBOX_H
#define FORMCENTEREDCHECKBOX_H

#include <QWidget>
#include <QCheckBox>

namespace Ui {
class FormCenteredCheckBox;
}

class FormCenteredCheckBox : public QWidget
{
    Q_OBJECT

public:
    explicit FormCenteredCheckBox(QWidget *parent = nullptr);
    ~FormCenteredCheckBox();

    QCheckBox* checkBox() const;

private:
    Ui::FormCenteredCheckBox *ui;
};

#endif // FORMCENTEREDCHECKBOX_H
