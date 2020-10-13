/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "pccharacter.h"
#include "npccharacter.h"
#include "characterclass.h"
#include "itemweapon.h"
#include "itemarmor.h"
#include "itemtrinket.h"
#include "../global.h"
#include "../globaldebug.h"
#include "item.h"
#include "player.h"
#include "group.h"
#include "world.h"
#include "color.h"
#include "room.h"
#include "quest.h"

#include <QJsonDocument>
#include <QSet>
#include <QFile>
#include <QUuid>

using namespace CORE;

PcCharacter::PcCharacter(PlayerConnection *conn, Player *player, Room *parent) :
    Character(Character::ChType::CHTYPE_PC, parent),
    m_ptrConnection(conn),
    m_ptrPlayer(player),
    m_ptrGroup(Q_NULLPTR),
    m_iMoveCounter(0),
    m_bResourceRadarIsOn(false),
    m_iLastSessionMapStaticId(QMUD::StaticIdInvalid),
    m_iLastSessionRoomStaticId(QMUD::StaticIdInvalid),
    m_ptrTracedRoom(Q_NULLPTR),
    m_iTracedRoomNpcMapId(QMUD::StaticIdInvalid),
    m_iTracedRoomNpcId(QMUD::StaticIdInvalid),
    m_ptrTracedRoomLastFromRoom(Q_NULLPTR),
    m_ptrTracedRoomLastRoomExitToTake(Q_NULLPTR),
    m_eTracedRoomLastRoomExitType(RoomExit::RoomExitType::EXIT_NORTH)
{

}

PlayerConnection *PcCharacter::connection() const
{
    return m_ptrConnection;
}

Player *PcCharacter::player() const
{
    return m_ptrPlayer;
}

Group *PcCharacter::group() const
{
    return m_ptrGroup;
}

void PcCharacter::setGroup(Group *group)
{
    Q_ASSERT(!m_ptrGroup);

    if (m_ptrGroup)
        return;

    m_ptrGroup = group;

    connect(m_ptrGroup, SIGNAL(disbanded(Group*)), this, SLOT(onGroupDisbanded(Group*)));
    connect(m_ptrGroup, SIGNAL(characterEnter(Group*,PcCharacter*)), this, SLOT(onGroupCharacterEnter(Group*,PcCharacter*)));
    connect(m_ptrGroup, SIGNAL(characterExit(Group*,PcCharacter*)), this, SLOT(onGroupCharacterExit(Group*,PcCharacter*)));
}

QVector<Character::LootItemData>& PcCharacter::loot()
{
    return m_vLootList;
}

void PcCharacter::addLoot(const Character::LootItemData &itemData)
{
    m_vLootList.push_back(itemData);

    sendDataLoot();
}

int PcCharacter::moveCounter() const
{
    return m_iMoveCounter;
}

void PcCharacter::updateMoveCounter()
{
    m_iMoveCounter++;
}

