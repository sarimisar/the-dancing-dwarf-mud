/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QJSEngine>

#include "common.h"

// ---------------------------------------------
// Directory
// ---------------------------------------------
#define QMUD_GLOBAL_DIR_ROOT						"data"
#define QMUD_GLOBAL_DIR_PLAYERS						QMUD_GLOBAL_DIR_ROOT "/players"
#define QMUD_GLOBAL_DIR_CHARACTERS					QMUD_GLOBAL_DIR_PLAYERS "/characters"
#define QMUD_GLOBAL_DIR_MAPS						QMUD_GLOBAL_DIR_ROOT "/maps"

// ---------------------------------------------
// Id
// ---------------------------------------------
#define QMUD_GLOBAL_STATIC_ID_ITEM_RESOURCE         42000000
#define QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RESOURCE     0
#define QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM      50000000
#define QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM  0

// ---------------------------------------------
// Timeout timers
// ---------------------------------------------
#define QMUD_GLOBAL_TIMEMS_FROM_SECONDS(__m)                    (__m * 1000)
#define QMUD_GLOBAL_TIMEMS_FROM_MINUTES(__m)                    (QMUD_GLOBAL_TIMEMS_FROM_SECONDS(__m) * 60)
#define QMUD_GLOBAL_TIMEMS_FROM_HOURS(__m)                      (QMUD_GLOBAL_TIMEMS_FROM_MINUTES(__m) * 60)
#define QMUD_GLOBAL_TIMEMS_FROM_DAYS(__m)                       (QMUD_GLOBAL_TIMEMS_FROM_HOURS(__m) * 24)
#define QMUD_GLOBAL_TIMEMS_FROM_TURN(__m)                       (__m * QMUD_GLOBAL_TIMEMS_FROM_SECONDS(5))

#define QMUD_GLOBAL_TURNS_FROM_MINUTES(__m)                     ((__m * 60 * 1000)/QMUD_GLOBAL_TIMEMS_FROM_TURN(1))
#define QMUD_GLOBAL_TURNS_FROM_SECONDS(__s)                     ((__s * 1000)/QMUD_GLOBAL_TIMEMS_FROM_TURN(1))
#define QMUD_GLOBAL_MINUTES_FROM_TURNS(__t)                     (qCeil(QMUD_GLOBAL_TIMEMS_FROM_TURN(__t)/1000.0/60.0))

#define QMUD_GLOBAL_NPC_RESURRECTION_COUNTER                    18

#define QMUD_GLOBAL_TIMER_UPDATE_PLAYERS_TIMEOUT_MS             QMUD_GLOBAL_TIMEMS_FROM_MINUTES(10)
#define QMUD_GLOBAL_TIMER_UPDATE_CHARACTERS_TIMEOUT_MS          QMUD_GLOBAL_TIMEMS_FROM_TURN(1)
#define QMUD_GLOBAL_TIMER_UPDATE_CHARACTERS_STATUS_TIMEOUT_MS   QMUD_GLOBAL_TIMEMS_FROM_TURN(1)
#define QMUD_GLOBAL_TIMER_UPDATE_NPC_RESURRECTION_MS            QMUD_GLOBAL_TIMEMS_FROM_SECONDS(10)
#define QMUD_GLOBAL_TIMER_UPDATE_ENTITY_SCRIPT_EVENT_TIMEOUT_MS QMUD_GLOBAL_TIMEMS_FROM_SECONDS(1)
#define QMUD_GLOBAL_TIMER_UPDATE_MAPS_TIMEOUT_MS                QMUD_GLOBAL_TIMEMS_FROM_SECONDS(30)
#define QMUD_GLOBAL_TIMER_UPDATE_ITEMS_TIMEOUT_MS               QMUD_GLOBAL_TIMEMS_FROM_MINUTES(1)
#define QMUD_GLOBAL_TIMER_REMOVE_INACTIVE_INSTANCE_MS           QMUD_GLOBAL_TIMEMS_FROM_MINUTES(2)
#define QMUD_GLOBAL_TIMER_UPDATE_WORLD_TIMEOUT_MS               QMUD_GLOBAL_TIMEMS_FROM_MINUTES(5)

#define QMUD_GLOBAL_ITEM_AUTO_DESTROY_COUNTER                   10
#define QMUD_GLOBAL_ITEM_RESOURCE_RESET_SEC                     (60 * 10)

