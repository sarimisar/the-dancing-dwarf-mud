/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef COMMON_H
#define COMMON_H

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <QString>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>
#include <QQmlEngine>
#include <QHash>

#define QMUD_GLOBAL_STATUS_DURATION_INFINITE                    (INT_MAX)


namespace QMUD
{

enum class TemporaryStatusType;

// ---------------------------------------------
// Tipi Id
// ---------------------------------------------
typedef unsigned long long int IdType;
static const IdType IdInvalid = ULLONG_MAX;
typedef unsigned int StaticIdType;
static const StaticIdType StaticIdInvalid = UINT_MAX;

// ---------------------------------------------
// Oggetti
// ---------------------------------------------
enum class ItemType
{
    UNKNOWN = -1,
    BASE,
    WEAPON,
    ARMOR,
    TRINKET,
    FOOD,
    DRINK,
    POTION,
    TONIC,
    RESOURCE,
    TOTEM,
    SYMBOL
};

QString itemTypeToString(ItemType type);
ItemType itemTypeFromString(QString str);
QString itemTypeToReadableString(ItemType type);

enum class ItemClassType
{
    UNKNOWN = -1,
    CASTER_WIS,
    CASTER_INT,
    DPS_DEX,
    DPS_STR,
    TANK
};

QString itemClassToString(ItemClassType type);
ItemClassType itemClassFromString(QString str);

enum class ItemRarityType
{
    UNKNOWN = -1,
    RARITY0,    // Si trova sempre
    RARITY1,    // Si trova una volta su 2
    RARITY2,    // Si trova una volta su 5
    RARITY3,    // Si trova una volta su 10
    RARITY4,    // Si trova una volta su 20
    RARITY5,    // Si trova una volta su 40
    RARITY6,    // Si trova una volta su 80
    RARITY7,    // Si trova una volta su 200
    RARITY8,    // Si trova una volta su 500
    RARITY9,    // Si trova una volta su 1000
    RARITY10    // Si trova una volta su 10000
};

QString itemRarityToString(ItemRarityType rarity);
ItemRarityType itemRarityFromString(QString str);
bool itemRarityCheck(ItemRarityType rarity);

enum class ItemQualityType
{
    JUNK,
    COMMON,
    UNCOMMON,
    VALUABLE,
    RARE,
    EPIC,
    LEGENDARY
};

QString itemQualityToString(ItemQualityType quality);
ItemQualityType itemQualityFromString(QString str);
QString itemQualityToReadableString(QMUD::ItemQualityType quality, bool withColor);
QString itemQualityToColor(QMUD::ItemQualityType quality);
QString itemQualityToHTMLColor(ItemQualityType quality);

// ---------------------------------------------
// Armature
// ---------------------------------------------
enum class ArmorType
{
    UNKNOWN = -1,
    SHIELD,                 // Scudo
    CHEST,                  // Armatura
    GLOVES,                 // Guanti
    SHOULDER,               // Spalline
    BRACERS,                // Bracciali
    HELM,                   // Elmo
    LEG,                    // Gambali
    BOOTS,                  // Stivali

    MAX_VALUE
};

QString armorTypeToString(ArmorType type);
ArmorType armorTypeFromString(QString str);
QString armorTypeToReadableString(ArmorType type);

// ---------------------------------------------
// Trinket
// ---------------------------------------------
enum class TrinketType
{
    UNKNOWN = -1,
    RING,
    NECKLACE,
    BRACELET,

    MAX_VALUE
};

QString trinketTypeToString(TrinketType type);
QString trinketTypeToReadableString(TrinketType type);
TrinketType trinketTypeFromString(QString str);

// ---------------------------------------------
// Armi
// ---------------------------------------------
enum class DamageType
{
    HANDS,           // Mani nude
    SLASHING,        // Taglio
    BLUDGEONING,     // Impatto
    PIERCING,        // Perforazione
    ENERGY,          // Energia
    FIRE,            // Fuoco
    DIVINE,          // Divino
    ICE,             // Ghiaccio
};

QString damageTypeToString(DamageType type);
void damageTypeJsRegistration(QQmlEngine &engine);

enum class WeaponType
{
    UNKNOWN = -1,
    CLUB,           // Clava
    DAGGER,         // Pugnale
    TWO_HAND_CLUB,  // Clava a due mani
    AXE,            // Ascia
    LIGHT_HAMMER,   // Martello
    MACE,           // Mazza
    QUARTERSTAFF,   // Bastone
    SICKLE,         // Falce
    TWO_HAND_AXE,   // Ascia a due mani
    HALBERD,        // Alabarda
    LANCE,          // Lancia
    SHORT_SWORD,    // Spada corta
    SWORD,          // Spada
    LONG_SWORD,     // Spada lunga
    MORNINGSTAR,    // Morningstar
    PIKE,           // Picca
    RAPIER,         // Stocco
    SCIMITAR,       // Scimitarra
    TRIDENT,        // Tridente
    WAR_HAMMER,     // Martello da guerra
    KATANA,         // Katana

    MAX_VALUE
};

QString weaponTypeToString(WeaponType type);
WeaponType weaponTypeFromString(QString str);
QString weaponTypeToReadableString(WeaponType type);
double weaponTypeToDamageMultiplier(WeaponType type);
double weaponTypeToSpeedMultiplier(WeaponType type);
DamageType weaponTypeToDamageType(WeaponType type);

// ---------------------------------------------
// Bonus degli oggetti
// ---------------------------------------------
class ItemBaseBonus
{
    friend class ItemArmor;
    friend class ItemTrinket;
    friend class ItemWeapon;

public:
    ItemBaseBonus();

