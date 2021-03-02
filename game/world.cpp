/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "world.h"
#include "playerconnection.h"
#include "player.h"
#include "character.h"
#include "command.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "characterclass.h"
#include "group.h"
#include "map.h"
#include "../network/netconnection.h"
#include "../global.h"
#include "../common.h"
#include "../globaldebug.h"
#include "color.h"
#include "itemweapon.h"
#include "mapsgraph.h"

#include <QFileInfo>
#include <QDir>
#include <QMetaType>
#include <QtQml>
#include <QStringList>


#define DEFAULT_MAP_STATIC_ID   2
#define DEFAULT_ROOM_STATIC_ID  1000010

#define DEFAULT_MAP_STATIC_ID_AFTER_DEATH   2
#define DEFAULT_ROOM_STATIC_ID_AFTER_DEATH  10


using namespace CORE;

World::World() :
    Entity(EntityType::WORLD, Q_NULLPTR),
    m_iInstanceStaticIdCurrent(QMUD_INSTANCE_FIRST_STATIC_ID)
{
    connect(&m_timerUpdatePlayers, SIGNAL(timeout()), this, SLOT(onTimerUpdatePlayersTimeout()));
    connect(&m_timerUpdateCharacters, SIGNAL(timeout()), this, SLOT(onTimerUpdateCharactersTimeout()));
    connect(&m_timerUpdateCharactersStatus, SIGNAL(timeout()), this, SLOT(onTimerUpdateCharactersStatusTimeout()));
    connect(&m_timerUpdateNpcResurrection, SIGNAL(timeout()), this, SLOT(onTimerUpdateNpcResurrectionTimeout()));
    connect(&m_timerUpdateEntityScriptEvent, SIGNAL(timeout()), this, SLOT(onTimerUpdateEntityScriptEvent()));
    connect(&m_timerUpdateMaps, SIGNAL(timeout()), this, SLOT(onTimerUpdateMapsTimeout()));
    connect(&m_timerUpdateItems, SIGNAL(timeout()), this, SLOT(onTimerUpdateItems()));
    connect(&m_timerUpdateWorld, SIGNAL(timeout()), this, SLOT(onTimerUpdateWorld()));

    m_timerUpdatePlayers.setSingleShot(false);
    m_timerUpdatePlayers.setInterval(QMUD_GLOBAL_TIMER_UPDATE_PLAYERS_TIMEOUT_MS);
    m_timerUpdatePlayers.start();

    m_timerUpdateCharacters.setSingleShot(false);
    m_timerUpdateCharacters.setInterval(QMUD_GLOBAL_TIMER_UPDATE_CHARACTERS_TIMEOUT_MS);
    m_timerUpdateCharacters.start();

    m_timerUpdateCharactersStatus.setSingleShot(false);
    m_timerUpdateCharactersStatus.setInterval(QMUD_GLOBAL_TIMER_UPDATE_CHARACTERS_STATUS_TIMEOUT_MS);
    m_timerUpdateCharactersStatus.start();

    m_timerUpdateNpcResurrection.setSingleShot(false);
    m_timerUpdateNpcResurrection.setInterval(QMUD_GLOBAL_TIMER_UPDATE_NPC_RESURRECTION_MS);
    m_timerUpdateNpcResurrection.start();

    m_timerUpdateEntityScriptEvent.setSingleShot(false);
    m_timerUpdateEntityScriptEvent.setInterval(QMUD_GLOBAL_TIMER_UPDATE_ENTITY_SCRIPT_EVENT_TIMEOUT_MS);
    m_timerUpdateEntityScriptEvent.start();

    m_timerUpdateMaps.setSingleShot(false);
    m_timerUpdateMaps.setInterval(QMUD_GLOBAL_TIMER_UPDATE_MAPS_TIMEOUT_MS);
    m_timerUpdateMaps.start();

    m_timerUpdateItems.setSingleShot(false);
    m_timerUpdateItems.setInterval(QMUD_GLOBAL_TIMER_UPDATE_ITEMS_TIMEOUT_MS);
    m_timerUpdateItems.start();

    m_timerUpdateWorld.setSingleShot(false);
    m_timerUpdateWorld.setInterval(QMUD_GLOBAL_TIMER_UPDATE_WORLD_TIMEOUT_MS);
    m_timerUpdateWorld.start();
}

World::~World()
{
    GLOBAL_DEBUG_DESTRUCTOR("World", "");

    // Salvo le statistiche
    GLOBAL_INFO(tr("Salvo le statistiche globali"));
    m_globalStatistics += m_globalStatisticsFromLastReboot;
    m_globalStatistics.save(m_dirRoot.path());
}

