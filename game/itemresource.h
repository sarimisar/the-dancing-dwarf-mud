/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMRESOURCE_H
#define ITEMRESOURCE_H

#include "item.h"
#include "../global.h"

#include <QDateTime>

namespace CORE
{

class ItemResource : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemResource() Q_DECL_OVERRIDE;

    static Item* create(QMUD::ResourceType resourceType, Entity *parent);

    QMUD::ResourceType resource() const;
    bool collectable(QDateTime now) const;
    bool collectable() const;
    int collect();
    QString action() const;
    QString actionOther() const;

public slots:

private:
    explicit ItemResource(Entity *parent = Q_NULLPTR);

    QMUD::ResourceType m_eResource;
    QDateTime m_timeCollectable;
    QString m_strAction;
    QString m_strActionOther;
    int m_iAmount;
};

} // namespace CORE

#endif // ITEMRESOURCE_H
