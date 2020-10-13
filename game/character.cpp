/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "character.h"
#include "player.h"
#include "room.h"
#include "npccharacter.h"
#include "pccharacter.h"
#include "item.h"
#include "itemweapon.h"
#include "itemarmor.h"
#include "itemtrinket.h"
#include "characterclass.h"
#include "characterclasswarrior.h"
#include "group.h"
#include "world.h"
#include "command.h"

#include "../global.h"
#include "../globaldebug.h"

#include <QPair>

#include <chrono>

using namespace CORE;

#define STATUS_TO_REMOVE (-1)

Character::Character(ChType type, Room *parent) :
    Entity(EntityType::CHARACTER, parent),
    m_ptrEquippedWeapon(Q_NULLPTR),
    m_ptrEquippedChest(Q_NULLPTR),
    m_ptrEquippedShield(Q_NULLPTR),
    m_ptrEquippedGloves(Q_NULLPTR),
    m_ptrEquippedShoulder(Q_NULLPTR),
    m_ptrEquippedBracers(Q_NULLPTR),
    m_ptrEquippedHelm(Q_NULLPTR),
    m_ptrEquippedLeg(Q_NULLPTR),
    m_ptrEquippedBoots(Q_NULLPTR),
    m_ptrEquippedRingLeft(Q_NULLPTR),
    m_ptrEquippedRingRight(Q_NULLPTR),
    m_ptrEquippedBraceletLeft(Q_NULLPTR),
    m_ptrEquippedBraceletRight(Q_NULLPTR),
    m_ptrEquippedNecklace(Q_NULLPTR),
    m_eType(type),
    m_eSex(QMUD::SexType::MALE),
    m_eRace(QMUD::RaceType::HUMAN),
    m_chClass(CharacterClass::fromType(QMUD::ClassType::WARRIOR)),
    m_iLevel(1),
    m_iExperience(0),
    m_iHitPointsCurrent(0),
    m_iActionsPointsCurrent(0),
    m_iManaPointsCurrent(0),
    m_iConstitutionBase(0),
    m_iDexterityBase(0),
    m_iIntelligenceBase(0),
    m_iWisdomBase(0),
    m_iCharismaBase(0),
    m_iStrengthBase(0),
    m_bInCombat(false),
    m_targetId(QMUD::IdInvalid),
    m_iChFollowed(QMUD::IdInvalid)
{
    // Creo il vettore degli oggetti equipaggiati
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedWeapon)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedChest)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedShield)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedGloves)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedShoulder)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedBracers)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedHelm)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedLeg)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedBoots)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedRingLeft)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedRingRight)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedBraceletLeft)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedBraceletRight)));
    m_vEquippedItems.push_back(reinterpret_cast<Item**>(&(m_ptrEquippedNecklace)));
    m_vEquippedItems.squeeze();
}

Character::~Character()
{
     GLOBAL_DEBUG_DESTRUCTOR("Character", name(Q_NULLPTR));
}

Character::ChType Character::chType() const
{
    return m_eType;
}

QString Character::name(Entity *suitor) const
{
    if (suitor && suitor->entityType() == EntityType::CHARACTER &&
            !static_cast<const Character*>(suitor)->canSee(this))
        return tr("qualcuno");
    else
        return m_strName;
}

int Character::level() const
{
    return m_iLevel;
}

qint64 Character::experience() const
{
    return m_iExperience;
}

int Character::hitDice() const
{
    return m_chClass->hitDice();
}

int Character::manaDice() const
{
    return m_chClass->manaDice();
}

QMUD::SexType Character::sex() const
{
    return m_eSex;
}

QMUD::RaceType Character::race() const
{
    return m_eRace;
}

QMUD::ClassType Character::chClassType() const
{
    return m_chClass->classType();
}

CharacterClass* Character::chClass() const
{
    return m_chClass;
}

int Character::hitPointsCurrentMaximum() const
{
    int currentMaximum = (chClass()->hitDice() * level() + constitution()) * 4;

    if (chType() == Character::ChType::CHTYPE_NPC)
    {
        currentMaximum *= NpcCharacter::baseManaAndHpMultiplier(static_cast<const NpcCharacter*>(this)->npcType());
    }

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            currentMaximum += (*item)->baseBonus().hitPoints();

    currentMaximum += m_tonicInfo.bonus.hitPoints();

    return currentMaximum;
}

int Character::hitPointsCurrent() const
{
    return m_iHitPointsCurrent;
}

int Character::actionPointsCurrentMaximum() const
{
    int currentMaximum = 200;

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            currentMaximum += (*item)->baseBonus().actionPoints();

    currentMaximum += m_tonicInfo.bonus.actionPoints();

    return currentMaximum;
}

int Character::actionPointsCurrent() const
{
    return m_iActionsPointsCurrent;
}

int Character::manaPointsCurrentMaximum() const
{
    int currentMaximum = (chClass()->manaDice() * level() + intelligence() + wisdom()) * 4;

    if (chType() == Character::ChType::CHTYPE_NPC)
    {
        currentMaximum *= NpcCharacter::baseManaAndHpMultiplier(static_cast<const NpcCharacter*>(this)->npcType());
    }

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            currentMaximum += (*item)->baseBonus().manaPoints();

    currentMaximum += m_tonicInfo.bonus.manaPoints();

    return currentMaximum;
}

int Character::manaPointsCurrent() const
{
    return m_iManaPointsCurrent;
}

int Character::constitution() const
{
    int value = qRound(m_iConstitutionBase + (level() - 1) / 5.0 * m_iConstitutionBase);

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            value += (*item)->baseBonus().constitution();

    value += constitutionItemBonus();

    value += m_tonicInfo.bonus.constitution();

    return value;
}

int Character::dexterity() const
{
    int value = qRound(m_iDexterityBase + (level() - 1) / 5.0 * m_iDexterityBase);

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            value += (*item)->baseBonus().dexterity();

    value += dexterityItemBonus();

    value += m_tonicInfo.bonus.dexterity();

    return value;
}

int Character::intelligence() const
{
    int value = qRound(m_iIntelligenceBase + (level() - 1) / 5.0 * m_iIntelligenceBase);

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            value += (*item)->baseBonus().intelligence();

    value += intelligenceItemBonus();

    value += m_tonicInfo.bonus.intelligence();

    return value;
}

int Character::wisdom() const
{
    int value = qRound(m_iWisdomBase + (level() - 1) / 5.0 * m_iWisdomBase);

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            value += (*item)->baseBonus().wisdom();

    value += wisdomItemBonus();

    value += m_tonicInfo.bonus.wisdom();

    return value;
}

int Character::charisma() const
{
    int value = qRound(m_iCharismaBase + (level() - 1) / 5.0 * m_iCharismaBase);

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            value += (*item)->baseBonus().charisma();

    value += charismaItemBonus();

    value += m_tonicInfo.bonus.charisma();

    return value;
}

int Character::strength() const
{
    int value = qRound(m_iStrengthBase + (level() - 1) / 5.0 * m_iStrengthBase);

    for (auto item : m_vEquippedItems)
        if (!!(*item))
            value += (*item)->baseBonus().strength();

    value += strengthItemBonus();

    if (hasTemporaryStatus(QMUD::TemporaryStatusType::STRENGHT))
        value = qRound(value * 1.05);

    value += m_tonicInfo.bonus.strength();

    return value;
}

int Character::constitutionBase() const
{
    return m_iConstitutionBase;
}

int Character::dexterityBase() const
{
    return m_iDexterityBase;
}

int Character::intelligenceBase() const
{
    return m_iIntelligenceBase;
}

int Character::wisdomBase() const
{
    return m_iWisdomBase;
}

int Character::charismaBase() const
{
    return m_iCharismaBase;
}

int Character::strengthBase() const
{
    return m_iStrengthBase;
}

void Character::setCurrentFood(QString foodName, int rounds, int hpBonus, int apBonus)
{
	m_foodInfo.name = foodName;
	m_foodInfo.rounds = rounds;
	m_foodInfo.hpBonus = hpBonus;
	m_foodInfo.apBonus = apBonus;
	m_foodInfo.eating = true;
}

QString Character::currentFood() const
{
	return m_foodInfo.name;
}

bool Character::currentFoodIsEating() const
{
	return m_foodInfo.eating;
}

void Character::currentFoodStopEating()
{
	m_foodInfo.eating = false;
	m_foodInfo.hpBonus = 0;
	m_foodInfo.apBonus = 0;
}

int Character::currentFoodHpGainBonus() const
{
	return m_foodInfo.hpBonus;
}

int Character::currentFoodApGainBonus() const
{
	return m_foodInfo.apBonus;
}

void Character::setCurrentDrink(QString drinkName, int rounds, int mpBonus)
{
	m_drinkInfo.name = drinkName;
	m_drinkInfo.rounds = rounds;
	m_drinkInfo.mpBonus = mpBonus;
	m_drinkInfo.drinking = true;
}

QString Character::currentDrink() const
{
	return m_drinkInfo.name;
}

bool Character::currentDrinkIsDrinking() const
{
	return m_drinkInfo.drinking;
}

void Character::currentDrinkStopDrinking()
{
	m_drinkInfo.drinking = false;
	m_drinkInfo.mpBonus = 0;
}

int Character::currentDrinkMpGainBonus() const
{
    return m_drinkInfo.mpBonus;
}

void Character::setCurrentTonic(QString tonicName, int rounds, QMUD::ItemBaseBonus bonus)
{
    if (rounds > 0)
    {
        m_tonicInfo.name = tonicName;
        m_tonicInfo.rounds = rounds;
        m_tonicInfo.bonus = bonus;
    }
    else
        currentTonicRemove();

    m_tonicInfo.waitTimeRounds = QMUD_GLOBAL_ITEM_POTION_WAIT_TIME_ROUNDS;
}

void Character::currentTonicRemove()
{
    m_tonicInfo.name.clear();
    m_tonicInfo.rounds = 0;
    m_tonicInfo.bonus = QMUD::ItemBaseBonus();
}