void PcCharacter::sendDataBasicInfo()
{
    QMUD::ClientDataBasicInfo data(
                name(Q_NULLPTR),
                race(),
                chClassType(),
                level(),
                experience(),
                chClass()->experienceToLevelUp(level()),
                id());

    if (data != m_clientDataBasicInfoLastSend)
    {
        m_clientDataBasicInfoLastSend = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendStatus()
{
    QMUD::ClientDataLife data(
            hitPointsCurrent(),
            hitPointsCurrentMaximum(),
            manaPointsCurrent(),
            manaPointsCurrentMaximum(),
            actionPointsCurrent(),
            actionPointsCurrentMaximum());

    if (data != m_clientDataLifeLastSend)
    {
        m_clientDataLifeLastSend = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendTargetInfo()
{
    Character* target = targetCharacter(false);

    QMUD::ClientDataTargetInfo data;
    QMUD::ClientDataTargetAggro aggro;

    if (target)
    {
        data = QMUD::ClientDataTargetInfo(
                    target->name(this),
                    target->level(),
                    target->hitPointsCurrent(),
                    target->hitPointsCurrentMaximum(),
                    target->manaPointsCurrent(),
                    target->manaPointsCurrentMaximum(),
                    target->actionPointsCurrent(),
                    target->actionPointsCurrentMaximum(),
                    target->id());

        if (target->chType() == Character::ChType::CHTYPE_NPC)
        {
            NpcCharacter *npc = static_cast<NpcCharacter*>(target);

            auto aggroList = npc->aggro();
            QVector<QMUD::ClientDataTargetAggro::AggroData> aggroData;

            for (auto it = aggroList.begin(); it != aggroList.end(); ++it)
            {
                Character* ch = QMUD::qmudWorld->character(it.key());

                if (ch)
                {
                    QMUD::ClientDataTargetAggro::AggroData a;
                    a.aggro = it.value();
                    a.name = ch->name(this);
                    aggroData.push_back(a);
                }
            }

            aggro = QMUD::ClientDataTargetAggro(aggroData);
        }
    }
    else
    {
        data = QMUD::ClientDataTargetInfo(QString(), 0);
    }

    if (data != m_clientDataTargetLifeLastSend)
    {
        m_clientDataTargetLifeLastSend = data;
        player()->sendData(data.serialize());
    }

    if (aggro != m_clientDatatargetAggroLastSend)
    {
        m_clientDatatargetAggroLastSend = aggro;
        player()->sendData(aggro.serialize());
    }


}

void PcCharacter::sendDataActionLag(bool update)
{
    auto itLag = m_mapTemporaryStatus.find(QMUD::TemporaryStatusType::ACTIONLAG);
    int lag = 0;

    if (itLag != m_mapTemporaryStatus.end())
        lag = itLag.value();

    if (lag > 0)
        lag = QMUD::qmudWorld->msNextTick() + ((lag - 1) * QMUD_GLOBAL_TIMER_UPDATE_CHARACTERS_TIMEOUT_MS);

    QMUD::ClientDataActionLag data(lag, update);

    if (data != m_clientDataActionLagLastSend)
    {
        m_clientDataActionLagLastSend = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendDataStatus()
{   
    QVector<QMUD::ClientDataStatus::StatusData> statusList;

    for (auto it = m_mapTemporaryStatus.begin(); it != m_mapTemporaryStatus.end(); ++it)
    {
        if (it.value() > 0)
        {
            QMUD::ClientDataStatus::StatusData status;
            status.status = it.key();
            status.time = it.value();
            statusList.push_back(status);
        }
    }

    QMUD::ClientDataStatus data(statusList);

    if (data != m_clientDataStatusLastSend)
    {
        m_clientDataStatusLastSend = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendDataLoot()
{
    QVector<QMUD::ClientDataLoot::ItemData> loot;
    loot.reserve(m_vLootList.size());

    for (const auto& e : m_vLootList)
    {
        QMUD::ClientDataLoot::ItemData data;
        data.name = e.name();
        data.toSell = e.toSell();
        data.quality = e.quality();
        loot.push_back(data);
    }

    QMUD::ClientDataLoot data(loot);

    if (data != m_clientDataLootLastSend)
    {
        m_clientDataLootLastSend = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendDataMessage(QMUD::SayType type, QString message, QString fromName, QString toName)
{
    QString toSend = QString("{\"t\":\"5\",\"s\":%1,\"fn\":\"%2\",\"tn\":\"%3\",\"m\":\"%4\"}")
            .arg(static_cast<int>(type))
            .arg(fromName)
            .arg(toName)
            .arg(message);

    QByteArray data = toSend.toUtf8();
    data.push_front(29);
    data.push_back(29);
    player()->sendData(data);
}

void PcCharacter::sendLook()
{
    sendRoomInfo();
    sendRoomContent();
}

void PcCharacter::sendRoomInfo()
{
    Room* room = this->room();

    QMUD::ClientDataRoomInfo data(room->name(),
                                  room->description(this),
                                  room->map()->staticId(),
                                  room->wildMapIndex(),
                                  room->wildX(),
                                  room->wildY(),
                                  room->clientDataNpcs(this));

    if (data != m_clientDataRoomInfo)
    {
        m_clientDataRoomInfo = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendRoomContent()
{
    Room* room = this->room();

    QVector<QMUD::ClientDataRoomContent::CharacterData> npcData;

    for (auto it = room->children().begin(); it != room->children().end(); ++it)
    {
        if (static_cast<Entity*>(*it)->entityType() == Entity::EntityType::CHARACTER)
        {
            Character* roomChar = static_cast<Character*>(*it);

            if (roomChar == this)
                continue;

            if (!this->canSee(roomChar))
                continue;

            QMUD::ClientDataRoomContent::CharacterData data;
            data.name = roomChar->name(Q_NULLPTR);
            data.level = roomChar->level();
            data.isNpc = roomChar->chType() == Character::ChType::CHTYPE_NPC;
            data.hpMax = roomChar->hitPointsCurrentMaximum();
            data.hpCurrent = roomChar->hitPointsCurrent();
            data.mpMax = roomChar->manaPointsCurrentMaximum();
            data.mpCurrent = roomChar->manaPointsCurrent();
            data.id = roomChar->id();

            npcData.push_back(data);
        }
    }

    QMUD::ClientDataRoomContent data(npcData);

    if (data != m_clientDataRoomContent)
    {
        m_clientDataRoomContent = data;
        player()->sendData(data.serialize());
    }
}

void PcCharacter::sendAttackInfo(int value, int suitorId, int targetId, bool is_damage,
                                  QMUD::DamageType damageType,
                                  QMUD::AbilityType ability,
                                  QMUD::SpellType spell)
{
    QMUD::ClientDataAttackInfo data(value, suitorId, targetId, is_damage, damageType, ability, spell);

    player()->sendData(data.serialize());
}

void PcCharacter::sendRequest(QString request, QStringList options)
{
    QMUD::ClientDataRequest data(request, options);

    player()->sendData(data.serialize());
}

void PcCharacter::sendNewCombatRound()
{
    QMUD::ClientDataNewCombatRound data;

    player()->sendData(data.serialize());
}

QVector<QPair<QMUD::StaticIdType, QMUD::StaticIdType> > PcCharacter::recipes() const
{
    return m_vRecipesByMapAndId;
}

int PcCharacter::professionLevel(QMUD::ProfessionType p) const
{
    auto it = m_mapProfessionsLevel.find(p);

    if (it != m_mapProfessionsLevel.end())
        return it.value();

    return 1;
}

bool PcCharacter::professionLevelInc(QMUD::ProfessionType p)
{
    auto it = m_mapProfessionsLevel.find(p);

    if (it == m_mapProfessionsLevel.end())
    {
        m_mapProfessionsLevel[p] = 1;
        it = m_mapProfessionsLevel.find(p);
    }

    if ((level() >    0 && it.value() <  50) ||
        (level() >   20 && it.value() < 100) ||
        (level() >   40 && it.value() < 150) ||
        (level() >   60 && it.value() < 200) ||
        (level() >   80 && it.value() < 250) ||
        (level() == 100 && it.value() < 300))
    {
        it.value()++;

        updateRecipes();

        return true;
    }

    return false;
}

void PcCharacter::setResourceRadarOn(bool on)
{
    m_bResourceRadarIsOn = on;
}

bool PcCharacter::resourceRadarIsOn() const
{
    return m_bResourceRadarIsOn;
}

QMUD::StaticIdType PcCharacter::lastSessionMapStaticId() const
{
    return m_iLastSessionMapStaticId;
}

QMUD::StaticIdType PcCharacter::lastSessionRoomStaticId() const
{
    return m_iLastSessionRoomStaticId;
}

QString PcCharacter::title() const
{
    if (m_strTitle.isEmpty())
        return tr("un viandante");
    else
        return m_strTitle;
}

void PcCharacter::setTitle(QString title)
{
    m_strTitle = title;
}

Room *PcCharacter::tracedRoom() const
{
    return m_ptrTracedRoom;
}

QMUD::StaticIdType PcCharacter::tracedRoomNpcMapId() const
{
    return m_iTracedRoomNpcMapId;
}

QMUD::StaticIdType PcCharacter::tracedRoomNpcId() const
{
    return m_iTracedRoomNpcId;
}

void PcCharacter::setTracedRoom(Room *room, QMUD::StaticIdType mapId, QMUD::StaticIdType npcId)
{
    m_ptrTracedRoom = room;
    m_iTracedRoomNpcMapId = mapId;
    m_iTracedRoomNpcId = npcId;
    m_ptrTracedRoomLastFromRoom = Q_NULLPTR;
    m_ptrTracedRoomLastRoomExitToTake = Q_NULLPTR;

    if (m_iTracedRoomNpcMapId == QMUD::StaticIdInvalid ||
        m_iTracedRoomNpcId == QMUD::StaticIdInvalid)
    {
        m_iTracedRoomNpcMapId = QMUD::StaticIdInvalid;
        m_iTracedRoomNpcId = QMUD::StaticIdInvalid;
    }
}

Room *PcCharacter::tracedRoomLastFromRoom() const
{
    return m_ptrTracedRoomLastFromRoom;
}

Room *PcCharacter::tracedRoomLastRoomExitToTake() const
{
    return m_ptrTracedRoomLastRoomExitToTake;
}

RoomExit::RoomExitType PcCharacter::tracedRoomLastRoomExitType() const
{
    return m_eTracedRoomLastRoomExitType;
}

void PcCharacter::setTracedRoomLastData(const Room *lastFromRoom, Room *lastRoomExitToTake, RoomExit::RoomExitType lastRoomExitType)
{
    m_ptrTracedRoomLastFromRoom = const_cast<Room*>(lastFromRoom);
    m_ptrTracedRoomLastRoomExitToTake = lastRoomExitToTake;
    m_eTracedRoomLastRoomExitType = lastRoomExitType;
}

QMap<Quest::QuestIdType, PcCharacter::ActiveQuest> PcCharacter::questListActive() const
{
    return m_mapActiveQuests;
}

QMap<Quest::QuestIdType, const Quest *> PcCharacter::questListCompleted() const
{
    return m_mapCompletedQuests;
}

bool PcCharacter::questIsSuitabled(Quest *quest)
{
    if (quest->level() > level())
        return false;

    if (m_mapCompletedQuests.contains(quest->id()))
        return false;

    if (m_mapActiveQuests.contains(quest->id()))
        return false;

    return true;
}

void PcCharacter::questKillCheck(NpcCharacter *npc)
{
    for (auto& quest : m_mapActiveQuests)
        quest.questKillCheck(npc, player());
}

bool PcCharacter::questAccept(Quest *quest)
{
    if (questIsSuitabled(quest))
    {
        m_mapActiveQuests[quest->id()] = ActiveQuest(quest);
        return true;
    }

    return false;
}

void PcCharacter::questComplete(const Quest::QuestIdType& id)
{
    auto it = m_mapActiveQuests.find(id);

    if (it != m_mapActiveQuests.end())
        m_mapActiveQuests.erase(it);

    auto quest = QMUD::qmudWorld->quest(id);

    if (quest)
        m_mapCompletedQuests[id] = quest;
}

void PcCharacter::onGroupDisbanded(Group *group)
{
    Q_ASSERT(group == m_ptrGroup);

    player()->sendLine(tr("&bIl gruppo di cui facevi parte e' stato sciolto&d"));

    m_ptrGroup->disconnect(this);
    m_ptrGroup = Q_NULLPTR;
}

void PcCharacter::onGroupCharacterEnter(Group *group, PcCharacter *ch)
{
    Q_ASSERT(group == m_ptrGroup);

    if (ch != this)
    {
        if (group->leader() == this)
            player()->sendLine(tr("&b%1 entra a far parte del tuo gruppo&d").arg(ch->name(this)));
        else
            player()->sendLine(tr("&b%1 entra a far parte del gruppo&d").arg(ch->name(this)));
    }
    else
        player()->sendLine(tr("&bEntri a far parte del gruppo di %1&d").arg(group->leader()->name(this)));

    QMUD::qmudWorld->flushPlayers();
}

void PcCharacter::onGroupCharacterExit(Group *group, PcCharacter *ch)
{
    Q_ASSERT(group == m_ptrGroup);

    if (ch != this)
    {
        if (group->leader() == this)
            player()->sendLine(tr("&b%1 esce dal tuo gruppo&d").arg(ch->name(this)));
        else
            player()->sendLine(tr("&b%1 esce dal gruppo&d").arg(ch->name(this)));
    }
    else
    {
        player()->sendLine(tr("&bEsci dal gruppo di %1&d").arg(group->leader()->name(this)));
        m_ptrGroup->disconnect(this);
        m_ptrGroup = Q_NULLPTR;
    }

    QMUD::qmudWorld->flushPlayers();
}

PcCharacter::~PcCharacter()
{
    GLOBAL_DEBUG_DESTRUCTOR("PcCharacter", QString::number(id()) + ":" + name(Q_NULLPTR));
}

PcCharacter *PcCharacter::loadCharacter(QDir rootDir, PlayerConnection *conn, Player *player, QString name, QMUD::FileOpStatus &status, Room *parent)
{
    QString chFilePath = rootDir.path() + "/" + name.toLower() + ".json";

    if (!QFileInfo(chFilePath).exists())
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME;
        return Q_NULLPTR;
    }

    QFile fp(chFilePath);
    if (!fp.open(QFile::ReadOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(chFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_OPEN_FILE;
        return Q_NULLPTR;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fp.readAll(), &error);
    fp.close();

    if (error.error != QJsonParseError::NoError)
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: %2")
                        .arg(chFilePath)
                        .arg(error.errorString()));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return Q_NULLPTR;
    }

    QJsonObject obj = doc.object();

    PcCharacter *ch = new PcCharacter(conn, player);
    ch->setParent(parent);
    ch->updateRecipes();
    QString errorStr;

    if (!ch->readJson(obj))
    {
        delete ch;
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1")
                        .arg(chFilePath));
        return Q_NULLPTR;
    }

    status = QMUD::FileOpStatus::FILE_OP_OK;
    ch->m_strFileName = chFilePath;
    return ch;
}

bool PcCharacter::createCharacter(QDir rootDir, PlayerConnection *conn, Player *player, QString name,
                                  QMUD::SexType sex,
                                  QMUD::ClassType chClass,
                                  QMUD::RaceType race,
                                  int intelligence,
                                  int wis,
                                  int str,
                                  int con,
                                  int dex,
                                  QMUD::FileOpStatus &status,
                                  bool baseEquipment)
{
    QString filePath = rootDir.path() + "/" + name.toLower() + ".json";

    if (QFileInfo(filePath).exists())
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_FILE_ALREADY_EXISTS;
        return false;
    }

    PcCharacter chTmp(conn, player, Q_NULLPTR);
    chTmp.setName(name.toLower().simplified());
    chTmp.setSex(sex);
    chTmp.setChClass(CharacterClass::fromType(chClass), 1, 0);
    chTmp.setRace(race);
    chTmp.setIntelligenceBase(intelligence);
    chTmp.setWisdomBase(wis);
    chTmp.setStrengthBase(str);
    chTmp.setConstitutionBase(con);
    chTmp.setDexterityBase(dex);
    chTmp.setCharismaBase(8);
    chTmp.setHitPointsCurrent(chTmp.hitPointsCurrentMaximum());
    chTmp.setManaPointsCurrent(chTmp.manaPointsCurrentMaximum());
    chTmp.setActionPointsCurrent(chTmp.actionPointsCurrentMaximum());
    chTmp.setResource(QMUD::ResourceType::COINS, 10);

    if (baseEquipment)
    {
        chTmp.equip(ItemWeapon::createFromRandomCode(ItemWeapon::generateRandomCode(1, QMUD::itemClassTypeFromChClass(chClass), QMUD::ItemQualityType::COMMON), &chTmp));
        chTmp.equip(ItemArmor::createFromRandomCode(ItemArmor::generateRandomCode(1, QMUD::itemClassTypeFromChClass(chClass), QMUD::ItemQualityType::COMMON, QMUD::ArmorType::CHEST), &chTmp));
        chTmp.equip(ItemArmor::createFromRandomCode(ItemArmor::generateRandomCode(1, QMUD::itemClassTypeFromChClass(chClass), QMUD::ItemQualityType::COMMON, QMUD::ArmorType::GLOVES), &chTmp));
        chTmp.equip(ItemArmor::createFromRandomCode(ItemArmor::generateRandomCode(1, QMUD::itemClassTypeFromChClass(chClass), QMUD::ItemQualityType::COMMON, QMUD::ArmorType::BOOTS), &chTmp));
        chTmp.equip(ItemArmor::createFromRandomCode(ItemArmor::generateRandomCode(1, QMUD::itemClassTypeFromChClass(chClass), QMUD::ItemQualityType::COMMON, QMUD::ArmorType::LEG), &chTmp));
    }

    QFile fp(filePath);
    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(filePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_OPEN_FILE;
        return false;
    }

    QJsonObject obj;
    chTmp.writeJson(obj);
    QJsonDocument doc;
    doc.setObject(obj);
    QTextStream stream(&fp);
    stream << doc.toJson(QJsonDocument::Indented);
    fp.close();

    status = QMUD::FileOpStatus::FILE_OP_OK;
    return true;
}

bool PcCharacter::checkCharacterFiles(QDir rootDir, QMap<QString, QMUD::PcCharacterBaseInfo>& pcBaseInfo)
{
    QMap<QUuid, QString> itemsUuid;

    // Controllo l'unicità degli oggetti
    bool isFail = false;
    bool isModify = false;
    QString chFilePath;
    QJsonObject obj;

    QStringList chFileList = rootDir.entryList(QDir::Files);
    for (QString file : chFileList)
    {
        if (isFail || isModify)
        {
            GLOBAL_INFO(tr("Backup del file %1").arg(chFilePath));

            // Backup senza modifica
            if (!QFile::copy(chFilePath, chFilePath + "." + QTime::currentTime().toString("HHmmsszzz") + ".bk"))
            {
                GLOBAL_CRITICAL(tr("Errore nella copia del file %1").arg(chFilePath));
                return false;
            }
        }

        if (isFail)
        {
            GLOBAL_INFO(tr("Rimozione del file %1").arg(chFilePath));
            if (!QFile::remove(chFilePath))
            {
                GLOBAL_CRITICAL(tr("Errore nella rimozione del file %1").arg(chFilePath));
                return false;
            }
        }

        if (isModify)
        {
            GLOBAL_INFO(tr("Modifica del file %1").arg(chFilePath));
            // Salvare la copia
            QFile fp(chFilePath);
            if (!fp.open(QFile::WriteOnly | QFile::Text))
            {
                GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(chFilePath));
                return false;
            }

            fp.write(QJsonDocument(obj).toJson());
            fp.close();
        }

        isFail = false;
        isModify = false;

        if (QFileInfo(file).suffix() == "json")
        {
            chFilePath = rootDir.path() + "/" + file;

            if (!QFileInfo(chFilePath).exists())
            {
                GLOBAL_CRITICAL(tr("Il file %1 non esiste").arg(chFilePath));
                return false;
            }

            QFile fp(chFilePath);
            if (!fp.open(QFile::ReadOnly | QFile::Text))
            {
                GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(chFilePath));
                return false;
            }

            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(fp.readAll(), &error);
            fp.close();

            if (error.error != QJsonParseError::NoError)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: %2")
                                .arg(chFilePath)
                                .arg(error.errorString()));

                isFail = true;
                continue;
            }

            obj = doc.object();

            auto itName = obj.find("name");

            if (itName == obj.end() || !itName->isString())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: chiave name non valida o non trovata")
                                .arg(chFilePath)
                                .arg(error.errorString()));

                isFail = true;
                continue;
            }

            // Classe
            auto itClass = obj.find("class");

            if (itClass == obj.end() || !itClass->isString())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: chiave class non valida o non trovata")
                                .arg(chFilePath)
                                .arg(error.errorString()));

                isFail = true;
                continue;
            }

            CharacterClass* chClass = CharacterClass::fromString(itClass->toString());

            if (!chClass)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: chiave class non valida o non trovata")
                                .arg(chFilePath)
                                .arg(error.errorString()));
                isFail = true;
                break;
            }

            auto itLevel = obj.find("level");

            if (itLevel == obj.end() || !itLevel->isDouble())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: chiave level non valida o non trovata")
                                .arg(chFilePath)
                                .arg(error.errorString()));

                isFail = true;
                continue;
            }

            QString pcName = itName->toString().toLower();

            // Race
            QString tmpRaceStr;
            if (!QMUD::fileJsonRead(obj, "race", tmpRaceStr))
                return false;

            bool ok;
            QMUD::RaceType tmpRace = QMUD::Race::fromString(tmpRaceStr, &ok);
            if (!ok)
            {
                GLOBAL_CRITICAL(tr("'%1' non e' una razza valida").arg(tmpRaceStr));
                return false;
            }

            pcBaseInfo[pcName] = QMUD::PcCharacterBaseInfo(pcName, tmpRace, chClass->classType(), itLevel->toInt());

            // Inventory
            auto itInventory = obj.find("inventory");

            if (itInventory != obj.end())
            {
                if (!itInventory.value().isArray())
                {
                    GLOBAL_CRITICAL(tr("Chiave 'inventory' non e' un array"));
                    isFail = true;
                    continue;
                }

                QJsonArray jsonArrayInventory = itInventory.value().toArray();

                for (auto it = jsonArrayInventory.begin(); it != jsonArrayInventory.end(); )
                {
                    if (!it->isObject())
                    {
                        GLOBAL_CRITICAL(tr("Array 'inventory' non valido"));
                        isFail = true;
                        break;
                    }

                    QJsonObject jsonObjItem = it->toObject();

                    // UUID
                    QString tmpUuidStr;
                    if (!QMUD::fileJsonRead(jsonObjItem, "uuid", tmpUuidStr, false, true))
                    {
                        // Rimuovo l'oggetto
                        GLOBAL_INFO(tr("Oggetto senza UUID identificato: %1:%2:%3")
                                        .arg(it->toObject()["name"].toString())
                                        .arg(it->toObject()["id"].toInt())
                                        .arg(it->toObject()["mapId"].toInt()));
                        it = jsonArrayInventory.erase(it);
                        obj["inventory"] = jsonArrayInventory;
                        isModify = true;
                        continue;
                    }

                    QUuid tmpUuid;
                    if (tmpUuidStr.isEmpty())
                    {
                        // Rimuovo l'oggetto
                        GLOBAL_INFO(tr("Oggetto senza UUID valido identificato: %1:%2:%3")
                                        .arg(it->toObject()["name"].toString())
                                        .arg(it->toObject()["id"].toInt())
                                        .arg(it->toObject()["mapId"].toInt()));
                        it = jsonArrayInventory.erase(it);
                        obj["inventory"] = jsonArrayInventory;
                        isModify = true;
                        continue;
                    }
                    else
                    {
                        tmpUuid = QUuid(tmpUuidStr);

                        if (tmpUuid.isNull())
                        {
                            // Rimuovo l'oggetto
                            GLOBAL_INFO(tr("Oggetto senza UUID valido identificato: %1:%2:%3")
                                            .arg(it->toObject()["name"].toString())
                                            .arg(it->toObject()["id"].toInt())
                                            .arg(it->toObject()["mapId"].toInt()));
                            it = jsonArrayInventory.erase(it);
                            obj["inventory"] = jsonArrayInventory;
                            isModify = true;
                            continue;
                        }

                        auto itItemUuid = itemsUuid.find(tmpUuid);
                        if (itItemUuid != itemsUuid.end())
                        {
                            // Rimuovo l'oggetto
                            GLOBAL_INFO(tr("Oggetto con UUID duplicato identificato: %1:%2:%3:%4")
                                            .arg(it->toObject()["name"].toString())
                                    .arg(it->toObject()["id"].toInt())
                                    .arg(it->toObject()["mapId"].toInt())
                                    .arg(itItemUuid.value()));
                            it = jsonArrayInventory.erase(it);
                            obj["inventory"] = jsonArrayInventory;
                            isModify = true;
                            continue;
                        }

                        itemsUuid[tmpUuid] = file;

                        ++it;
                    }
                }
            }
        }
    }

    if (isFail || isModify)
    {
        GLOBAL_INFO(tr("Backup del file %1").arg(chFilePath));

        // Backup senza modifica
        if (!QFile::copy(chFilePath, chFilePath + "." + QTime::currentTime().toString("HHmmsszzz") + ".bk"))
        {
            GLOBAL_CRITICAL(tr("Errore nella copia del file %1").arg(chFilePath));
            return false;
        }
    }

    if (isFail)
    {
        GLOBAL_INFO(tr("Rimozione del file %1").arg(chFilePath));

        if (!QFile::remove(chFilePath))
        {
            GLOBAL_CRITICAL(tr("Errore nella rimozione del file %1").arg(chFilePath));
            return false;
        }
    }

    if (isModify)
    {
        GLOBAL_INFO(tr("Modifica del file %1").arg(chFilePath));

        // Salvare la copia
        QFile fp(chFilePath);
        if (!fp.open(QFile::WriteOnly | QFile::Text))
        {
            GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(chFilePath));
            return false;
        }

        fp.write(QJsonDocument(obj).toJson());
        fp.close();
    }


    return true;
}

