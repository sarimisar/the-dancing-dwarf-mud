/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "scripteditordialog.h"
#include "ui_scripteditordialog.h"

#include <QFile>
#include <QSyntaxStyle>
#include <QCXXHighlighter>
#include <QMessageBox>

ScriptEditorDialog::ScriptEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptEditorDialog),
    m_ptrEditor(new QCodeEditor())
{
    ui->setupUi(this);

    ui->layout->addWidget(m_ptrEditor);

    QFile fl(":/code_editor/QCodeEditor/example/resources/styles/drakula.xml");

    if (fl.open(QIODevice::ReadOnly))
    {
        auto style = new QSyntaxStyle(this);

        if (!style->load(fl.readAll()))
        {
            delete style;
        }
        else
        {
            m_ptrEditor->setSyntaxStyle(style);
        }
    }

    m_ptrEditor->setHighlighter(new QCXXHighlighter());
}

ScriptEditorDialog::~ScriptEditorDialog()
{
    delete ui;
}

void ScriptEditorDialog::setText(QString text)
{
    m_ptrEditor->clear();
    m_ptrEditor->setText(text);

    m_strOrigText = text;
}

QString ScriptEditorDialog::text() const
{
    return m_ptrEditor->toPlainText();
}

void ScriptEditorDialog::done(int v)
{
    if (v == QDialog::Rejected && m_ptrEditor->toPlainText() != m_strOrigText)
    {
        auto ret = QMessageBox::question(this, tr("Chiusura"), tr("Vuoi chiudere senza salvare le modifiche?"), QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret != QMessageBox::Yes)
        {
            return;
        }
    }

    QDialog::done(v);
}
