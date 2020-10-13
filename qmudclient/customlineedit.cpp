/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "customlineedit.h"

#include <QShortcut>
#include <QKeyEvent>

CustomLineEdit::CustomLineEdit(QWidget *parent) : QLineEdit(parent),
    m_ptrConnection(Q_NULLPTR)
{
    //new QShortcut(QKeySequence(Qt::Key_1 + Qt::KeypadModifier), this, SLOT(sendCommandD()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_2 + Qt::KeypadModifier), this, SLOT(sendCommandS()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_3 + Qt::KeypadModifier), this, SLOT(sendCommandD()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_4 + Qt::KeypadModifier), this, SLOT(sendCommandW()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_5 + Qt::KeypadModifier), this, SLOT(sendCommandLook()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_6 + Qt::KeypadModifier), this, SLOT(sendCommandE()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_7 + Qt::KeypadModifier), this, SLOT(sendCommandU()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_8 + Qt::KeypadModifier), this, SLOT(sendCommandN()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
    //new QShortcut(QKeySequence(Qt::Key_9 + Qt::KeypadModifier), this, SLOT(sendCommandU()), Q_NULLPTR, Qt::WidgetWithChildrenShortcut);
}

void CustomLineEdit::setConnection(Connection *conn)
{
    m_ptrConnection = conn;
}

void CustomLineEdit::sendCommandN()
{
    emit appendStr("nord");
    m_ptrConnection->send("nord");
}

void CustomLineEdit::sendCommandS()
{
    emit appendStr("sud");
    m_ptrConnection->send("sud");
}

void CustomLineEdit::sendCommandW()
{
    emit appendStr("ovest");
    m_ptrConnection->send("ovest");
}

void CustomLineEdit::sendCommandE()
{
    emit appendStr("est");
    m_ptrConnection->send("est");
}

void CustomLineEdit::sendCommandU()
{
   emit  appendStr("alto");
    m_ptrConnection->send("alto");
}

void CustomLineEdit::sendCommandD()
{
    emit appendStr("basso");
    m_ptrConnection->send("basso");
}

void CustomLineEdit::sendCommandLook()
{
    emit appendStr("guarda");
    m_ptrConnection->send("guarda");
}

void CustomLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers().testFlag(Qt::KeypadModifier))
    {
             if (event->key() == Qt::Key_1) sendCommandD();
        else if (event->key() == Qt::Key_2) sendCommandS();
        else if (event->key() == Qt::Key_3) sendCommandD();
        else if (event->key() == Qt::Key_4) sendCommandW();
        else if (event->key() == Qt::Key_5) sendCommandLook();
        else if (event->key() == Qt::Key_6) sendCommandE();
        else if (event->key() == Qt::Key_7) sendCommandU();
        else if (event->key() == Qt::Key_8) sendCommandN();
        else if (event->key() == Qt::Key_9) sendCommandU();
        else
            return QLineEdit::keyPressEvent(event);

        event->accept();
        return;
    }

    return QLineEdit::keyPressEvent(event);
}

void CustomLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (event->modifiers().testFlag(Qt::KeypadModifier))
    {
        event->accept();
        return;
    }
}