    bool isValid() const;

    int constitution() const;
    int dexterity() const;
    int intelligence() const;
    int wisdom() const;
    int charisma() const;
    int strength() const;
    int hit() const;
    int damage() const;
    int hitPoints() const;
    int manaPoints() const;
    int actionPoints() const;

    void setConstitution(int value);
    void setDexterity(int value);
    void setIntelligence(int value);
    void setWisdom(int value);
    void setCharisma(int value);
    void setStrength(int value);
    void setHit(int value);
    void setDamage(int value);
    void setHitPoints(int value);
    void setManaPoints(int value);
    void setActionPoints(int value);

    bool load(const QJsonObject &obj);
    void write(QJsonObject &obj) const;

private:
    int m_iConstitution;
    int m_iDexterity;
    int m_iIntelligence;
    int m_iWisdom;
    int m_iCharisma;
    int m_iStrength;
    int m_iHit;
    int m_iDamage;
    int m_iHitPoints;
    int m_iManaPoints;
    int m_iActionPoints;
};

// ---------------------------------------------
// Modifiche di stato degli oggetti
// ---------------------------------------------
class ItemBaseStatus
{
public:
    ItemBaseStatus();

    bool isValid() const;

    QVector<QMUD::TemporaryStatusType> temporaryStatus() const;
    void setTemporaryStatus(QVector<QMUD::TemporaryStatusType> status);

    bool load(const QJsonObject &obj);
    void write(QJsonObject &obj) const;

private:
    QVector<QMUD::TemporaryStatusType> m_vTemporaryStatus;
};

// ---------------------------------------------
// Incantesimi e abilita'
// ---------------------------------------------
enum class AbilityType
{
    UNKNOWN = 0,
    CHARGE,
    KICK,
    BACKSTAB,
    PARRY,
    DODGE,

    MAX_VALUE
};

QString abilityToReadableString(AbilityType ability);

enum class SpellType
{
    UNKNOWN = 0,             // None
    MAGIC_MISSILE,        // Dardo incantato
    CURE,                 // Cura
    SHIELD,               // Scudo
    STRENGHT,             // Forza
    SANCTIFY,             // Santificazione
    FIREBALL,             // Palla di fuoco
    BURNING_HANDS,        // Mani brucianti
    DIVINE_SHIELD,        // Scudo divino
    INFLICTING_WOUNDS,    // Infliggi ferite
    FLY,                  // Volo
    INVISIBILITY,         // Invisibilita
    SEE_INVISIBILITY,     // Vedi invisibile
    FIRE_SHIELD,          // Scudo di fuoco
    ICE_SHIELD,           // Scudo di ghiaccio
    BLADES_SHIELD,        // Scudo di lame
    FIERY_ARROW,          // Freccia infuocata
    ICE_STORM,            // Tempesta di ghiaccio
    ILLUSORY_IMAGES,      // Immagini illusorie

    MAX_VALUE
};

QString spellToReadableString(SpellType spell);
SpellType spellFromString(QString str);

// ---------------------------------------------
// PC e NPC
// ---------------------------------------------
enum class NpcCharacterFlagType
{
    UNKNOWN = -1,
    AUTOIA_COMBAT,
    AGGRESSIVE,
    PATROL,
    PATROL_WILD_ZONE_SMALL,
    PATROL_WILD_ZONE_LARGE,
    HUNT_MAP,
    ASSIST
};

NpcCharacterFlagType npcCharacterFlagTypeFromString(QString str);
QString npcCharacterFlagTypeToString(QMUD::NpcCharacterFlagType flag);

inline uint qHash(NpcCharacterFlagType val, uint seed = 0)
{
    return ::qHash(static_cast<int>(val), seed);
}

enum class NpcType
{
    UNKNOWN = -1,
    NORMAL,
    ELITE,
    RARE,
    LEGENDARY,
    BOSS_A,
    BOSS_B,

    MAX_VALUE
};

NpcType npcTypeFromString(QString str);
QString npcTypeToString(NpcType type);
int npcTypeToLootPercentage(NpcType type);

enum class SexType
{
    UNKNOWN,
    MALE,
    FEMALE
};

QString sexTypeToString(SexType type);
SexType sexTypeFromString(QString str);
void sexTypeJsRegistration(QQmlEngine &engine);

enum class RaceType
{
    RACE_FIRST  = 0,

    // Razze PC e NPC
    HUMAN       = 0,        // Umano
    DWARF   ,               // Nano
    GNOME   ,               // Gnomo
    HALF_ORC,               // Mezz'orco
    HIGH_ELF,               // Elfo alto
    HALF_ELF,               // Mezz'elfo