QString Character::currentTonic()
{
    return m_tonicInfo.name;
}

int Character::currentTonicRounds()
{
    return m_tonicInfo.rounds;
}

bool Character::canDrinkTonic()
{
    return m_tonicInfo.waitTimeRounds <= 0;
}

qint64 Character::coins() const
{
    auto it = m_mapResource.find(QMUD::ResourceType::COINS);
    return it == m_mapResource.end() ? 0 : it.value();
}

void Character::addCoins(qint64 coins)
{
    if (coins > 0)
    {
        m_mapResource[QMUD::ResourceType::COINS] += coins;
        QMUD::qmudWorld->statisticsFromLastReboot().addResource(QMUD::ResourceType::COINS, static_cast<quint64>(coins));
    }
}

bool Character::removeCoins(qint64 coins)
{
    auto it = m_mapResource.find(QMUD::ResourceType::COINS);

    if (it == m_mapResource.end())
        return false;

    if (coins > 0 && it.value() >= coins)
    {
        it.value() -= coins;
        return true;
    }

    return false;
}

QMap<QMUD::ResourceType, qint64> Character::resources() const
{
    return m_mapResource;
}

void Character::addResource(QMUD::ResourceType resource, qint64 v, bool countInStatistics)
{
    m_mapResource[resource] += v;

    if (countInStatistics)
        QMUD::qmudWorld->statisticsFromLastReboot().addResource(resource, static_cast<quint64>(v));
}

bool Character::hasResouces(QMap<QMUD::ResourceType, qint64> resources) const
{
    for (auto it = resources.begin(); it != resources.end(); ++it)
    {
        auto it2 = m_mapResource.find(it.key());

        if (it.value() >= 0)
            if (it2 == m_mapResource.end() || it2.value() < it.value())
                return false;
    }

    return true;
}

void Character::removeResource(QMUD::ResourceType resource, qint64 v)
{
    auto it = m_mapResource.find(resource);

    if (it != m_mapResource.end())
    {
        it.value() -= v;
        if (it.value() < 0)
            it.value() = 0;
    }
}

bool Character::useResources(QMap<QMUD::ResourceType, qint64> resources)
{
    if (!hasResouces(resources))
        return false;

    for (auto it = resources.begin(); it != resources.end(); ++it)
    {
        auto it2 = m_mapResource.find(it.key());


        Q_ASSERT(it2 != m_mapResource.end() && it2.value() >= it.value());

        if (it.value() >= 0)
        {
            it2.value() -= it.value();
            QMUD::qmudWorld->statisticsFromLastReboot().addSpentResource(it.key(), static_cast<quint64>(it.value()));
        }
    }

    return true;
}

int Character::tsConstitution() const
{
    return QMUD::rollDice(1, level()) + constitution();
}

int Character::tsDexterity() const
{
    return QMUD::rollDice(1, level()) + dexterity();
}

int Character::tsIntelligence() const
{
    return QMUD::rollDice(1, level()) + intelligence();
}

int Character::tsWisdom() const
{
    return QMUD::rollDice(1, level()) + wisdom();
}

int Character::tsCharisma() const
{
    return QMUD::rollDice(1, level()) + charisma();
}

int Character::tsStrength() const
{
    return QMUD::rollDice(1, level()) + strength();
}

int Character::cdConstitution() const
{
    return level()/2 + constitution();
}

int Character::cdDexterity() const
{
    return level()/2 + dexterity();
}

int Character::cdIntelligence() const
{
    return level()/2 + intelligence();
}

int Character::cdWisdom() const
{
    return level()/2 + wisdom();
}

int Character::cdCharisma() const
{
    return level()/2 + charisma();
}

int Character::cdStrength() const
{
    return level()/2 + strength();
}

