/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/
#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#include "entity.h"
#include "item.h"
#include "../global.h"

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <QTime>



namespace CORE
{

class Room;
class ItemWeapon;
class ItemArmor;
class ItemTrinket;
class CharacterClass;

class Character : public Entity
{
    Q_OBJECT

    Q_PROPERTY(int level READ level)
    Q_PROPERTY(QMUD::SexType sex READ sex)
    Q_PROPERTY(QMUD::RaceType race READ race)
    Q_PROPERTY(QMUD::ClassType chClass READ chClassType)
    Q_PROPERTY(int hpMax READ hitPointsCurrentMaximum)
    Q_PROPERTY(int hp READ hitPointsCurrent)
    Q_PROPERTY(int apMax READ actionPointsCurrentMaximum)
    Q_PROPERTY(int ap READ actionPointsCurrent)
    Q_PROPERTY(int mpMax READ manaPointsCurrentMaximum)
    Q_PROPERTY(int mp READ manaPointsCurrent)
    Q_PROPERTY(int constitution READ constitution)
    Q_PROPERTY(int dexterity READ dexterity)
    Q_PROPERTY(int intelligence READ intelligence)
    Q_PROPERTY(int wisdom READ wisdom)
    Q_PROPERTY(int charisma READ charisma)
    Q_PROPERTY(int strength READ strength)
    Q_PROPERTY(int armorClass READ armorClass)
    Q_PROPERTY(int hitBonus READ hitBonus)
    Q_PROPERTY(int damageBonus READ damageBonus)
    Q_PROPERTY(int tsConstitution READ tsConstitution)
    Q_PROPERTY(int tsDexterity READ tsDexterity)
    Q_PROPERTY(int tsIntelligence READ tsIntelligence)
    Q_PROPERTY(int tsWisdom READ tsWisdom)
    Q_PROPERTY(int tsCharisma READ tsCharisma)
    Q_PROPERTY(int tsStrength READ tsStrength)
    Q_PROPERTY(int cdConstitution READ cdConstitution)
    Q_PROPERTY(int cdDexterity READ cdDexterity)
    Q_PROPERTY(int cdIntelligence READ cdIntelligence)
    Q_PROPERTY(int cdWisdom READ cdWisdom)
    Q_PROPERTY(int cdCharisma READ cdCharisma)
    Q_PROPERTY(int cdStrength READ cdStrength)
    Q_PROPERTY(bool isUnconscious READ isUnconscious)
    Q_PROPERTY(bool isDead READ isDead)
    Q_PROPERTY(qint64 coins READ coins)
    Q_PROPERTY(int numberOfAttacks READ numberOfAttacks)

public:
    enum class ChType
    {
        CHTYPE_NPC,
        CHTYPE_PC
    };

    // Questo enumerativo viene usato in JSON
    // direttamente. Non eliminare le entry e non
    // sostituirle
    enum class EquipmentSlot
    {
        EQSLOT_WEAPON,
        EQSLOT_CHEST,
        EQSLOT_SHIELD,
        EQSLOT_GLOVES,
        EQSLOT_SHOULDER,
        EQSLOT_BRACERS,
        EQSLOT_HELM,
        EQSLOT_LEG,
        EQSLOT_BOOTS,
        EQSLOT_RING_LEFT,
        EQSLOT_RING_RIGHT,
        EQSLOT_BRACELET_LEFT,
        EQSLOT_BRACELET_RIGHT,
        EQSLOT_NECKLACE
    };

    struct ChClassData
    {
        friend class Character;

        ChClassData();
        virtual ~ChClassData();

        CharacterClass* chClass;

        int level() const;
        qint64 experience() const;

    private:
        int& levelRef();
        qint64& experienceRef();

        int m_iLevel;
        qint64 m_iExperience;
    };

    struct LootItemData
    {
        LootItemData();
        LootItemData(QJsonObject json,
                     QString randomCode,
            QMUD::StaticIdType mapStaticId,
            QMUD::ItemQualityType quality,
            QMUD::ItemRarityType rarity,
            QMUD::StaticIdType staticId,
            QString name,
            bool toSell,
            qint64 coinsValue);