    // Razze NPC
    ACQUATIC_BEAST,         // Bestia acquatica
    DROW,                   // Drow
    SWARM,                  // Sciame
    ABERRATION,             // Aberrazione
    BEAST,                  // Bestia
    CONSTRUCT,              // Costrutto
    DRAGON,                 // Drago
    DRAGON_AIR,             // Drago Aria
    DRAGON_ACQUATIC,        // Drago Acquatico
    DRAGON_CHAOS,           // Drago Chaos
    DRAGON_COLD,            // Drago Freddo
    DRAGON_EARTH,           // Drago Terra
    DRAGON_ELECTRICITY,     // Drago Elettricità
    DRAGON_FIRE,            // Drago Fuoco
    DRAGON_FORCE,           // Drago Forza
    DRAGON_LIGHT,           // Drago Luce
    DRAGON_SHADOW,          // Drago Ombra
    DRAGON_WATER,           // Drago Acqua
    ELEMENTAL_AIR,          // Elementale Aria
    ELEMENTAL_EARTH,        // Elementale Terra
    ELEMENTAL_FIRE,         // Elementale Fuoco
    ELEMENTAL_WATER,        // Elementale Acqua
    ELEMENTAL_COLD,         // Elementale del ghiaccio
    FEY,                    // Fata
    GIANT,                  // Gigante
    GIANT_AIR,              // Gigante dell'aria
    GIANT_EARTH,            // Gigante della terra
    GIANT_FIRE,             // Gigante del fuoco
    GIANT_COLD,             // Gigante dei ghiacci
    GNOLL,                  // Gnoll
    GOBLINOID,              // Goblinoide
    MINOTAUR,               // Minotauro
    ORC,                    // Orco
    MAGICAL_BEAST,          // Bestia magica
    OOZE,                   // Melma
    OOZE_AIR,               // Melma d'aria
    OOZE_EARTH,             // Melma di terra
    OOZE_FIRE,              // Melma di fuoco
    OOZE_WATER,             // Melma d'acqua
    PLANT,                  // Pianta
    UNDEAD,                 // Non morto
    LICH,                   // Lich
    VERMIN,                 // Verme
    VERMIN_AIR,             // Verme d'aria
    VERMIN_EARTH,           // Verme di terra
    VERMIN_FIRE,            // Verme di fuoco
    VERMIN_WATER,           // Verme d'acqua
    VERMIN_COLD,            // Verme di ghiaccio

    // Animali,
    BALEEN,                 // Balena
    BLACK_BEAR,             // Orso
    HAWK,                   // Falco
    BROWN_BEAR,             // Orso bruno
    BUFFALO,                // Bufalo
    CAT,                    // Gatto
    CATTLE,                 // Bestiame
    SQUIRREL,               // Scoiattolo
    BAT,                    // Pipistrello
    DEER,                   // Renna
    DOG,                    // Cane
    DOLPHIN,                // Delfino
    FISH,                   // Pesce
    EAGLE,                  // Acquila
    ELEPHANT,               // Elefante
    FOX,                    // Volpe
    GOAT,                   // Capra
    GULLION,                // Gabbiano
    HORSE,                  // Cavallo
    JACKAL,                 // Sciacallo
    MONKEY,                 // Scimmia
    MOLE,                   // Talpa
    MOUSE,                  // Topo
    OWL,                    // Gufo
    PENGUIN,                // Pinguino
    PIRANHA,                // Piragna
    FROG,                   // Rana
    POLAR_BEAR,             // Orso polare
    RAT,                    // Ratto
    RAVEN,                  // Corvo
    SEAL,                   // Foca
    SHARK,                  // Squalo
    SHEEP,                  // Pecora
    SERPENT,                // Serpente
    SPIDER,                 // Ragno
    GIANT_SPIDER,           // Ragno gigante
    WOLF,                   // Lupo
    BOAR,                   // Cinghiale
    ANT,                    // Formica
    GIANT_ANT,              // Formica gigante
    KOBOLD,                 // Coboldo

    RACE_TYPE_MAX,
    RACE_TYPE_PC_MAX = HALF_ELF + 1,
};

void raceTypeJsRegistration(QQmlEngine &engine);

class Race
{
public:
    static RaceType fromString(QString str, bool* isOk = Q_NULLPTR);
    static QString toString(RaceType race);
    static QString toReadableString(RaceType race);
    static int hitDiceBonus(RaceType race);
    static int manaDiceBonus(RaceType race);
    static int basePCInt(RaceType race);
    static int basePCWis(RaceType race);
    static int basePCCon(RaceType race);
    static int basePCStr(RaceType race);
    static int basePCDex(RaceType race);
    static int baseNPCInt(RaceType race);
    static int baseNPCWis(RaceType race);
    static int baseNPCCon(RaceType race);
    static int baseNPCStr(RaceType race);
    static int baseNPCDex(RaceType race);
    static DamageType baseDamageType(RaceType race);

private:
    Race();
};

// Tipo classi base
enum class ClassType
{
    UNKNOWN = -1,
    WARRIOR,
    MAGE,
    ROGUE,
    PRIEST,
    DRUID,
    BARBARIAN,
    MONK,

    MAX_VALUE
};

QString classTypeToString(QMUD::ClassType type);
QMUD::ClassType classTypeFromString(QString str);
QString classTypeToReadableString(ClassType type);
bool classTypeIsCaster(ClassType type);
void classTypeJsRegistration(QQmlEngine &engine);
ItemClassType itemClassTypeFromChClass(ClassType classType);
void classTypeToMaxStatistics(ClassType classType, int& maxInt, int& maxWis, int& maxStr, int& maxCon, int& maxDex);

// Non riordinare! Usato in Json
enum class TemporaryStatusType
{
    UNKNOWN = -1,
    ACTIONLAG,
    STUNNED,
    BLINDED,
    SANCTIFY,            // Incantesimo santificazione
    SHIELD,              // Incantesimo scudo
    STRENGHT,            // Incantesimo forza
    DIVINE_SHIELD,       // Incantesimo scudo divino
    FLY,                 // Incantesimo volo
    INVISIBILITY,        // Invisibilita
    SEE_INVISIBILITY,    // Vedi invisibile
    FIRE_SHIELD,         // Scudo di fuoco
    ICE_SHIELD,          // Scudo di ghiaccio
    BLADES_SHIELD,       // Scudo di lame
    ILLUSORY_IMAGES_1,   // Immagini illusorie
    ILLUSORY_IMAGES_2,
    ILLUSORY_IMAGES_3,
    ILLUSORY_IMAGES_4,
    ILLUSORY_IMAGES_5
};

QString statusToReadableString(TemporaryStatusType status);
QString statusToString(TemporaryStatusType status);
TemporaryStatusType statusFromString(QString status, bool &ok);

struct PcCharacterBaseInfo {
    PcCharacterBaseInfo();
    PcCharacterBaseInfo(QString name, QMUD::RaceType race, QMUD::ClassType classType, int level);
    bool operator==(const PcCharacterBaseInfo& other) const;