void Character::setTemporaryStatus(QMUD::TemporaryStatusType status, int turns, bool message)
{
    if (turns != QMUD_GLOBAL_STATUS_DURATION_INFINITE && turns <= 0)
        return;

    if (m_mapTemporaryStatus[status] != QMUD_GLOBAL_STATUS_DURATION_INFINITE)
        m_mapTemporaryStatus[status] = qMax(m_mapTemporaryStatus[status], turns);

    // Gestisco gli status temporanei
    if (message)
    {
        auto characters = room()->characters();

        QString msgCh;
        QString msgOther;

        if (status == QMUD::TemporaryStatusType::SHIELD)
        {
            msgCh = tr("&WUno scudo magico ti avvolge&d");
            msgOther = tr("&WUno scudo magico avvolge %1&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::STRENGHT)
        {
            msgCh = tr("&WDiventi piu' forte&d");
            msgOther = tr("&W%1 diventa piu' forte&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::SANCTIFY)
        {
            msgCh = tr("&WVieni avvolto da un'aurea di santificazione&d");
            msgOther = tr("&WUn'aurea di santificazione avvolge %1&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::DIVINE_SHIELD)
        {
            msgCh = tr("&WVieni avvolto da uno scudo divino&d");
            msgOther = tr("&WUno scudo divino avvolge %1&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::FLY)
        {
            msgCh = tr("&CInizi a volare&d");
            msgOther = tr("&C%1 inizia a volare&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::INVISIBILITY)
        {
            msgCh = tr("&WDiventi invisibile&d");
            msgOther = tr("&W%1 diventa invisibile&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::SEE_INVISIBILITY)
        {
            msgCh = tr("&WInizi a vedere invisibile&d");
            msgOther = tr("&WGli occhi di %1 si illuminano per un istante&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::FIRE_SHIELD)
        {
            msgCh = tr("&RVieni avvolto da uno scudo di fuoco&d");
            msgOther = tr("&RUno scudo di fuoco avvolge %1&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::ICE_SHIELD)
        {
            msgCh = tr("&CVieni avvolto da uno scudo di ghiaccio&d");
            msgOther = tr("&CUno scudo di ghiaccio avvolge %1&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::BLADES_SHIELD)
        {
            msgCh = tr("&zVieni avvolto da uno scudo di lame&d");
            msgOther = tr("&zUno scudo di lame avvolge %1&d").arg(name(Q_NULLPTR));
        }
        else if (status == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1 ||
                 status == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2 ||
                 status == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3 ||
                 status == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4 ||
                 status == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5)
        {
            msgCh = tr("&GDelle copie di te stesso compaiono dal nulla&d");
            msgOther = tr("&GDelle copie di %1 compaiono dal nulla&d").arg(name(Q_NULLPTR));
        }

        if (chType() == ChType::CHTYPE_PC)
            static_cast<PcCharacter*>(this)->player()->sendLine(msgCh);

        for (Character* chInRoom : characters)
        {
            if (chInRoom == this)
                continue;

            if (chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(this))
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                playerOther->sendLine(msgOther);
            }
        }
    }

    if (chType() == ChType::CHTYPE_PC)
    {
        if (status == QMUD::TemporaryStatusType::ACTIONLAG)
            static_cast<PcCharacter*>(this)->sendDataActionLag(false);
        else
            static_cast<PcCharacter*>(this)->sendDataStatus();
    }
}

QMap<QMUD::TemporaryStatusType, int> Character::temporaryStatus() const
{
    return m_mapTemporaryStatus;
}

int Character::numberOfAttacks() const
{
    int maxAttacks = m_chClass->maxNumberOfAttacks(level());
    auto weapon = equippedWeapon();
    int nAttacks = 1;

    if (!weapon)
        nAttacks = qMax(1, qRound(static_cast<double>(level())/QMUD_GLOBAL_CH_MAX_LEVEL * maxAttacks));
    else
    {
        double attacks = static_cast<double>(level())/QMUD_GLOBAL_CH_MAX_LEVEL * maxAttacks * weapon->speed();

        nAttacks = qRound(attacks);

        if (nAttacks < maxAttacks && QMUD::rollDice(1, 4) < qRound((attacks - nAttacks) * 4.0) + 1)
            nAttacks++;

        nAttacks = qMax(1, nAttacks);
    }

    nAttacks += numberOfAttacksBonus();

    return nAttacks;
}

void Character::addExperience(qint64 experience)
{
    int levelCounter = 0;

    if (level() < 100)
    {
        m_iExperience += experience;

        while (m_iExperience >= m_chClass->experienceToLevelUp(level()) && level() < 100)
        {
            m_iExperience -= m_chClass->experienceToLevelUp(level());

            // Incremento il livello
            m_iLevel++;
            levelCounter++;

            if (chType() == ChType::CHTYPE_PC)
                static_cast<PcCharacter*>(this)->player()->sendLine(tr("&YHai guadagnato abbastanza esperienza per il livello %1&d").arg(level()));
        }
    }

    if (chType() == ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(this)->sendDataBasicInfo();
}

bool Character::isInCombat() const
{
    return m_bInCombat;
}

void Character::setInCombat(bool v)
{
    if (v)
    {
        auto target = targetCharacter(true);

        if (target)
        {
            m_bInCombat = true;

            if (chType() == ChType::CHTYPE_NPC)
                static_cast<NpcCharacter*>(this)->addHostileCh(target->id());
        }
    }
    else
    {
        m_bInCombat = false;
    }
}

QMUD::IdType Character::targetId() const
{
    return m_targetId;
}

void Character::setTarget(QMUD::IdType id, bool updateCurrent)
{
    // Se non ho piu' un target devo uscire dal combattimento
    if (id == QMUD::IdInvalid)
    {
        m_bInCombat = false;
        m_targetId = id;
    }

    if (!updateCurrent && isInCombat())
    {
        auto currentTarget = targetCharacter(true);

        if (currentTarget)
            return;
    }

    auto ch = room()->map()->character(id);

    if (ch)
        m_targetId = id;
    else
    {
        m_bInCombat = false;
        m_targetId = QMUD::IdInvalid;
    }
}

Character *Character::targetCharacter(bool inRoom)
{
    if (m_targetId == QMUD::IdInvalid)
        return Q_NULLPTR;

    auto ch = inRoom ?  room()->character(m_targetId) : room()->map()->character(m_targetId);

    if (!ch && !inRoom)
        m_targetId = QMUD::IdInvalid;

    return ch;
}

void Character::executeAttack()
{
    const int parryDodgeActionPoint = 5;

    // Se sono in stanza pace esco dal combattimento
    if (room()->flagCheck(QMUD::RoomFlagType::PACE))
    {
        setInCombat(false);
        return;
    }

    // Se sono incosciente o morto non eseguo l'attacco
    if (isUnconscious() || isDead())
        return;

    // Cerco il target
    auto targetCh = targetCharacter(false);

    // Se il target non esiste piu' (morto ad esempio) esco dallo
    // stato di combattimento
    if (!targetCh)
    {
        setInCombat(false);
        return;
    }

    // Se il target non e' nella mia stanza non faccio nulla
    if (targetCh->room() != room())
        return;

    // Se il target non e' in combattimento con nessuno allora
    // lo faccio entrare in combattimento con il character
    // corrente
    if (!targetCh->isInCombat())
    {
        targetCh->setTarget(id(), true);
        targetCh->setInCombat(true);
    }

    // Valuto l'esistenza dei player
    Player* player = Q_NULLPTR;
    Player* targetPlayer = Q_NULLPTR;

    if (chType() == ChType::CHTYPE_PC)
        player = dynamic_cast<PcCharacter*>(this)->player();

    if (targetCh->chType() == ChType::CHTYPE_PC)
        targetPlayer = dynamic_cast<PcCharacter*>(targetCh)->player();

    QVector<Player*> otherPlayers;
    QList<Entity*> entities = qobject_cast<Room*>(parent())->entities(Q_NULLPTR);

    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        if ((*it)->entityType() == EntityType::CHARACTER &&
            (*it)->id() != id() &&
            (*it)->id() != targetCh->id())
        {
            Character *chTmp = qobject_cast<Character*>(*it);

            if (chTmp->chType() == ChType::CHTYPE_PC)
                otherPlayers.push_back(dynamic_cast<PcCharacter*>(chTmp)->player());
        }
    }

    // Se sono invisibile allora torno visibile
    removeTemporaryStatus(QMUD::TemporaryStatusType::INVISIBILITY);

    // Calcolo colpire e danno degli attacchi
    bool hasShield = targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::SHIELD);
    int attackCount = numberOfAttacks();

    QVector<int> vAttackDamage;
    QVector<int> vAttackHit;
    QVector<bool> vAttackParry;
    QVector<bool> vAttackDodge;

    vAttackDamage.reserve(attackCount);
    vAttackHit.reserve(attackCount);
    vAttackParry.reserve(attackCount);
    vAttackDodge.reserve(attackCount);

    auto weapon = equippedWeapon();

    for (int i = 0; i < attackCount; ++i)
    {
        int attackHit = rollAttackHit();
        int attackDamage = rollAttackDamage();

        if (weapon)
        {
            attackHit -= (targetCh->level() * 2 - weapon->level());
            attackHit = qMax(0, attackHit);

            attackDamage -= (targetCh->level() * 2 - weapon->level());
            attackDamage = qMax(0, attackDamage);
        }

        vAttackDamage.push_back(attackDamage);
        vAttackHit.push_back(attackHit);

        int rollParry = QMUD::rollDice(1, targetCh->constitution());
        int rollDodge = QMUD::rollDice(1, targetCh->dexterity());

        if (!targetCh->canUseAbility(QMUD::AbilityType::PARRY) ||
                (targetCh->chType() == Character::ChType::CHTYPE_PC && !targetCh->equippedShield()) ||
                rollParry < attackHit)
            vAttackParry.push_back(false);
        else
            vAttackParry.push_back(true);

        if (!targetCh->canUseAbility(QMUD::AbilityType::DODGE) ||
                rollDodge < attackHit)
            vAttackDodge.push_back(false);
        else
            vAttackDodge.push_back(true);
    }

    // Eseguo gli attacchi
    for (int i = 0; i < vAttackHit.size(); ++i)
    {
        // Controllo se il target ha abbastanza action point
        // per poter parare o shivare
        if (vAttackDodge[i] || vAttackParry[i])
        {
            if (targetCh->actionPointsCurrent() < parryDodgeActionPoint)
            {
                vAttackDodge[i] = false;
                vAttackParry[i] = false;
            }
        }

        // Gestione immagini illusorie
        bool illusoryImageHit = false;
        int illusoryImageHitIndex = 0;

        if (!vAttackDodge[i] && ! vAttackParry[i])
        {
            int numberOfIllusoryImages = 0;
            if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1))
                numberOfIllusoryImages = 1;
            else if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2))
                numberOfIllusoryImages = 2;
            else if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3))
                numberOfIllusoryImages = 3;
            else if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4))
                numberOfIllusoryImages = 4;
            else if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5))
                numberOfIllusoryImages = 5;

            if (numberOfIllusoryImages != 0)
            {
                int index = QMUD::rollDice(1, numberOfIllusoryImages + 1);
                if (index != 1)
                {
                    illusoryImageHit = true;
                    illusoryImageHitIndex = index - 2;
                }
            }
        }

        // Valuto se l'attacco va a segno
        int attackDamageOrig = vAttackDamage[i] - qMax(0, qRound((targetCh->armorClass() * (hasShield ? 1.05 : 1.0)) - vAttackHit[i]));
        // Calcolo il danno dell'attacco
        int attackDamage = targetCh->calcDamage(attackDamageOrig, attackDamageType());

        // Eseguo l'attacco
        if (vAttackParry[i] || vAttackDodge[i])
            targetCh->useActionPoint(parryDodgeActionPoint);

        // Eseguo il danno al target
        if (attackDamageOrig > 0 && !vAttackParry[i] && !vAttackDodge[i])
            targetCh->damage(attackDamageOrig, attackDamageType(), this);
        else if (illusoryImageHit)
            targetCh->illusoryImageDamage(illusoryImageHitIndex, attackDamage);

        // Invio i dati al client
        int attackDamageValue = attackDamage;

        if (vAttackParry[i])
            attackDamageValue = QMUD::ClientDataAttackInfo::VALUE_PARRY;
        else if (vAttackDodge[i])
            attackDamageValue = QMUD::ClientDataAttackInfo::VALUE_DODGE;
        else if (illusoryImageHit)
            attackDamageValue = QMUD::ClientDataAttackInfo::VALUE_ILLUSORY_IMAGE;

        if (player)
            player->pcCharacter()->sendAttackInfo(attackDamageValue, id(), targetCh->id(), true, attackDamageType());

        if (targetPlayer)
            targetPlayer->pcCharacter()->sendAttackInfo(attackDamageValue, id(), targetCh->id(), true, attackDamageType());

        for (auto otherPlayer : otherPlayers)
            otherPlayer->pcCharacter()->sendAttackInfo(attackDamageValue, id(), targetCh->id(), true, attackDamageType());

        // Scudo divino o altri scudi simili
        if (attackDamageOrig > 0)
        {
            int damageToCh = 0;
            int shieldDamage = 0;
            QMUD::DamageType damageType = QMUD::DamageType::FIRE;
            QMUD::SpellType spellType = QMUD::SpellType::FIRE_SHIELD;

            if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD))
            {
                shieldDamage = QMUD::rollDice(qMax(1, targetCh->level()/10), 4) +
                        qMax(0, qMax(targetCh->wisdom(), targetCh->intelligence()));
                damageToCh = calcDamage(shieldDamage, QMUD::DamageType::FIRE);
                damageType = QMUD::DamageType::FIRE;
                spellType = QMUD::SpellType::FIRE_SHIELD;
            }

            if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD))
            {
                shieldDamage = QMUD::rollDice(qMax(1, targetCh->level()/10), 4) +
                        qMax(0, qMax(targetCh->wisdom(), targetCh->intelligence()));
                damageToCh = calcDamage(shieldDamage, QMUD::DamageType::ICE);
                damageType = QMUD::DamageType::ICE;
                spellType = QMUD::SpellType::ICE_SHIELD;
            }

            if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD))
            {
                shieldDamage = QMUD::rollDice(qMax(1, targetCh->level()/10), 6) +
                        qMax(0, targetCh->intelligence());
                damageToCh = calcDamage(shieldDamage, QMUD::DamageType::SLASHING);
                damageType = QMUD::DamageType::SLASHING;
                spellType = QMUD::SpellType::BLADES_SHIELD;
            }

            if (targetCh->hasTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD))
            {
                bool targetIsPriest = targetCh->chClass()->classType() == QMUD::ClassType::PRIEST;
                shieldDamage = targetIsPriest ? QMUD::rollDice(attackDamage/2, 2) : QMUD::rollDice(attackDamage/4, 2);
                shieldDamage = qMin(shieldDamage, targetCh->level()) + qMax(0, targetCh->wisdom());
                damageToCh = calcDamage(shieldDamage, QMUD::DamageType::DIVINE);
                damageType = QMUD::DamageType::DIVINE;
                spellType = QMUD::SpellType::DIVINE_SHIELD;
            }

            damage(shieldDamage, damageType, targetCh);

            // Invio i dati al client
            if (damageToCh > 0)
            {
                if (player)
                    player->pcCharacter()->sendAttackInfo(damageToCh, targetCh->id(), id(), true, damageType, QMUD::AbilityType::UNKNOWN, spellType);

                if (targetPlayer)
                    targetPlayer->pcCharacter()->sendAttackInfo(damageToCh, targetCh->id(), id(), true, damageType, QMUD::AbilityType::UNKNOWN, spellType);

                for (auto otherPlayer : otherPlayers)
                    otherPlayer->pcCharacter()->sendAttackInfo(damageToCh, targetCh->id(), id(), true, damageType, QMUD::AbilityType::UNKNOWN, spellType);
            }
        }

        // Controllo se il target e' morto, in caso termino l'attacco
        // in anticipo e esco dal combattimento
        if (targetCh->isDead())
        {
            setInCombat(false);
            break;
        }

        // Se il target e' incosciente lo notifico
        if (targetCh->isUnconscious() && player)
            player->sendLine(tr("%1 e' a terra privo di sensi").arg(targetCh->name(this)));

        // Se sono morto o incosciente termino l'attacco
        // in anticipo
        if (isDead() || isUnconscious())
            break;
    }

    // Assistenza
    // Se nella stanza e' presente un NPC con le seguenti caratteristiche
    //  - flag ASSIST
    //  - lo stesso assistGroup del target
    //  - lo stesso mapStaticId del target
    // allora iniziera' a combattere con il character this.
    // Questo pero' e' vero solo se si verifica almeno una di queste ipotesi:
    //  - il character this e' un PC
    //  - il character this non ha lo stesso assist group dell'NPC
    //  - il character this non ha lo stesso mapStaticId dell'NPC
    if (targetCh->chType() == Character::ChType::CHTYPE_NPC)
    {
        auto chsInRoom = room()->characters();

        for (auto chInRoom : chsInRoom)
        {
            if (chInRoom != targetCh &&
                    !chInRoom->isInCombat() &&
                    chInRoom != this &&
                    chInRoom->chType() == Character::ChType::CHTYPE_NPC &&
                    chInRoom->canSee(this))
            {
                NpcCharacter *npc = static_cast<NpcCharacter*>(chInRoom);
                NpcCharacter *assistNpc = static_cast<NpcCharacter*>(targetCh);

                if (npc->flagCheck(QMUD::NpcCharacterFlagType::ASSIST) &&
                        npc->assistGroup() == assistNpc->assistGroup() &&
                        npc->mapStaticId() == assistNpc->mapStaticId())
                {
                    if (this->chType() == Character::ChType::CHTYPE_PC ||
                            npc->assistGroup() != static_cast<NpcCharacter*>(this)->assistGroup() ||
                            npc->mapStaticId() != static_cast<NpcCharacter*>(this)->mapStaticId() )
                    {
                        for (auto chInRoom2 : chsInRoom)
                            if (chInRoom2->chType() == Character::ChType::CHTYPE_PC)
                                static_cast<PcCharacter*>(chInRoom2)->player()->sendLine(tr("%1 inizia ad assistere %2").arg(npc->name(chInRoom2)).arg(assistNpc->name(chInRoom2)));

                        npc->setTarget(id(), true);
                        npc->setInCombat(true);
                    }
                }
            }
        }
    }
}