bool World::init(QDir rootDir)
{
    // Registro gli enumerativi per gli script
    qRegisterMetaType<QMUD::IdType>("QMUD::IdType");
    qRegisterMetaType<QMUD::SexType>("QMUD::SexType");
    qRegisterMetaType<QMUD::RaceType>("QMUD::RaceType");
    qRegisterMetaType<QMUD::ClassType>("QMUD::ClassType");
    qRegisterMetaType<QMUD::DamageType>("QMUD::DamageType");
    //qmlRegisterUncreatableType<Character>("", 1, 0, "Character", "Don't create Character type");
    //qmlRegisterUncreatableType<NpcCharacter>("", 1, 0, "NpcCharacter", "Don't create NpcCharacter type");
    //qmlRegisterUncreatableType<PcCharacter>("", 1, 0, "PcCharacter", "Don't create PcCharacter type");
    qRegisterMetaType<Character*>("const Character*");

    QMUD::sexTypeJsRegistration(m_jsEngine);
    QMUD::raceTypeJsRegistration(m_jsEngine);
    QMUD::classTypeJsRegistration(m_jsEngine);
    QMUD::damageTypeJsRegistration(m_jsEngine);

    // Controllo le directory
    m_dirRoot = rootDir;
    m_dirPlayers = QDir(rootDir.path() + "/" + QMUD_GLOBAL_DIR_PLAYERS);
    m_dirCharacters = QDir(rootDir.path() + "/" + QMUD_GLOBAL_DIR_CHARACTERS);
    m_dirMaps = QDir(rootDir.path() + "/" + QMUD_GLOBAL_DIR_MAPS);

    QStringList paths;
    paths << m_dirPlayers.path();
    paths << m_dirCharacters.path();
    paths << m_dirMaps.path();

    for (QString path : paths)
    {
        GLOBAL_INFO(tr("Controllo directory %1").arg(path));

        if (!QFileInfo(path).exists() && !QDir().mkpath(path))
        {
            GLOBAL_CRITICAL(tr("Impossibile creare la directory %1").arg(path));
            return false;
        }
        else if (!QFileInfo(path).isDir())
        {
            GLOBAL_CRITICAL(tr("Il path %1 non e' una directory"));
            return false;
        }
    }

    GLOBAL_INFO(tr("Caricamento della lista ban"));

    // Carico la lista dei nomi bannati
    if (!loadForbiddenPcNames(m_dirRoot.path() + "/forbidden_names.txt"))
    {
        GLOBAL_CRITICAL(tr("Errore durante il caricamento del file forbidden_names.txt"));
        return false;
    }

    GLOBAL_INFO(tr("Caricamento statistiche"));

    // Carico le statistiche
    if (!m_globalStatistics.load(rootDir.path()))
    {
        GLOBAL_CRITICAL(tr("Errore durante il caricamento del file delle statistiche globali"));
        return false;
    }

    // Controllo i Character
    GLOBAL_INFO(tr("Controllo file characters: %1 file da controllare").arg(m_dirCharacters.entryList().size()));
    QMap<QString, QMUD::PcCharacterBaseInfo> pcBaseInfo;
    if (!PcCharacter::checkCharacterFiles(m_dirCharacters, pcBaseInfo))
        return false;

    m_mapPcCharactersBaseInfo = pcBaseInfo;

    // Carico le mappe
    QStringList mapFileList = m_dirMaps.entryList(QDir::Files);

    GLOBAL_INFO(tr("Caricamento mappe: %1 mappe da caricare").arg(mapFileList.size()));

    for (QString file : mapFileList)
    {
        if (QFileInfo(file).suffix() == "json")
        {
            GLOBAL_INFO(tr(" >> Caricamento mappa %1").arg(file));

            if (!loadMap(file))
                return false;
        }
    }

    if (m_mapMaps.isEmpty())
    {
        GLOBAL_CRITICAL(tr("Nessuna mappa caricata"));
        return false;
    }

    GLOBAL_INFO(tr("Controllo uscite"));
    for (auto map : m_mapMaps)
        checkRoomExit(map);

    GLOBAL_INFO(tr("Creazione albero delle mappe"));
    if (!createMapsTree())
        return false;

    GLOBAL_INFO(tr("Inizializzazione delle quest"));
    initQuests();

    return true;
}

Player* World::player(QMUD::IdType id) const
{
    auto it = m_mapPlayersByConnectionId.find(id);

    if (it != m_mapPlayersByConnectionId.end())
        return it.value();
    else
        return Q_NULLPTR;
}

QMap<QMUD::IdType, Player *> World::playersByConnectionId() const
{
    return m_mapPlayersByConnectionId;
}

QString World::name(Entity *suitor) const
{
    Q_UNUSED(suitor)

    return "Il mondo";
}

Map *World::map(QMUD::StaticIdType id) const
{
    auto it = m_mapMaps.find(id);

    if (it == m_mapMaps.end())
        return Q_NULLPTR;
    else
        return it.value();
}

QMap<QMUD::StaticIdType, Map *> World::maps() const
{
    return m_mapMaps;
}

QMap<QMUD::IdType, Item *> World::items() const
{
    return m_mapItems;
}

const MapsGraph &World::mapsGraph() const
{
    return m_mapsGraph;
}

Map *World::loadMap(QString filename)
{
    QMUD::FileOpStatus status;
    QVector<NpcCharacter*> npcs;
    QVector<Item*> items;
    QVector<QMUD::Recipe> recipes;
    QVector<Quest> quests;

    Map *map = Map::loadMap(m_dirMaps.path() + "/" + filename, status, this, npcs, items, recipes, quests);

    if (!map || status != QMUD::FileOpStatus::FILE_OP_OK)
    {
        GLOBAL_CRITICAL(tr("Mappa '%1' non caricata, errore %2")
                        .arg(filename)
                        .arg(static_cast<int>(status)));
        if (map)
            delete map;
        return Q_NULLPTR;
    }

    auto itMap = m_mapMaps.find(map->staticId());

    if (itMap != m_mapMaps.end())
    {
        GLOBAL_CRITICAL(tr("La mappa '%1' ha lo stesso id della mappa %2")
                        .arg(filename)
                        .arg(itMap.value()->filePath()));
        delete map;
        return Q_NULLPTR;
    }

    // Aggiungo le quest
    for (const auto& quest : quests)
        if (m_mapQuestsById.contains(quest.id()))
        {
            GLOBAL_CRITICAL(tr("La mappa '%1' ha contiene delle quest con id già esistenti (%2, %3)")
                            .arg(filename)
                            .arg(quest.mapId())
                            .arg(quest.questId()));
            delete map;
            return Q_NULLPTR;
        }

    for (const auto& quest : quests)
        m_mapQuestsById[quest.id()] = quest;

    // Aggiungo gli NPC al world
    for (auto npc : npcs)
        addCharacter(npc);

    for (auto recipe : recipes)
        m_mapRecipes[recipe.mapId()][recipe.id()] = recipe;

    m_mapMaps[map->staticId()] = map;

    return map;
}

void World::removeMap(QMUD::StaticIdType id)
{
    auto it = m_mapMaps.find(id);

    if (it == m_mapMaps.end())
    {
        return;
    }

    // Rimuovo la mappa dalla lista
    Map* map = it.value();
    m_mapMaps.erase(it);

    deleteMap(map);
}