void PcCharacter::update()
{
    Character::update();

    m_iMoveCounter -= QMUD_GLOBAL_TIMER_UPDATE_CHARACTERS_TIMEOUT_MS/1000;

    if (m_iMoveCounter < 0)
        m_iMoveCounter = 0;
}

void PcCharacter::save()
{
    if (!QFileInfo(m_strFileName).exists())
    {
        GLOBAL_CRITICAL(tr("Il file %1 non esiste").arg(m_strFileName));
    }

    QFile fp(m_strFileName);

    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(m_strFileName));
        return;
    }

    QJsonObject obj;
    writeJson(obj);
    QJsonDocument doc;
    doc.setObject(obj);
    QTextStream stream(&fp);
    stream << doc.toJson(QJsonDocument::Indented);
    fp.close();
}

bool PcCharacter::readJson(const QJsonObject &obj)
{
    QString tmpStr;
    int tmpInt;

    // Name
    if (!QMUD::fileJsonRead(obj, "name", tmpStr))
        return false;

    tmpStr = tmpStr.toLower();
    tmpStr[0] = tmpStr[0].toUpper();
    setName(tmpStr);

    // Sex
    QString tempString;
    if (!QMUD::fileJsonRead(obj, "sex", tempString, false, true, QMUD::sexTypeToString(QMUD::SexType::MALE)))
        return false;

    QMUD::SexType tmpSex = QMUD::sexTypeFromString(tempString);

    if (tmpSex == QMUD::SexType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave 'sex' non valida: %1").arg(tempString));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return false;
    }

    setSex(tmpSex);

    // Classe
    auto itClass = obj.find("class");
    auto itLevel = obj.find("level");
    auto itExperience = obj.find("experience");

    if (itClass == obj.end())
    {
        GLOBAL_CRITICAL(tr("Chiave 'class' non trovata"));
        return false;
    }

    if (!itClass->isString())
    {
        GLOBAL_CRITICAL(tr("Chiave 'class' non e' una stringa"));
        return false;
    }

    if (itLevel == obj.end())
    {
        GLOBAL_CRITICAL(tr("Chiave 'level' non trovata"));
        return false;
    }

    if (!itLevel->isDouble())
    {
        GLOBAL_CRITICAL(tr("Chiave 'level' non e' un numero"));
        return false;
    }

    if (itExperience == obj.end())
    {
        GLOBAL_CRITICAL(tr("Chiave 'experience' non trovata"));
        return false;
    }

    if (!itExperience->isDouble())
    {
        GLOBAL_CRITICAL(tr("Chiave 'experience' non e' un numero"));
        return false;
    }

    CharacterClass * chClass = CharacterClass::fromString(itClass->toString());

    setChClass(chClass, itLevel->toInt(), qRound64(itExperience->toDouble()));

    // Race
    QString tmpRaceStr;
    if (!QMUD::fileJsonRead(obj, "race", tmpRaceStr))
        return false;

    bool ok;
    QMUD::RaceType tmpRace = QMUD::Race::fromString(tmpRaceStr, &ok);
    if (!ok)
    {
        GLOBAL_CRITICAL(tr("'%1' non e' una razza valida").arg(tmpRaceStr));
        return false;
    }

    setRace(tmpRace);

    // HpCurrent
    if (!QMUD::fileJsonRead(obj, "hpCurrent", tmpInt))
        return false;

    if (tmpInt < QMUD_GLOBAL_CH_MIN_MAXIMUM_HP)
        tmpInt = QMUD_GLOBAL_CH_MIN_MAXIMUM_HP;

    setHitPointsCurrent(tmpInt);

    // ApCurrent
    if (!QMUD::fileJsonRead(obj, "apCurrent", tmpInt))
        return false;

    setActionPointsCurrent(tmpInt);

    // MpCurrent
    if (!QMUD::fileJsonRead(obj, "mpCurrent", tmpInt))
        return false;

    setManaPointsCurrent(tmpInt);

    // Str
    if (!QMUD::fileJsonRead(obj, "str", tmpInt))
        return false;

    setStrengthBase(tmpInt);

    // Cos
    if (!QMUD::fileJsonRead(obj, "con", tmpInt))
        return false;

    setConstitutionBase(tmpInt);

    // Int
    if (!QMUD::fileJsonRead(obj, "int", tmpInt))
        return false;

    setIntelligenceBase(tmpInt);

    // Wis
    if (!QMUD::fileJsonRead(obj, "wis", tmpInt))
        return false;

    setWisdomBase(tmpInt);

    // Dex
    if (!QMUD::fileJsonRead(obj, "dex", tmpInt))
        return false;

    setDexterityBase(tmpInt);

    // Cha
    if (!QMUD::fileJsonRead(obj, "cha", tmpInt))
        return false;

    setCharismaBase(tmpInt);

    // RRadar
    if (!QMUD::fileJsonRead(obj, "rradar", m_bResourceRadarIsOn, true, false))
        return false;

    // Resources
    auto itResources = obj.find("resources");

    if (itResources != obj.end())
    {
        if (!itResources.value().isObject())
        {
            GLOBAL_CRITICAL(tr("Chiave 'resources' non e' un oggetto"));
            return false;
        }

        auto objResources = itResources->toObject();

        for (int i = static_cast<int>(QMUD::ResourceType::RESOURCE_FIRST);
             i < static_cast<int>(QMUD::ResourceType::MAX_VALUE); ++i)
        {
            auto itResource = objResources.find(QMUD::resourceToString(static_cast<QMUD::ResourceType>(i)));

            if (itResource != objResources.end() && itResource.value().isDouble())
                setResource(static_cast<QMUD::ResourceType>(i), qRound64(itResource.value().toDouble()));
        }
    }

    // Inventario
    auto itInventory = obj.find("inventory");

    if (itInventory != obj.end())
    {
        if (!itInventory.value().isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'inventory' non e' un array"));
            return false;
        }

        QJsonArray jsonArrayInventory = itInventory.value().toArray();

        for (auto elem : jsonArrayInventory)
        {
            if (!elem.isObject())
            {
                GLOBAL_CRITICAL(tr("Array 'inventory' non valido"));
                return false;
            }

            QJsonObject jsonObjItem = elem.toObject();

            Item *item = Item::loadItem(jsonObjItem, this);

            if (!item)
                return false;

            // Controllo se l'oggetto va equippato
            if (QMUD::fileJsonRead(jsonObjItem, "equipped", tmpInt, true, -1))
            {
                if (tmpInt != -1)
                {
                    equip(item, false);
                }
            }
        }
    }

    // Loot
    auto itLoot = obj.find("loot");

    if (itLoot != obj.end())
    {
        if (!itLoot.value().isArray())
        {
            GLOBAL_CRITICAL(tr("Array 'loot' non valido"));
            return false;
        }

        auto arrayLoot = itLoot.value().toArray();

        for (auto e : arrayLoot)
        {
            if (!e.isObject())
            {
                GLOBAL_CRITICAL(tr("Array 'loot' non valido"));
                return false;
            }

            LootItemData data;

            if (!data.fromJson(e.toObject()))
            {
                GLOBAL_CRITICAL(tr("Array 'loot' non valido"));
                return false;
            }

            m_vLootList.push_back(data);
        }
    }

    // Status
    auto itStatus = obj.find("status");

    if (itStatus != obj.end())
    {
        if (!itStatus->isArray())
        {
            GLOBAL_CRITICAL(tr("Array 'status' non valido"));
            return false;
        }

        QJsonArray array = itStatus->toArray();

        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if (!it->isObject())
            {
                GLOBAL_CRITICAL(tr("Array 'status' non valido"));
                return false;
            }

            QJsonObject objStatus = it->toObject();

            auto itStatus2 =  objStatus.find("status");
            auto itTime = objStatus.find("time");

            if (itStatus2 == objStatus.end() ||
                    !itStatus2->isDouble() ||
                    itTime == objStatus.end() ||
                    !itTime->isDouble())
            {
                GLOBAL_CRITICAL(tr("Array 'status' non valido"));
                return false;
            }

            int time = qRound(itTime->toDouble());

            if (time <= 0)
                continue;

            QMUD::TemporaryStatusType status;
            int tmpStatus = qRound(itStatus2->toDouble());

            if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::BLINDED))
                status = QMUD::TemporaryStatusType::BLINDED;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::STUNNED))
                status = QMUD::TemporaryStatusType::STUNNED;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::SANCTIFY))
                status = QMUD::TemporaryStatusType::SANCTIFY;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ACTIONLAG))
                status = QMUD::TemporaryStatusType::ACTIONLAG;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::SHIELD))
                status = QMUD::TemporaryStatusType::SHIELD;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::STRENGHT))
                status = QMUD::TemporaryStatusType::STRENGHT;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::DIVINE_SHIELD))
                status = QMUD::TemporaryStatusType::DIVINE_SHIELD;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::FLY))
                status = QMUD::TemporaryStatusType::FLY;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::INVISIBILITY))
                status = QMUD::TemporaryStatusType::INVISIBILITY;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::SEE_INVISIBILITY))
                status = QMUD::TemporaryStatusType::SEE_INVISIBILITY;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::FIRE_SHIELD))
                status = QMUD::TemporaryStatusType::FIRE_SHIELD;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ICE_SHIELD))
                status = QMUD::TemporaryStatusType::ICE_SHIELD;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::BLADES_SHIELD))
                status = QMUD::TemporaryStatusType::BLADES_SHIELD;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1))
                status = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2))
                status = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3))
                status = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4))
                status = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4;
            else if (tmpStatus == static_cast<int>(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5))
                status = QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5;
            else
            {
                GLOBAL_CRITICAL(tr("Array 'status' non valido"));
                return false;
            }

            m_mapTemporaryStatus[status] = time;
        }
    }

    // Professioni e ricette
    m_mapProfessionsLevel.clear();
    auto itProfessions = obj.find("professions");
    if (itProfessions != obj.end())
    {
        if (!itProfessions->isObject())
        {
            GLOBAL_CRITICAL(tr("Oggetto 'professions' non valido"));
            return false;
        }

        QJsonObject objProfessions = itProfessions->toObject();

        for (auto it = objProfessions.begin(); it != objProfessions.end(); ++it)
        {
            QMUD::ProfessionType p = QMUD::professionFromString(it.key());

            if (p == QMUD::ProfessionType::UNKNOWN || !it.value().isDouble())
            {
                GLOBAL_CRITICAL(tr("Oggetto 'professions' non valido: professione '%1' non valida").arg(it.key()));
                return false;
            }

            m_mapProfessionsLevel[p] = it.value().toInt();
        }
    }

    // Ultima posizione
    QMUD::StaticIdType tmpStaticId;
    if (!QMUD::fileJsonReadStaticId(obj, "lastSessionMapId", tmpStaticId, true, 0))
        return false;

    m_iLastSessionMapStaticId = tmpStaticId;

    if (!QMUD::fileJsonReadStaticId(obj, "lastSessionRoomId", tmpStaticId, true, 0))
        return false;

    m_iLastSessionRoomStaticId = tmpStaticId;

    // Titolo
    if (!QMUD::fileJsonRead(obj, "title", m_strTitle, true, true))
        return false;

    // Quest
    m_mapCompletedQuests.clear();
    auto itCompletedQuests = obj.find("completedQuests");
    if (itCompletedQuests != obj.end())
    {
        if (!itCompletedQuests->isArray())
        {
            GLOBAL_CRITICAL(tr("Array 'completedQuests' non valido"));
            return false;
        }

        auto array = itCompletedQuests->toArray();
        bool questOk = false;

        for (auto elem : array)
        {
            auto questId = Quest::idFromJsonValue(elem, questOk);

            if (questOk)
            {
                auto quest = QMUD::qmudWorld->quest(questId);

                if (quest)
                    m_mapCompletedQuests[questId] = quest;
            }
            else
            {
                GLOBAL_CRITICAL(tr("Quest completata %1 non valida").arg(Quest::idToString(questId)));
                return false;
            }
        }
    }

    m_mapActiveQuests.clear();
    auto itActiveQuests = obj.find("activeQuests");
    if (itActiveQuests != obj.end())
    {
        if (!itActiveQuests->isArray())
        {
            GLOBAL_CRITICAL(tr("Array 'activeQuests' non valido"));
            return false;
        }

        auto array = itActiveQuests->toArray();

        for (auto elem : array)
        {
            auto activeQuestObj = elem.toObject();

            ActiveQuest quest;
            if (quest.read(activeQuestObj))
            {
                m_mapActiveQuests[quest.quest()->id()] = quest;
            }
            else
            {
                GLOBAL_CRITICAL(tr("Array quest attive non valido"));
                return false;
            }
        }
    }

    return true;
}