    void serialize(QJsonObject& obj) const;
    void deserialize(const QJsonObject& obj);

    QString name() const;
    QMUD::RaceType race() const;
    QMUD::ClassType classType() const;
    int level() const;

private:
    QString m_strName;
    QMUD::RaceType m_eRace;
    QMUD::ClassType m_eClassType;
    int m_iLevel;
};

// ---------------------------------------------
// Room
// ---------------------------------------------
enum class RoomFlagType
{
    UNKNOWN,
    PACE,
    DIFFICULT,
    IMPERVIOUS,
    IMPOSSIBLE,
    WATER,
    NO_RESOURCES,
    OUTSIDE
};

QString roomFlagToString(RoomFlagType flag);
RoomFlagType roomFlagFromString(QString str);

// ---------------------------------------------
// Risorse e professioni
// ---------------------------------------------
enum class ResourceType
{
    UNKNOWN = -1,

    COINS,
    LEATHER,
    IRON_BAR,
    TANNED_LEATHER,
    RAW_MEAT,
    SCALE,

    // -------------------------------------------
    // Erbalismo
    // -------------------------------------------
    // Livello 1
    WOOD,
    GARLIC,        // Aglio
    MARIGOLD,      // Calendula
    HYSSOP,        // Issopo
    PRIMULA,       // Primula
    RED_CURRANT,   // Ribes rosso
    GOOSEBERRY,    // Uva spina
    GINGER,        // Ginger
    LAVENDER,      // Lavanda
    POPPY,         // Papavero
    BLACK_CURRANT, // Ribes nero
    TILIA,         // Tiglio
    HOP,           // Luppolo

    // Livello 2
    WITCH_HAZEL,   // Amamelide
    SAFFLOWER,     // Cartamo
    GINKGO,        // Ginkgo
    VERBENA,       // Verbena
    ANGELICA,      // Angelica
    ANISE,         // Anice
    EDELWEISS,     // Stella alpina

    // Luvello 3
    HELICHRYSUM,   // Elicriso
    HOLLY,         // Pungitopo
    ILEX,          // Agrifoglio
    ELDER,         // Sambuco
    FOXGLOVE,      // Digitalis purpurea

    // Livello 4
    AGAVE,         // Agave
    AMBROSIA,      // Ambrosia
    SUNDEW,        // Drosera
    HAWTHORN,      // Biancospino
    LOTUS,         // Fior di loto
    MORINGA,       // Moringa

    // Livello 5
    ALCHEMILLA,    // Alchemilla
    ROBINIA,       // Robinia
    ESCOLZIA,      // Escolzia
    NETTLE,        // Ortica
    STRAMONIUM,    // Stramonio

    // -------------------------------------------
    // Mining
    // -------------------------------------------
    // Livello 1
    COPPER,         // Rame
    TIN,            // Stagno
    IRON,           // Ferro

    // Livello 2
    ZINC,           // Zinco (Calamina)
    ANTIMONY,       // Antimonio (Ammon)

    // Luvello 3
    ALUMINIUM,      // Alluminio
    CHROME,         // Cromo (Piombo Rosso)
    LEAD,           // Piombo
    CADMIUM,        // Cadmio (Argento nero)
    SILVER,         // Argento

    // Livello 4
    NICKEL,         // Nichel (Rame bianco)
    BISMUTH,        // Bismuto (Argento alchemico)
    GOLD,           // Oro
    TITANIUM,       // Titanio

    // Livello 5
    PLATINUM,       // Platino
    IRIDIUM,        // Iridio (Iride)
    RHODIUM,        // Rodio (Oro alchemico)
    OSMIUM,         // Osmio (Osmiridio)

    // -------------------------------------------
    // Mining leghe
    // -------------------------------------------
    // Livello 1
    BRONZE,         // Bronzo -> Rame + Stagno

    // Livello 2
    BRASS,          // Ottone -> Rame + Zinco
    PEWTER,         // Peltro -> Stagno + Rame + Antimonio

    // Luvello 3
    DURALLUMINIUM,  // Duralluminio -> Alluminio + Rame
    ALPACCA,        // Alpacca (Argentite) -> Rame + Zinco + Nichel

    // Livello 4
    NICROSIL,       // Nicrosil -> Cromo + Nichel
    CORROBEND,      // Corrobend -> Bismuto + Piombo + Stagno + Cadmio

    // Livello 5
    ELECTRUM,       // Electrum -> Oro + Argento
    PLATINUM_MAX,   // Diamantite -> Platino + Oro
    LUCENS,         // Lucens -> Platino + Iride + Oro alchemico
    ALTIUM,         // Altium -> Platino + Osmiridio