Map *World::loadInstance(QString filename)
{
    QMUD::StaticIdType instanceStaticId = m_iInstanceStaticIdCurrent++;

    QMUD::FileOpStatus status;
    QVector<NpcCharacter*> npcs;
    QVector<Item*> items;
    QVector<QMUD::Recipe> recipes;
    QVector<Quest> quests;

    // Ignoro le recipe
    Map *map = Map::loadMap(m_dirMaps.path() + "/" + filename, status, this, npcs, items, recipes, quests, instanceStaticId);

    if (!map || status != QMUD::FileOpStatus::FILE_OP_OK)
    {
        GLOBAL_CRITICAL(tr("Mappa '%1' non caricata, errore %2")
                        .arg(filename)
                        .arg(static_cast<int>(status)));
        if (map)
            delete map;
        return Q_NULLPTR;
    }

    // Fix delle uscite
    auto rooms = map->rooms();

    for (auto room : rooms)
    {
        auto exits = room->exits();

        for (auto exit : exits)
            exit->m_iMapStaticId = instanceStaticId;
    }

    // Aggiungo gli npc al world
    for (auto npc : npcs)
        addCharacter(npc);

    m_mapMaps[instanceStaticId] = map;

    checkRoomExit(map);

    return map;
}

void World::unloadEmptyInstances()
{
    // Rimuovo le istanze vuote
    for (auto itMap = m_mapMaps.begin(); itMap != m_mapMaps.end(); )
    {
        if (!itMap.value()->isInstance())
        {
            ++itMap;
            continue;
        }

        if (itMap.value()->instanceLastEntryTime().addMSecs(QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS) >= QDateTime::currentDateTime())
        {
            ++itMap;
            continue;
        }

        auto characters = itMap.value()->characters();
        bool pcFound = false;

        for (auto c : characters)
        {
            if (c->chType() == Character::ChType::CHTYPE_PC)
            {
                pcFound = true;
                break;
            }
        }

        // Controllo se la mappa e' vuota
        if (pcFound)
        {
            ++itMap;
            continue;
        }

        GLOBAL_INFO(tr("Rimuovo istanza %1").arg(itMap.value()->staticId()));

        // Rimuovo la mappa dalla lista
        Map* map = itMap.value();
        itMap = m_mapMaps.erase(itMap);

        deleteMap(map);
    }
}

void World::addGroup(Group *group)
{
    m_vGroups.insert(group);

    connect(group, SIGNAL(disbanded(Group*)), this, SLOT(onGroupDisbanded(Group*)));
}

Character *World::character(QMUD::IdType id) const
{
    auto it = m_mapCharacters.find(id);

    if (it == m_mapCharacters.end())
        return Q_NULLPTR;
    else
        return it.value();
}

QMap<QMUD::IdType, Character *> World::characters() const
{
    return m_mapCharacters;
}

void World::onTimerUpdatePlayersTimeout()
{
    for (auto player : m_mapPlayersByConnectionId)
        emit player->update();
}

void World::onTimerUpdateCharactersTimeout()
{
    for (auto ch : m_mapCharacters)
    {
        if (ch->chType() == Character::ChType::CHTYPE_PC)
            static_cast<PcCharacter*>(ch)->sendNewCombatRound();
    }

    for (auto ch : m_mapCharacters)
    {
        ch->update();
    }

    flushPlayers();
}

void World::onTimerUpdateCharactersStatusTimeout()
{
    for (auto ch : m_mapCharacters)
        ch->updateStatus();

    flushPlayers();
}

void World::onTimerUpdateNpcResurrectionTimeout()
{
    for (auto it = m_vNpcToResurrect.begin(); it != m_vNpcToResurrect.end(); ++it)
    {
        it->resurrectionCounter--;

        if (it->resurrectionCounter <= 0)
        {
            auto itMap = m_mapMaps.find(it->mapId);

            if (itMap != m_mapMaps.end())
            {
                Room* room = itMap.value()->room(it->roomId);
                auto jsonNpcs = itMap.value()->mapJsonObjNPCsById();
                auto jsonItems = itMap.value()->mapJsonObjItemsById();
                auto itJsonNpc = jsonNpcs.find(it->id);

                if (room && itJsonNpc != jsonNpcs.end())
                {
                    NpcCharacter* npcCh = NpcCharacter::loadNpcCharacter(itJsonNpc.value(), jsonItems, itMap.key(), room);

                    if (npcCh)
                    {
                        if (npcCh->flagCheck(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL))
                        {
                            int wildX = room->wildX();
                            int wildY = room->wildY();

                            Room* newRoom = Q_NULLPTR;
                            int count = 0;

                            while (!newRoom && count < 10)
                            {
                                wildX += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL;
                                wildY += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL;

                                newRoom = itMap.value()->room(wildX, wildY, room->wildMapIndex());

                                if (newRoom)
                                {
                                    room = newRoom;
                                    break;
                                }

                                count++;
                            }

                            npcCh->setParent(room);
                        }
                        else if (npcCh->flagCheck(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE))
                        {
                            int wildX = room->wildX();
                            int wildY = room->wildY();

                            Room* newRoom = Q_NULLPTR;
                            int count = 0;

                            while (!newRoom && count < 10)
                            {
                                wildX += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE;
                                wildY += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE;

                                newRoom = itMap.value()->room(wildX, wildY, room->wildMapIndex());

                                if (newRoom)
                                {
                                    room = newRoom;
                                    break;
                                }

                                count++;
                            }

                            npcCh->setParent(room);
                        }

                        npcCh->setToResurrect(true);
                        addCharacter(npcCh);
                    }
                }
            }

            it = m_vNpcToResurrect.erase(it);

            if (it == m_vNpcToResurrect.end())
                break;
        }
    }
}

void World::onTimerUpdateEntityScriptEvent()
{
    for (auto it = m_vScriptEvent.begin(); it != m_vScriptEvent.end(); ++it)
    {
        it->counter--;

        if (it->counter <= 0)
        {
            auto itCh = m_mapCharacters.find(it->entity);

            if (itCh != m_mapCharacters.end())
            {
                itCh.value()->eventCustom(it->event);
            }


            it = m_vScriptEvent.erase(it);

            if (it == m_vScriptEvent.end())
                break;
        }
    }
}

void World::onTimerUpdateMapsTimeout()
{
    unloadEmptyInstances();

    for (auto& map : m_mapMaps)
    {
        map->update();
    }
}

