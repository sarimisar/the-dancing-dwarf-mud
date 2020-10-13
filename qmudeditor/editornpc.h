/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef EDITORNPC_H
#define EDITORNPC_H

#include <QObject>

#include "../common.h"

class EditorNpc : public QObject
{
    Q_OBJECT
public:
    explicit EditorNpc(QObject *parent = Q_NULLPTR);

    struct LootItemInfo
    {
        LootItemInfo() :
            id(0),
            rarity(QMUD::ItemRarityType::RARITY1)
        { }

        int id;
        QMUD::ItemRarityType rarity;
    };

    bool read(const QJsonObject& obj);
    bool write(QJsonObject& obj) const;

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(QString name);

    QMUD::SexType sex() const;
    void setSex(QMUD::SexType type);

    QString description() const;
    void setDescription(QString description);

    QMUD::RaceType race() const;
    void setRace(QMUD::RaceType race);

    QMUD::ClassType classType() const;
    void setClassType(QMUD::ClassType c);

    int level() const;
    void setLevel(int level);

    QMUD::NpcType type() const;
    void setType(QMUD::NpcType type);

    int bonusAttacks() const;
    void setBonusAttacks(int count);

    QVector<QMUD::NpcCharacterFlagType> flags() const;
    void setFlags(const QVector<QMUD::NpcCharacterFlagType> &flags);

    int assistGroup() const;
    void setAssistGroup(int group);

    QString eventCustom() const;
    void setEventCustom(QString str);

    QString eventEnter() const;
    void setEventEnter(QString str);

    QString eventAbility() const;
    void setEventAbility(QString str);

    QVector<LootItemInfo> loot() const;
    void setLoot(QVector<LootItemInfo> loot);

    // Shop
    bool shopEnabled() const;
    void setShopEnabled(bool enabled);
    QVector<int> shopList() const;
    void setShopList(QVector<int> list);
    double shopSaleMultiplier() const;
    void setShopSaleMultiplier(double value);
    double shopPurchaseMultiplier() const;
    void setShopPurchaseMultiplier(double value);
    bool shopPurchaseBase() const;
    void setShopPurchaseBase(bool value);
    bool shopPurchaseArmor() const;
    void setShopPurchaseArmor(bool value);
    bool shopPurchaseWeapon() const;
    void setShopPurchaseWeapon(bool value);

private:
    int m_id;

    QString m_name;
    QMUD::SexType m_eSex;
    QString m_description;
    QMUD::RaceType m_eRace;
    QMUD::ClassType m_eClass;
    QMUD::NpcType m_eType;
    int m_iLevel;
    int m_iBonusAttacks;
    QVector<QMUD::NpcCharacterFlagType> m_flags;
    int m_iAssistGroup;
    QString m_strEventCustom;
    QString m_strEventEnter;
    QString m_strEventAbility;
    QVector<LootItemInfo> m_vLoot;

    // Shop
    bool m_bShopEnabled;
    QVector<int> m_vShopList;
    double m_dShopSaleMultiplier;
    double m_dShopPurchaseMultiplier;
    bool m_bShopPurchaseBase;
    bool m_bShopPurchaseArmor;
    bool m_bShopPurchaseWeapon;
};

Q_DECLARE_METATYPE(EditorNpc*)

#endif // EDITORNPC_H
