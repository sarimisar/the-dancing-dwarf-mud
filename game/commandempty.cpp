/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandempty.h"

using namespace CORE;

CommandEmpty::CommandEmpty() :
    Command()
{

}

CommandEmpty::~CommandEmpty()
{

}

bool CommandEmpty::isOf(QString cmd)
{
    return cmd.isEmpty();
}

void CommandEmpty::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)
    Q_UNUSED(player)
}

void CommandEmpty::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}