void World::onTimerUpdateItems()
{
    QVector<Item*> itemsToDestroy;
    for (auto item : m_mapItems)
        if (item->isAutoDestroy())
        {
            if (item->autoDestroyCounter() > 0)
                item->setAutoDestroy(item->autoDestroyCounter() - 1);
            else
                itemsToDestroy.push_back(item);
        }

    for (auto item : itemsToDestroy)
        item->destroy();

}

void World::onTimerUpdateWorld()
{

}

void World::onConnection(NET::NetConnection *conn)
{
    Q_ASSERT(!!conn);

    PlayerConnection *pConn = new PlayerConnection(conn, this);

    m_mapConnections[pConn->id()] = pConn;

    connect(pConn, SIGNAL(disconnected(PlayerConnection*, Player*)), this, SLOT(onDisconnected(PlayerConnection*, Player*)));
    connect(pConn, SIGNAL(commandReceived(PlayerConnection*, Player*, QString)), this, SLOT(onCommandReceived(PlayerConnection*, Player*, QString)));

    // Invio la richiesta di inserimento username
    pConn->setPlayerConnectionStatus(PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_LOGIN);
}

void World::onDisconnected(PlayerConnection *conn, Player* player)
{
    Q_UNUSED(player)

    GLOBAL_INFO(tr("Rimozione della connessione %1").arg(conn->address()));

    removeConnection(conn->id());
}

void World::onCommandReceived(PlayerConnection *conn, Player* player, QString cmd)
{
    if (!conn->checkConnection())
        return;

    if (conn->playerConnectionStatus() == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_LOGIN)
    {
        Q_ASSERT(!player);

        // Controllo lo username inserito
        QStringList list = cmd.simplified().split(" ");

        if (list.isEmpty() ||
            (list[0].compare("login", Qt::CaseInsensitive) != 0 &&
             list[0].compare("signin", Qt::CaseInsensitive) != 0))
        {
            conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_GENERIC);
            return;
        }

        if (list.size() != 3 && list[0].compare("login", Qt::CaseInsensitive) == 0)
        {
            conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED);
            return;
        }

        if (list.size() != 3 && list[0].compare("signin", Qt::CaseInsensitive) == 0)
        {
            conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_SIGNIN_FAILED);
            return;
        }

        QString username = list[1].simplified().toLower();
        QString password = list[2].simplified().toLower();

        if (list[0].compare("login", Qt::CaseInsensitive) == 0)
        {
            // Carico il player
            QMUD::FileOpStatus loadStatus;
            Player* newPlayer = Player::loadPlayer(m_dirPlayers, conn, username, password, loadStatus, this);

            if (loadStatus == QMUD::FileOpStatus::FILE_OP_OK && newPlayer)
            {
                GLOBAL_INFO(tr("Player %1 connesso dall'indirizzo %2")
                            .arg(newPlayer->username())
                            .arg(conn->address()));

                conn->setPlayer(newPlayer);

                // Aggiungo il player al world
                addPlayer(newPlayer);

                // Invio la characters list al client
                newPlayer->sendCharactersList();
                conn->setPlayerConnectionStatus(PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_CHARACTERS_LIST);
            }
            else
            {
                QString strError;

                if (loadStatus == QMUD::FileOpStatus::FILE_OP_ERROR_GENERIC ||
                        loadStatus == QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME)
                    conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED);
                else
                {
                    GLOBAL_CRITICAL(tr("Tentativo di connessione da %1 [username:%2, status:%3]; Errore nel caricamento del player file")
                                .arg(conn->address())
                                .arg(username)
                                .arg(static_cast<int>(loadStatus)));
                    conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_LOGIN_FAILED_CONTACT_ADMINISTRATOR);
                }
            }
        }
        else
        {
            Q_ASSERT(!player);

            if (!checkPlayerAndCharacterName(username))
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_USERNAME_FAILED);
                return;
            }

            if (password.simplified().isEmpty())
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_PASSWORD_FAILED);
                return;
            }

            if (Player::exists(m_dirPlayers, username))
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_USERNAME_EXISTS);
                return;
            }

            QMUD::FileOpStatus status;

            // Creo il player
            if (!Player::createPlayer(m_dirPlayers, conn, username, password, status))
            {
                GLOBAL_INFO(tr("Creazione del player %1 fallita [connessione:%2, status:%3]")
                            .arg(username)
                            .arg(conn->address())
                            .arg(static_cast<int>(status)));

                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_SIGNIN_FAILED);
                return;
            }

            GLOBAL_INFO(tr("Creazione del player %1 avvenuta con successo [connessione:%2, status:%3]")
                        .arg(username)
                        .arg(conn->address())
                        .arg(static_cast<int>(status)));

            conn->sendMessage(QMUD::ClientDataMessage::Message::INFO_SIGNIN_OK);
        }
    }
    else if (conn->playerConnectionStatus() == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_CHARACTERS_LIST)
    {
        Q_ASSERT(!!player);
        Q_ASSERT(!player->pcCharacter());

        // Controllo lo username inserito
        QStringList list = cmd.simplified().split(" ");

        if (list.isEmpty() ||
            (list[0].compare("ch_connect", Qt::CaseInsensitive) != 0 &&
             list[0].compare("ch_create", Qt::CaseInsensitive) != 0))
        {
            conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_GENERIC);
            return;
        }

        if (list[0].compare("ch_connect", Qt::CaseInsensitive) == 0 && list.size() != 2)
        {
            conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_GENERIC);
            return;
        }

        if (list[0].compare("ch_create", Qt::CaseInsensitive) == 0 && list.size() != 5)
        {
            conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_GENERIC);
            return;
        }

        if (list[0].compare("ch_connect", Qt::CaseInsensitive) == 0)
        {
            QString characterName = list[1].toLower();

            if (!checkPlayerAndCharacterName(characterName))
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED);
                return;
            }

            QMUD::FileOpStatus status;

            PcCharacter* ch = player->loadCharacter(m_dirCharacters, characterName, status, Q_NULLPTR);

            if (!ch)
            {
                if (status != QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME)
                {
                    GLOBAL_CRITICAL(tr("Caricamento del personaggio %1 fallito: %2").arg(characterName).arg(static_cast<int>(status)));
                    conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED_CONTACT_ADMINISTRATOR);
                }
                else
                    conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CONNECTION_FAILED);
            }
            else
            {
                GLOBAL_INFO(tr("Il player %1 si e' connesso con il character %2")
                            .arg(player->username())
                            .arg(ch->name(Q_NULLPTR)));

                player->setPcCharacter(ch);
                player->forceSave();

                // Aggiungo il character al world
                addCharacter(ch);

                conn->setPlayerConnectionStatus(PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_IN_GAME);
                player->pcCharacter()->sendLook();
                player->pcCharacter()->sendDataBasicInfo();
                player->pcCharacter()->sendDataLoot();
                player->pcCharacter()->sendDataStatus();
            }
        }
        else
        {
            QString characterName = list[1].toLower();
            QString strChClass = list[2].toLower();
            QString strChRace = list[3].toLower();
            QString strChSex = list[4].toLower();

            if (!checkPlayerAndCharacterName(characterName))
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED);
                return;
            }

            QMUD::ClassType chClass = QMUD::classTypeFromString(strChClass);

            if (chClass == QMUD::ClassType::UNKNOWN)
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR);
                return;
            }

            bool isOk = true;
            QMUD::RaceType chRace = QMUD::Race::fromString(strChRace, &isOk);

            if (!isOk)
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR);
                return;
            }

            QMUD::SexType chSex = QMUD::sexTypeFromString(strChSex);

            if (chSex == QMUD::SexType::UNKNOWN)
            {
                conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR);
                return;
            }

            int classInt = 18;
            int classWis = 18;
            int classStr = 18;
            int classCon = 18;
            int classDex = 18;

            QMUD::classTypeToMaxStatistics(chClass, classInt, classWis, classStr, classCon, classDex);

            classInt += QMUD::Race::basePCInt(chRace);
            classWis += QMUD::Race::basePCWis(chRace);
            classStr += QMUD::Race::basePCCon(chRace);
            classCon += QMUD::Race::basePCStr(chRace);
            classDex += QMUD::Race::basePCDex(chRace);

            QMUD::FileOpStatus status;

            // Creo il character
            if (!PcCharacter::createCharacter(m_dirCharacters, conn, player, characterName, chSex,
                                              chClass, chRace,
                                              classInt, classWis, classStr, classCon, classDex,
                                              status, true))
            {
                if (status != QMUD::FileOpStatus::FILE_OP_ERROR_FILE_ALREADY_EXISTS)
                    GLOBAL_CRITICAL(tr("Creazione del character %1 fallita: %2").arg(characterName).arg(static_cast<int>(status)));

                if (status == QMUD::FileOpStatus::FILE_OP_ERROR_FILE_ALREADY_EXISTS)
                    conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED);
                else
                    conn->sendMessage(QMUD::ClientDataMessage::Message::ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR);
            }
            else
            {
                player->addCharacterToList(characterName);
                player->forceSave();

                QMUD::PcCharacterBaseInfo baseInfo(characterName.toLower(), chRace, chClass, 1);
                m_mapPcCharactersBaseInfo[characterName.toLower()] = baseInfo;
            }

            player->sendCharactersList();
            conn->sendMessage(QMUD::ClientDataMessage::Message::INFO_CH_CREATION_OK);
            return;
        }
    }
    else if (conn->playerConnectionStatus() == PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_IN_GAME)
    {
        Q_ASSERT(!!player);
        Q_ASSERT(!!player->pcCharacter());

        if (player->waitConfirmation())
        {
            if (cmd == tr("SI"))
            {
                Command::parse(player->clearConfirmationRequest(), player->pcCharacter())->execute(true);
                Command::parse("", player->pcCharacter())->execute();
                return;
            }

            player->clearConfirmationRequest();

            if (cmd == tr("NO"))
            {
                Command::parse("", player->pcCharacter())->execute();
                return;
            }
        }

        Command::parse(cmd, player->pcCharacter())->execute();
    }
}

