/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "entity.h"
#include "character.h"

using namespace CORE;

Entity::Entity(EntityType type, Entity *parent) :
    QObject(parent),
    m_type(type)
{
    static QMUD::IdType currentId = 0;
    m_id = currentId++;
}

QJSValue Entity::jsValue() const
{
    return m_jsValue;
}

Entity::~Entity()
{

}

QMUD::IdType Entity::id() const
{
    return m_id;
}

Entity::EntityType Entity::entityType() const
{
    return m_type;
}

void Entity::initJS(QJSValue value)
{
    m_jsValue = value;

    initJS();
}

void Entity::eventCustom(QString event)
{
    Q_UNUSED(event)
}

void Entity::pause(int seconds, QString event)
{
    emit insertEventCustom(id(), seconds, event);
}

void Entity::initJS()
{
    // Nothing
}