void PcCharacter::writeJson(QJsonObject &obj)
{
    // Caratteristiche base
    obj["name"] = name(Q_NULLPTR);
    obj["sex"] = QMUD::sexTypeToString(sex());
    obj["class"] = CharacterClass::toString(chClass()->classType());
    obj["level"] = level();
    obj["experience"] = experience();
    obj["race"] = QMUD::Race::toString(race());
    obj["hpCurrent"] = hitPointsCurrent();
    obj["apCurrent"] = actionPointsCurrent();
    obj["mpCurrent"] = manaPointsCurrent();
    obj["str"] = strengthBase();
    obj["con"] = constitutionBase();
    obj["int"] = intelligenceBase();
    obj["wis"] = wisdomBase();
    obj["dex"] = dexterityBase();
    obj["cha"] = charismaBase();
    obj["rradar"] = m_bResourceRadarIsOn;

    // Risorse
    QJsonObject objResources;
    auto r = resources();
    for (auto it = r.begin(); it != r.end(); ++it)
        if (it.value() > 0)
            objResources[QMUD::resourceToString(it.key())] = it.value();

    obj["resources"] = objResources;

    // Inventario
    QJsonArray jsonArrayInventory;

    auto inv = inventory(true);

    for (auto item : inv)
    {
        QJsonObject jsonObjItem;

        item->writeJson(jsonObjItem);

        EquipmentSlot slot;

        if (isEquipped(item, slot))
            jsonObjItem["equipped"] = static_cast<int>(slot);

        jsonArrayInventory.push_back(jsonObjItem);
    }

    if (!jsonArrayInventory.isEmpty())
        obj["inventory"] = jsonArrayInventory;

    // Loot
    QJsonArray arrayLoot;
    for (auto data : m_vLootList)
        arrayLoot.push_back(data.toJson());

    obj["loot"] = arrayLoot;

    // Status
    QJsonArray arrayStatus;
    for (auto it = m_mapTemporaryStatus.begin(); it != m_mapTemporaryStatus.end(); ++it)
    {
        if (it.value() > 0 && it.value() != QMUD_GLOBAL_STATUS_DURATION_INFINITE)
        {
            QJsonObject objStatus;
            objStatus["status"] = static_cast<int>(it.key());
            objStatus["time"] = it.value();
            arrayStatus.push_back(objStatus);
        }
    }

    if (!arrayStatus.isEmpty())
        obj["status"] = arrayStatus;

    // Professioni e ricette
    QJsonObject professionObj;
    for (auto it = m_mapProfessionsLevel.begin(); it != m_mapProfessionsLevel.end(); ++it)
        professionObj[QMUD::professionToString(it.key())] = it.value();

    obj["professions"] = professionObj;

    // Dati ultima sessione
    if (parent())
    {
        Room *currentRoom = room();
        obj["lastSessionMapId"] = static_cast<long long int>(currentRoom->map()->staticId());
        obj["lastSessionRoomId"] = static_cast<long long int>(currentRoom->staticId());
    }
    else
    {
        obj["lastSessionMapId"] = static_cast<long long int>(m_iLastSessionMapStaticId);
        obj["lastSessionRoomId"] = static_cast<long long int>(m_iLastSessionRoomStaticId);
    }

    // Titolo
    if (!m_strTitle.isEmpty())
        obj["title"] = m_strTitle;

    // Quest
    if (!m_mapCompletedQuests.isEmpty())
    {
        QJsonArray array;
        for (auto quest : m_mapCompletedQuests)
            array.push_back(quest->idToJsonValue());
        obj["completedQuests"] = array;
    }

    if (!m_mapActiveQuests.isEmpty())
    {
        QJsonArray array;
        for (auto quest : m_mapActiveQuests)
        {
            QJsonObject objQuest;
            quest.write(objQuest);
            array.push_back(objQuest);
        }

        obj["activeQuests"] = array;
    }
}