void World::onNpcCommandReceived(NpcCharacter *npc, QString cmd)
{
    Q_ASSERT(!!npc);

    Command::parse(cmd, npc)->execute();
}


void World::onRemoveFromTheWorld(Character *ch)
{
    QList<Entity*> entities = qobject_cast<Room*>(ch->parent())->entities(Q_NULLPTR);

    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        if ((*it)->entityType() == EntityType::CHARACTER &&
            (*it)->id() != ch->id())
        {
            Character *chTmp = qobject_cast<Character*>(*it);

            if (chTmp->chType() == Character::ChType::CHTYPE_PC)
            {
                Player* chTmpPlayer = Q_NULLPTR;
                chTmpPlayer = dynamic_cast<PcCharacter*>(chTmp)->player();
                chTmpPlayer->sendLine(tr("&W%1 cade a terra morto! &RRIP&d").arg(ch->name(chTmp)));
            }
        }
    }

    if (ch->chType() == Character::ChType::CHTYPE_PC)
    {
        Player* player = dynamic_cast<PcCharacter*>(ch)->player();

        player->sendLine(tr("&WPeccato sei morto! &RRIP&d"));
        player->pcCharacter()->setParent(m_mapMaps[DEFAULT_MAP_STATIC_ID_AFTER_DEATH]->room(DEFAULT_ROOM_STATIC_ID_AFTER_DEATH));
        player->forceSave();

        removeCharacter(ch->id());

        player->setPcCharacter(Q_NULLPTR);

        // Invio la characters list al client
        player->sendCharactersList();
        player->connection()->setPlayerConnectionStatus(PlayerConnection::PlayerConnectionStatus::PLAYER_CONN_STATUS_CHARACTERS_LIST);
    }
    else if (ch->chType() == Character::ChType::CHTYPE_NPC)
    {
        NpcCharacter *npc = dynamic_cast<NpcCharacter*>(ch);

        if (npc->isToResurrect())
        {
            NpcToResurrectData data;
            data.resurrectionCounter = QMUD_GLOBAL_NPC_RESURRECTION_COUNTER;
            data.id = npc->staticId();
            data.mapId = npc->mapStaticId();
            data.roomId = npc->roomStaticId();
            m_vNpcToResurrect.push_back(data);
        }

        removeCharacter(ch->id());
    }
    else
    {
        Q_ASSERT(false);

        removeCharacter(ch->id());
    }
}

