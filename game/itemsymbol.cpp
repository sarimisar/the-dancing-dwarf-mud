/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemsymbol.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"
#include "world.h"

#include <QJsonArray>

using namespace CORE;

ItemSymbol::ItemSymbol(Entity *parent) :
    Item(QMUD::ItemType::SYMBOL, parent),
    m_iLevel(1),
    m_eType(SymbolType::DIVINE)
{

}

ItemSymbol::~ItemSymbol()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemSymbol", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}