void PcCharacter::updateRecipes()
{
    m_vRecipesByMapAndId.clear();

    auto recipes = QMUD::qmudWorld->recipes();
    for (auto m : recipes)
        for (auto recipe : m)
        {
            auto profLevelIt = m_mapProfessionsLevel.find(recipe.profession());

            int profLevel = 0;
            if (profLevelIt != m_mapProfessionsLevel.end())
                profLevel = profLevelIt.value();

            if (recipe.price() <= 0 && recipe.level() <= profLevel)
                m_vRecipesByMapAndId.push_back(QPair<QMUD::StaticIdType, QMUD::StaticIdType>(recipe.mapId(), recipe.id()));
        }

    m_vRecipesByMapAndId.append(m_vRecipesByMapAndIdLearned);
}

PcCharacter::ActiveQuest::ActiveQuest(Quest *quest) :
    m_ptrQuest(quest)
{
    m_vProgress.resize(quest->targetData().size());
}

PcCharacter::ActiveQuest::ActiveQuest() :
    m_ptrQuest(Q_NULLPTR)
{

}

bool PcCharacter::ActiveQuest::isCompleted() const
{
    Q_ASSERT(m_vProgress.size() == m_ptrQuest->targetData().size());

    int targetIndex = 0;
    for (const auto& target : m_ptrQuest->targetData())
    {
        if (!m_vProgress[targetIndex].isCompleted(target))
            return false;

        targetIndex++;
    }

    return true;
}