bool Character::isUnconscious() const
{
    return m_iHitPointsCurrent <= 0;
}

bool Character::isDead() const
{
    return m_iHitPointsCurrent <= QMUD_GLOBAL_CH_MIN_CURRENT_HP;
}

bool Character::canSee(const Character *target) const
{
    if (target == this)
        return true;

    if (target->chType() == ChType::CHTYPE_PC && !static_cast<const PcCharacter*>(target)->player()->isVisible())
        return false;

    if (target->hasTemporaryStatus(QMUD::TemporaryStatusType::INVISIBILITY) && !hasTemporaryStatus(QMUD::TemporaryStatusType::SEE_INVISIBILITY))
        return false;

    return true;
}

void Character::illusoryImagesResetHP()
{
    if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1))
        m_vIllusoryImagesCurrentHP.resize(1);
    else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2))
        m_vIllusoryImagesCurrentHP.resize(2);
    else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3))
        m_vIllusoryImagesCurrentHP.resize(3);
    else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4))
        m_vIllusoryImagesCurrentHP.resize(4);
    else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5))
        m_vIllusoryImagesCurrentHP.resize(5);
    else
        m_vIllusoryImagesCurrentHP.clear();

    for (auto& value : m_vIllusoryImagesCurrentHP)
        value = level();
}

void Character::illusoryImageDamage(int index, int damage)
{
    if (index < 0 || index >= m_vIllusoryImagesCurrentHP.size())
    {
        Q_ASSERT(false);
        return;
    }

    m_vIllusoryImagesCurrentHP[index] -= damage;
    if (m_vIllusoryImagesCurrentHP[index] <= 0)
    {
        m_vIllusoryImagesCurrentHP.remove(index);

        if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1))
            removeTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1, true);
        else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2))
        {
            m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1] = m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2];
            m_mapTemporaryStatus.remove(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2);
        }
        else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3))
        {
            m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2] = m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3];
            m_mapTemporaryStatus.remove(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3);
        }
        else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4))
        {
            m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3] = m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4];
            m_mapTemporaryStatus.remove(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4);
        }
        else if (hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5))
        {
            m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4] = m_mapTemporaryStatus[QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5];
            m_mapTemporaryStatus.remove(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5);
        }
    }
}

void Character::update()
{
    // Combattimento
    if (isInCombat())
    {
        if (chType() == ChType::CHTYPE_NPC)
        {
            NpcCharacter* npc = static_cast<NpcCharacter*>(this);

            // Controllo l'aggro e cerco il target
            Character *targetCh = static_cast<NpcCharacter*>(this)->updateAggro();

            if (targetCh)
                setTarget(targetCh->id(), true);
            else
                targetCh = targetCharacter(false);

            if (targetCh)
            {
                if (targetCh->room() == room())
                {
                    // Uso una abilita'
                    if (!npc->eventAbility(targetCh))
                        if (npc->flagCheck(QMUD::NpcCharacterFlagType::AUTOIA_COMBAT))
                            m_chClass->AICombatAction(this, targetCh);

                    // Uso gli attacchi
                    executeAttack();
                }
                else
                {
                    // Inseguo il target
                    if (!npc->isHunting() && npc->flagCheck(QMUD::NpcCharacterFlagType::HUNT_MAP))
                        npc->startHunt(targetCh);
                }
            }
            else
                setInCombat(false);
        }
        else
        {
            // Uso gli attacchi
            executeAttack();
        }
    }
    else
    {
        if (chType() == ChType::CHTYPE_NPC)
        {
            NpcCharacter* npc = static_cast<NpcCharacter*>(this);

            // Resetto l'aggro
            npc->clearAggro();

            // Uso una abilita'
            if (npc->flagCheck(QMUD::NpcCharacterFlagType::AUTOIA_COMBAT))
                m_chClass->AIIdleAction(this);
        }
    }

    updateTemporaryStatus(true);
}

void Character::updateStatus()
{
    bool update = false;
	Player* player = Q_NULLPTR;

    if (chType() == ChType::CHTYPE_PC)
        player = static_cast<PcCharacter*>(this)->player();

	// Fermo il recupero Food e Drink se sono in combattimento
    if (isInCombat())
    {
        if (m_foodInfo.eating)
		{
			if (player)
                player->sendLine(tr("Smetti di mangiare %1").arg(currentFood()));

			currentFoodStopEating();
		}

		if (m_drinkInfo.drinking)
		{
			if (player)
                player->sendLine(tr("Smetti di bere %1").arg(currentDrink()));

			currentDrinkStopDrinking();
		}
    }

	if (player && m_foodInfo.eating)
        player->sendLine(tr("&ODai un morso a %1&d").arg(currentFood()));

	if (player && m_drinkInfo.drinking)
        player->sendLine(tr("&BBevi un sorso di %1&d").arg(currentDrink()));

    // Recupero HP
    if (!isInCombat() && m_iHitPointsCurrent < hitPointsCurrentMaximum())
    {
        if (isUnconscious())
            m_iHitPointsCurrent += 1;
        else if (this->chType() == ChType::CHTYPE_PC)
            m_iHitPointsCurrent += qMax(1, constitution()) + m_foodInfo.hpBonus;
        else
            m_iHitPointsCurrent += qMax(1, hitPointsCurrentMaximum() / 10) + m_foodInfo.hpBonus;

        if (m_iHitPointsCurrent >= hitPointsCurrentMaximum())
            m_iHitPointsCurrent = hitPointsCurrentMaximum();

        update = true;
    }

    // Recupero AP
    if (m_iActionsPointsCurrent < actionPointsCurrentMaximum())
    {
        if (!isUnconscious())
        {
            if (isInCombat())
                m_iActionsPointsCurrent += QMUD::rollDice(1, qMax(1, dexterity()));
            else
                m_iActionsPointsCurrent += qMax(1, dexterity() + m_foodInfo.apBonus);

            if (m_iActionsPointsCurrent >= actionPointsCurrentMaximum())
                m_iActionsPointsCurrent = actionPointsCurrentMaximum();

            update = true;
        }
    }

    // Recupero MP
    if (m_iManaPointsCurrent < manaPointsCurrentMaximum())
    {
        if (!isUnconscious())
        {
            if (isInCombat())
                m_iManaPointsCurrent += qMax(0, intelligence() + wisdom());
            else if (this->chType() == ChType::CHTYPE_PC)
                m_iManaPointsCurrent += qMax(1, qMax(intelligence(), wisdom())) + m_drinkInfo.mpBonus;
            else
                m_iManaPointsCurrent += qMax(1, manaPointsCurrentMaximum() / 10) + m_drinkInfo.mpBonus;

            if (m_iManaPointsCurrent >= manaPointsCurrentMaximum())
                m_iManaPointsCurrent = manaPointsCurrentMaximum();

            update = true;
        }
    }

    // Food and Drink
	m_foodInfo.update();
	m_drinkInfo.update();
    m_tonicInfo.update();

	// Aggiornamento client
    if (update)
    {
        if (chType() == ChType::CHTYPE_PC)
        {
            static_cast<PcCharacter*>(this)->sendStatus();
        }
    }
}