void World::onRemoveFromTheWorld(Item *item)
{
    removeItem(item->id());
}

void World::onEntityInsertEventCustom(QMUD::IdType id, int seconds, QString event)
{
    ScriptEventCustomData data;
    data.event = event;
    data.entity = id;
    data.counter = (seconds * 1000) / QMUD_GLOBAL_TIMER_UPDATE_ENTITY_SCRIPT_EVENT_TIMEOUT_MS;

    m_vScriptEvent.push_back(data);
}

void World::onGroupDisbanded(Group *group)
{
    m_vGroups.remove(group);

    group->deleteLater();
}

void World::flushPlayers()
{
    for (auto player : m_mapPlayersByConnectionId)
    {
        if (player->pcCharacter())
        {
            player->pcCharacter()->sendRoomContent();
            player->pcCharacter()->sendTargetInfo();
        }
    }
}

void World::checkRoomExit(Map *map)
{
    auto rooms = map->rooms();

    for (auto room : rooms)
    {
        QMUD::StaticIdType instanceExitRoom;
        QMUD::StaticIdType instanceExitMap;

        room->instanceExitRoomStaticId(instanceExitMap, instanceExitRoom);

        if (instanceExitMap != QMUD::StaticIdInvalid &&
                instanceExitRoom != QMUD::StaticIdInvalid)
        {
            Map *exitMap = this->map(instanceExitMap);

            if (exitMap)
            {
                Room* exitRoom = exitMap->room(instanceExitRoom);

                if (exitRoom)
                    room->setInstanceExitRoom(exitRoom);
            }
        }

        auto exits = room->exits();

        for (auto exit : exits)
        {
            if (exit->m_strCode.isEmpty())
            {
                Map *exitMap = this->map(exit->m_iMapStaticId);

                if (!exitMap)
                {
                    GLOBAL_WARNING(tr(" >> [USCITA RIMOSSA] L'uscita %1 della stanza %2:%3 (mappa %4:%5)) non e' valida - Mappa non trovata")
                               .arg(exit->roomExitToReadableString())
                               .arg(room->staticId())
                               .arg(room->name())
                               .arg(map->staticId())
                               .arg(map->name()));

                    room->removeExit(exit->exitType());
                }
                else
                {
                    Room* exitRoom = exitMap->room(exit->m_iRoomStaticId);

                    if (!exitRoom)
                    {
                        GLOBAL_WARNING(tr(" >> [USCITA RIMOSSA] L'uscita %1 della stanza %2:%3 (mappa %4:%5)) non e' valida - Stanza non trovata")
                                   .arg(exit->roomExitToReadableString())
                                   .arg(room->staticId())
                                   .arg(room->name())
                                   .arg(map->staticId())
                                   .arg(map->name()));

                        room->removeExit(exit->exitType());
                    }
                    else
                    {
                        if (exit->exitType() == RoomExit::RoomExitType::EXIT_NORTH)
                            exit->initExit(exitRoom, &exitRoom->exitSouth());
                        else if (exit->exitType() == RoomExit::RoomExitType::EXIT_SOUTH)
                            exit->initExit(exitRoom, &exitRoom->exitNorth());
                        else if (exit->exitType() == RoomExit::RoomExitType::EXIT_WEST)
                            exit->initExit(exitRoom, &exitRoom->exitEast());
                        else if (exit->exitType() == RoomExit::RoomExitType::EXIT_EAST)
                            exit->initExit(exitRoom, &exitRoom->exitWest());
                        else if (exit->exitType() == RoomExit::RoomExitType::EXIT_UP)
                            exit->initExit(exitRoom, &exitRoom->exitDown());
                        else if (exit->exitType() == RoomExit::RoomExitType::EXIT_DOWN)
                            exit->initExit(exitRoom, &exitRoom->exitUp());
                        else
                            Q_ASSERT(false);
                    }
                }
            }
            else
            {
                bool found = false;

                for (auto map : m_mapMaps)
                {
                    auto tmpRooms = map->rooms();

                    for (auto toRoom : tmpRooms)
                    {
                        if (toRoom != room)
                        {
                            auto toRoomExits = toRoom->exits();

                            for (auto toRoomExit : toRoomExits)
                            {
                                if (toRoomExit->code() == exit->code())
                                {
                                    exit->initExit(toRoom, toRoomExit);
                                    found = true;
                                    break;
                                }
                            }
                        }

                        if (found)
                            break;
                    }

                    if (found)
                        break;
                }

                if (!found)
                {
                    GLOBAL_WARNING(tr(" >> [USCITA RIMOSSA] L'uscita %1 della stanza %2:%3 (mappa %4:%5)) non e' valida - Stanza non trovata")
                               .arg(exit->roomExitToReadableString())
                               .arg(room->staticId())
                               .arg(room->name())
                               .arg(map->staticId())
                               .arg(map->name()));

                    room->removeExit(exit->exitType());
                }
            }
        }
    }
}

bool World::createMapsTree()
{
    int groupCounter = 0;

    // Valorizzo il gruppo di ogni stanza
    for (auto map : m_mapMaps)
    {
        int currentGroup = 0;
        auto rooms = map->rooms();

        for (auto room : rooms)
        {
            if (room->group() == -1)
            {
                QVector<Room*> roomsToParse;
                roomsToParse.push_back(room);
                room->setGroup(currentGroup);
                map->setGroupCount(currentGroup + 1);

                while (!roomsToParse.isEmpty())
                {
                    auto currRoom = roomsToParse.front();
                    roomsToParse.pop_front();

                    auto exits = currRoom->exits();

                    for (auto exit : exits)
                    {
                        if (exit->exitRoom()->group() == -1 &&
                            exit->exitRoom()->wildGroup() == room->wildGroup() &&
                            exit->exitRoom()->wildMapIndex() == room->wildMapIndex() &&
                            exit->exitRoom()->map()->staticId() == room->map()->staticId())
                        {
                            roomsToParse.push_back(exit->exitRoom());
                            exit->exitRoom()->setGroup(currentGroup);
                        }
                    }
                }

                currentGroup++;
                groupCounter++;
            }
        }
    }

    GLOBAL_INFO(tr(" >> %1 gruppi identificati").arg(groupCounter));

    if (!m_mapsGraph.init(m_mapMaps.values().toVector()))
        return false;

    return true;
}