void PcCharacter::ActiveQuest::questKillCheck(NpcCharacter *npc, Player* player)
{
    Q_ASSERT(m_vProgress.size() == m_ptrQuest->targetData().size());

    int targetIndex = 0;
    for (auto& target : m_ptrQuest->targetData())
    {
        if (target.npcToKillEnabled() &&
                target.npcToKillMapId() == npc->mapStaticId() &&
                target.npcToKillId() == npc->staticId() &&
                m_vProgress[targetIndex].npcToKillCount() < target.npcToKillCount())
        {
            if (target.npcToKillGiveItemEnabled())
            {
                if (QMUD::rollDice(1, target.npcToKillGiveItemProbability()) == 1)
                    m_vProgress[targetIndex].incNpcToKillCount();
            }
            else
            {
                m_vProgress[targetIndex].incNpcToKillCount();
            }

            player->sendLine(tr("&Y[%1] %2: %3 di %4&d")
                               .arg(m_ptrQuest->name())
                               .arg(target.description())
                               .arg(m_vProgress[targetIndex].npcToKillCount())
                               .arg(target.npcToKillCount()));

            if (m_vProgress[targetIndex].npcToKillCount() == target.npcToKillCount())
                player->sendLine("&Y" + target.completeDescription() + "&d");
        }

        targetIndex++;
    }
}

