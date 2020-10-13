/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEM_H
#define ITEM_H

#include "entity.h"
#include "../global.h"

#include <QUuid>

namespace CORE
{

class Character;

class Item : public Entity
{
    Q_OBJECT

    friend class ItemResource;
    friend class ItemArmor;
    friend class ItemWeapon;
    friend class ItemTrinket;

public:
    virtual ~Item() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent, QMUD::StaticIdType mapStaticId = QMUD::StaticIdInvalid);
    static QString generateRandomCodeForCharacter(Character* ch, int level);
    static QString generateRandomCodeForCharacter(Character* ch, int level, QMUD::ItemQualityType quality);
    static QString generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality);
    static Item* createFromRandomCode(QString randomCode, Entity *parent);
    static bool fromRandomCodeToValues(QString randomCode, int& level, QMUD::ItemType& itemType, QString& name, QMUD::ItemClassType& classType, QMUD::ItemQualityType& quality);
    static qint64 jsonToSellValue(const QJsonObject &obj);
    static QString jsonToName(const QJsonObject &obj);
    static int jsonToLevel(const QJsonObject &obj);
    static QMUD::ItemQualityType jsonToQuality(const QJsonObject &obj);
    static qint64 getDefaultCoinsValue(int level, QMUD::ItemQualityType quality);

    static int basePrimaryStatBonus(int level);
    static int baseSecondaryStatBonus(int level);
    static int baseTertiaryStatBonus(int level);

    virtual void writeJson(QJsonObject &obj);

    // Tipo
    QMUD::ItemType itemType() const;

    // Id dell'oggetto
    QMUD::StaticIdType staticId() const;

    // Id della mappa dove l'oggetto e' definito
    QMUD::StaticIdType mapStaticId() const;

    // Statistiche
    QString name(Entity *suitor = Q_NULLPTR) const Q_DECL_OVERRIDE;
    QString description() const;
    QMUD::ItemQualityType quality() const;
    qint64 coinsValue() const;
    QUuid uuid() const;
    bool isBind() const;
    void setIsBind(bool bind);
    int level() const;

    // Controlla se l'oggetto e' stato modificato rispetto all'oggetto
    // base definito nella mappa
    bool IsModify() const;

    // Elimina l'oggetto dal mondo
    void destroy();
    bool isAutoDestroy() const;
    void setAutoDestroy(int count = QMUD_GLOBAL_ITEM_AUTO_DESTROY_COUNTER);
    int autoDestroyCounter() const;

    // Bonus base dell'oggetto
    virtual const QMUD::ItemBaseBonus& baseBonus() const;
    virtual const QMUD::ItemBaseStatus& baseTemporaryStatus() const;

signals:
    void removeFromTheWorld(Item*);

public slots:

protected:
    explicit Item(QMUD::ItemType type, Entity *parent = Q_NULLPTR);

private:
    QUuid m_uuid;
    QMUD::ItemType m_eItemType;
    QMUD::StaticIdType m_iStaticId;
    QMUD::StaticIdType m_iMapStaticId;
    QString m_strName;
    QString m_strDescription;
    QMUD::ItemQualityType m_eQuality;
    qint64 m_iCoinsValue;
    int m_iAutoDestroyCounter;
    bool m_bIsBind;
    int m_iLevel;
    QString m_strRandomCode;
};

} // namespace CORE

#endif // ITEM_H
