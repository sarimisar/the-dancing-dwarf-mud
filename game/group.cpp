/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "group.h"

using namespace CORE;

Group::Group(QObject *parent) :
    QObject(parent)
{

}

PcCharacter *Group::leader() const
{
    Q_ASSERT(!m_vCharacters.isEmpty());

    return m_vCharacters.first();
}

void Group::addCharacter(PcCharacter *ch)
{
    m_vCharacters.push_back(ch);

    emit characterEnter(this, ch);
}

void Group::removeCharacter(PcCharacter *ch)
{
    if (m_vCharacters.removeAll(ch) != 1)
        Q_ASSERT(false);

    emit characterExit(this, ch);

    if (m_vCharacters.size() <= 1)
        emit disbanded(this);
}

QVector<PcCharacter *> Group::characters() const
{
    return m_vCharacters;
}