        QJsonObject toJson() const;
        bool fromJson(QJsonObject obj);

        QJsonObject json() const { return m_json; }
        QString randomCode() const { return m_randomCode; }
        QMUD::StaticIdType mapStaticId() const { return m_mapStaticId; }
        QMUD::ItemQualityType quality() const { return m_eQuality; }
        QMUD::ItemRarityType rarity() const { return m_eRarity; }
        QMUD::StaticIdType staticId() const { return m_iStaticId; }
        QString name() const { return m_strName; }
        bool toSell() const { return m_bToSell; }
        qint64 coinsValue() const { return m_iCoinsValue; }

        void setToSell(bool toSell) { m_bToSell = toSell; }

    private:
        QJsonObject m_json;
        QString m_randomCode;
        QMUD::StaticIdType m_mapStaticId;
        QMUD::ItemQualityType m_eQuality;
        QMUD::ItemRarityType m_eRarity;
        QMUD::StaticIdType m_iStaticId;
        QString m_strName;
        bool m_bToSell;
        qint64 m_iCoinsValue;
    };

    virtual ~Character() Q_DECL_OVERRIDE;

    ChType chType() const;

    // Update
    virtual void update();
    virtual void updateStatus();

    // Stanza corrente
    Room* room() const;

    // Statistiche
    int level() const;
    qint64 experience() const;
    int hitDice() const;
    int manaDice() const;
    QMUD::SexType sex() const;
    QMUD::RaceType race() const;
    QMUD::ClassType chClassType() const;
    CharacterClass* chClass() const;

public slots:
    QString name(Entity *suitor) const Q_DECL_OVERRIDE;

public:
    // HP, MANA e AP
    int hitPointsCurrentMaximum() const;
    int hitPointsCurrent() const;
    int actionPointsCurrentMaximum() const;
    int actionPointsCurrent() const;
    int manaPointsCurrentMaximum() const;
    int manaPointsCurrent() const;
    void useManaPoint(int mana);
    void useActionPoint(int ap);

    // Caratteristiche
    int constitution() const;
    int dexterity() const;
    int intelligence() const;
    int wisdom() const;
    int charisma() const;
    int strength() const;

    int constitutionBase() const;
    int dexterityBase() const;
    int intelligenceBase() const;
    int wisdomBase() const;
    int charismaBase() const;
    int strengthBase() const;

    // Food, Drink e Tonici
    void setCurrentFood(QString foodName, int rounds, int hpBonus, int apBonus);
    QString currentFood() const;
    bool currentFoodIsEating() const;
    void currentFoodStopEating();
    int currentFoodHpGainBonus() const;
    int currentFoodApGainBonus() const;
    void setCurrentDrink(QString drinkName, int rounds, int mpBonus);
    QString currentDrink() const;
    bool currentDrinkIsDrinking() const;
    void currentDrinkStopDrinking();
    int currentDrinkMpGainBonus() const;
    void setCurrentTonic(QString tonicName, int rounds, QMUD::ItemBaseBonus bonus);
    void currentTonicRemove();
    QString currentTonic();
    int currentTonicRounds();
    bool canDrinkTonic();

    // Altri bonus
    int armorClass() const;
    int hitBonus() const;
    int damageBonus() const;

    // Tiri salvezza
    int tsConstitution() const;
    int tsDexterity() const;
    int tsIntelligence() const;
    int tsWisdom() const;
    int tsCharisma() const;
    int tsStrength() const;

    // Classi difficolta'
    int cdConstitution() const;
    int cdDexterity() const;
    int cdIntelligence() const;
    int cdWisdom() const;
    int cdCharisma() const;
    int cdStrength() const;