bool Character::equip(Item *item, bool message)
{
    if (item->itemType() == QMUD::ItemType::WEAPON)
    {
        if (m_ptrEquippedWeapon)
        {
            return false;
        }

        m_ptrEquippedWeapon = static_cast<ItemWeapon*>(item);
    }
    else if (item->itemType() == QMUD::ItemType::ARMOR)
    {
        ItemArmor* armor = static_cast<ItemArmor*>(item);

        if (armor->armorType() == QMUD::ArmorType::CHEST)
        {
            if (m_ptrEquippedChest)
            {
                return false;
            }

            m_ptrEquippedChest = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::SHIELD)
        {
            if (m_ptrEquippedShield)
            {
                return false;
            }

            m_ptrEquippedShield = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::GLOVES)
        {
            if (m_ptrEquippedGloves)
            {
                return false;
            }

            m_ptrEquippedGloves = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::SHOULDER)
        {
            if (m_ptrEquippedShoulder)
            {
                return false;
            }

            m_ptrEquippedShoulder = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::BRACERS)
        {
            if (m_ptrEquippedBracers)
            {
                return false;
            }

            m_ptrEquippedBracers = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::HELM)
        {
            if (m_ptrEquippedHelm)
            {
                return false;
            }

            m_ptrEquippedHelm = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::LEG)
        {
            if (m_ptrEquippedLeg)
            {
                return false;
            }

            m_ptrEquippedLeg = armor;
        }
        else if (armor->armorType() == QMUD::ArmorType::BOOTS)
        {
            if (m_ptrEquippedBoots)
            {
                return false;
            }

            m_ptrEquippedBoots = armor;
        }
        else
            return false;
    }
    else if (item->itemType() == QMUD::ItemType::TRINKET)
    {
        ItemTrinket* trinket = static_cast<ItemTrinket*>(item);

        if (trinket->trinketType() == QMUD::TrinketType::RING)
        {
            if (m_ptrEquippedRingLeft && m_ptrEquippedRingRight)
            {
                return false;
            }

            if (!m_ptrEquippedRingLeft)
                m_ptrEquippedRingLeft = trinket;
            else if (!m_ptrEquippedRingRight)
                m_ptrEquippedRingRight = trinket;
        }
        else if (trinket->trinketType() == QMUD::TrinketType::BRACELET)
        {
            if (m_ptrEquippedBraceletLeft && m_ptrEquippedBraceletRight)
            {
                return false;
            }

            if (!m_ptrEquippedBraceletLeft)
                m_ptrEquippedBraceletLeft = trinket;
            else if (!m_ptrEquippedBraceletRight)
                m_ptrEquippedBraceletRight = trinket;
        }
        else if (trinket->trinketType() == QMUD::TrinketType::NECKLACE)
        {
            if (m_ptrEquippedNecklace)
            {
                return false;
            }

            m_ptrEquippedNecklace = trinket;
        }
        else
            return false;
    }
    else
        return false;

    // Gestisco gli status temporanei forniti dall'oggetto
    if (!item->baseTemporaryStatus().temporaryStatus().isEmpty())
    {
        auto statusList = item->baseTemporaryStatus().temporaryStatus();
        for (auto status : statusList)
            setTemporaryStatus(status, QMUD_GLOBAL_STATUS_DURATION_INFINITE, message);
    }

    return true;
}

bool Character::canUnequip(Item *item)
{
    for (auto equip : m_vEquippedItems)
        if (!!(*equip) && (*equip) == item)
        {
            Q_ASSERT(item->parent() == this);
            return true;
        }

    return false;
}

Item* Character::canUnequip(QString itemName)
{
    for (auto equip : m_vEquippedItems)
    {
        if (!!(*equip) && (*equip)->name().contains(itemName, Qt::CaseInsensitive))
        {
            return *equip;
        }
    }

    return Q_NULLPTR;
}

bool Character::unequip(Item *item)
{
    bool remove = false;
    for (auto equip : m_vEquippedItems)
        if (!!(*equip) && (*equip) == item)
        {
            Q_ASSERT(item->parent() == this);
            *equip = Q_NULLPTR;
            remove = true;
        }

    if (!remove)
        return false;

    if (m_iHitPointsCurrent > hitPointsCurrentMaximum())
        m_iHitPointsCurrent = hitPointsCurrentMaximum();

    if (m_iManaPointsCurrent > manaPointsCurrentMaximum())
        m_iManaPointsCurrent = manaPointsCurrentMaximum();

    if (m_iActionsPointsCurrent > actionPointsCurrentMaximum())
        m_iActionsPointsCurrent = actionPointsCurrentMaximum();

    updateInfiniteTemporaryStatus();

    return true;
}

Item* Character::unequip(QString itemName)
{
    Item* item = Q_NULLPTR;

    for (auto equip : m_vEquippedItems)
    {
        if (!!(*equip) && (*equip)->name().contains(itemName, Qt::CaseInsensitive))
        {
            item = *equip;
            *equip = Q_NULLPTR;

            if (m_iHitPointsCurrent > hitPointsCurrentMaximum())
                m_iHitPointsCurrent = hitPointsCurrentMaximum();

            if (m_iManaPointsCurrent > manaPointsCurrentMaximum())
                m_iManaPointsCurrent = manaPointsCurrentMaximum();

            if (m_iActionsPointsCurrent > actionPointsCurrentMaximum())
                m_iActionsPointsCurrent = actionPointsCurrentMaximum();

            break;
        }
    }

    updateInfiniteTemporaryStatus();

    return item;
}

Room *Character::room() const
{
    if (parent())
        return qobject_cast<Room*>(parent());
    else
    {
        // TODO spostare il character in una stanza di default per non
        // dover ritornare NULL
        Q_ASSERT(false);

        return Q_NULLPTR;
    }
}

QVector<Item *> Character::inventory(bool withEquippedItems) const
{
    QVector<Item*> items;

    for (auto child : children())
    {
        if (static_cast<Entity*>(child)->entityType() == Entity::EntityType::ITEM)
        {
            Item *item = static_cast<Item*>(child);

            if (withEquippedItems)
                items.push_back(item);
            else
            {
                if (!isEquipped(item))
                    items.push_back(item);
            }
        }
    }

    return items;
}

QVector<Item *> Character::equipment() const
{
    QVector<Item*> equipment;

    for (auto equip : m_vEquippedItems)
        if (!!(*equip))
            equipment.push_back(*equip);

    return equipment;
}

int Character::inventorySize(bool withEquippedItems) const
{
    int count = 0;

    for (auto child : children())
    {
        if (static_cast<Entity*>(child)->entityType() == Entity::EntityType::ITEM)
        {
            Item *item = static_cast<Item*>(child);

            if (withEquippedItems)
                count++;
            else
            {
                if (!isEquipped(item))
                    count++;
            }
        }
    }

    return count;
}


bool Character::canMakeAction(QString& message) const
{
    if (hasTemporaryStatus(QMUD::TemporaryStatusType::ACTIONLAG))
    {
        message = tr("&WE' ancora troppo presto per agire!");
        return false;
    }
    else if (isUnconscious())
    {
        message = tr("&WSei privo di sensi!");
        return false;
    }
    else if (isDead())
    {
        message = tr("&WSei morto!");
        return false;
    }
    else
        return true;
}

bool Character::canAttack(Character *ch) const
{
    static QString message;
    return canAttack(ch, message);
}

bool Character::canAttack(Character *ch, QString& message) const
{
    if (ch == this)
    {
        message = QObject::tr("&wNon puoi attaccare te stesso");
        return false;
    }
    else if (chType() == ChType::CHTYPE_PC && ch->chType() == ChType::CHTYPE_PC)
    {
        message = QObject::tr("&wNon ti e' permesso attaccare altri giocatori qui");
        return false;
    }
    else if (ch->room()->flagCheck(QMUD::RoomFlagType::PACE))
    {
        message = QObject::tr("&wNon ti e' permesso combattere qui");
        return false;
    }
    else
        return true;
}

bool Character::hasTemporaryStatus(QMUD::TemporaryStatusType status) const
{
    auto it = m_mapTemporaryStatus.find(status);
    return it != m_mapTemporaryStatus.end() && (it.value() == QMUD_GLOBAL_STATUS_DURATION_INFINITE || it.value() > 0);
}

bool Character::hasTemporaryStatus(QMUD::TemporaryStatusType status, int &turns) const
{
    auto it = m_mapTemporaryStatus.find(status);

    if (it != m_mapTemporaryStatus.end() &&
            (it.value() == QMUD_GLOBAL_STATUS_DURATION_INFINITE || it.value() > 0))
    {
        turns = it.value();
        return true;
    }

    turns = 0;
    return false;
}

void Character::removeTemporaryStatus(QMUD::TemporaryStatusType status, bool force)
{
    if (hasTemporaryStatus(status))
    {
        if (m_mapTemporaryStatus[status] != QMUD_GLOBAL_STATUS_DURATION_INFINITE || force)
        {
            m_mapTemporaryStatus[status] = STATUS_TO_REMOVE;
            updateTemporaryStatus(false);
        }
    }
}

bool Character::canUseAbility(QMUD::AbilityType ability) const
{
    return m_chClass->canUseAbility(level(), ability);
}

bool Character::canUseSpell(QMUD::SpellType spell) const
{
    return m_chClass->canUseSpell(level(), spell);
}

void Character::useManaPoint(int mana)
{
    Q_ASSERT(mana >= 0);

    m_iManaPointsCurrent -= mana;
    if (m_iManaPointsCurrent < 0)
        m_iManaPointsCurrent = 0;
}

void Character::useActionPoint(int ap)
{
    Q_ASSERT(ap >= 0);

    m_iActionsPointsCurrent -= ap;
    if (m_iActionsPointsCurrent < 0)
        m_iActionsPointsCurrent = 0;
}

void Character::healingHitPoints(int value, Character *chStriker)
{
    Q_UNUSED(chStriker)

    if (value < 0)
        value = 0;

    m_iHitPointsCurrent += value;
    auto hpMax = hitPointsCurrentMaximum();
    if (m_iHitPointsCurrent > hpMax)
        m_iHitPointsCurrent = hpMax;
}

