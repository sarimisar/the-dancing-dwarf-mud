/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <QVector>

namespace CORE
{

class PcCharacter;

class Group : public QObject
{
    Q_OBJECT
public:
    explicit Group(QObject *parent = Q_NULLPTR);

    PcCharacter* leader() const;
    void addCharacter(PcCharacter* ch);
    void removeCharacter(PcCharacter* ch);
    QVector<PcCharacter*> characters() const;

signals:
    void disbanded(Group*);
    void characterEnter(Group*, PcCharacter* ch);
    void characterExit(Group*, PcCharacter* ch);

public slots:

private:
    QVector<PcCharacter*> m_vCharacters;
};

}

#endif // GROUP_H