    // Danno e cura
    void healingHitPoints(int value, Character* chStriker = Q_NULLPTR);
    void healingManaPoints(int value, Character* chStriker = Q_NULLPTR);
    void healingActionPoints(int value, Character* chStriker = Q_NULLPTR);
public slots:
    int calcDamage(int damage, QMUD::DamageType type);
    void damage(int damage, QMUD::DamageType type, Character* chStriker);
    void kill();

public:
    // Gestione status
    void setTemporaryStatus(QMUD::TemporaryStatusType status, int turns, bool message);
    QMap<QMUD::TemporaryStatusType, int> temporaryStatus() const;
    bool canMakeAction(QString &message) const;
    bool canAttack(Character* ch) const;
    bool canAttack(Character* ch, QString &message) const;
    bool hasTemporaryStatus(QMUD::TemporaryStatusType status) const;
    bool hasTemporaryStatus(QMUD::TemporaryStatusType status, int& turns) const;
    void removeTemporaryStatus(QMUD::TemporaryStatusType status, bool force = false);
    bool isUnconscious() const;
    bool isDead() const;
    void sendRIP() const;
    void illusoryImagesResetHP();
    void illusoryImageDamage(int index, int damage);
public slots:
    bool canSee(const Character *target) const;

public:
    // Gestione abilita' e spell
    bool canUseAbility(QMUD::AbilityType ability) const;
    bool canUseSpell(QMUD::SpellType spell) const;

    // Gestione monete e risorse
    qint64 coins() const;
    QMap<QMUD::ResourceType, qint64> resources() const;
    void addResource(QMUD::ResourceType resource, qint64 v, bool countInStatistics = true);
    void removeResource(QMUD::ResourceType resource, qint64 v);
    bool hasResouces(QMap<QMUD::ResourceType, qint64> resources) const;
    bool useResources(QMap<QMUD::ResourceType, qint64> resources);
public slots:
    void addCoins(qint64 coins);
    bool removeCoins(qint64 coins);

public:
    // Gestione esperienza
    void addExperience(qint64 experience);

    // Target
    QMUD::IdType targetId() const;
    void setTarget(QMUD::IdType id, bool updateCurrent);
    Character* targetCharacter(bool inRoom);

    // Gestione combattimento
    void setInCombat(bool v);
    void executeAttack();
    int numberOfAttacks() const;
    int rollAttackHit() const;
    int rollAttackDamage() const;
    double attackAggroMultiplier() const;
public slots:
    bool isInCombat() const;

public:
    // Gestione equipaggiamento ed inventario
    QVector<Item*> inventory(bool withEquippedItems = false) const;
    QVector<Item*> equipment() const;
    int inventorySize(bool withEquippedItems) const;
    Item* equippedItem(EquipmentSlot slot) const;
    ItemWeapon* equippedWeapon() const;
    ItemArmor* equippedShield() const;
    bool canEquip(Item* item, QString &alreadyWearItemName) const;
    bool equip(Item* item, bool message = true);
    bool canUnequip(Item* item);
    Item *canUnequip(QString itemName);
    bool unequip(Item* item);
    Item* unequip(QString itemName);
    bool isEquipped(Item* item, EquipmentSlot &slot) const;
    bool isEquipped(Item* item) const;

    // Gestione follow
    void setChFollowed(QMUD::IdType id, QString name);
    QMUD::IdType chFollowed() const;
    QString chFollowedName() const;

signals:
    void removeFromTheWorld(Character *ch);

protected:
    explicit Character(ChType chType, Room *parent = Q_NULLPTR);

    virtual int armorClassBaseBonus() const { return 0; }
    virtual int numberOfAttacksBonus() const { return 0; }

    virtual int constitutionItemBonus() const { return 0; }
    virtual int dexterityItemBonus() const { return 0; }
    virtual int intelligenceItemBonus() const { return 0; }
    virtual int wisdomItemBonus() const { return 0; }
    virtual int charismaItemBonus() const { return 0; }
    virtual int strengthItemBonus() const { return 0; }

    void setName(QString name);
    void setSex(QMUD::SexType sex);
    void setHitPointsCurrent(int hp);
    void setActionPointsCurrent(int ap);
    void setManaPointsCurrent(int mp);