void Character::healingManaPoints(int value, Character *chStriker)
{
    Q_UNUSED(chStriker)

    if (value < 0)
        value = 0;

    m_iManaPointsCurrent += value;
    auto mpMax = manaPointsCurrentMaximum();
    if (m_iManaPointsCurrent > mpMax)
        m_iManaPointsCurrent = mpMax;
}

void Character::healingActionPoints(int value, Character *chStriker)
{
    Q_UNUSED(chStriker)

    if (value < 0)
        value = 0;

    m_iActionsPointsCurrent += value;
    auto apMax = actionPointsCurrentMaximum();
    if (m_iActionsPointsCurrent > apMax)
        m_iActionsPointsCurrent = apMax;
}

int Character::calcDamage(int damage, QMUD::DamageType type)
{
    Q_UNUSED(type)

    if (hasTemporaryStatus(QMUD::TemporaryStatusType::SANCTIFY))
        damage = static_cast<int>(qCeil(damage / 2.0));

    if (damage < 0)
        return 0;

    return damage;
}

void Character::damage(int damage, QMUD::DamageType type, Character *chStriker)
{
    Q_UNUSED(type)

    damage = calcDamage(damage, type);

    if (damage <= 0)
        return;

    m_iHitPointsCurrent -= damage;

    if (m_iHitPointsCurrent <= 0)
    {
        if (chType() == ChType::CHTYPE_PC)
        {
            Player* player = dynamic_cast<PcCharacter*>(this)->player();

            if (player->level() == Player::PlayerLevel::PLAYER_LEVEL_ADMIN ||
                player->level() == Player::PlayerLevel::PLAYER_LEVEL_CREATOR)
            {
                m_iHitPointsCurrent = 1;
            }
        }
    }

    if (isDead())
    {
        emit removeFromTheWorld(this);

        if (chType() == ChType::CHTYPE_NPC)
            QMUD::qmudWorld->statisticsFromLastReboot().addKilledNPC(1);
        else if (chType() == ChType::CHTYPE_PC)
            QMUD::qmudWorld->statisticsFromLastReboot().addKilledPC(1);

        // Gestisco l'esperienza
        if (chType() == ChType::CHTYPE_NPC && chStriker->chType() == ChType::CHTYPE_PC)
        {
            NpcCharacter* npc = static_cast<NpcCharacter*>(this);

            QVector<PcCharacter*> chs;
            chs.append(static_cast<PcCharacter*>(chStriker));
            if (static_cast<PcCharacter*>(chStriker)->group())
            {
                auto chInGroup = static_cast<PcCharacter*>(chStriker)->group()->characters();

                for (auto ch : chInGroup)
                {
                    if (ch->id() == chStriker->id())
                        continue;

                    if (ch->room()->id() == chStriker->room()->id() &&
                            ch->chType() == Character::ChType::CHTYPE_PC)
                    {
                        chs.append(static_cast<PcCharacter*>(ch));
                    }
                }
            }

            qint64 experience = npc->giveExperience();
            qint64 copp = coins() / chs.size();
            auto resources = this->resources();

            for (auto ch : chs)
            {
                double expToChMod = 1.0;

                int difference = this->level() - ch->level();

                if (difference == -1)
                    expToChMod = 0.8;
                else if (difference == -2)
                    expToChMod = 0.6;
                else if (difference == -3)
                    expToChMod = 0.4;
                else if (difference == -4)
                    expToChMod = 0.2;
                else if (difference == -5)
                    expToChMod = 0.1;
                else if (difference == 1)
                    expToChMod = 1.2;
                else if (difference == 2)
                    expToChMod = 1.4;
                else if (difference == 3)
                    expToChMod = 1.6;
                else if (difference == 4)
                    expToChMod = 1.8;
                else if (difference == 5)
                    expToChMod = 2.0;
                else if (difference < 0)
                    expToChMod = 0.0;
                else
                    expToChMod = 2.0;

                qint64 expToCh = static_cast<int>(experience * expToChMod);
                qint64 maxExp = QMUD::experienceByLevelBase(ch->level())/ch->level();

                if (expToCh > maxExp)
                    expToCh = maxExp;

                ch->player()->sendLine(tr("Guadagni &W%1&d punti esperienza").arg(expToCh));

                if (coins() > 0)
                    ch->player()->sendLine(tr("Guadagni &W%1&d").arg(QMUD::coinsToReadableString(copp)));

                for (auto itRes = resources.begin(); itRes != resources.end(); ++itRes)
                {
                    if (itRes.key() != QMUD::ResourceType::COINS)
                    {
                        qint64 resource = QMUD::rollDice(1, itRes.value() + 1) - 1;

                        if (resource > 0)
                        {
                            ch->player()->sendLine(tr("Guadagni &W%1&d %2").arg(resource).arg(QMUD::resourceToReadableString(itRes.key())));
                            ch->addResource(itRes.key(), resource);
                        }
                    }
                }

                ch->addExperience(expToCh);
                ch->addCoins(copp);

                // Loot
                bool hasLoot = false;

                if (ch->loot().size() < QMUD_GLOBAL_CH_MAX_LOOT_SIZE)
                {
                    for (auto item : npc->loot())
                    {
                        if (item.quality() >= QMUD::ItemQualityType::VALUABLE &&
                                QMUD::itemRarityCheck(item.rarity()))
                        {
                            ch->addLoot(item);
                            ch->player()->sendLine(tr("Ottieni %2%1&d").arg(item.name()).arg(QMUD::itemQualityToColor(item.quality())));
                            hasLoot = true;
                        }
                    }
                }

                if (ch->loot().size() >= QMUD_GLOBAL_CH_MAX_LOOT_SIZE)
                {
                    ch->player()->sendLine(tr("&RHai raggiunto il limite massimo di oggetti nel bottino!&d"));
                }

                if (!hasLoot && npc->rollForRandomLoot())
                {
                    QString randomCode = Item::generateRandomCodeForCharacter(ch, qMin(ch->level(), level()));
                    int level;
                    QMUD::ItemType itemType;
                    QString name;
                    QMUD::ItemClassType classType;
                    QMUD::ItemQualityType quality;

                    if (Item::fromRandomCodeToValues(randomCode, level, itemType, name, classType, quality))
                    {
                        LootItemData itemLoot(QJsonObject(),
                                              randomCode,
                                              QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM,
                                              quality,
                                              QMUD::ItemRarityType::RARITY0,
                                              QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM,
                                              name,
                                              false,
                                              Item::getDefaultCoinsValue(level, quality));

                        ch->player()->sendLine(tr("Ottieni %2%1&d").arg(itemLoot.name()).arg(QMUD::itemQualityToColor(itemLoot.quality())));
                        ch->addLoot(itemLoot);
                    }
                }
            }

            for (auto item : npc->loot())
            {
                if (item.quality() < QMUD::ItemQualityType::VALUABLE &&
                        QMUD::itemRarityCheck(item.rarity()))
                {
                    Item *i = Item::loadItem(item.json(), room(), item.mapStaticId());
                    i->setAutoDestroy();

                    if (i)
                    {
                        for (auto ch : chs)
                        {
                            ch->player()->sendLine(tr("%1 lascia cadere %3%2&d").arg(name(ch)).arg(item.name()).arg(QMUD::itemQualityToColor(item.quality())));
                        }
                    }
                }
            }

            // Quest
            for (auto ch : chs)
                ch->questKillCheck(npc);
        }
    }
    else
    {
        // Aggiorno la tabella dell'aggro
        if (chType() == ChType::CHTYPE_NPC)
        {
            NpcCharacter* npc = static_cast<NpcCharacter*>(this);
            npc->addAggro(chStriker, damage);
        }
    }
}

void Character::kill()
{
    emit removeFromTheWorld(this);
}

void Character::setName(QString name)
{
    m_strName = name;
}

void Character::setSex(QMUD::SexType sex)
{
    m_eSex = sex;
}

void Character::setHitPointsCurrent(int hp)
{
    m_iHitPointsCurrent = qMax(QMUD_GLOBAL_CH_MIN_CURRENT_HP, hp);
}

void Character::setActionPointsCurrent(int ap)
{
    m_iActionsPointsCurrent = qMax(QMUD_GLOBAL_CH_MIN_CURRENT_AP, ap);
}

void Character::setManaPointsCurrent(int mp)
{
    m_iManaPointsCurrent = qMax(QMUD_GLOBAL_CH_MIN_CURRENT_MP, mp);
}

void Character::setConstitutionBase(int v)
{
    m_iConstitutionBase = qMax(1, v);
}

void Character::setDexterityBase(int v)
{
    m_iDexterityBase = qMax(1, v);
}

void Character::setIntelligenceBase(int v)
{
    m_iIntelligenceBase = qMax(1, v);
}

void Character::setWisdomBase(int v)
{
    m_iWisdomBase = qMax(1, v);
}

void Character::setCharismaBase(int v)
{
    m_iCharismaBase = qMax(1, v);
}

void Character::setStrengthBase(int v)
{
    m_iStrengthBase = qMax(1, v);
}

void Character::setResource(QMUD::ResourceType resource, qint64 v)
{
    if (v > 0)
        m_mapResource[resource] = v;
}

void Character::setResources(QMap<QMUD::ResourceType, qint64> v)
{
    m_mapResource = v;
}

void Character::setChClass(CharacterClass* chClass, int level, qint64 experience)
{
    Q_ASSERT(level >= 1);

    m_chClass = chClass;
    m_iLevel = level;
    m_iExperience = experience;
}

void Character::setRace(QMUD::RaceType race)
{
    m_eRace = race;
}

int Character::armorClass() const
{
    int ac = 0;

    for (auto equip : m_vEquippedItems)
    {
        if (!!(*equip) && (*equip)->itemType() == QMUD::ItemType::ARMOR)
        {
            auto armor = dynamic_cast<ItemArmor*>(*equip);
            ac += armor->armorClassBonus();
        }
    }

    if (ac < armorClassBaseBonus())
        ac = armorClassBaseBonus();

    ac += level();

    if (hasTemporaryStatus(QMUD::TemporaryStatusType::SHIELD))
        ac = qRound(ac * 1.05);

    if (ac < 0)
        ac = 0;

    return ac;
}