    // -------------------------------------------
    // Essenze
    // -------------------------------------------
    ESSENCE_LIGHT,                 // Essenza di luce
    ESSENCE_DARKNESS,              // Essenza oscura
    ESSENCE_FIRE,                  // Essenza del fuoco
    ESSENCE_AIR,                   // Essenza dell'aria
    ESSENCE_WATER,                 // Essenza dell'acqua
    ESSENCE_EARTH,                 // Essenza della terra
    ESSENCE_SUPERIOR_LIGHT,        // Essenza superiore di luce
    ESSENCE_SUPERIOR_DARKNESS,     // Essenza superiore oscura
    ESSENCE_SUPERIOR_FIRE,         // Essenza superiore del fuoco
    ESSENCE_SUPERIOR_AIR,          // Essenza superiore dell'aria
    ESSENCE_SUPERIOR_WATER,        // Essenza superiore dell'acqua
    ESSENCE_SUPERIOR_EARTH,        // Essenza superiore della terra
    ESSENCE_SUPREME_LIGHT,         // Essenza suprema di luce
    ESSENCE_SUPREME_DARKNESS,      // Essenza suprema oscura
    ESSENCE_SUPREME_FIRE,          // Essenza suprema del fuoco
    ESSENCE_SUPREME_AIR,           // Essenza suprema dell'aria
    ESSENCE_SUPREME_WATER,         // Essenza suprema dell'acqua
    ESSENCE_SUPREME_EARTH,         // Essenza suprema della terra
    ESSENCE_PERFECT_LIGHT,         // Essenza perfetta di luce
    ESSENCE_PERFECT_DARKNESS,      // Essenza perfetta oscura
    ESSENCE_PERFECT_FIRE,          // Essenza perfetta del fuoco
    ESSENCE_PERFECT_AIR,           // Essenza perfetta dell'aria
    ESSENCE_PERFECT_WATER,         // Essenza perfetta dell'acqua
    ESSENCE_PERFECT_EARTH,         // Essenza perfetta della terra
    ESSENCE_LEGENDARY_LIGHT,       // Essenza leggendaria di luce
    ESSENCE_LEGENDARY_DARKNESS,    // Essenza leggendaria oscura
    ESSENCE_LEGENDARY_FIRE,        // Essenza leggendaria del fuoco
    ESSENCE_LEGENDARY_AIR,         // Essenza leggendaria dell'aria
    ESSENCE_LEGENDARY_WATER,       // Essenza leggendaria dell'acqua
    ESSENCE_LEGENDARY_EARTH,       // Essenza leggendaria della terra

    MAX_VALUE,

    RESOURCE_FIRST = COINS,
    RESOURCE_HERBALISM_FIRST = WOOD,
    RESOURCE_HERBALISM_MAX_VALUE = STRAMONIUM + 1,
    RESOURCE_MINING_FIRST = COPPER,
    RESOURCE_MINING_MAX_VALUE = ALTIUM + 1,
    RESOURCE_ESSENCE_FIRST = ESSENCE_LIGHT,
    RESOURCE_ESSENCE_MAX_VALUE = ESSENCE_LEGENDARY_EARTH + 1
};

QString resourceToString(ResourceType resource);
ResourceType resourceFromString(QString resource);
QString resourceToReadableString(ResourceType resource);
bool resourceIsForMining(ResourceType resource);
bool resourceIsForErbalism(ResourceType resource);
int resourceToLevel(ResourceType resource);

enum class ResourceRarityType
{
    UNKNOWN = -1,
    NONE,
    NORMAL,
    UNCOMMON,
    RARE,
    VERY_RARE,
    EXTREMELY_RARE
};

QString resourceRarityToString(ResourceRarityType rarity);
ResourceRarityType resourceRarityFromString(QString rarity);

enum class ProfessionType
{
    UNKNOWN = 0,
    BLACKSMITHING,
    COOKING,
    TAILORING,
    JEWELCRAFTING,
    ENCHANTING,
    ALCHEMY,

    MAX_VALUE
};

QString professionToString(ProfessionType profession);
ProfessionType professionFromString(QString profession);
QString professionToReadableString(ProfessionType profession);

// ---------------------------------------------
// Azioni
// ---------------------------------------------
enum class ActionType
{
    UNKNOWN,
    OPEN,        // Apri
    CLOSE,       // Chiudi
    MOVE         // Sposta
};

ActionType actionFromString(QString str);
QString actionToString(ActionType action);

enum class ClientDataType
{
    LIFE,
    TARGET_INFO,
    BASIC_INFO,
    ACTION_LAG,
    LOOT,
    STATUS,
    AGGRO,
    ROOM_INFO,
    ROOM_CONTENT,
    ATTACK_INFO,
    NEW_COMBAT_ROUND,
    REQUEST,
    CDT_MESSAGE,
    CHARACTERS_LIST
};

struct ClientData
{
    ClientData(ClientDataType type, bool isValid);
    virtual ~ClientData();

    ClientDataType type() const;
    bool isValid() const;
    QByteArray serialize() const;
    static ClientData* deserialize(QByteArray data);

    void operator=(const ClientData& o);
    bool operator==(const ClientData& o) const;
    bool operator!=(const ClientData& o) const;

protected:
    virtual void serializePrivate(QJsonObject& obj) const = 0;
    virtual void deserializePrivate(const QJsonObject& obj) = 0;

private:
    ClientDataType m_eType;
    bool m_bIsValid;
};

struct ClientDataLife : public ClientData
{
    ClientDataLife();
    ClientDataLife(
            int hpCurrent,
            int hpCurrentMaximum,
            int mpCurrent,
            int mpCurrentMaximum,
            int apCurrent,
            int apCurrentMaximum);
    virtual ~ClientDataLife() Q_DECL_OVERRIDE;

    void operator=(const ClientDataLife& o);
    bool operator==(const ClientDataLife& o) const;
    bool operator!=(const ClientDataLife& o) const;

