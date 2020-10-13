/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef NPCCHARACTER_H
#define NPCCHARACTER_H

#include "character.h"

namespace CORE
{

class RoomExit;
class Quest;

class NpcCharacter : public Character
{
    Q_OBJECT

public:
    struct ShopItemData
    {
        ShopItemData() :
            id(QMUD::StaticIdInvalid),
            mapId(QMUD::StaticIdInvalid)
        { }

        QMUD::StaticIdType id;
        QMUD::StaticIdType mapId;
    };

    virtual ~NpcCharacter() Q_DECL_OVERRIDE;

    static NpcCharacter* loadNpcCharacter(QJsonObject& obj, QMap<QMUD::StaticIdType, QJsonObject> objs, QMUD::StaticIdType mapStaticId, Room *room);

    virtual void update() Q_DECL_OVERRIDE;

    static double baseManaAndHpMultiplier(QMUD::NpcType type);
    static double baseStatMultiplier(QMUD::NpcType type);
    static qint64 baseCoins(int level, QMUD::NpcType type);
    static qint64 baseExperience(int level, QMUD::NpcType type);

    QMUD::NpcType npcType() const;
    QString description() const;
    qint64 giveExperience() const;
    bool isToResurrect() const;
    void setToResurrect(bool v);

    QMUD::StaticIdType staticId() const;
    QMUD::StaticIdType mapStaticId() const;
    QMUD::StaticIdType roomStaticId() const;

    QMap<QMUD::EventType, QString> scripts() const;

    bool flagCheck(QMUD::NpcCharacterFlagType flag) const;
    QSet<QMUD::NpcCharacterFlagType> flags() const;

    void eventCustom(QString event) Q_DECL_OVERRIDE;
    void eventEnter(Character* ch);
    bool eventAbility(Character* ch);

    Q_INVOKABLE void executeCommand(QString cmd);
    Q_INVOKABLE void sendToRoom(QString message);

    QMap<QMUD::StaticIdType, LootItemData> loot() const;

    // Negozio
    bool isVendor() const;
    QVector<ShopItemData> shopItems() const;
    double shopSaleInflation() const;
    double shopPurchaseInflation() const;
    bool shopPurchaseBase() const;
    bool shopPurchaseArmor() const;
    bool shopPurchaseWeapon() const;

    // Hunt
    Q_INVOKABLE void startHunt(Character* target);
    Q_INVOKABLE void stopHunt();
    Q_INVOKABLE bool isHunting();

    // Assistenza
    int assistGroup() const;

    QMap<QMUD::IdType, qint64> aggro() const;
    qint64 aggro(Character* ch) const;
    void clearAggro();
    void addAggro(Character* ch, qint64 value);
    void setAggro(Character* ch, qint64 value);
    Character *updateAggro();

    void addHostileCh(QMUD::IdType id);
    QMap<QMUD::IdType, int> hostiles() const;

    // Loot
    bool rollForRandomLoot() const;

    // Quest
    QVector<Quest*> questsBeGived() const;
    void setQuestsBeGived(QVector<Quest*> questsBeGived);

signals:
    void command(NpcCharacter* npc, QString cmd);

public slots:

protected:
    explicit NpcCharacter(Room *parent = Q_NULLPTR);
    virtual void initJS() Q_DECL_OVERRIDE;

    virtual int armorClassBaseBonus() const Q_DECL_OVERRIDE;
    virtual int numberOfAttacksBonus() const Q_DECL_OVERRIDE;
    virtual int constitutionItemBonus() const Q_DECL_OVERRIDE;
    virtual int dexterityItemBonus() const Q_DECL_OVERRIDE;
    virtual int intelligenceItemBonus() const Q_DECL_OVERRIDE;
    virtual int wisdomItemBonus() const Q_DECL_OVERRIDE;
    virtual int charismaItemBonus() const Q_DECL_OVERRIDE;
    virtual int strengthItemBonus() const Q_DECL_OVERRIDE;

    RoomExit* hunt(Character* target, bool currentMapOnly, bool &isCurrRoom);

private:
    QMUD::StaticIdType m_iStaticId;
    QMUD::StaticIdType m_iMapStaticId;
    QMUD::StaticIdType m_iRoomStaticId;
    bool m_bIsToResurrect;

    QString m_strDescription;
    qint64 m_iGiveExperience;

    QMUD::NpcType m_eNpcType;

    // Statistiche
    int m_iConstitutionItemsBonus;
    int m_iDexterityItemsBonus;
    int m_iIntelligenceItemsBonus;
    int m_iWisdomItemsBonus;
    int m_iCharismaItemsBonus;
    int m_iStrengthItemsBonus;

    // Base bonus
    int m_iArmorClassBaseBonus;
    int m_iNumberOfAttacksBonus;

    // Flags
    QSet<QMUD::NpcCharacterFlagType> m_vFlags;

    QMap<QMUD::EventType, QString> m_mapEventFunction;
    QJSValue m_jsFunctionEventCustom;
    QJSValue m_jsFunctionEventEnter;
    QJSValue m_jsFunctionEventAbility;

    QMap<QMUD::StaticIdType, LootItemData> m_mapLoot;

    // Negozio
    QVector<ShopItemData> m_vShopItems;
    double m_dShopSaleInflation;
    double m_dShopPurchaseInflation;
    bool m_bShopPurchaseBase;
    bool m_bShopPurchaseArmor;
    bool m_bShopPurchaseWeapon;

    // Hunt
    QMUD::IdType m_iChHunted;
    QString m_strChHuntedPcCharacterName;
    int m_iChHuntedFailedTry;
    int m_iChHuntedSuccessedTry;

    // Assistenza
    int m_iAssistGroup;

    // Aggro
    QMap<QMUD::IdType, qint64> m_mapAggro;

    // Lista degli avversari ostili
    QMap<QMUD::IdType, int> m_mapHostiles;

    // Quest
    QVector<Quest*> m_vQuests;
};

} // namespace CORE


#endif // NPCCHARACTER_H