#define QMUD_GLOBAL_ITEM_POTION_WAIT_TIME_ROUNDS                2

// ---------------------------------------------
// Personaggi
// ---------------------------------------------
#define QMUD_GLOBAL_CH_MAX_LEVEL                                100
#define QMUD_GLOBAL_CH_NUMBER_OF_STATISTICS_POINT               26
#define QMUD_GLOBAL_CH_MAX_LOOT_SIZE                            25
#define QMUD_GLOBAL_CH_MAX_NUMBER_OF_ITEMS_IN_INVENTORY         100
#define QMUD_GLOBAL_CH_NPC_MAX_HUNTING_FAILED_TRY               5
#define QMUD_GLOBAL_CH_NPC_MAX_HUNTING_SUCCESSED_TRY            20
#define QMUD_GLOBAL_CH_NPC_HUNTING_DISTANCE                     3000
#define QMUD_GLOBAL_CH_NAME_MAX_SIZE                            15

// ---------------------------------------------
// Stringhe
// ---------------------------------------------
#define QMUD_GLOBAL_NEW_LINE                        "\n\r"
#define QMUD_GLOBAL_STR_DIV							"&G--------------------------------------------------------&d"

//#define QMUD_GLOBAL_STR_WELLCOME_MESSAGE            "&G--------------------------------------------------------&d" QMUD_GLOBAL_NEW_LINE

#define QMUD_GLOBAL_STR_WELLCOME_MESSAGE            "&G--------------------------------------------------------" QMUD_GLOBAL_NEW_LINE \
                                                    "&YBenvenuto!" QMUD_GLOBAL_NEW_LINE \
                                                    QMUD_GLOBAL_NEW_LINE \
                                                    "                      &W/^--^\\     /^--^\\     /^--^\\                       " QMUD_GLOBAL_NEW_LINE \
                                                    "                      \\____/     \\____/     \\____/                       " QMUD_GLOBAL_NEW_LINE \
                                                    "                     /      \\   /      \\   /      \\                      " QMUD_GLOBAL_NEW_LINE \
                                                    "                    |        | |        | |        |                     " QMUD_GLOBAL_NEW_LINE \
                                                    "                     \\__  __/   \\__  __/   \\__  __/                      " QMUD_GLOBAL_NEW_LINE \
                                                    "&O|^|^|^|^|^|^|^|^|^|^|^|^&W\\ \\&O^|^|^|^&W/ /&O^|^|^|^|^&W\\ \\&O^|^|^|^|^|^|^|^|^|^|^|^|" QMUD_GLOBAL_NEW_LINE \
                                                    "| | | | | | | | | | | | |&W\\ \\&O| | |&W/ /&O| | | | | |&W\\ \\&O| | | | | | | | | | | |" QMUD_GLOBAL_NEW_LINE \
                                                    "########################&W/ /&O######&W\\ \\&O###########&W/ /&O#######################" QMUD_GLOBAL_NEW_LINE \
                                                    "| | | | | | | | | | | | &W\\/&O| | | | &W\\/&O| | | | | |&W\\/&O | | | | | | | | | | | |" QMUD_GLOBAL_NEW_LINE \
                                                    "|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|" QMUD_GLOBAL_NEW_LINE \
                                                    QMUD_GLOBAL_NEW_LINE \
                                                    "&YQuesto e' un MUD basato su QT5 in via di sviluppo" QMUD_GLOBAL_NEW_LINE \
                                                    "&G--------------------------------------------------------&d" QMUD_GLOBAL_NEW_LINE


#define QMUD_GLOBAL_STR_INSERT_USERNAME				"Inserisci 'nuovo' per creare un nuovo utente." QMUD_GLOBAL_NEW_LINE \
                                                    "Inserisci il nome utente:"
#define QMUD_GLOBAL_STR_INSERT_PASSWORD				"Inserisci la password:"
#define QMUD_GLOBAL_STR_INSERT_CHARACTER_NAME		"Inserisci 'nuovo' per creare un nuovo personaggio." QMUD_GLOBAL_NEW_LINE \
                                                    "Inserisci il nome del personaggio:"
