/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include "connection.h"

#include <QLineEdit>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomLineEdit(QWidget * parent = Q_NULLPTR);

    void setConnection(Connection* conn);

private slots:
    void sendCommandN();
    void sendCommandS();
    void sendCommandW();
    void sendCommandE();
    void sendCommandU();
    void sendCommandD();
    void sendCommandLook();

signals:
    void appendStr(QString str);

protected:
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

private:
    Connection* m_ptrConnection;
};

#endif // CUSTOMLINEEDIT_H