int Character::hitBonus() const
{
    int bonus = 0;

    for (auto equip : m_vEquippedItems)
    {
        if (!!(*equip))
        {
            bonus += (*equip)->baseBonus().hit();
        }
    }

    bonus += m_tonicInfo.bonus.hit();
    bonus += m_chClass->hitBonus(level());

    return bonus + dexterity();
}

int Character::damageBonus() const
{
    int bonus = 0;

    for (auto equip : m_vEquippedItems)
    {
        if (!!(*equip))
        {
            bonus += (*equip)->baseBonus().damage();
        }
    }

    bonus += m_tonicInfo.bonus.damage();
    bonus += m_chClass->damageBonus(level());

    return bonus + strength();
}

int Character::rollAttackHit() const
{
    return hitBonus() + QMUD::rollDice(1, level());
}

int Character::rollAttackDamage() const
{
    ItemWeapon *weapon = equippedWeapon();

    int damage = 0;

    if (weapon)
        damage = weapon->rollDamage() + damageBonus();
    else
        damage = m_chClass->handDamageMin(level()) + QMUD::rollDice(1, m_chClass->handDamageMax(level()) - m_chClass->handDamageMin(level())) + damageBonus();

    return damage;
}

double Character::attackAggroMultiplier() const
{
    return 1.0; // TODO gestire il moltiplicatore in base alla classe
}

QMUD::DamageType Character::attackDamageType() const
{
    ItemWeapon *weapon = equippedWeapon();

    if (weapon)
        return weapon->damageType();
    else
        return QMUD::Race::baseDamageType(race());
}