#define QMUD_GLOBAL_STR_INSERT_NEW_CHARACTER_NAME	"Inserisci il nome del personaggio oppure 'B' per tornare indietro:"
#define QMUD_GLOBAL_STR_INSERT_NEW_USERNAME			"Inserisci il nome utente:"
#define QMUD_GLOBAL_STR_INSERT_SEX                  "Scegli il sesso del tuo personaggio, (M)aschio o (F)emmina:"
#define QMUD_GLOBAL_STR_RACES_LIST                  QMUD_GLOBAL_STR_DIV QMUD_GLOBAL_NEW_LINE "&wScegli una delle razze disponibili" QMUD_GLOBAL_NEW_LINE \
                                                    "1 - Umano         4 - Elfo alto" QMUD_GLOBAL_NEW_LINE \
                                                    "2 - Nano          5 - Mezz'elfo" QMUD_GLOBAL_NEW_LINE \
                                                    "3 - Gnomo         6 - Mezz'orco" QMUD_GLOBAL_NEW_LINE \
                                                    "Inserisci il numero corrispondente alla razza oppure 'B' per tornare indietro:"
#define QMUD_GLOBAL_STR_CLASSES_LIST                QMUD_GLOBAL_STR_DIV QMUD_GLOBAL_NEW_LINE "Scegli una delle classi disponibili" QMUD_GLOBAL_NEW_LINE \
                                                    "1 - Guerriero" QMUD_GLOBAL_NEW_LINE \
                                                    "2 - Mago" QMUD_GLOBAL_NEW_LINE \
                                                    "3 - Barbaro" QMUD_GLOBAL_NEW_LINE \
                                                    "4 - Druido" QMUD_GLOBAL_NEW_LINE \
                                                    "5 - Monaco" QMUD_GLOBAL_NEW_LINE \
                                                    "6 - Prete" QMUD_GLOBAL_NEW_LINE \
                                                    "7 - Ladro" QMUD_GLOBAL_NEW_LINE \
                                                    "Inserisci il numero corrispondente alla classe oppure 'B' per tornare indietro:"
#define QMUD_GLOBAL_STR_INSERT_STATISTICS(player)   QMUD_GLOBAL_STR_DIV QMUD_GLOBAL_NEW_LINE + QString("(I)nt:%1, (S)ag:%2, (F)or:%3, (C)os:%4, (D)ex:%5") \
                                                    .arg(player->property("newCharacterInt").toInt()) \
                                                    .arg(player->property("newCharacterWis").toInt()) \
                                                    .arg(player->property("newCharacterStr").toInt()) \
                                                    .arg(player->property("newCharacterCon").toInt()) \
                                                    .arg(player->property("newCharacterDex").toInt()) + QMUD_GLOBAL_NEW_LINE + \
                                                    QString("Ti restano %1 punti da assegnare").arg(player->property("newCharacterStatPoints").toInt()) + QMUD_GLOBAL_NEW_LINE \
                                                    "Inserisci la statistica da aumentare seguita dai punti da assegnare" + QMUD_GLOBAL_NEW_LINE + \
                                                    "oppure 'E' per terminare, 'A' per annullare o 'B' per tornare indietro:"
#define QMUD_GLOBAL_STR_COMMAND_NOT_VALID			"Cosa?"
#define QMUD_GLOBAL_STR_SPELL_FAILED_STUNNED        "&RNon riesci a recitare correttamente la formula magica e fallisci l'incantesimo!&d"


// ---------------------------------------------
// Gestione character
// ---------------------------------------------
#define QMUD_GLOBAL_CH_MIN_CURRENT_HP           (-10)
#define QMUD_GLOBAL_CH_MIN_MAXIMUM_HP           (1)
#define QMUD_GLOBAL_CH_MIN_CURRENT_AP           (0)
#define QMUD_GLOBAL_CH_MIN_MAXIMUM_AP           (0)
#define QMUD_GLOBAL_CH_MIN_CURRENT_MP           (0)
#define QMUD_GLOBAL_CH_MIN_MAXIMUM_MP           (0)


namespace CORE
{
    class World;
    class Map;
}

namespace QMUD
{

// ---------------------------------------------
// Puntatore globale world
// ---------------------------------------------
extern ::CORE::World* qmudWorld;

// ---------------------------------------------
// Eventi
// ---------------------------------------------
enum class EventType
{
    ENTER,
    CUSTOM,
    ABILITY
};

QString eventTypeToString(EventType event);

// Crafting
struct Recipe
{
    Recipe();