void PcCharacter::ActiveQuest::sendProgress(Player *player) const
{
    Q_ASSERT(m_vProgress.size() == m_ptrQuest->targetData().size());

    for (int i = 0; i < m_vProgress.size(); ++i)
    {
        if (m_ptrQuest->targetData()[i].npcToKillEnabled())
            player->sendLine(QObject::tr("%1 - %2 di %3%4")
                             .arg(m_ptrQuest->targetData()[i].description())
                             .arg(m_vProgress[i].npcToKillCount())
                             .arg(m_ptrQuest->targetData()[i].npcToKillCount())
                             .arg(m_vProgress[i].isCompleted(m_ptrQuest->targetData()[i]) ? QObject::tr(" [COMPLETATO]") :
                                                                                            QObject::tr(" [%[quest seguo %1 %2]&C&uSEGUI&d%[]]").arg(m_ptrQuest->idToString()).arg(i)));
    }
}

bool PcCharacter::ActiveQuest::read(const QJsonObject &obj)
{
    bool questOk = false;

    auto itQuestId = obj.find("id");

    if (itQuestId == obj.end())
        return false;

    auto questId = Quest::idFromJsonValue(itQuestId.value(), questOk);

    if (!questOk)
        return false;

    auto itProgress = obj.find("progress");

    if (itProgress == obj.end() || !itProgress->isArray())
        return false;

    auto progressArray = itProgress->toArray();

    m_vProgress.reserve(progressArray.size());

    for (auto objProg : progressArray)
    {
        TargetProgress progress;

        if (!progress.read(objProg.toObject()))
            return false;

        m_vProgress.push_back(progress);
    }

    auto quest = QMUD::qmudWorld->quest(questId);

    if (!quest || quest->targetData().size() != m_vProgress.size())
        return false;

    m_ptrQuest = quest;
    return true;
}