    virtual void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    virtual void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    int hpCurrent() const { return m_iHpCurrent; }
    int hpCurrentMaximum() const { return m_iHpCurrentMaximum; }
    int mpCurrent() const { return m_iMpCurrent; }
    int mpCurrentMaximum() const { return m_iMpCurrentMaximum; }
    int apCurrent() const { return m_iApCurrent; }
    int apCurrentMaximum() const { return m_iApCurrentMaximum; }

private:
    const QString KEY_HP_CURRENT = "0";
    const QString KEY_HP_CURRENT_MAX = "1";
    const QString KEY_MP_CURRENT = "2";
    const QString KEY_MP_CURRENT_MAX = "3";
    const QString KEY_AP_CURRENT = "4";
    const QString KEY_AP_CURRENT_MAX = "5";

    int m_iHpCurrent;
    int m_iHpCurrentMaximum;
    int m_iMpCurrent;
    int m_iMpCurrentMaximum;
    int m_iApCurrent;
    int m_iApCurrentMaximum;
};

struct ClientDataTargetInfo : public ClientData
{
    ClientDataTargetInfo();
    ClientDataTargetInfo(QString name,
            int level,
            int hpCurrent = 0,
            int hpCurrentMaximum = 0,
            int mpCurrent = 0,
            int mpCurrentMaximum = 0,
            int apCurrent = 0,
            int apCurrentMaximum = 0,
            IdType id = IdInvalid);
    virtual ~ClientDataTargetInfo() Q_DECL_OVERRIDE;

    void operator=(const ClientDataTargetInfo& o);
    bool operator==(const ClientDataTargetInfo& o) const;
    bool operator!=(const ClientDataTargetInfo& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    int level() const { return m_iLevel; }
    int hpCurrent() const { return m_iHpCurrent; }
    int hpCurrentMaximum() const { return m_iHpCurrentMaximum; }
    int mpCurrent() const { return m_iMpCurrent; }
    int mpCurrentMaximum() const { return m_iMpCurrentMaximum; }
    int apCurrent() const { return m_iApCurrent; }
    int apCurrentMaximum() const { return m_iApCurrentMaximum; }
    QString name() const { return m_strName; }
    IdType id() const { return m_iId; }

private:
    const QString KEY_HP_CURRENT = "0";
    const QString KEY_HP_CURRENT_MAX = "1";
    const QString KEY_MP_CURRENT = "2";
    const QString KEY_MP_CURRENT_MAX = "3";
    const QString KEY_AP_CURRENT = "4";
    const QString KEY_AP_CURRENT_MAX = "5";
    const QString KEY_NAME = "6";
    const QString KEY_ID = "7";
    const QString KEY_LEVEL = "8";

    int m_iHpCurrent;
    int m_iHpCurrentMaximum;
    int m_iMpCurrent;
    int m_iMpCurrentMaximum;
    int m_iApCurrent;
    int m_iApCurrentMaximum;
    QString m_strName;
    IdType m_iId;
    int m_iLevel;
};

struct ClientDataBasicInfo : public ClientData
{
    ClientDataBasicInfo();
    ClientDataBasicInfo(QString name,
            QMUD::RaceType race,
            QMUD::ClassType classType,
            int level,
            qint64 experience,
            qint64 experienceToLevelUp,
            IdType id);
    virtual ~ClientDataBasicInfo() Q_DECL_OVERRIDE;

    void operator=(const ClientDataBasicInfo& o);
    bool operator==(const ClientDataBasicInfo& o) const;
    bool operator!=(const ClientDataBasicInfo& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QString name() const { return m_strName; }
    QMUD::RaceType race() const { return m_eRace; }
    QMUD::ClassType classType() const { return m_eClassType; }
    int level() const { return m_iLevel; }
    qint64 experience() const { return m_iExperience; }
    qint64 experienceToLevelUp() const { return m_iExperienceToLevelUp; }
    IdType id() const { return m_iId; }

private:
    const QString KEY_NAME = "0";
    const QString KEY_RACE = "1";
    const QString KEY_CLASS = "3";
    const QString KEY_LEVEL = "4";
    const QString KEY_EXPERIENCE = "5";
    const QString KEY_EXPERIENCE_TO_LEVEL_UP = "6";
    const QString KEY_ID = "7";

    QString m_strName;
    QMUD::RaceType m_eRace;
    QMUD::ClassType m_eClassType;
    int m_iLevel;
    qint64 m_iExperience;
    qint64 m_iExperienceToLevelUp;
    IdType m_iId;
};

struct ClientDataActionLag : public ClientData
{
    ClientDataActionLag();
    ClientDataActionLag(
            int actionLag,
            bool update);
    virtual ~ClientDataActionLag() Q_DECL_OVERRIDE;

    void operator=(const ClientDataActionLag& o);
    bool operator==(const ClientDataActionLag& o) const;
    bool operator!=(const ClientDataActionLag& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    int actionLag() const { return m_iActionLag; }
    bool update() const { return m_bUpdate; }

private:
    const QString KEY_ACTION_LAG = "0";
    const QString KEY_UPDATE = "1";

    int m_iActionLag;
    bool m_bUpdate;
};

struct ClientDataLoot : public ClientData
{
    struct ItemData
    {
        ItemData();
        bool operator==(const ItemData& o) const;

        QString name;
        bool toSell;
        ItemQualityType quality;
    };

    ClientDataLoot();
    ClientDataLoot(QVector<ItemData> loot);
    virtual ~ClientDataLoot() Q_DECL_OVERRIDE;