void Character::updateTemporaryStatus(bool updateRounds)
{
    // Gestisco gli status temporanei
    bool sendLag = false;
    bool sendStatus = false;

    auto characters = room()->characters();

    for (auto it = m_mapTemporaryStatus.begin(); it != m_mapTemporaryStatus.end(); ++it)
    {
        if (it.value() == STATUS_TO_REMOVE || (it.value() > 0 && it.value() != QMUD_GLOBAL_STATUS_DURATION_INFINITE))
        {
            if (updateRounds)
                it.value()--;

            if (it.value() == 0 || it.value() == STATUS_TO_REMOVE)
            {
                it.value() = 0;

                QString msgCh;
                QString msgOther;

                if (it.key() == QMUD::TemporaryStatusType::SHIELD)
                {
                    msgCh = tr("&WIl tuo scudo magico svanisce nel nulla&d");
                    msgOther = tr("&WLo scudo magico di %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::STRENGHT)
                {
                    msgCh = tr("&WIl tuo incantesimo di forza svanisce nel nulla&d");
                    msgOther = tr("&WL'incantesimo di forza di %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::SANCTIFY)
                {
                    msgCh = tr("&WL'aurea di santificazione che ti avvolgeva svanisce nel nulla&d");
                    msgOther = tr("&WL'aurea di santificazione che avvolgeva %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::DIVINE_SHIELD)
                {
                    msgCh = tr("&WLo scudo divino che ti avvolgeva svanisce nel nulla&d");
                    msgOther = tr("&WLo scudo divino che avvolgeva %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::FLY)
                {
                    msgCh = tr("&CLentamente smetti di fluttuare e rimetti i piedi a terra&d");
                    msgOther = tr("&C%1 smette di fluttuare e lentamento rimette i piedi a terra&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::INVISIBILITY)
                {
                    msgCh = tr("&WTorni visibile&d");
                    msgOther = tr("&W%1 torna visibile&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::SEE_INVISIBILITY)
                {
                    msgCh = tr("&WI tuoi occhi tornano normali&d");
                    msgOther = tr("&WGli occhi di %1 tornano normali&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::FIRE_SHIELD)
                {
                    msgCh = tr("&RLo scudo di fuoco che ti avvolgeva svanisce nel nulla&d");
                    msgOther = tr("&RLo scudo di fuoco che avvolgeva %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::ICE_SHIELD)
                {
                    msgCh = tr("&CLo scudo di ghiaccio che ti avvolgeva svanisce nel nulla&d");
                    msgOther = tr("&CLo scudo di ghiaccio che avvolgeva %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::BLADES_SHIELD)
                {
                    msgCh = tr("&zLo scudo di lame che ti avvolgeva svanisce nel nulla&d");
                    msgOther = tr("&zLo scudo di lame che avvolgeva %1 svanisce nel nulla&d").arg(name(Q_NULLPTR));
                }
                else if (it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1 ||
                    it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2 ||
                    it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3 ||
                    it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4 ||
                    it.key() == QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5)
                {
                    msgCh = tr("&GLe tue immagini illusorie svaniscono nel nulla&d");
                    msgOther = tr("&WLe immagini illusorie di %1 svaniscono nel nulla&d").arg(name(Q_NULLPTR));
                    illusoryImagesResetHP();
                }
                else if (it.key() == QMUD::TemporaryStatusType::ACTIONLAG)
                {
                    sendLag = true;
                    continue;
                }

                sendStatus = true;

                if (chType() == ChType::CHTYPE_PC)
                    static_cast<PcCharacter*>(this)->player()->sendLine(msgCh);

                for (Character* chInRoom : characters)
                {
                    if (chInRoom == this)
                        continue;

                    if (chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(this))
                    {
                        Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                        playerOther->sendLine(msgOther);
                    }
                }
            }
        }
    }

    if (sendLag && chType() == ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(this)->sendDataActionLag(true);

    if (sendStatus && chType() == ChType::CHTYPE_PC)
        static_cast<PcCharacter*>(this)->sendDataStatus();
}

void Character::updateInfiniteTemporaryStatus()
{
    for (auto itStatus = m_mapTemporaryStatus.begin(); itStatus != m_mapTemporaryStatus.end(); ++itStatus)
    {
        if (itStatus.value() == QMUD_GLOBAL_STATUS_DURATION_INFINITE)
        {
            for (auto equip : m_vEquippedItems)
                if (!!(*equip))
                {
                    auto tempStatus = (*equip)->baseTemporaryStatus().temporaryStatus();

                    int found = false;
                    for (auto eqStatus : tempStatus)
                    {
                        if (eqStatus == itStatus.key())
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        itStatus.value() = STATUS_TO_REMOVE;
                    }
                    else
                        break;
                }
        }
    }

    updateTemporaryStatus(false);
}

bool Character::isEquipped(Item *item, Character::EquipmentSlot &slot) const
{
    if (item->itemType() == QMUD::ItemType::WEAPON)
    {
        if (m_ptrEquippedWeapon && m_ptrEquippedWeapon == item)
        {
            slot = EquipmentSlot::EQSLOT_WEAPON;
            return true;
        }

        return false;
    }
    else if (item->itemType() == QMUD::ItemType::ARMOR)
    {
        if (m_ptrEquippedChest && m_ptrEquippedChest == item)
        {
            slot = EquipmentSlot::EQSLOT_CHEST;
            return true;
        }
        else if (m_ptrEquippedShield && m_ptrEquippedShield == item)
        {
            slot = EquipmentSlot::EQSLOT_SHIELD;
            return true;
        }
        else if (m_ptrEquippedGloves && m_ptrEquippedGloves == item)
        {
            slot = EquipmentSlot::EQSLOT_GLOVES;
            return true;
        }
        else if (m_ptrEquippedShoulder && m_ptrEquippedShoulder == item)
        {
            slot = EquipmentSlot::EQSLOT_SHOULDER;
            return true;
        }
        else if (m_ptrEquippedBracers && m_ptrEquippedBracers == item)
        {
            slot = EquipmentSlot::EQSLOT_BRACERS;
            return true;
        }
        else if (m_ptrEquippedHelm && m_ptrEquippedHelm == item)
        {
            slot = EquipmentSlot::EQSLOT_HELM;
            return true;
        }
        else if (m_ptrEquippedLeg && m_ptrEquippedLeg == item)
        {
            slot = EquipmentSlot::EQSLOT_LEG;
            return true;
        }
        else if (m_ptrEquippedBoots && m_ptrEquippedBoots == item)
        {
            slot = EquipmentSlot::EQSLOT_BOOTS;
            return true;
        }

        return false;
    }
    else if (item->itemType() == QMUD::ItemType::TRINKET)
    {
        if (m_ptrEquippedRingLeft && m_ptrEquippedRingLeft == item)
        {
            slot = EquipmentSlot::EQSLOT_RING_LEFT;
            return true;
        }
        else if (m_ptrEquippedRingRight && m_ptrEquippedRingRight == item)
        {
            slot = EquipmentSlot::EQSLOT_RING_RIGHT;
            return true;
        }
        else if (m_ptrEquippedBraceletLeft && m_ptrEquippedBraceletLeft == item)
        {
            slot = EquipmentSlot::EQSLOT_BRACELET_LEFT;
            return true;
        }
        else if (m_ptrEquippedBraceletRight && m_ptrEquippedBraceletRight == item)
        {
            slot = EquipmentSlot::EQSLOT_BRACELET_RIGHT;
            return true;
        }
        else if (m_ptrEquippedNecklace && m_ptrEquippedNecklace == item)
        {
            slot = EquipmentSlot::EQSLOT_NECKLACE;
            return true;
        }

        return false;
    }

    return false;
}

bool Character::isEquipped(Item *item) const
{
    EquipmentSlot slot;

    return isEquipped(item, slot);
}

void Character::setChFollowed(QMUD::IdType id, QString name)
{
    m_iChFollowed = id;
    m_strChFollowedName = name;
}

QMUD::IdType Character::chFollowed() const
{
    return m_iChFollowed;
}

QString Character::chFollowedName() const
{
    return m_strChFollowedName;
}

Item *Character::equippedItem(Character::EquipmentSlot slot) const
{
    if (slot == EquipmentSlot::EQSLOT_WEAPON)
        return m_ptrEquippedWeapon;
    else if (slot == EquipmentSlot::EQSLOT_CHEST)
        return m_ptrEquippedChest;
    else if (slot == EquipmentSlot::EQSLOT_SHIELD)
        return m_ptrEquippedShield;
    else if (slot == EquipmentSlot::EQSLOT_GLOVES)
        return m_ptrEquippedGloves;
    else if (slot == EquipmentSlot::EQSLOT_SHOULDER)
        return m_ptrEquippedShoulder;
    else if (slot == EquipmentSlot::EQSLOT_BRACERS)
        return m_ptrEquippedBracers;
    else if (slot == EquipmentSlot::EQSLOT_HELM)
        return m_ptrEquippedHelm;
    else if (slot == EquipmentSlot::EQSLOT_LEG)
        return m_ptrEquippedLeg;
    else if (slot == EquipmentSlot::EQSLOT_BOOTS)
        return m_ptrEquippedBoots;
    else if (slot == EquipmentSlot::EQSLOT_RING_LEFT)
        return m_ptrEquippedRingLeft;
    else if (slot == EquipmentSlot::EQSLOT_RING_RIGHT)
        return m_ptrEquippedRingRight;
    else if (slot == EquipmentSlot::EQSLOT_BRACELET_LEFT)
        return m_ptrEquippedBraceletLeft;
    else if (slot == EquipmentSlot::EQSLOT_BRACELET_RIGHT)
        return m_ptrEquippedBraceletRight;
    else if (slot == EquipmentSlot::EQSLOT_NECKLACE)
        return m_ptrEquippedNecklace;
    else
        return Q_NULLPTR;
}

ItemWeapon *Character::equippedWeapon() const
{
    Item *weapon = equippedItem(EquipmentSlot::EQSLOT_WEAPON);

    if (weapon && weapon->itemType() == QMUD::ItemType::WEAPON)
    {
        return static_cast<ItemWeapon*>(weapon);
    }
    else if (weapon)
        Q_ASSERT(false);

    return Q_NULLPTR;
}

ItemArmor *Character::equippedShield() const
{
    return m_ptrEquippedShield;
}

bool Character::canEquip(Item *item, QString &alreadyWearItemName) const
{
    if (item->itemType() == QMUD::ItemType::WEAPON)
    {
        if (m_ptrEquippedWeapon)
        {
            alreadyWearItemName = m_ptrEquippedWeapon->name();
            return false;
        }
    }
    else if (item->itemType() == QMUD::ItemType::ARMOR)
    {
        ItemArmor* armor = static_cast<ItemArmor*>(item);

        if (armor->armorType() == QMUD::ArmorType::CHEST)
        {
            if (m_ptrEquippedChest)
            {
                alreadyWearItemName = m_ptrEquippedChest->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::SHIELD)
        {
            if (m_ptrEquippedShield)
            {
                alreadyWearItemName = m_ptrEquippedShield->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::GLOVES)
        {
            if (m_ptrEquippedGloves)
            {
                alreadyWearItemName = m_ptrEquippedGloves->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::SHOULDER)
        {
            if (m_ptrEquippedShoulder)
            {
                alreadyWearItemName = m_ptrEquippedShoulder->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::BRACERS)
        {
            if (m_ptrEquippedBracers)
            {
                alreadyWearItemName = m_ptrEquippedBracers->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::HELM)
        {
            if (m_ptrEquippedHelm)
            {
                alreadyWearItemName = m_ptrEquippedHelm->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::LEG)
        {
            if (m_ptrEquippedLeg)
            {
                alreadyWearItemName = m_ptrEquippedLeg->name();
                return false;
            }
        }
        else if (armor->armorType() == QMUD::ArmorType::BOOTS)
        {
            if (m_ptrEquippedBoots)
            {
                alreadyWearItemName = m_ptrEquippedBoots->name();
                return false;
            }
        }
        else
            return false;
    }
    else if (item->itemType() == QMUD::ItemType::TRINKET)
    {
        ItemTrinket* trinket = static_cast<ItemTrinket*>(item);

        if (trinket->trinketType() == QMUD::TrinketType::RING)
        {
            if (m_ptrEquippedRingLeft && m_ptrEquippedRingRight)
            {
                alreadyWearItemName = m_ptrEquippedRingLeft->name();
                return false;
            }
        }
        else if (trinket->trinketType() == QMUD::TrinketType::BRACELET)
        {
            if (m_ptrEquippedBraceletLeft && m_ptrEquippedBraceletRight)
            {
                alreadyWearItemName = m_ptrEquippedBraceletLeft->name();
                return false;
            }
        }
        else if (trinket->trinketType() == QMUD::TrinketType::NECKLACE)
        {
            if (m_ptrEquippedNecklace)
            {
                alreadyWearItemName = m_ptrEquippedNecklace->name();
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

Character::ChClassData::ChClassData() :
    chClass(Q_NULLPTR),
    m_iLevel(1),
    m_iExperience(0)
{

}

Character::ChClassData::~ChClassData()
{

}

int Character::ChClassData::level() const
{
    return m_iLevel;
}

qint64 Character::ChClassData::experience() const
{
    return m_iExperience;
}

int &Character::ChClassData::levelRef()
{
    return m_iLevel;
}

qint64 &Character::ChClassData::experienceRef()
{
    return m_iExperience;
}

Character::LootItemData::LootItemData() :
    m_mapStaticId(QMUD::StaticIdInvalid),
    m_eQuality(QMUD::ItemQualityType::JUNK),
    m_eRarity(QMUD::ItemRarityType::RARITY0),
    m_iStaticId(QMUD::StaticIdInvalid),
    m_bToSell(false)
{

}

Character::LootItemData::LootItemData(QJsonObject json, QString randomCode, QMUD::StaticIdType mapStaticId,
                                      QMUD::ItemQualityType quality, QMUD::ItemRarityType rarity,
                                      QMUD::StaticIdType staticId, QString name,
                                      bool toSell, qint64 coinsValue) :
    m_json(json),
    m_randomCode(randomCode),
    m_mapStaticId(mapStaticId),
    m_eQuality(quality),
    m_eRarity(rarity),
    m_iStaticId(staticId),
    m_strName(name),
    m_bToSell(toSell),
    m_iCoinsValue(coinsValue)
{

}

QJsonObject Character::LootItemData::toJson() const
{
    QJsonObject obj;
    if (!m_randomCode.isEmpty())
    {
        obj["randomCode"] = m_randomCode;
    }
    else
    {
        obj["id"] = static_cast<double>(m_iStaticId);
        obj["mapId"] = static_cast<double>(m_mapStaticId);
    }

    obj["toSell"] = m_bToSell;

    return obj;
}

bool Character::LootItemData::fromJson(QJsonObject obj)
{
    // ToSell
    if (!QMUD::fileJsonRead(obj, "toSell", m_bToSell))
        return false;

    // RandomCode
    if (!QMUD::fileJsonRead(obj, "randomCode", m_randomCode, false, true))
        return false;

    if (m_randomCode.isEmpty())
    {
        // Id
        double tmpStaticId;
        if (!QMUD::fileJsonRead(obj, "id", tmpStaticId))
            return false;

        m_iStaticId = static_cast<QMUD::StaticIdType>(tmpStaticId);

        // MapId
        double tmpStaticMapId;
        if (!QMUD::fileJsonRead(obj, "mapId", tmpStaticMapId))
            return false;

        m_mapStaticId = static_cast<QMUD::StaticIdType>(tmpStaticMapId);

        // Carico il resto dal world
        auto map = QMUD::qmudWorld->map(static_cast<QMUD::StaticIdType>(qRound64(tmpStaticMapId)));
    
        if (!map)
        {
            GLOBAL_CRITICAL(tr("Chiave 'mapId' non valida: %1").arg(tmpStaticMapId));
            return false;
        }
    
        auto items = map->mapJsonObjItemsById();
        auto itemJson = items.find(static_cast<QMUD::StaticIdType>(qRound64(tmpStaticId)));
    
        if (itemJson == items.end())
        {
            GLOBAL_CRITICAL(tr("Chiave 'id' non valida: %1").arg(tmpStaticId));
            return false;
        }
    
        // Item
        m_json = itemJson.value();
    
        // Rarity
        QString tmpRarity;
        if (!QMUD::fileJsonRead(m_json, "rarity", tmpRarity, false, true, QMUD::itemRarityToString(QMUD::ItemRarityType::RARITY1)))
            return false;

        m_eRarity = QMUD::itemRarityFromString(tmpRarity);
    
        // Quality
        QString tmpQuality;
        if (!QMUD::fileJsonRead(m_json, "quality", tmpQuality))
            return false;
    
        m_eQuality = QMUD::itemQualityFromString(tmpQuality);
    
        // Name
        if (!QMUD::fileJsonRead(m_json, "name", m_strName))
            return false;
    
        // Value
        int tmpLevel;
        if (!QMUD::fileJsonRead(m_json, "level", tmpLevel))
            return false;
    
        m_iCoinsValue = Item::getDefaultCoinsValue(tmpLevel, m_eQuality);
    }
    else
    {
        // Id
        m_iStaticId = QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM;

        // MapId
        m_mapStaticId = QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM;

        // Rarity
        m_eRarity = QMUD::ItemRarityType::RARITY0;

        QMUD::ItemType tmpItemType = QMUD::ItemType::UNKNOWN;
        QMUD::ItemClassType tmpClassType = QMUD::ItemClassType::UNKNOWN;
        int level;
        if (!Item::fromRandomCodeToValues(m_randomCode, level, tmpItemType, m_strName, tmpClassType, m_eQuality))
            return false;

        m_iCoinsValue = Item::getDefaultCoinsValue(level, m_eQuality);
    }

    return true;
}