void PcCharacter::ActiveQuest::write(QJsonObject &obj) const
{
    Q_ASSERT(m_vProgress.size() == m_ptrQuest->targetData().size());

    obj["id"] = m_ptrQuest->idToJsonValue();

    QJsonArray arrayProgress;

    for (const auto& tProg : m_vProgress)
    {
        QJsonObject progress;
        tProg.write(progress);
        arrayProgress.push_back(progress);
    }

    obj["progress"] = arrayProgress;
}

const Quest *PcCharacter::ActiveQuest::quest() const
{
    return m_ptrQuest;
}


PcCharacter::ActiveQuest::TargetProgress::TargetProgress() :
    m_bNpcToTalkToCompleted(false),
    m_iNpcToKillCount(0)
{

}

bool PcCharacter::ActiveQuest::TargetProgress::isCompleted(const Quest::TargetData &targetData) const
{
    if (targetData.npcToTalkToEnabled() &&
            !m_bNpcToTalkToCompleted)
        return false;

    if (targetData.npcToKillEnabled() &&
            m_iNpcToKillCount < targetData.npcToKillCount())
        return false;

    return true;
}

void PcCharacter::ActiveQuest::TargetProgress::setNpcToTalkToCompleted(bool isCompleted)
{
    m_bNpcToTalkToCompleted = isCompleted;
}

void PcCharacter::ActiveQuest::TargetProgress::incNpcToKillCount(int count)
{
    m_iNpcToKillCount += count;
}

void PcCharacter::ActiveQuest::TargetProgress::setNpcToKillCount(int count)
{
    m_iNpcToKillCount = count;
}

int PcCharacter::ActiveQuest::TargetProgress::npcToKillCount() const
{
    return m_iNpcToKillCount;
}

bool PcCharacter::ActiveQuest::TargetProgress::read(const QJsonObject &obj)
{
    if (!QMUD::fileJsonRead(obj, "t", m_bNpcToTalkToCompleted))
        return false;

    if (!QMUD::fileJsonRead(obj, "k", m_iNpcToKillCount))
        return false;

    return true;
}

void PcCharacter::ActiveQuest::TargetProgress::write(QJsonObject &obj) const
{
    obj["t"] = m_bNpcToTalkToCompleted;
    obj["k"] = m_iNpcToKillCount;
}

