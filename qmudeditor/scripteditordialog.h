/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef SCRIPTEDITORDIALOG_H
#define SCRIPTEDITORDIALOG_H

#include <QDialog>
#include <QCodeEditor>

namespace Ui {
class ScriptEditorDialog;
}

class ScriptEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptEditorDialog(QWidget *parent = nullptr);
    virtual ~ScriptEditorDialog() Q_DECL_OVERRIDE;

    void setText(QString text);
    QString text() const;

protected:
    virtual void done(int v) Q_DECL_OVERRIDE;

private:
    Ui::ScriptEditorDialog *ui;

    QCodeEditor *m_ptrEditor;

    QString m_strOrigText;
};

#endif // SCRIPTEDITORDIALOG_H