    void operator=(const ClientDataLoot& o);
    bool operator==(const ClientDataLoot& o) const;
    bool operator!=(const ClientDataLoot& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QVector<ItemData> lootList() const { return m_vLootList; }

private:
    const QString KEY_LOOT_LIST = "0";
    const QString KEY_TO_SELL = "1";
    const QString KEY_NAME = "2";
    const QString KEY_QUALITY = "3";

    QVector<ItemData> m_vLootList;
};

struct ClientDataStatus : public ClientData
{
    struct StatusData
    {
        StatusData();
        bool operator==(const StatusData& o) const;

        TemporaryStatusType status;
        int time;
    };

    ClientDataStatus();
    ClientDataStatus(QVector<StatusData> status);
    virtual ~ClientDataStatus() Q_DECL_OVERRIDE;

    void operator=(const ClientDataStatus& o);
    bool operator==(const ClientDataStatus& o) const;
    bool operator!=(const ClientDataStatus& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QVector<StatusData> statusList() const { return m_vStatusList; }

private:
    const QString KEY_STATUS_LIST = "0";
    const QString KEY_STATUS = "1";
    const QString KEY_TIME = "2";

    QVector<StatusData> m_vStatusList;
};

struct ClientDataTargetAggro : public ClientData
{
    struct AggroData
    {
        AggroData();
        bool operator==(const AggroData& o) const;

        QString name;
        qint64 aggro;
    };

    ClientDataTargetAggro();
    ClientDataTargetAggro(QVector<AggroData> aggroList);
    virtual ~ClientDataTargetAggro() Q_DECL_OVERRIDE;

    void operator=(const ClientDataTargetAggro& o);
    bool operator==(const ClientDataTargetAggro& o) const;
    bool operator!=(const ClientDataTargetAggro& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QVector<AggroData> aggroList() const { return m_vAggroList; }

private:
    const QString KEY_AGGRO_LIST = "0";
    const QString KEY_NAME = "1";
    const QString KEY_AGGRO = "2";

    QVector<AggroData> m_vAggroList;
};

struct ClientDataRoomInfo : public ClientData
{
    enum class ChType
    {
        PLAYER,
        NPC_AGGRESSIVE,
        NPC
    };

    struct ChInfo
    {
        void operator=(const ChInfo& o)
        {
            id = o.id;
            x = o.x;
            y = o.y;
            name = o.name;
            race = o.race;
            type = o.type;
        }

        bool operator==(const ChInfo& o) const
        {
            return id == o.id &&
            x == o.x &&
            y == o.y &&
            name == o.name &&
            race == o.race &&
            type == o.type;
        }

        bool operator!=(const ChInfo& o) const
        {
            return !(operator==(o));
        }

        QMUD::IdType id = QMUD::IdInvalid;

        int x = 0;
        int y = 0;

        QString name;
        QMUD::RaceType race = QMUD::RaceType::HUMAN;

        ChType type = ChType::PLAYER;
    };

    ClientDataRoomInfo();
    ClientDataRoomInfo(QString name,
                       QString description,
                       StaticIdType map,
                       StaticIdType index,
                       int x,
                       int y,
                       QVector<ChInfo> chs);
    virtual ~ClientDataRoomInfo() Q_DECL_OVERRIDE;

    void operator=(const ClientDataRoomInfo& o);
    bool operator==(const ClientDataRoomInfo& o) const;
    bool operator!=(const ClientDataRoomInfo& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QString name() const { return m_strName; }
    QString description() const { return m_strDescription; }
    StaticIdType map() const { return m_iMap; }
    StaticIdType index() const { return m_iIndex; }
    int x() const { return m_iX; }
    int y() const { return m_iY; }
    QVector<ChInfo> chs() const { return m_vChs; }

private:
    const QString KEY_NAME = "0";
    const QString KEY_DESCRIPTION = "1";
    const QString KEY_MAP = "9";
    const QString KEY_INDEX = "2";
    const QString KEY_X = "a";
    const QString KEY_Y = "b";
    const QString KEY_CHS = "c";
    const QString KEY_CHS_X = "d";
    const QString KEY_CHS_Y = "e";
    const QString KEY_CHS_NAME = "f";
    const QString KEY_CHS_RACE = "g";
    const QString KEY_CHS_ID = "h";
    const QString KEY_CHS_TYPE = "i";

    QString m_strName;
    QString m_strDescription;
    StaticIdType m_iMap;
    StaticIdType m_iIndex;
    int m_iX;
    int m_iY;
    QVector<ChInfo> m_vChs;
};

struct ClientDataRoomContent : public ClientData
{
    struct CharacterData
    {
        CharacterData() :
            level(0),
            isNpc(false),
            hpMax(0),
            hpCurrent(0),
            mpMax(0),
            mpCurrent(0),
            id(IdInvalid)
        {}

        bool operator==(const CharacterData& o) const
        {
            return name == o.name &&
                level == o.level &&
                isNpc == o.isNpc &&
                hpMax == o.hpMax &&
                hpCurrent == o.hpCurrent &&
                mpMax == o.mpMax &&
                mpCurrent == o.mpCurrent &&
                id == o.id;
        }

        bool operator!=(const CharacterData& o) const
        {
            return !(operator==(o));
        }

        QString name;
        int level;
        bool isNpc;
        int hpMax;
        int hpCurrent;
        int mpMax;
        int mpCurrent;
        IdType id;
    };

    ClientDataRoomContent();
    ClientDataRoomContent(QVector<CharacterData> characters);
    virtual ~ClientDataRoomContent() Q_DECL_OVERRIDE;