    void setConstitutionBase(int v);
    void setDexterityBase(int v);
    void setIntelligenceBase(int v);
    void setWisdomBase(int v);
    void setCharismaBase(int v);
    void setStrengthBase(int v);

    void setResource(QMUD::ResourceType resource, qint64 v);
    void setResources(QMap<QMUD::ResourceType, qint64> v);

    void setChClass(CharacterClass *chClass, int level, qint64 experience);
    void setRace(QMUD::RaceType race);

    QMUD::DamageType attackDamageType() const;

    void updateTemporaryStatus(bool updateRounds);
    void updateInfiniteTemporaryStatus();

protected:
    ItemWeapon *m_ptrEquippedWeapon;
    ItemArmor *m_ptrEquippedChest;
    ItemArmor *m_ptrEquippedShield;
    ItemArmor *m_ptrEquippedGloves;
    ItemArmor *m_ptrEquippedShoulder;
    ItemArmor *m_ptrEquippedBracers;
    ItemArmor *m_ptrEquippedHelm;
    ItemArmor *m_ptrEquippedLeg;
    ItemArmor *m_ptrEquippedBoots;
    ItemTrinket *m_ptrEquippedRingLeft;
    ItemTrinket *m_ptrEquippedRingRight;
    ItemTrinket *m_ptrEquippedBraceletLeft;
    ItemTrinket *m_ptrEquippedBraceletRight;
    ItemTrinket *m_ptrEquippedNecklace;

    QVector<Item**> m_vEquippedItems;
    QMap<QMUD::TemporaryStatusType, int> m_mapTemporaryStatus;

private:
    ChType m_eType;

    QString m_strName;
    QMUD::SexType m_eSex;
    QMUD::RaceType m_eRace;
    CharacterClass* m_chClass;
    int m_iLevel;
    qint64 m_iExperience;

    int m_iHitPointsCurrent;
    int m_iActionsPointsCurrent;
    int m_iManaPointsCurrent;

    int m_iConstitutionBase;
    int m_iDexterityBase;
    int m_iIntelligenceBase;
    int m_iWisdomBase;
    int m_iCharismaBase;
    int m_iStrengthBase;

    QVector<int> m_vIllusoryImagesCurrentHP;

    // Food and Drink
    struct FoodInfo
    {
		FoodInfo() :
			eating(false),
			hpBonus(0),
			apBonus(0),
			rounds(0)
		{ }

		void update()
		{
			if (rounds > 0)
				rounds--;

			if (rounds <= 0)
			{
				name.clear();
				eating = false;
				hpBonus = 0;
				apBonus = 0;
				rounds = 0;
			}
		}

		QString name;
		bool eating;
        int hpBonus;
		int apBonus;
        int rounds;
    };

    struct DrinkInfo
    {
		DrinkInfo() :
			drinking(false),
			mpBonus(0),
			rounds(0)
		{ }

		void update()
		{
			if (rounds > 0)
				rounds--;

			if (rounds <= 0)
			{
				name.clear();
				drinking = false;
				mpBonus = 0;
				rounds = 0;
			}
		}

		QString name;
		bool drinking;
		int mpBonus;
        int rounds;
    };

    struct TonicInfo
    {
        TonicInfo() :
            rounds(0),
            waitTimeRounds(0)
        { }

        void update()
        {
            if (rounds > 0)
                rounds--;

            if (waitTimeRounds > 0)
                waitTimeRounds--;

            if (rounds <= 0)
            {
                name.clear();
                rounds = 0;
                bonus = QMUD::ItemBaseBonus();
            }
        }

        QString name;
        QMUD::ItemBaseBonus bonus;
        int rounds;
        int waitTimeRounds;
    };

	FoodInfo m_foodInfo;
	DrinkInfo m_drinkInfo;
    TonicInfo m_tonicInfo;

	// Risorse
    QMap<QMUD::ResourceType, qint64> m_mapResource;

    // Flags
    bool m_bInCombat;
    QMUD::IdType m_targetId;

    // Follow
    QMUD::IdType m_iChFollowed;
    QString m_strChFollowedName;
};

} // namespace CORE

#endif // CHARACTER_H
