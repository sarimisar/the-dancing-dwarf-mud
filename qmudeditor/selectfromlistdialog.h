/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef SELECTFROMLISTDIALOG_H
#define SELECTFROMLISTDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class SelectFromListDialog;
}

class SelectFromListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectFromListDialog(QWidget *parent = nullptr);
    ~SelectFromListDialog();

    QListWidget* listWidget() const;
    void addElement(QString name, int index);

    int selectedData() const;

private slots:
    void on_listWidget_itemSelectionChanged();

private:
    Ui::SelectFromListDialog *ui;
};

#endif // SELECTFROMLISTDIALOG_H