    void operator=(const ClientDataRoomContent& o);
    bool operator==(const ClientDataRoomContent& o) const;
    bool operator!=(const ClientDataRoomContent& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QVector<CharacterData> characters() const { return m_vCharacters; }

private:
    const QString KEY_CHDATA = "2";
    const QString KEY_CHDATA_LEVEL = "3";
    const QString KEY_CHDATA_NAME = "4";
    const QString KEY_CHDATA_HP = "5";
    const QString KEY_CHDATA_HP_MAX = "6";
    const QString KEY_CHDATA_MP = "7";
    const QString KEY_CHDATA_MP_MAX = "8";
    const QString KEY_CHDATA_IS_NPC = "d";
    const QString KEY_CHDATA_ID = "e";

    QVector<CharacterData> m_vCharacters;
};

struct ClientDataAttackInfo : public ClientData
{
    ClientDataAttackInfo();
    ClientDataAttackInfo(int value, IdType suitorId, IdType targetId, bool is_damage,
                         QMUD::DamageType damageType,
                         QMUD::AbilityType ability,
                         QMUD::SpellType spell);
    virtual ~ClientDataAttackInfo() Q_DECL_OVERRIDE;

    void operator=(const ClientDataAttackInfo& o);
    bool operator==(const ClientDataAttackInfo& o) const;
    bool operator!=(const ClientDataAttackInfo& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    static const int VALUE_PARRY = -1;
    static const int VALUE_DODGE = -2;
    static const int VALUE_ILLUSORY_IMAGE = -3;
    static const int VALUE_NONE = -100;

    int value() const { return m_iValue; }
    bool isDamage() const { return m_bIsDamage; }
    IdType suitorId() const { return m_iSuitorId; }
    IdType targetId() const { return m_iTargetId; }
    QMUD::DamageType damageType() const { return m_eDamageType; }
    QMUD::AbilityType ability() const { return m_eAbility; }
    QMUD::SpellType spell() const { return m_eSpell; }

private:
    const QString KEY_VALUE = "0";
    const QString KEY_IS_DAMAGE = "1";
    const QString KEY_SUITOR_ID = "2";
    const QString KEY_TARGET_ID = "3";
    const QString KEY_DAMAGE_TYPE = "4";
    const QString KEY_ABILITY = "5";
    const QString KEY_SPELL = "6";

    int m_iValue;
    bool m_bIsDamage;
    IdType m_iSuitorId;
    IdType m_iTargetId;
    QMUD::DamageType m_eDamageType;
    QMUD::AbilityType m_eAbility;
    QMUD::SpellType m_eSpell;
};

struct ClientDataNewCombatRound : public ClientData
{
    ClientDataNewCombatRound();
    virtual ~ClientDataNewCombatRound() Q_DECL_OVERRIDE;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;
};

struct ClientDataRequest : public ClientData
{
    ClientDataRequest();
    ClientDataRequest(QString request, QStringList options);
    virtual ~ClientDataRequest() Q_DECL_OVERRIDE;

    void operator=(const ClientDataRequest& o);
    bool operator==(const ClientDataRequest& o) const;
    bool operator!=(const ClientDataRequest& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QStringList options() const { return m_vOptions; }
    QString request() const { return m_strRequest; }

private:
    const QString KEY_OPTIONS = "0";
    const QString KEY_REQUEST = "1";

    QStringList m_vOptions;
    QString m_strRequest;
};

struct ClientDataMessage : public ClientData
{
    enum class Message
    {
        // Errori
        ERROR_GENERIC,
        ERROR_LOGIN_FAILED,
        ERROR_LOGIN_FAILED_CONTACT_ADMINISTRATOR,
        ERROR_SIGNIN_FAILED,
        ERROR_USERNAME_FAILED,
        ERROR_USERNAME_EXISTS,
        ERROR_PASSWORD_FAILED,
        ERROR_CONNECTION_FAILED,
        ERROR_CONNECTION_FAILED_CONTACT_ADMINISTRATOR,
        ERROR_CH_CREATION_FAILED_CONTACT_ADMINISTRATOR,
        ERROR_CH_NAME_FAILED,

        // Info
        INFO_SIGNIN_OK,
        INFO_CH_CREATION_OK,

        // Connection status
        CONN_STATUS_IDLE,
        CONN_STATUS_LOGIN,
        CONN_STATUS_CHARACTERS_LIST,
        CONN_STATUS_IN_GAME
    };

    ClientDataMessage();
    ClientDataMessage(Message error);
    virtual ~ClientDataMessage() Q_DECL_OVERRIDE;

    void operator=(const ClientDataMessage& o);
    bool operator==(const ClientDataMessage& o) const;
    bool operator!=(const ClientDataMessage& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    Message message() const { return m_eMessage; }

private:
    const QString KEY_MESSAGE = "0";

    Message m_eMessage;
};

struct ClientDataCharactersList : public ClientData
{
    ClientDataCharactersList();
    ClientDataCharactersList(QVector<PcCharacterBaseInfo> chsList);
    virtual ~ClientDataCharactersList() Q_DECL_OVERRIDE;

    void operator=(const ClientDataCharactersList& o);
    bool operator==(const ClientDataCharactersList& o) const;
    bool operator!=(const ClientDataCharactersList& o) const;

    void serializePrivate(QJsonObject& obj) const Q_DECL_OVERRIDE;
    void deserializePrivate(const QJsonObject& obj) Q_DECL_OVERRIDE;

    QVector<PcCharacterBaseInfo> chsList() const { return m_vChsList; }

private:
    const QString KEY_LIST = "0";

    QVector<PcCharacterBaseInfo> m_vChsList;
};

}

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif

#endif