    bool isValid() const;

    QMUD::StaticIdType id() const;
    QMUD::StaticIdType mapId() const;
    QMUD::StaticIdType itemId() const;
    ResourceType resource() const;
    QString name() const;
    QString description() const;
    QString action() const;
    ProfessionType profession() const;
    int level() const;
    QMap<QMUD::ResourceType, qint64> resources() const;
    qint64 price() const;

    bool read(const QJsonObject& obj, StaticIdType mapId);

private:
    StaticIdType m_iId;
    StaticIdType m_iItemId;
    StaticIdType m_iMapId;
    ResourceType m_eResource;
    QString m_strName;
    QString m_strDescription;
    QString m_strAction;
    ProfessionType m_eProfession;
    int m_iLevel;
    QMap<QMUD::ResourceType, qint64> m_mapResources;
    qint64 m_iPrice;
};

// ---------------------------------------------
// Comunicazione
// ---------------------------------------------
enum class SayType
{
    CHARACTER,
    GROUP,
    ROOM,
    AREA
};

// ---------------------------------------------
// Utility
// ---------------------------------------------
int rollDice(int numdice, int die);
qint64 rollDice(int numdice, qint64 die);
QString coinsToReadableString(qint64 coppers);
qint64 experienceByLevelBase(int level);
QString capitalize(QString str);
QString minutesToString(int minutes);

// ---------------------------------------------
// Wilderness
// ---------------------------------------------
#define QMUD_WILD_DESCRIPTION_WIDTH_RADIUS     8
#define QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS    8
#define QMUD_WILD_DESCRIPTION_WIDTH            (QMUD_WILD_DESCRIPTION_WIDTH_RADIUS * 2 + 1)
#define QMUD_WILD_DESCRIPTION_HEIGHT           (QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS * 2 + 1)
#define QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL    8
#define QMUD_WILD_PATROL_HEIGHT_RADIUS_SMALL   4
#define QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE    16
#define QMUD_WILD_PATROL_HEIGHT_RADIUS_LARGE   8
#define QMUD_WILD_ROOM_BASE_INDEX              1000000
#define QMUD_WILD_ROOM_RESERVE_INDEX_FOR_MAP   (100*100)

extern bool WildDarkKernel[QMUD_WILD_DESCRIPTION_HEIGHT][QMUD_WILD_DESCRIPTION_WIDTH];
extern bool WildAlmostDarkKernel[QMUD_WILD_DESCRIPTION_HEIGHT][QMUD_WILD_DESCRIPTION_WIDTH];

// ---------------------------------------------
// Instance
// ---------------------------------------------
#define QMUD_INSTANCE_FIRST_STATIC_ID           100000

// ---------------------------------------------
// Gestione file
// ---------------------------------------------
enum class FileOpStatus
{
    FILE_OP_OK,
    FILE_OP_ERROR_INVALID_NAME,
    FILE_OP_ERROR_CORRUPTED_FILE,
    FILE_OP_ERROR_FILE_ALREADY_EXISTS,
    FILE_OP_ERROR_OPEN_FILE,
    FILE_OP_ERROR_GENERIC
};

bool fileJsonRead(const QJsonObject &obj, QString key, QString& value,
                         bool canBeEmpty = false, bool optional = false, QString defaultValue = "");
bool fileJsonRead(const QJsonObject &obj, QString key, int& value,
                         bool optional = false, int defaultValue = 0);
bool fileJsonRead(const QJsonObject &obj, QString key, bool& value,
                         bool optional = false, bool defaultValue = 0);
bool fileJsonRead(const QJsonObject &obj, QString key, double& value,
                         bool optional = false, double defaultValue = 0.0);
bool fileJsonReadStaticId(const QJsonObject &obj, QString key, StaticIdType& value,
                         bool optional = false, StaticIdType defaultValue = StaticIdInvalid);
bool fileJsonRead(const QJsonObject &obj, QString key, QStringList& value,
                         bool simplified, bool lower, bool strCanBeEmpty = false, bool optional = false);
}

Q_DECLARE_METATYPE(QMUD::AbilityType)
Q_DECLARE_METATYPE(QMUD::SayType)
Q_DECLARE_METATYPE(QMUD::ActionType)

#endif // GLOBAL_H
