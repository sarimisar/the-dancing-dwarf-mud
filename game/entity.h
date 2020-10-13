/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>
#include <QSharedPointer>
#include <QJSValue>

#include "../global.h"

namespace CORE
{

class Entity : public QObject
{
    Q_OBJECT

public:
    enum class EntityType
    {
        WORLD,
        MAP,
        ROOM,
        ITEM,
        CHARACTER
    };

    explicit Entity(EntityType type, Entity* parent);
    virtual ~Entity();

    QMUD::IdType id() const;
    EntityType entityType() const;
    Q_INVOKABLE virtual QString name(Entity *suitor) const = 0;

    // Gestione script
    QJSValue jsValue() const;
    void initJS(QJSValue value);
    Q_INVOKABLE void pause(int seconds, QString event);
    virtual void eventCustom(QString event);

signals:
    void insertEventCustom(QMUD::IdType id, int seconds, QString event);

public slots:

protected:
    virtual void initJS();

private:
    QMUD::IdType m_id;
    EntityType m_type;

    QJSValue m_jsValue;
};

} // namespace CORE

#endif // ENTITY_H