bool World::checkPlayerAndCharacterName(QString name)
{
    QRegExp re("[^A-Za-z]");

    if (name.size() < 3 ||
            name.size() > QMUD_GLOBAL_CH_NAME_MAX_SIZE ||
        re.indexIn(name) >= 0)
        return false;

    name = name.simplified().toLower();

    for (auto word : m_setForbiddenPcNames)
        if (name.contains(word))
            return false;

    return true;
}

void World::removeConnection(QMUD::IdType id)
{
    // Cerco la connessione
    auto itConn = m_mapConnections.find(id);

    if (itConn == m_mapConnections.end())
        return;

    // Salvo lo stato attuale del player
    if (itConn.value()->player())
        itConn.value()->player()->forceSave();

    // Rimuovo il player e il character associato
    if (itConn.value()->player() && itConn.value()->player()->pcCharacter())
        removeCharacter(itConn.value()->player()->pcCharacter()->id());

    if (itConn.value()->player())
        itConn.value()->player()->deleteLater();

    m_mapPlayersByConnectionId.remove(id);

    // Rimuovo la connessione
    itConn.value()->deleteLater();
    m_mapConnections.erase(itConn);
}

void World::addPlayer(Player* player)
{
    m_mapPlayersByConnectionId[player->connection()->id()] = player;
}

void World::addCharacter(Character* ch)
{
    Q_ASSERT(!ch->name(Q_NULLPTR).isEmpty());

    m_mapCharacters[ch->id()] = ch;

    connect(ch, SIGNAL(removeFromTheWorld(Character*)), this, SLOT(onRemoveFromTheWorld(Character*)));

    // Aggiungo il character alla stanza corretta
    if (ch->chType() == Character::ChType::CHTYPE_PC)
    {
        Q_ASSERT(!ch->parent());

        PcCharacter* pc = static_cast<PcCharacter*>(ch);

        auto itMap = m_mapMaps.find(pc->lastSessionMapStaticId());

        if (itMap != m_mapMaps.end())
        {
            if (itMap.value()->isInstance() && !itMap.value()->instancePcCharacterIsBind(pc->name(Q_NULLPTR)))
                ch->setParent(m_mapMaps[DEFAULT_MAP_STATIC_ID]->room(DEFAULT_ROOM_STATIC_ID));
            else
            {
                Room *room = itMap.value()->room(pc->lastSessionRoomStaticId());

                if (room)
                    ch->setParent(room);
                else
                    ch->setParent(m_mapMaps[DEFAULT_MAP_STATIC_ID]->room(DEFAULT_ROOM_STATIC_ID));
            }
        }
        else
            ch->setParent(m_mapMaps[DEFAULT_MAP_STATIC_ID]->room(DEFAULT_ROOM_STATIC_ID));

        statisticsFromLastReboot().addConnection(1);
    }
    else if (ch->chType() == Character::ChType::CHTYPE_NPC)
    {
        Q_ASSERT(ch->parent());

        NpcCharacter *npc = dynamic_cast<NpcCharacter*>(ch);

        connect(npc, SIGNAL(command(NpcCharacter*,QString)), this, SLOT(onNpcCommandReceived(NpcCharacter*, QString)), Qt::QueuedConnection);
        connect(npc, SIGNAL(insertEventCustom(QMUD::IdType, int, QString)), this, SLOT(onEntityInsertEventCustom(QMUD::IdType, int, QString)), Qt::QueuedConnection);

        // Assegno le quest
        auto itQuests = m_mapQuestsByNpcStaticId.find(QPair<QMUD::StaticIdType, QMUD::StaticIdType>(npc->mapStaticId(), npc->staticId()));

        if (itQuests != m_mapQuestsByNpcStaticId.end())
            npc->setQuestsBeGived(itQuests.value());
    }
    else
    {
        Q_ASSERT(false);
    }

    ch->initJS(m_jsEngine.newQObject(ch));
}

void World::addItem(Item *item)
{
    Q_ASSERT(!item->name().isEmpty());
    Q_ASSERT(item->parent());

    m_mapItems[item->id()] = item;

    connect(item, SIGNAL(removeFromTheWorld(Item*)), this, SLOT(onRemoveFromTheWorld(Item*)));

    item->initJS(m_jsEngine.newQObject(item));
}

int World::msNextTick() const
{
    return m_timerUpdateCharacters.remainingTime();
}

const QMap<QString, QMUD::PcCharacterBaseInfo> &World::pcCharactersBaseInfo() const
{
    return m_mapPcCharactersBaseInfo;
}

QMap<QMUD::StaticIdType, QMap<QMUD::StaticIdType, QMUD::Recipe> > World::recipes() const
{
    return m_mapRecipes;
}

const QMUD::Recipe &World::recipe(QMUD::StaticIdType mapId, QMUD::StaticIdType id) const
{
    static QMUD::Recipe recipe;

    auto it = m_mapRecipes.find(mapId);

    if (it != m_mapRecipes.end())
    {
        auto it2 = it.value().find(id);

        if (it2 != it.value().end())
            return it2.value();
    }

    return recipe;
}

const GlobalStatistics &World::statistics() const
{
    return m_globalStatistics;
}

GlobalStatistics &World::statisticsFromLastReboot()
{
    return m_globalStatisticsFromLastReboot;
}

QSet<QString> World::forbiddenPcNames() const
{
    return m_setForbiddenPcNames;
}

bool World::addForbiddenName(QString name)
{
    name = name.simplified().toLower();

    if (m_setForbiddenPcNames.contains(name))
        return false;
    else
    {
        m_setForbiddenPcNames.insert(name);

        if (!saveForbiddenPcNames(m_dirRoot.path() + "/forbidden_names.txt"))
            GLOBAL_CRITICAL(tr("Errore durante il salvataggio del file forbidden_names.txt"));

        return true;
    }
}

bool World::removeForbiddenName(QString name)
{
    name = name.simplified().toLower();

    if (!m_setForbiddenPcNames.contains(name))
        return false;
    else
    {
        m_setForbiddenPcNames.remove(name);

        if (!saveForbiddenPcNames(m_dirRoot.path() + "/forbidden_names.txt"))
            GLOBAL_CRITICAL(tr("Errore durante il salvataggio del file forbidden_names.txt"));

        return true;
    }
}

const Quest *World::quest(const Quest::QuestIdType &id) const
{
    auto it = m_mapQuestsById.find(id);

    if (it == m_mapQuestsById.end())
        return Q_NULLPTR;
    else
        return &(it.value());
}

void World::deleteMap(Map *map)
{
    // Sposto fuori tutti i PcCharacter presenti nella mappa.
    // Elimino dalla mappa tutti gli NpcCharacter
    // Elimino tutti gli Item
    auto rooms = map->rooms();

    for (auto room : rooms)
    {
        auto entities = room->entities(Q_NULLPTR);

        for (auto entity : entities)
        {
            if (entity->entityType() == EntityType::ITEM)
            {
                removeItem(entity->id());
            }
            else if (entity->entityType() == EntityType::CHARACTER)
            {
                if (static_cast<Character*>(entity)->chType() == Character::ChType::CHTYPE_PC)
                {
                    entity->setParent(this->map(0)->room(0));
                }
                else
                {
                    removeCharacter(entity->id());
                }
            }
            else
            {
                Q_ASSERT(false);
            }
        }
    }

    // Elimino dal world tutti gli NpcCharacter che appartengono alla mappa.
    QVector<QMUD::IdType> npcToRemove;
    for (auto it = m_mapCharacters.begin(); it != m_mapCharacters.end(); ++it)
    {
        if (it.value()->chType() == Character::ChType::CHTYPE_NPC)
        {
            if (static_cast<NpcCharacter*>(it.value())->mapStaticId() == map->staticId())
            {
                npcToRemove.push_back(it.value()->id());
            }
        }
    }

    for (auto npc : npcToRemove)
        removeCharacter(npc);

    // Elimino i reload degli NPC della mappa
    for (auto it = m_vNpcToResurrect.begin(); it != m_vNpcToResurrect.end(); )
    {
        if (it->mapId == map->staticId())
            it = m_vNpcToResurrect.erase(it);
        else
            ++it;
    }

    // Elimino la mappa
    QString filename = map->filePath();
    delete map;
}

bool World::loadForbiddenPcNames(QString filePath)
{
    if (!QFileInfo(filePath).exists())
        return true;

    QFile fp(filePath);
    fp.open(QFile::ReadOnly | QFile::Text);

    if (!fp.isOpen())
        return false;

    QString str = fp.readAll();

    fp.close();

    auto list = str.split("\n");

    for (auto name : list)
    {
        if (!name.simplified().isEmpty())
            m_setForbiddenPcNames.insert(name.simplified().toLower());
    }

    return true;
}

bool World::saveForbiddenPcNames(QString filePath)
{
    if (!QFileInfo(filePath).exists())
        return true;

    QFile fp(filePath);
    fp.open(QFile::WriteOnly | QFile::Text);

    if (!fp.isOpen())
        return false;

    QTextStream out(&fp);

    for (auto name : m_setForbiddenPcNames)
        out << name << "\n";

    fp.close();

    return true;
}

bool World::initQuests()
{
    // Popolo m_mapQuestsByNpcStaticId rimuovendo le quest non valide
    for (auto itQuest = m_mapQuestsById.begin(); itQuest != m_mapQuestsById.end(); )
    {
        Quest& quest = itQuest.value();

        if (!quest.checkIsValid())
        {
            GLOBAL_WARNING(tr("Quest %1 (%2:%3) rimossa")
                           .arg(quest.name())
                           .arg(quest.mapId())
                           .arg(quest.questId()));
            itQuest = m_mapQuestsById.erase(itQuest);
            continue;
        }

        auto mapId = quest.giveNpcMapId();
        auto npcId = quest.giveNpcId();

        m_mapQuestsByNpcStaticId[QPair<QMUD::StaticIdType, QMUD::StaticIdType>(mapId, npcId)].push_back(&quest);

        ++itQuest;
    }

    for (auto& v : m_mapQuestsByNpcStaticId)
        v.squeeze();

    // Aggiorno le quest associate agli NPC attualmente registrati
    for (auto& ch : m_mapCharacters)
    {
        if (ch->chType() == Character::ChType::CHTYPE_NPC)
        {
            NpcCharacter* npc = static_cast<NpcCharacter*>(ch);

            auto itQuests = m_mapQuestsByNpcStaticId.find(QPair<QMUD::StaticIdType, QMUD::StaticIdType>(npc->mapStaticId(), npc->staticId()));

            if (itQuests != m_mapQuestsByNpcStaticId.end())
                npc->setQuestsBeGived(itQuests.value());
        }
    }

    return true;
}

void World::removeCharacter(QMUD::IdType id)
{
    auto it = m_mapCharacters.find(id);

    if (it != m_mapCharacters.end())
    {
        // Rimuovo dal world gli oggetti dell'inventario
        auto inventory = it.value()->inventory(true);

        for (auto item : inventory)
            removeItem(item->id());

        // Rimuovo dal gruppo
        if (it.value()->chType() == Character::ChType::CHTYPE_PC)
        {
            auto pc = static_cast<PcCharacter*>(it.value());

            if (pc->group())
                pc->group()->removeCharacter(pc);
        }

        // Rimuovo dalle liste interne della room
        it.value()->room()->removeRIPCharacter(it.value());

        // Deleto il character
        it.value()->deleteLater();

        // Rimuovo dal world il character
        m_mapCharacters.erase(it);
    }
}

void World::removeItem(QMUD::IdType id)
{
    auto it = m_mapItems.find(id);

    if (it != m_mapItems.end())
    {
        it.value()->deleteLater();

        m_mapItems.erase(it);
    }
}
