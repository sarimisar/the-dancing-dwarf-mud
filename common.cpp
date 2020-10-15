/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "common.h"

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <QJsonArray>

static QString raceString[static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX)] =
{
    "human",
    "dwarf",
    "gnome",
    "halforc",
    "highelf",
    "halfelf",
    "acquatic_beast",
    "drow",
    "swarm",
    "aberration",
    "beast",
    "construct",
    "dragon",
    "dragon_air",
    "dragon_acquatic",
    "dragon_chaos",
    "dragon_cold",
    "dragon_earth",
    "dragon_electricity",
    "dragon_fire",
    "dragon_force",
    "dragon_light",
    "dragon_shadow",
    "dragon_water",
    "elemental_air",
    "elemental_earth",
    "elemental_fire",
    "elemental_water",
    "elemental_cold",
    "fey",
    "giant",
    "giant_air",
    "giant_earth",
    "giant_fire",
    "giant_cold",
    "gnoll",
    "goblinoid",
    "minotaur",
    "orc",
    "magical_beast",
    "ooze",
    "ooze_air",
    "ooze_earth",
    "ooze_fire",
    "ooze_water",
    "plant",
    "undead",
    "lich",
    "vermin",
    "vermin_air",
    "vermin_earth",
    "vermin_fire",
    "vermin_water",
    "vermin_cold",
    "baleen",
    "black_bear",
    "hawk",
    "brown_bear",
    "buffalo",
    "cat",
    "cattle",
    "squirrel",
    "bat",
    "deer",
    "dog",
    "dolphin",
    "fish",
    "eagle",
    "elephant",
    "fox",
    "goat",
    "gullion",
    "horse",
    "jackal",
    "monkey",
    "mole",
    "mouse",
    "owl",
    "penguin",
    "piranha",
    "frog",
    "polar_bear",
    "rat",
    "raven",
    "seal",
    "shark",
    "sheep",
    "serpent",
    "spider",
    "giant_spider",
    "wolf",
    "boar",
    "ant",
    "giant_ant",
    "kobold"
};

static const char* raceJsString[static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX)] =
{
    "RACE_HUMAN",
    "RACE_DWARF",
    "RACE_GNOME",
    "RACE_HALF_ORC",
    "RACE_HIGH_ELF",
    "RACE_HALF_ELF",
    "RACE_ACQUATIC_BEAST",
    "RACE_DROW",
    "RACE_SWARM",
    "RACE_ABERRATION",
    "RACE_BEAST",
    "RACE_CONSTRUCT",
    "RACE_DRAGON",
    "RACE_DRAGON_AIR",
    "RACE_DRAGON_ACQUATIC",
    "RACE_DRAGON_CHAOS",
    "RACE_DRAGON_COLD",
    "RACE_DRAGON_EARTH",
    "RACE_DRAGON_ELECTRICITY",
    "RACE_DRAGON_FIRE",
    "RACE_DRAGON_FORCE",
    "RACE_DRAGON_LIGHT",
    "RACE_DRAGON_SHADOW",
    "RACE_DRAGON_WATER",
    "RACE_ELEMENTAL_AIR",
    "RACE_ELEMENTAL_EARTH",
    "RACE_ELEMENTAL_FIRE",
    "RACE_ELEMENTAL_WATER",
    "RACE_ELEMENTAL_COLD",
    "RACE_FEY",
    "RACE_GIANT",
    "RACE_GIANT_AIR",
    "RACE_GIANT_EARTH",
    "RACE_GIANT_FIRE",
    "RACE_GIANT_COLD",
    "RACE_GNOLL",
    "RACE_GOBLINOID",
    "RACE_MINOTAUR",
    "RACE_ORC",
    "RACE_MAGICAL_BEAST",
    "RACE_OOZE",
    "RACE_OOZE_AIR",
    "RACE_OOZE_EARTH",
    "RACE_OOZE_FIRE",
    "RACE_OOZE_WATER",
    "RACE_PLANT",
    "RACE_UNDEAD",
    "RACE_LICH",
    "RACE_VERMIN",
    "RACE_VERMIN_AIR",
    "RACE_VERMIN_EARTH",
    "RACE_VERMIN_FIRE",
    "RACE_VERMIN_WATER",
    "RACE_VERMIN_COLD",
    "RACE_BALEEN",
    "RACE_BLACK_BEAR",
    "RACE_HAWK",
    "RACE_BROWN_BEAR",
    "RACE_BUFFALO",
    "RACE_CAT",
    "RACE_CATTLE",
    "RACE_SQUIRREL",
    "RACE_BAT",
    "RACE_DEER",
    "RACE_DOG",
    "RACE_DOLPHIN",
    "RACE_FISH",
    "RACE_EAGLE",
    "RACE_ELEPHANT",
    "RACE_FOX",
    "RACE_GOAT",
    "RACE_GULLION",
    "RACE_HORSE",
    "RACE_JACKAL",
    "RACE_MONKEY",
    "RACE_MOLE",
    "RACE_MOUSE",
    "RACE_OWL",
    "RACE_PENGUIN",
    "RACE_PIRANHA",
    "RACE_FROG",
    "RACE_POLAR_BEAR",
    "RACE_RAT",
    "RACE_RAVEN",
    "RACE_SEAL",
    "RACE_SHARK",
    "RACE_SHEEP",
    "RACE_SERPENT",
    "RACE_SPIDER",
    "RACE_GIANT_SPIDER",
    "RACE_WOLF",
    "RACE_BOAR",
    "RACE_ANT",
    "RACE_GIANT_ANT"
    "RACE_KOBOLD"
};

static QString raceReadableString[static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX)] =
{
    QObject::tr("Umano"),
    QObject::tr("Nano"),
    QObject::tr("Gnomo"),
    QObject::tr("Mezz'orco"),
    QObject::tr("Elfo alto"),
    QObject::tr("Mezz'elfo"),
    QObject::tr("Bestia acquatica"),
    QObject::tr("Drow"),
    QObject::tr("Sciame"),
    QObject::tr("Aberrazione"),
    QObject::tr("Bestia"),
    QObject::tr("Costrutto"),
    QObject::tr("Drago"),
    QObject::tr("Drago Aria"),
    QObject::tr("Drago Acquatico"),
    QObject::tr("Drago Chaos"),
    QObject::tr("Drago Freddo"),
    QObject::tr("Drago Terra"),
    QObject::tr("Drago Elettricità"),
    QObject::tr("Drago Fuoco"),
    QObject::tr("Drago Forza"),
    QObject::tr("Drago Luce"),
    QObject::tr("Drago Ombra"),
    QObject::tr("Drago Acqua"),
    QObject::tr("Elementale Aria"),
    QObject::tr("Elementale Terra"),
    QObject::tr("Elementale Fuoco"),
    QObject::tr("Elementale Acqua"),
    QObject::tr("Elementale del ghiaccio"),
    QObject::tr("Fata"),
    QObject::tr("Gigante"),
    QObject::tr("Gigante dell'aria"),
    QObject::tr("Gigante della terra"),
    QObject::tr("Gigante del fuoco"),
    QObject::tr("Gigante dei ghiacci"),
    QObject::tr("Gnoll"),
    QObject::tr("Goblinoide"),
    QObject::tr("Minotauro"),
    QObject::tr("Orco"),
    QObject::tr("Bestia magica"),
    QObject::tr("Melma"),
    QObject::tr("Melma d'aria"),
    QObject::tr("Melma di terra"),
    QObject::tr("Melma di fuoco"),
    QObject::tr("Melma d'acqua"),
    QObject::tr("Pianta"),
    QObject::tr("Non morto"),
    QObject::tr("Lich"),
    QObject::tr("Verme"),
    QObject::tr("Verme d'aria"),
    QObject::tr("Verme di terra"),
    QObject::tr("Verme di fuoco"),
    QObject::tr("Verme d'acqua"),
    QObject::tr("Verme di ghiaccio"),
    QObject::tr("Balena"),
    QObject::tr("Orso"),
    QObject::tr("Falco"),
    QObject::tr("Orso bruno"),
    QObject::tr("Bufalo"),
    QObject::tr("Gatto"),
    QObject::tr("Bestiame"),
    QObject::tr("Scoiattolo"),
    QObject::tr("Pipistrello"),
    QObject::tr("Renna"),
    QObject::tr("Cane"),
    QObject::tr("Delfino"),
    QObject::tr("Pesce"),
    QObject::tr("Acquila"),
    QObject::tr("Elefante"),
    QObject::tr("Volpe"),
    QObject::tr("Capra"),
    QObject::tr("Gabbiano"),
    QObject::tr("Cavallo"),
    QObject::tr("Sciacallo"),
    QObject::tr("Scimmia"),
    QObject::tr("Talpa"),
    QObject::tr("Topo"),
    QObject::tr("Gufo"),
    QObject::tr("Pinguino"),
    QObject::tr("Piragna"),
    QObject::tr("Rana"),
    QObject::tr("Orso polare"),
    QObject::tr("Ratto"),
    QObject::tr("Corvo"),
    QObject::tr("Foca"),
    QObject::tr("Squalo"),
    QObject::tr("Pecora"),
    QObject::tr("Serpente"),
    QObject::tr("Ragno"),
    QObject::tr("Ragno gigante"),
    QObject::tr("Lupo"),
    QObject::tr("Cinghiale"),
    QObject::tr("Formica"),
    QObject::tr("Formica gigante"),
    QObject::tr("Coboldo")
};

//static bool raceCanEquip[static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX)] =
//{
//    true,   // Umano
//    true,   // Nano
//    true,   // Gnomo
//    true,   // Mezz'orco
//    true,   // Elfo alto
//    true,   // Mezz'elfo
//    false,  // Bestia acquatica
//    true,   // Drow
//    false,  // Sciame
//    false,  // Aberrazione
//    false,  // Bestia
//    true,   // Costrutto
//    false,  // Drago
//    false,  // Drago Aria
//    false,  // Drago Acquatico
//    false,  // Drago Chaos
//    false,  // Drago Freddo
//    false,  // Drago Terra
//    false,  // Drago Elettricità
//    false,  // Drago Fuoco
//    false,  // Drago Forza
//    false,  // Drago Luce
//    false,  // Drago Ombra
//    false,  // Drago Acqua
//    false,  // Elementale Aria
//    false,  // Elementale Terra
//    false,  // Elementale Fuoco
//    false,  // Elementale Acqua
//    false,  // Elementale del ghiaccio
//    true,   // Fata
//    true,   // Gigante
//    true,   // Gigante dell'aria
//    true,   // Gigante della terra
//    true,   // Gigante del fuoco
//    true,   // Gigante dei ghiacci
//    true,   // Gnoll
//    true,   // Goblinoide
//    true,   // Minotauro
//    true,   // Orco
//    false,  // Bestia magica
//    false,  // Melma
//    false,  // Melma d'aria
//    false,  // Melma di terra
//    false,  // Melma di fuoco
//    false,  // Melma d'acqua
//    false,  // Pianta
//    true,   // Non morto
//    true,   // Lich
//    false,  // Verme
//    false,  // Verme d'aria
//    false,  // Verme di terra
//    false,  // Verme di fuoco
//    false,  // Verme d'acqua
//    false,  // Verme di ghiaccio
//    false,  // Balena
//    false,  // Orso
//    false,  // Falco
//    false,  // Orso bruno
//    false,  // Bufalo
//    false,  // Gatto
//    false,  // Bestiame
//    false,  // Scoiattolo
//    false,  // Pipistrello
//    false,  // Renna
//    false,  // Cane
//    false,  // Delfino
//    false,  // Pesce
//    false,  // Acquila
//    false,  // Elefante
//    false,  // Volpe
//    false,  // Capra
//    false,  // Gabbiano
//    false,  // Cavallo
//    false,  // Sciacallo
//    false,  // Scimmia
//    false,  // Talpa
//    false,  // Topo
//    false,  // Gufo
//    false,  // Pinguino
//    false,  // Piragna
//    false,  // Rana
//    false,  // Orso polare
//    false,  // Ratto
//    false,  // Corvo
//    false,  // Foca
//    false,  // Squalo
//    false,  // Pecora
//    false,  // Serpente
//    false,  // Ragno
//    false,  // Ragno gigante
//    false,  // Lupo
//    false,  // Cinghiale
//    false,  // Formica
//    false,  // Formica gigante
//    false,  // Coboldo
//};

// INT, WIS, CON, STR, DEX
static int raceStatsPC[static_cast<int>(QMUD::RaceType::RACE_TYPE_PC_MAX)][5] =
{
    { 6,    6,	6,	6,	6 },          // Umano
    { 4,	8,	8,	6,	4 },          // Nano
    { 8,	8,	4,	4,	6 },          // Gnomo
    { 4,	6,	8,	8,	4 },          // Mezz'orco
    { 8,	8,	4,	4,	6 },          // Elfo alto
    { 4,	4,	6,	8,	8 }          // Mezz'elfo
};

// INT, WIS, CON, STR, DEX
static int raceStatsNPC[static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX)][5] =
{
    { 10,   10, 10, 10, 10 },          // Umano
    { 8,    10, 12, 12, 8  },          // Nano
    { 14,   14,  6,  6, 10 },          // Gnomo
    { 6,    8,  14, 14, 8  },          // Mezz'orco
    { 12,   12, 6,  8,  12 },          // Elfo alto
    { 8,    8,  10, 10, 14 },          // Mezz'elfo
    { 3,	3,	16,	16,	16 },         // Bestia acquatica
    { 16,	14,	14,	14,	16 },         // Drow
    { 3,	3,	10,	16,	16 },         // Sciame
    { 3,	3,	16,	16,	14 },         // Aberrazione
    { 3,	3,	16,	16,	14 },         // Bestia
    { 3,	3,	16,	16,	10 },         // Costrutto
    { 16,	16,	18,	16,	14 },         // Drago
    { 18,	20,	16,	22,	18 },         // Drago Aria
    { 16,	16,	14,	20,	18 },         // Drago Acquatico
    { 18,	14,	18,	22,	14 },         // Drago Chaos
    { 18,	20,	16,	22,	18 },         // Drago Freddo
    { 18,	18,	24,	24,	14 },         // Drago Terra
    { 18,	14,	14,	20,	18 },         // Drago Elettricità
    { 20,	18,	20,	24,	16 },         // Drago Fuoco
    { 16,	16,	18,	24,	16 },         // Drago Forza
    { 20,	20,	18,	18,	18 },         // Drago Luce
    { 20,	20,	18,	18,	18 },         // Drago Ombra
    { 18,	14,	18,	20,	20 },         // Drago Acqua
    { 8,	8,	18,	18,	18 },         // Elementale Aria
    { 8,	8,	22,	18,	18 },         // Elementale Terra
    { 8,	8,	18,	22,	18 },         // Elementale Fuoco
    { 8,	8,	18,	20,	18 },         // Elementale Acqua
    { 8,	8,	18,	18,	18 },         // Elementale del ghiaccio
    { 16,	16,	8,	12,	16 },         // Fata
    { 4,	8,	20,	20,	8 },          // Gigante
    { 4,	8,	20,	20,	16 },         // Gigante dell'aria
    { 4,	8,	24,	20,	8 },          // Gigante della terra
    { 4,	8,	20,	24,	8 },          // Gigante del fuoco
    { 8,	8,	20,	24,	8 },          // Gigante dei ghiacci
    { 6,	6,	14,	16,	14 },         // Gnoll
    { 6,	6,	14,	14,	14 },         // Goblinoide
    { 8,	8,	18,	20,	12 },         // Minotauro
    { 6,	6,	18,	18,	14 },         // Orco
    { 10,	10,	14,	18,	18 },         // Bestia magica
    { 3,	3,	16,	16,	3 },          // Melma
    { 3,	3,	18,	18,	3 },          // Melma d'aria
    { 3,	3,	18,	18,	3 },          // Melma di terra
    { 3,	3,	18,	18,	3 },          // Melma di fuoco
    { 3,	3,	18,	18,	3 },          // Melma d'acqua
    { 3,	3,	12,	14,	3 },          // Pianta
    { 10,	10,	10,	16,	14 },         // Non morto
    { 20,	20,	10,	14,	14 },         // Lich
    { 3,	3,	16,	16,	14 },         // Verme
    { 3,	3,	16,	18,	14 },         // Verme d'aria
    { 3,	3,	16,	18,	14 },         // Verme di terra
    { 3,	3,	16,	18,	14 },         // Verme di fuoco
    { 3,	3,	16,	18,	14 },         // Verme d'acqua
    { 3,	3,	16,	18,	14 },         // Verme di ghiaccio
    { 3,	3,	22,	18,	6 },          // Balena
    { 3,	3,	18,	18,	8 },          // Orso
    { 3,	3,	12,	14,	18 },         // Falco
    { 3,	3,	20,	20,	8 },          // Orso bruno
    { 3,	3,	18,	16,	10 },         // Bufalo
    { 3,	3,	4,	6,	20 },         // Gatto
    { 3,	3,	16,	16,	6 },          // Bestiame
    { 3,	3,	3,	4,	12 },         // Scoiattolo
    { 3,	3,	3,	6,	18 },         // Pipistrello
    { 3,	3,	10,	12,	12 },         // Renna
    { 3,	3,	12,	14,	14 },         // Cane
    { 3,	3,	10,	10,	16 },         // Delfino
    { 3,	3,	3,	3,	16 },         // Pesce
    { 3,	3,	12,	12,	16 },         // Acquila
    { 3,	3,	20,	18,	4 },          // Elefante
    { 6,	3,	8,	12,	16 },         // Volpe
    { 3,	3,	8,	12,	10 },         // Capra
    { 3,	3,	4,	6,	10 },         // Gabbiano
    { 3,	3,	12,	14,	14 },         // Cavallo
    { 3,	3,	8,	10,	16 },         // Sciacallo
    { 3,	3,	12,	12,	16 },         // Scimmia
    { 3,	3,	6,	3,	8 },          // Talpa
    { 3,	3,	3,	3,	3 },          // Topo
    { 3,	3,	6,	6,	10 },         // Gufo
    { 3,	3,	6,	3,	3 },          // Pinguino
    { 3,	3,	3,	3,	16 },         // Piragna
    { 3,	3,	3,	3,	12 },         // Rana
    { 3,	3,	20,	18,	12 },         // Orso polare
    { 3,	3,	3,	3,	3 },          // Ratto
    { 3,	3,	6,	3,	10 },         // Corvo
    { 3,	3,	8,	3,	8 },          // Foca
    { 3,	3,	12,	16,	16 },         // Squalo
    { 3,	3,	3,	3,	3 },          // Pecora
    { 3,	3,	10,	10,	14 },         // Serpente
    { 3,	3,	3,	3,	3 },          // Ragno
    { 3,	3,	16,	16,	16 },         // Ragno gigante
    { 3,	3,	12,	14,	16 },         // Lupo
    { 3,	3,	14,	16,	10 },         // Cinghiale
    { 3,	3,	3,	3,	3 },          // Formica
    { 3,	3,	16,	16,	16 },         // Formica gigante
    { 5,	5,	14,	14,	12 }          // Coboldo
};

static QMUD::DamageType raceDamageType[static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX)] =
{
    QMUD::DamageType::HANDS,             // Umano
    QMUD::DamageType::HANDS,             // Nano
    QMUD::DamageType::HANDS,             // Gnomo
    QMUD::DamageType::HANDS,             // Mezz'orco
    QMUD::DamageType::HANDS,             // Elfo alto
    QMUD::DamageType::HANDS,             // Mezz'elfo
    QMUD::DamageType::HANDS,             // Bestia acquatica
    QMUD::DamageType::SLASHING,          // Drow
    QMUD::DamageType::HANDS,             // Sciame
    QMUD::DamageType::HANDS,             // Aberrazione
    QMUD::DamageType::HANDS,             // Bestia
    QMUD::DamageType::BLUDGEONING,       // Costrutto
    QMUD::DamageType::HANDS,             // Drago
    QMUD::DamageType::HANDS,             // Drago Aria
    QMUD::DamageType::HANDS,             // Drago Acquatico
    QMUD::DamageType::HANDS,             // Drago Chaos
    QMUD::DamageType::HANDS,             // Drago Freddo
    QMUD::DamageType::HANDS,             // Drago Terra
    QMUD::DamageType::HANDS,             // Drago Elettricità
    QMUD::DamageType::HANDS,             // Drago Fuoco
    QMUD::DamageType::HANDS,             // Drago Forza
    QMUD::DamageType::HANDS,             // Drago Luce
    QMUD::DamageType::HANDS,             // Drago Ombra
    QMUD::DamageType::HANDS,             // Drago Acqua
    QMUD::DamageType::HANDS,             // Elementale Aria
    QMUD::DamageType::HANDS,             // Elementale Terra
    QMUD::DamageType::HANDS,             // Elementale Fuoco
    QMUD::DamageType::HANDS,             // Elementale Acqua
    QMUD::DamageType::HANDS,             // Elementale del ghiaccio
    QMUD::DamageType::PIERCING,          // Fata
    QMUD::DamageType::BLUDGEONING,       // Gigante
    QMUD::DamageType::BLUDGEONING,       // Gigante dell'aria
    QMUD::DamageType::BLUDGEONING,       // Gigante della terra
    QMUD::DamageType::BLUDGEONING,       // Gigante del fuoco
    QMUD::DamageType::BLUDGEONING,       // Gigante dei ghiacci
    QMUD::DamageType::SLASHING,          // Gnoll
    QMUD::DamageType::SLASHING,          // Goblinoide
    QMUD::DamageType::HANDS,             // Minotauro
    QMUD::DamageType::SLASHING,          // Orco
    QMUD::DamageType::HANDS,             // Bestia magica
    QMUD::DamageType::HANDS,             // Melma
    QMUD::DamageType::HANDS,             // Melma d'aria
    QMUD::DamageType::HANDS,             // Melma di terra
    QMUD::DamageType::HANDS,             // Melma di fuoco
    QMUD::DamageType::HANDS,             // Melma d'acqua
    QMUD::DamageType::HANDS,             // Pianta
    QMUD::DamageType::SLASHING,          // Non morto
    QMUD::DamageType::SLASHING,          // Lich
    QMUD::DamageType::HANDS,             // Verme
    QMUD::DamageType::HANDS,             // Verme d'aria
    QMUD::DamageType::HANDS,             // Verme di terra
    QMUD::DamageType::HANDS,             // Verme di fuoco
    QMUD::DamageType::HANDS,             // Verme d'acqua
    QMUD::DamageType::HANDS,             // Verme di ghiaccio
    QMUD::DamageType::HANDS,             // Balena
    QMUD::DamageType::HANDS,             // Orso
    QMUD::DamageType::HANDS,             // Falco
    QMUD::DamageType::HANDS,             // Orso bruno
    QMUD::DamageType::HANDS,             // Bufalo
    QMUD::DamageType::HANDS,             // Gatto
    QMUD::DamageType::HANDS,             // Bestiame
    QMUD::DamageType::HANDS,             // Scoiattolo
    QMUD::DamageType::HANDS,             // Pipistrello
    QMUD::DamageType::HANDS,             // Renna
    QMUD::DamageType::HANDS,             // Cane
    QMUD::DamageType::HANDS,             // Delfino
    QMUD::DamageType::HANDS,             // Pesce
    QMUD::DamageType::HANDS,             // Acquila
    QMUD::DamageType::HANDS,             // Elefante
    QMUD::DamageType::HANDS,             // Volpe
    QMUD::DamageType::HANDS,             // Capra
    QMUD::DamageType::HANDS,             // Gabbiano
    QMUD::DamageType::HANDS,             // Cavallo
    QMUD::DamageType::HANDS,             // Sciacallo
    QMUD::DamageType::HANDS,             // Scimmia
    QMUD::DamageType::HANDS,             // Talpa
    QMUD::DamageType::HANDS,             // Topo
    QMUD::DamageType::HANDS,             // Gufo
    QMUD::DamageType::HANDS,             // Pinguino
    QMUD::DamageType::HANDS,             // Piragna
    QMUD::DamageType::HANDS,             // Rana
    QMUD::DamageType::HANDS,             // Orso polare
    QMUD::DamageType::HANDS,             // Ratto
    QMUD::DamageType::HANDS,             // Corvo
    QMUD::DamageType::HANDS,             // Foca
    QMUD::DamageType::HANDS,             // Squalo
    QMUD::DamageType::HANDS,             // Pecora
    QMUD::DamageType::HANDS,             // Serpente
    QMUD::DamageType::HANDS,             // Ragno
    QMUD::DamageType::HANDS,             // Ragno gigante
    QMUD::DamageType::HANDS,             // Lupo
    QMUD::DamageType::HANDS,             // Cinghiale
    QMUD::DamageType::HANDS,             // Formica
    QMUD::DamageType::HANDS,             // Formica gigante
    QMUD::DamageType::SLASHING           // Coboldo
};


QString QMUD::itemTypeToString(QMUD::ItemType type)
{
    switch (type)
    {
    case ItemType::BASE: return "BASE";
    case ItemType::ARMOR: return "ARMOR";
    case ItemType::WEAPON: return "WEAPON";
    case ItemType::TRINKET: return "TRINKET";
    case ItemType::FOOD: return "FOOD";
    case ItemType::DRINK: return "DRINK";
    case ItemType::POTION: return "POTION";
    case ItemType::TONIC: return "TONIC";
    case ItemType::RESOURCE: return "RESOURCE";
    case ItemType::TOTEM: return "TOTEM";
    case ItemType::SYMBOL: return "SYMBOL";
    case ItemType::UNKNOWN:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ItemType QMUD::itemTypeFromString(QString str)
{
    if      (str.compare("BASE", Qt::CaseInsensitive) == 0) return QMUD::ItemType::BASE;
    else if (str.compare("WEAPON", Qt::CaseInsensitive) == 0) return QMUD::ItemType::WEAPON;
    else if (str.compare("ARMOR", Qt::CaseInsensitive) == 0) return QMUD::ItemType::ARMOR;
    else if (str.compare("TRINKET", Qt::CaseInsensitive) == 0) return QMUD::ItemType::TRINKET;
    else if (str.compare("FOOD", Qt::CaseInsensitive) == 0) return QMUD::ItemType::FOOD;
    else if (str.compare("DRINK", Qt::CaseInsensitive) == 0) return QMUD::ItemType::DRINK;
    else if (str.compare("POTION", Qt::CaseInsensitive) == 0) return QMUD::ItemType::POTION;
    else if (str.compare("TONIC", Qt::CaseInsensitive) == 0) return QMUD::ItemType::TONIC;
    else if (str.compare("RESOURCE", Qt::CaseInsensitive) == 0) return QMUD::ItemType::RESOURCE;
    else if (str.compare("TOTEM", Qt::CaseInsensitive) == 0) return QMUD::ItemType::TOTEM;
    else if (str.compare("SYMBOL", Qt::CaseInsensitive) == 0) return QMUD::ItemType::SYMBOL;
    else {
        return QMUD::ItemType::UNKNOWN;
    }
}

QString QMUD::itemTypeToReadableString(QMUD::ItemType type)
{
    switch (type)
    {
    case ItemType::BASE: return QObject::tr("Oggetto");
    case ItemType::ARMOR: return QObject::tr("Armatura");
    case ItemType::WEAPON: return QObject::tr("Arma");
    case ItemType::TRINKET: return QObject::tr("Gioiello");
    case ItemType::FOOD: return QObject::tr("Cibo");
    case ItemType::DRINK: return QObject::tr("Bevanda");
    case ItemType::POTION: return QObject::tr("Pozione");
    case ItemType::TONIC: return QObject::tr("Tonico");
    case ItemType::RESOURCE: return QObject::tr("Risorsa");
    case ItemType::TOTEM: return QObject::tr("Totem");
    case ItemType::SYMBOL: return QObject::tr("Simbolo");
    case ItemType::UNKNOWN:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QString QMUD::itemClassToString(QMUD::ItemClassType type)
{
    switch (type)
    {
    case ItemClassType::CASTER_WIS: return "CASTER_WIS";
    case ItemClassType::CASTER_INT: return "CASTER_INT";
    case ItemClassType::DPS_DEX: return "DPS_DEX";
    case ItemClassType::DPS_STR: return "DPS_STR";
    case ItemClassType::TANK: return "TANK";
    case ItemClassType::UNKNOWN:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ItemClassType QMUD::itemClassFromString(QString str)
{
    if      (str.compare("CASTER_WIS", Qt::CaseInsensitive) == 0) return QMUD::ItemClassType::CASTER_WIS;
    else if (str.compare("CASTER_INT", Qt::CaseInsensitive) == 0) return QMUD::ItemClassType::CASTER_INT;
    else if (str.compare("DPS_DEX", Qt::CaseInsensitive) == 0) return QMUD::ItemClassType::DPS_DEX;
    else if (str.compare("DPS_STR", Qt::CaseInsensitive) == 0) return QMUD::ItemClassType::DPS_STR;
    else if (str.compare("TANK", Qt::CaseInsensitive) == 0) return QMUD::ItemClassType::TANK;
    else {
        return QMUD::ItemClassType::UNKNOWN;
    }
}

QString QMUD::itemRarityToString(QMUD::ItemRarityType rarity)
{
    switch (rarity)
    {
    case QMUD::ItemRarityType::RARITY0:   return "rarity0";
    case QMUD::ItemRarityType::RARITY1:   return "rarity1";
    case QMUD::ItemRarityType::RARITY2:   return "rarity2";
    case QMUD::ItemRarityType::RARITY3:   return "rarity3";
    case QMUD::ItemRarityType::RARITY4:   return "rarity4";
    case QMUD::ItemRarityType::RARITY5:   return "rarity5";
    case QMUD::ItemRarityType::RARITY6:   return "rarity6";
    case QMUD::ItemRarityType::RARITY7:   return "rarity7";
    case QMUD::ItemRarityType::RARITY8:   return "rarity8";
    case QMUD::ItemRarityType::RARITY9:   return "rarity9";
    case QMUD::ItemRarityType::RARITY10:  return "rarity10";
    case QMUD::ItemRarityType::UNKNOWN:
        Q_ASSERT(false);
        return "rarity0";
    }

    Q_ASSERT(false);
    return "rarity0";
}

QMUD::ItemRarityType QMUD::itemRarityFromString(QString str)
{
    if (str.compare("rarity0", Qt::CaseInsensitive) == 0)       return QMUD::ItemRarityType::RARITY0;
    else if (str.compare("rarity1", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY1;
    else if (str.compare("rarity2", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY2;
    else if (str.compare("rarity3", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY3;
    else if (str.compare("rarity4", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY4;
    else if (str.compare("rarity5", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY5;
    else if (str.compare("rarity6", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY6;
    else if (str.compare("rarity7", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY7;
    else if (str.compare("rarity8", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY8;
    else if (str.compare("rarity9", Qt::CaseInsensitive) == 0)  return QMUD::ItemRarityType::RARITY9;
    else if (str.compare("rarity10", Qt::CaseInsensitive) == 0) return QMUD::ItemRarityType::RARITY10;
    else
    {
        return QMUD::ItemRarityType::UNKNOWN;
    }
}

bool QMUD::itemRarityCheck(QMUD::ItemRarityType rarity)
{
    switch (rarity)
    {
    case QMUD::ItemRarityType::RARITY0:   return true;
    case QMUD::ItemRarityType::RARITY1:   return std::rand() % 2 == 0;
    case QMUD::ItemRarityType::RARITY2:   return std::rand() % 5 == 0;
    case QMUD::ItemRarityType::RARITY3:   return std::rand() % 10 == 0;
    case QMUD::ItemRarityType::RARITY4:   return std::rand() % 20 == 0;
    case QMUD::ItemRarityType::RARITY5:   return std::rand() % 40 == 0;
    case QMUD::ItemRarityType::RARITY6:   return std::rand() % 80 == 0;
    case QMUD::ItemRarityType::RARITY7:   return std::rand() % 200 == 0;
    case QMUD::ItemRarityType::RARITY8:   return std::rand() % 500 == 0;
    case QMUD::ItemRarityType::RARITY9:   return std::rand() % 1000 == 0;
    case QMUD::ItemRarityType::RARITY10:  return std::rand() % 10000 == 0;
    case QMUD::ItemRarityType::UNKNOWN:
        Q_ASSERT(false);
        return false;
    }

    Q_ASSERT(false);
    return false;
}

QString QMUD::itemQualityToString(QMUD::ItemQualityType quality)
{
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:         return "junk";
        case QMUD::ItemQualityType::COMMON:       return "common";
        case QMUD::ItemQualityType::UNCOMMON:     return "uncommon";
        case QMUD::ItemQualityType::VALUABLE:     return "valuable";
        case QMUD::ItemQualityType::RARE:         return "rare";
        case QMUD::ItemQualityType::EPIC:         return "epic";
        case QMUD::ItemQualityType::LEGENDARY:    return "legendary";
    }

    Q_ASSERT(false);
    return "junk";
}

QMUD::ItemQualityType QMUD::itemQualityFromString(QString str)
{
    if (str.compare("junk", Qt::CaseInsensitive) == 0)              return QMUD::ItemQualityType::JUNK;
    else if (str.compare("common", Qt::CaseInsensitive) == 0)       return QMUD::ItemQualityType::COMMON;
    else if (str.compare("uncommon", Qt::CaseInsensitive) == 0)     return QMUD::ItemQualityType::UNCOMMON;
    else if (str.compare("valuable", Qt::CaseInsensitive) == 0)     return QMUD::ItemQualityType::VALUABLE;
    else if (str.compare("rare", Qt::CaseInsensitive) == 0)         return QMUD::ItemQualityType::RARE;
    else if (str.compare("epic", Qt::CaseInsensitive) == 0)         return QMUD::ItemQualityType::EPIC;
    else if (str.compare("legendary", Qt::CaseInsensitive) == 0)    return QMUD::ItemQualityType::LEGENDARY;
    else
    {
        return QMUD::ItemQualityType::JUNK;
    }
}

QString QMUD::itemQualityToReadableString(QMUD::ItemQualityType quality, bool withColor)
{
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:         return withColor ? QObject::tr("SPAZZATURA")        : QObject::tr("SPAZZATURA");
        case QMUD::ItemQualityType::COMMON:       return withColor ? QObject::tr("&WCOMUNE&d")        : QObject::tr("COMUNE");
        case QMUD::ItemQualityType::UNCOMMON:     return withColor ? QObject::tr("&WNON COMUNE&d")    : QObject::tr("NON COMUNE");
        case QMUD::ItemQualityType::VALUABLE:     return withColor ? QObject::tr("&GPREGIATO&d")      : QObject::tr("PREGIATO");
        case QMUD::ItemQualityType::RARE:         return withColor ? QObject::tr("&CRARO&d")          : QObject::tr("RARO");
        case QMUD::ItemQualityType::EPIC:         return withColor ? QObject::tr("&pEPICO&d")         : QObject::tr("EPICO");
        case QMUD::ItemQualityType::LEGENDARY:    return withColor ? QObject::tr("&YLEGGENDARIO&d")   : QObject::tr("LEGGENDARIO");
    }

    Q_ASSERT(false);
    return "";
}

QString QMUD::itemQualityToColor(QMUD::ItemQualityType quality)
{
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:         return "&w";
        case QMUD::ItemQualityType::COMMON:       return "&W";
        case QMUD::ItemQualityType::UNCOMMON:     return "&W";
        case QMUD::ItemQualityType::VALUABLE:     return "&G";
        case QMUD::ItemQualityType::RARE:         return "&C";
        case QMUD::ItemQualityType::EPIC:         return "&p";
        case QMUD::ItemQualityType::LEGENDARY:    return "&Y";
    }

    Q_ASSERT(false);
    return "&w";
}

QString QMUD::itemQualityToHTMLColor(QMUD::ItemQualityType quality)
{
    switch (quality)
    {
    case ItemQualityType::JUNK:
        return "#C2C2C2";
    case ItemQualityType::COMMON:
        return "#FFFFFF";
    case ItemQualityType::UNCOMMON:
        return "#FFFFFF";
    case ItemQualityType::VALUABLE:
        return "#00F000";
    case ItemQualityType::RARE:
        return "#098AAE";
    case ItemQualityType::EPIC:
        return "#E743D4";
    case ItemQualityType::LEGENDARY:
        return "#EDF00E";
    }

    return "";
}

QString QMUD::armorTypeToString(QMUD::ArmorType type)
{
    switch (type)
    {
    case ArmorType::CHEST: return "CHEST";
    case ArmorType::SHIELD: return "SHIELD";
    case ArmorType::GLOVES: return "GLOVES";
    case ArmorType::SHOULDER: return "SHOULDER";
    case ArmorType::BRACERS: return "BRACERS";
    case ArmorType::HELM: return "HELM";
    case ArmorType::LEG: return "LEG";
    case ArmorType::BOOTS: return "BOOTS";
    case ArmorType::UNKNOWN:
    case ArmorType::MAX_VALUE:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ArmorType QMUD::armorTypeFromString(QString str)
{
    if (str.compare("SHIELD", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::SHIELD;
    else if (str.compare("CHEST", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::CHEST;
    else if (str.compare("GLOVES", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::GLOVES;
    else if (str.compare("SHOULDER", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::SHOULDER;
    else if (str.compare("BRACERS", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::BRACERS;
    else if (str.compare("HELM", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::HELM;
    else if (str.compare("LEG", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::LEG;
    else if (str.compare("BOOTS", Qt::CaseInsensitive) == 0)  return QMUD::ArmorType::BOOTS;
    else
        return QMUD::ArmorType::UNKNOWN;
}

QString QMUD::armorTypeToReadableString(QMUD::ArmorType type)
{
    switch (type)
    {
    case ArmorType::CHEST: return "Armatura";
    case ArmorType::SHIELD: return "Scudo";
    case ArmorType::GLOVES: return "Guanti";
    case ArmorType::SHOULDER: return "Spalline";
    case ArmorType::BRACERS: return "Bracciali";
    case ArmorType::HELM: return "Elmo";
    case ArmorType::LEG: return "Gambali";
    case ArmorType::BOOTS: return "Stivali";
    case ArmorType::UNKNOWN:
    case ArmorType::MAX_VALUE:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QString QMUD::trinketTypeToString(QMUD::TrinketType type)
{
    switch (type)
    {
    case TrinketType::RING: return "ring";
    case TrinketType::NECKLACE: return "necklace";
    case TrinketType::BRACELET: return "bracelet";
    case TrinketType::MAX_VALUE:
    case TrinketType::UNKNOWN:
        Q_ASSERT(false);
        return "ring";
    }

    Q_ASSERT(false);
    return "ring";
}

QString QMUD::trinketTypeToReadableString(QMUD::TrinketType type)
{
    switch (type)
    {
    case TrinketType::RING: return "Anello";
    case TrinketType::NECKLACE: return "Collana";
    case TrinketType::BRACELET: return "Bracciale";
    case TrinketType::MAX_VALUE:
    case TrinketType::UNKNOWN:
        Q_ASSERT(false);
        return "Anello";
    }

    Q_ASSERT(false);
    return "Anello";
}

QMUD::TrinketType QMUD::trinketTypeFromString(QString str)
{
    if (str.compare("ring", Qt::CaseInsensitive) == 0)
        return TrinketType::RING;
    else if (str.compare("necklace", Qt::CaseInsensitive) == 0)
        return TrinketType::NECKLACE;
    else if (str.compare("bracelet", Qt::CaseInsensitive) == 0)
        return TrinketType::BRACELET;
    else
        return TrinketType::UNKNOWN;
}

QString QMUD::damageTypeToString(QMUD::DamageType type)
{
    switch (type)
    {
    case DamageType::HANDS: return "HANDS";
    case DamageType::SLASHING: return "SLASHING";
    case DamageType::BLUDGEONING: return "BLUDGEONING";
    case DamageType::PIERCING: return "PIERCING";
    case DamageType::ENERGY: return "ENERGY";
    case DamageType::FIRE: return "FIRE";
    case DamageType::DIVINE: return "DIVINE";
    case DamageType::ICE: return "ICE";
    }

    return "";
}

void QMUD::damageTypeJsRegistration(QQmlEngine &engine)
{
    engine.globalObject().setProperty("DAMAGE_HANDS", QJSValue(static_cast<int>(DamageType::HANDS)));
    engine.globalObject().setProperty("DAMAGE_SLASHING", QJSValue(static_cast<int>(DamageType::SLASHING)));
    engine.globalObject().setProperty("DAMAGE_BLUDGEONING", QJSValue(static_cast<int>(DamageType::BLUDGEONING)));
    engine.globalObject().setProperty("DAMAGE_PIERCING", QJSValue(static_cast<int>(DamageType::PIERCING)));
    engine.globalObject().setProperty("DAMAGE_ENERGY", QJSValue(static_cast<int>(DamageType::ENERGY)));
    engine.globalObject().setProperty("DAMAGE_FIRE", QJSValue(static_cast<int>(DamageType::FIRE)));
    engine.globalObject().setProperty("DAMAGE_DIVINE", QJSValue(static_cast<int>(DamageType::DIVINE)));
    engine.globalObject().setProperty("DAMAGE_ICE", QJSValue(static_cast<int>(DamageType::ICE)));
}


QString QMUD::weaponTypeToString(QMUD::WeaponType type)
{
    switch (type)
    {
    case QMUD::WeaponType::CLUB:             return "club";
    case QMUD::WeaponType::DAGGER:           return "dagger";
    case QMUD::WeaponType::TWO_HAND_CLUB:    return "two_hand_club";
    case QMUD::WeaponType::AXE:              return "axe";
    case QMUD::WeaponType::LIGHT_HAMMER:     return "light_hammer";
    case QMUD::WeaponType::MACE:             return "mace";
    case QMUD::WeaponType::QUARTERSTAFF:     return "quarterstaff";
    case QMUD::WeaponType::SICKLE:           return "sickle";
    case QMUD::WeaponType::TWO_HAND_AXE:     return "two_hand_axe";
    case QMUD::WeaponType::HALBERD:          return "halberd";
    case QMUD::WeaponType::LANCE:            return "lance";
    case QMUD::WeaponType::SHORT_SWORD:      return "short_sword";
    case QMUD::WeaponType::SWORD:            return "sword";
    case QMUD::WeaponType::LONG_SWORD:       return "long_sword";
    case QMUD::WeaponType::MORNINGSTAR:      return "morningstar";
    case QMUD::WeaponType::PIKE:             return "pike";
    case QMUD::WeaponType::RAPIER:           return "rapier";
    case QMUD::WeaponType::SCIMITAR:         return "scimitar";
    case QMUD::WeaponType::TRIDENT:          return "trident";
    case QMUD::WeaponType::WAR_HAMMER:       return "war_hammer";
    case QMUD::WeaponType::KATANA:           return "katana";
    case QMUD::WeaponType::UNKNOWN:
    case QMUD::WeaponType::MAX_VALUE:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::WeaponType QMUD::weaponTypeFromString(QString str)
{
    if (str.compare("club",         Qt::CaseInsensitive) == 0) return QMUD::WeaponType::CLUB;
    if (str.compare("dagger",       Qt::CaseInsensitive) == 0) return QMUD::WeaponType::DAGGER;
    if (str.compare("two_hand_club",Qt::CaseInsensitive) == 0) return QMUD::WeaponType::TWO_HAND_CLUB;
    if (str.compare("axe",          Qt::CaseInsensitive) == 0) return QMUD::WeaponType::AXE;
    if (str.compare("light_hammer", Qt::CaseInsensitive) == 0) return QMUD::WeaponType::LIGHT_HAMMER;
    if (str.compare("mace",         Qt::CaseInsensitive) == 0) return QMUD::WeaponType::MACE;
    if (str.compare("quarterstaff", Qt::CaseInsensitive) == 0) return QMUD::WeaponType::QUARTERSTAFF;
    if (str.compare("sickle",       Qt::CaseInsensitive) == 0) return QMUD::WeaponType::SICKLE;
    if (str.compare("two_hand_axe", Qt::CaseInsensitive) == 0) return QMUD::WeaponType::TWO_HAND_AXE;
    if (str.compare("halberd",      Qt::CaseInsensitive) == 0) return QMUD::WeaponType::HALBERD;
    if (str.compare("lance",        Qt::CaseInsensitive) == 0) return QMUD::WeaponType::LANCE;
    if (str.compare("short_sword",  Qt::CaseInsensitive) == 0) return QMUD::WeaponType::SHORT_SWORD;
    if (str.compare("sword",        Qt::CaseInsensitive) == 0) return QMUD::WeaponType::SWORD;
    if (str.compare("long_sword",   Qt::CaseInsensitive) == 0) return QMUD::WeaponType::LONG_SWORD;
    if (str.compare("morningstar",  Qt::CaseInsensitive) == 0) return QMUD::WeaponType::MORNINGSTAR;
    if (str.compare("pike",         Qt::CaseInsensitive) == 0) return QMUD::WeaponType::PIKE;
    if (str.compare("rapier",       Qt::CaseInsensitive) == 0) return QMUD::WeaponType::RAPIER;
    if (str.compare("scimitar",     Qt::CaseInsensitive) == 0) return QMUD::WeaponType::SCIMITAR;
    if (str.compare("trident",      Qt::CaseInsensitive) == 0) return QMUD::WeaponType::TRIDENT;
    if (str.compare("war_hammer",   Qt::CaseInsensitive) == 0) return QMUD::WeaponType::WAR_HAMMER;
    if (str.compare("katana",       Qt::CaseInsensitive) == 0) return QMUD::WeaponType::KATANA;
    else return QMUD::WeaponType::UNKNOWN;
}

QString QMUD::weaponTypeToReadableString(QMUD::WeaponType type)
{
    switch (type)
    {
    case QMUD::WeaponType::CLUB:             return "Clava";
    case QMUD::WeaponType::DAGGER:           return "Pugnale";
    case QMUD::WeaponType::TWO_HAND_CLUB:    return "Clava a due mani";
    case QMUD::WeaponType::AXE:              return "Ascia";
    case QMUD::WeaponType::LIGHT_HAMMER:     return "Martello";
    case QMUD::WeaponType::MACE:             return "Mazza";
    case QMUD::WeaponType::QUARTERSTAFF:     return "Bastone";
    case QMUD::WeaponType::SICKLE:           return "Falce";
    case QMUD::WeaponType::TWO_HAND_AXE:     return "Ascia a due mani";
    case QMUD::WeaponType::HALBERD:          return "Alabarda";
    case QMUD::WeaponType::LANCE:            return "Lancia";
    case QMUD::WeaponType::SHORT_SWORD:      return "Spada corta";
    case QMUD::WeaponType::SWORD:            return "Spada";
    case QMUD::WeaponType::LONG_SWORD:       return "Spada lunga";
    case QMUD::WeaponType::MORNINGSTAR:      return "Morningstar";
    case QMUD::WeaponType::PIKE:             return "Picca";
    case QMUD::WeaponType::RAPIER:           return "Stocco";
    case QMUD::WeaponType::SCIMITAR:         return "Scimitarra";
    case QMUD::WeaponType::TRIDENT:          return "Tridente";
    case QMUD::WeaponType::WAR_HAMMER:       return "Martello da guerra";
    case QMUD::WeaponType::KATANA:           return "Katana";
    case QMUD::WeaponType::UNKNOWN:
    case QMUD::WeaponType::MAX_VALUE:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

double QMUD::weaponTypeToDamageMultiplier(QMUD::WeaponType type)
{
    switch (type)
    {
    case QMUD::WeaponType::CLUB:             return 0.6;
    case QMUD::WeaponType::DAGGER:           return 0.5;
    case QMUD::WeaponType::TWO_HAND_CLUB:    return 1.4;
    case QMUD::WeaponType::AXE:              return 0.7;
    case QMUD::WeaponType::LIGHT_HAMMER:     return 0.7;
    case QMUD::WeaponType::MACE:             return 0.8;
    case QMUD::WeaponType::QUARTERSTAFF:     return 1.3;
    case QMUD::WeaponType::SICKLE:           return 1.0;
    case QMUD::WeaponType::TWO_HAND_AXE:     return 1.6;
    case QMUD::WeaponType::HALBERD:          return 1.0;
    case QMUD::WeaponType::LANCE:            return 0.8;
    case QMUD::WeaponType::SHORT_SWORD:      return 0.6;
    case QMUD::WeaponType::SWORD:            return 1.0;
    case QMUD::WeaponType::LONG_SWORD:       return 1.5;
    case QMUD::WeaponType::MORNINGSTAR:      return 0.8;
    case QMUD::WeaponType::PIKE:             return 1.0;
    case QMUD::WeaponType::RAPIER:           return 0.6;
    case QMUD::WeaponType::SCIMITAR:         return 0.6;
    case QMUD::WeaponType::TRIDENT:          return 1.1;
    case QMUD::WeaponType::WAR_HAMMER:       return 1.6;
    case QMUD::WeaponType::KATANA:           return 0.6;
    case QMUD::WeaponType::UNKNOWN:
    case QMUD::WeaponType::MAX_VALUE:
        Q_ASSERT(false);
        return 0.1;
    }

    Q_ASSERT(false);
    return 0.1;
}

double QMUD::weaponTypeToSpeedMultiplier(QMUD::WeaponType type)
{
    switch (type)
    {
    case QMUD::WeaponType::CLUB:             return 1.125;
    case QMUD::WeaponType::DAGGER:           return 1.375;
    case QMUD::WeaponType::TWO_HAND_CLUB:    return 0.75;
    case QMUD::WeaponType::AXE:              return 1.125;
    case QMUD::WeaponType::LIGHT_HAMMER:     return 1.125;
    case QMUD::WeaponType::MACE:             return 1;
    case QMUD::WeaponType::QUARTERSTAFF:     return 0.5;
    case QMUD::WeaponType::SICKLE:           return 1;
    case QMUD::WeaponType::TWO_HAND_AXE:     return 0.75;
    case QMUD::WeaponType::HALBERD:          return 1;
    case QMUD::WeaponType::LANCE:            return 1;
    case QMUD::WeaponType::SHORT_SWORD:      return 1.25;
    case QMUD::WeaponType::SWORD:            return 1;
    case QMUD::WeaponType::LONG_SWORD:       return 0.75;
    case QMUD::WeaponType::MORNINGSTAR:      return 1;
    case QMUD::WeaponType::PIKE:             return 1;
    case QMUD::WeaponType::RAPIER:           return 1.25;
    case QMUD::WeaponType::SCIMITAR:         return 1.25;
    case QMUD::WeaponType::TRIDENT:          return 0.875;
    case QMUD::WeaponType::WAR_HAMMER:       return 0.75;
    case QMUD::WeaponType::KATANA:           return 1.25;
    case QMUD::WeaponType::UNKNOWN:
    case QMUD::WeaponType::MAX_VALUE:
        Q_ASSERT(false);
        return 0.1;
    }

    Q_ASSERT(false);
    return 0.1;
}

QMUD::DamageType QMUD::weaponTypeToDamageType(QMUD::WeaponType type)
{
    switch (type)
    {
    case QMUD::WeaponType::CLUB:             return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::DAGGER:           return QMUD::DamageType::PIERCING;
    case QMUD::WeaponType::TWO_HAND_CLUB:    return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::AXE:              return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::LIGHT_HAMMER:     return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::MACE:             return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::QUARTERSTAFF:     return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::SICKLE:           return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::TWO_HAND_AXE:     return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::HALBERD:          return QMUD::DamageType::PIERCING;
    case QMUD::WeaponType::LANCE:            return QMUD::DamageType::PIERCING;
    case QMUD::WeaponType::SHORT_SWORD:      return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::SWORD:            return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::LONG_SWORD:       return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::MORNINGSTAR:      return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::PIKE:             return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::RAPIER:           return QMUD::DamageType::PIERCING;
    case QMUD::WeaponType::SCIMITAR:         return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::TRIDENT:          return QMUD::DamageType::PIERCING;
    case QMUD::WeaponType::WAR_HAMMER:       return QMUD::DamageType::BLUDGEONING;
    case QMUD::WeaponType::KATANA:           return QMUD::DamageType::SLASHING;
    case QMUD::WeaponType::UNKNOWN:
    case QMUD::WeaponType::MAX_VALUE:
        Q_ASSERT(false);
        return QMUD::DamageType::SLASHING;
    }

    Q_ASSERT(false);
    return QMUD::DamageType::SLASHING;
}

QMUD::ItemBaseBonus::ItemBaseBonus() :
    m_iConstitution(0),
    m_iDexterity(0),
    m_iIntelligence(0),
    m_iWisdom(0),
    m_iCharisma(0),
    m_iStrength(0),
    m_iHit(0),
    m_iDamage(0),
    m_iHitPoints(0),
    m_iManaPoints(0),
    m_iActionPoints(0)
{

}

bool QMUD::ItemBaseBonus::isValid() const
{
    return m_iConstitution != 0 ||
            m_iDexterity != 0 ||
            m_iIntelligence != 0 ||
            m_iWisdom != 0 ||
            m_iCharisma != 0 ||
            m_iStrength != 0 ||
            m_iHit != 0 ||
            m_iDamage != 0 ||
            m_iHitPoints != 0 ||
            m_iManaPoints != 0 ||
            m_iActionPoints;
}

bool QMUD::ItemBaseBonus::load(const QJsonObject &obj)
{
    QJsonObject::const_iterator it;

    if ((it = obj.find("constitution")) != obj.end() && it.value().isDouble()) m_iConstitution = it.value().toInt();
    if ((it = obj.find("dexterity")) != obj.end() && it.value().isDouble()) m_iDexterity = it.value().toInt();
    if ((it = obj.find("intelligence")) != obj.end() && it.value().isDouble()) m_iIntelligence = it.value().toInt();
    if ((it = obj.find("wisdom")) != obj.end() && it.value().isDouble()) m_iWisdom = it.value().toInt();
    if ((it = obj.find("charisma")) != obj.end() && it.value().isDouble()) m_iCharisma = it.value().toInt();
    if ((it = obj.find("strength")) != obj.end() && it.value().isDouble()) m_iStrength = it.value().toInt();
    if ((it = obj.find("hit")) != obj.end() && it.value().isDouble()) m_iHit = it.value().toInt();
    if ((it = obj.find("damage")) != obj.end() && it.value().isDouble()) m_iDamage = it.value().toInt();
    if ((it = obj.find("hitPoints")) != obj.end() && it.value().isDouble()) m_iHitPoints = it.value().toInt();
    if ((it = obj.find("manaPoints")) != obj.end() && it.value().isDouble()) m_iManaPoints = it.value().toInt();
    if ((it = obj.find("actionPoints")) != obj.end() && it.value().isDouble()) m_iActionPoints = it.value().toInt();

    return true;
}

void QMUD::ItemBaseBonus::write(QJsonObject &obj) const
{
    if (m_iConstitution != 0)  obj["constitution"] =  m_iConstitution;
    if (m_iDexterity != 0)     obj["dexterity"] =     m_iDexterity;
    if (m_iIntelligence != 0)  obj["intelligence"] =  m_iIntelligence;
    if (m_iWisdom != 0)        obj["wisdom"] =        m_iWisdom;
    if (m_iCharisma != 0)      obj["charisma"] =      m_iCharisma;
    if (m_iStrength != 0)      obj["strength"] =      m_iStrength;
    if (m_iHit != 0)           obj["hit"] =           m_iHit;
    if (m_iDamage != 0)        obj["damage"] =        m_iDamage;
    if (m_iHitPoints != 0)     obj["hitPoints"] =     m_iHitPoints;
    if (m_iManaPoints != 0)    obj["manaPoints"] =    m_iManaPoints;
    if (m_iActionPoints != 0)  obj["actionPoints"] =  m_iActionPoints;
}

int QMUD::ItemBaseBonus::manaPoints() const
{
    return m_iManaPoints;
}

int QMUD::ItemBaseBonus::hitPoints() const
{
    return m_iHitPoints;
}

int QMUD::ItemBaseBonus::actionPoints() const
{
    return m_iActionPoints;
}

void QMUD::ItemBaseBonus::setConstitution(int value)
{
    m_iConstitution = value;
}

void QMUD::ItemBaseBonus::setDexterity(int value)
{
    m_iDexterity = value;
}

void QMUD::ItemBaseBonus::setIntelligence(int value)
{
    m_iIntelligence = value;
}

void QMUD::ItemBaseBonus::setWisdom(int value)
{
    m_iWisdom = value;
}

void QMUD::ItemBaseBonus::setCharisma(int value)
{
    m_iCharisma = value;
}

void QMUD::ItemBaseBonus::setStrength(int value)
{
    m_iStrength = value;
}

void QMUD::ItemBaseBonus::setHit(int value)
{
    m_iHit = value;
}

void QMUD::ItemBaseBonus::setDamage(int value)
{
    m_iDamage = value;
}

void QMUD::ItemBaseBonus::setHitPoints(int value)
{
    m_iHitPoints = value;
}

void QMUD::ItemBaseBonus::setManaPoints(int value)
{
    m_iManaPoints = value;
}

void QMUD::ItemBaseBonus::setActionPoints(int value)
{
    m_iActionPoints = value;
}

int QMUD::ItemBaseBonus::damage() const
{
    return m_iDamage;
}

int QMUD::ItemBaseBonus::hit() const
{
    return m_iHit;
}

int QMUD::ItemBaseBonus::strength() const
{
    return m_iStrength;
}

int QMUD::ItemBaseBonus::charisma() const
{
    return m_iCharisma;
}

int QMUD::ItemBaseBonus::wisdom() const
{
    return m_iWisdom;
}

int QMUD::ItemBaseBonus::intelligence() const
{
    return m_iIntelligence;
}

int QMUD::ItemBaseBonus::dexterity() const
{
    return m_iDexterity;
}

int QMUD::ItemBaseBonus::constitution() const
{
    return m_iConstitution;
}

QMUD::ItemBaseStatus::ItemBaseStatus()
{

}

bool QMUD::ItemBaseStatus::isValid() const
{
    return !m_vTemporaryStatus.isEmpty();
}

QVector<QMUD::TemporaryStatusType> QMUD::ItemBaseStatus::temporaryStatus() const
{
    return m_vTemporaryStatus;
}

void QMUD::ItemBaseStatus::setTemporaryStatus(QVector<QMUD::TemporaryStatusType> status)
{
    m_vTemporaryStatus = status;
}

bool QMUD::ItemBaseStatus::load(const QJsonObject &obj)
{
    auto objStatus = obj.find("status");

    if (objStatus != obj.end())
    {
        if (!objStatus->isArray())
            return false;

        auto array = objStatus->toArray();

        for (auto elem : array)
        {
            if (elem.isString())
            {
                bool ok;
                QMUD::TemporaryStatusType status = QMUD::statusFromString(elem.toString(), ok);
                if (ok)
                    m_vTemporaryStatus.push_back(status);
                else
                    return false;
            }
            else
                return false;
        }
    }

    return true;
}

void QMUD::ItemBaseStatus::write(QJsonObject &obj) const
{
    if (!m_vTemporaryStatus.isEmpty())
    {
        QJsonArray statusArray;
        for (auto status : m_vTemporaryStatus)
            statusArray.push_back(QMUD::statusToString(status));

        obj["status"] = statusArray;
    }
}


QString QMUD::abilityToReadableString(QMUD::AbilityType ability)
{
    switch (ability)
    {
    case AbilityType::CHARGE:   return QObject::tr("carica");
    case AbilityType::KICK:     return QObject::tr("calcio");
    case AbilityType::BACKSTAB: return QObject::tr("pugnala");
    case AbilityType::PARRY:    return QObject::tr("para");
    case AbilityType::DODGE:    return QObject::tr("schiva");
    case AbilityType::UNKNOWN:
    case AbilityType::MAX_VALUE:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QString QMUD::spellToReadableString(QMUD::SpellType spell)
{
    switch (spell)
    {
    case SpellType::CURE:                 return QObject::tr("cura");
    case SpellType::MAGIC_MISSILE:        return QObject::tr("dardo incantato");
    case SpellType::SHIELD:               return QObject::tr("scudo");
    case SpellType::STRENGHT:             return QObject::tr("forza");
    case SpellType::SANCTIFY:             return QObject::tr("santificazione");
    case SpellType::FIREBALL:             return QObject::tr("palla di fuoco");
    case SpellType::BURNING_HANDS:        return QObject::tr("mani brucianti");
    case SpellType::DIVINE_SHIELD:        return QObject::tr("scudo divino");
    case SpellType::INFLICTING_WOUNDS:    return QObject::tr("infliggi ferite");
    case SpellType::FLY:                  return QObject::tr("volo");
    case SpellType::INVISIBILITY:         return QObject::tr("invisibilita");
    case SpellType::SEE_INVISIBILITY:     return QObject::tr("vedi invisibile");
    case SpellType::FIRE_SHIELD:          return QObject::tr("scudo di fuoco");
    case SpellType::ICE_SHIELD:           return QObject::tr("scudo di ghiaccio");
    case SpellType::BLADES_SHIELD:        return QObject::tr("scudo di lame");
    case SpellType::FIERY_ARROW:          return QObject::tr("freccia infuocata");
    case SpellType::ICE_STORM:            return QObject::tr("tempesta di ghiaccio");
    case SpellType::ILLUSORY_IMAGES:      return QObject::tr("immagini illusorie");
    case SpellType::UNKNOWN:
    case SpellType::MAX_VALUE:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::SpellType QMUD::spellFromString(QString str)
{
    if (spellToReadableString(SpellType::CURE).startsWith(str))
        return SpellType::CURE;
    else if (spellToReadableString(SpellType::MAGIC_MISSILE).startsWith(str))
        return SpellType::MAGIC_MISSILE;
    else if (spellToReadableString(SpellType::SHIELD).startsWith(str))
        return SpellType::SHIELD;
    else if (spellToReadableString(SpellType::SANCTIFY).startsWith(str))
        return SpellType::SANCTIFY;
    else if (spellToReadableString(SpellType::STRENGHT).startsWith(str))
        return SpellType::STRENGHT;
    else if (spellToReadableString(SpellType::FIREBALL).startsWith(str))
        return SpellType::FIREBALL;
    else if (spellToReadableString(SpellType::BURNING_HANDS).startsWith(str))
        return SpellType::BURNING_HANDS;
    else if (spellToReadableString(SpellType::DIVINE_SHIELD).startsWith(str))
        return SpellType::DIVINE_SHIELD;
    else if (spellToReadableString(SpellType::INFLICTING_WOUNDS).startsWith(str))
        return SpellType::INFLICTING_WOUNDS;
    else if (spellToReadableString(SpellType::FLY).startsWith(str))
        return SpellType::FLY;
    else if (spellToReadableString(SpellType::INVISIBILITY).startsWith(str))
        return SpellType::INVISIBILITY;
    else if (spellToReadableString(SpellType::SEE_INVISIBILITY).startsWith(str))
        return SpellType::SEE_INVISIBILITY;
    else if (spellToReadableString(SpellType::FIRE_SHIELD).startsWith(str))
        return SpellType::FIRE_SHIELD;
    else if (spellToReadableString(SpellType::ICE_SHIELD).startsWith(str))
        return SpellType::ICE_SHIELD;
    else if (spellToReadableString(SpellType::BLADES_SHIELD).startsWith(str))
        return SpellType::BLADES_SHIELD;
    else if (spellToReadableString(SpellType::FIERY_ARROW).startsWith(str))
        return SpellType::FIERY_ARROW;
    else if (spellToReadableString(SpellType::ICE_STORM).startsWith(str))
        return SpellType::ICE_STORM;
    else if (spellToReadableString(SpellType::ILLUSORY_IMAGES).startsWith(str))
        return SpellType::ILLUSORY_IMAGES;

    return SpellType::UNKNOWN;
}

QMUD::NpcCharacterFlagType QMUD::npcCharacterFlagTypeFromString(QString str)
{
    if (str.compare("autoia_combat", Qt::CaseInsensitive) == 0)                    return NpcCharacterFlagType::AUTOIA_COMBAT;
    else if (str.compare("aggressive", Qt::CaseInsensitive) == 0)                  return NpcCharacterFlagType::AGGRESSIVE;
    else if (str.compare("patrol", Qt::CaseInsensitive) == 0)                      return NpcCharacterFlagType::PATROL;
    else if (str.compare("patrol_wild_zone_small", Qt::CaseInsensitive) == 0)      return NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL;
    else if (str.compare("patrol_wild_zone_large", Qt::CaseInsensitive) == 0)      return NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE;
    else if (str.compare("hunt_map", Qt::CaseInsensitive) == 0)                    return NpcCharacterFlagType::HUNT_MAP;
    else if (str.compare("assist", Qt::CaseInsensitive) == 0)                      return NpcCharacterFlagType::ASSIST;
    else return NpcCharacterFlagType::UNKNOWN;
}

QString QMUD::npcCharacterFlagTypeToString(NpcCharacterFlagType flag)
{
    switch (flag)
    {
    case NpcCharacterFlagType::AUTOIA_COMBAT:               return "autoia_combat";
    case NpcCharacterFlagType::AGGRESSIVE:                  return "aggressive";
    case NpcCharacterFlagType::PATROL:                      return "patrol";
    case NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL:      return "patrol_wild_zone_small";
    case NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE:      return "patrol_wild_zone_large";
    case NpcCharacterFlagType::HUNT_MAP:                    return "hunt_map";
    case NpcCharacterFlagType::ASSIST:                      return "assist";
    case NpcCharacterFlagType::UNKNOWN:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::NpcType QMUD::npcTypeFromString(QString str)
{
    if (str.compare("normal", Qt::CaseInsensitive) == 0)
        return QMUD::NpcType::NORMAL;
    else if (str.compare("elite", Qt::CaseInsensitive) == 0)
        return QMUD::NpcType::ELITE;
    else if (str.compare("rare", Qt::CaseInsensitive) == 0)
        return QMUD::NpcType::RARE;
    else if (str.compare("legendary", Qt::CaseInsensitive) == 0)
        return QMUD::NpcType::LEGENDARY;
    else if (str.compare("bossA", Qt::CaseInsensitive) == 0)
        return QMUD::NpcType::BOSS_A;
    else if (str.compare("bossB", Qt::CaseInsensitive) == 0)
        return QMUD::NpcType::BOSS_B;
    else
        return QMUD::NpcType::UNKNOWN;
}

QString QMUD::npcTypeToString(NpcType type)
{
    switch (type)
    {
    case QMUD::NpcType::NORMAL:     return "normal";
    case QMUD::NpcType::ELITE:      return "elite";
    case QMUD::NpcType::RARE:       return "rare";
    case QMUD::NpcType::LEGENDARY:  return "legendary";
    case QMUD::NpcType::BOSS_A:     return "bossA";
    case QMUD::NpcType::BOSS_B:     return "bossB";
    case QMUD::NpcType::UNKNOWN:
    case QMUD::NpcType::MAX_VALUE:
        return "normal";
    }

    Q_ASSERT(false);
    return "normal";
}

int QMUD::npcTypeToLootPercentage(QMUD::NpcType type)
{
    switch (type)
    {
    case QMUD::NpcType::NORMAL:     return 50;
    case QMUD::NpcType::ELITE:      return 100;
    case QMUD::NpcType::RARE:       return 100;
    case QMUD::NpcType::LEGENDARY:  return 100;
    case QMUD::NpcType::BOSS_A:     return 100;
    case QMUD::NpcType::BOSS_B:     return 100;
    case QMUD::NpcType::UNKNOWN:
    case QMUD::NpcType::MAX_VALUE:
        return 0;
    }

    Q_ASSERT(false);
    return 0;
}

QString QMUD::sexTypeToString(QMUD::SexType type)
{
    if (type == SexType::MALE)
        return "male";
    else if (type == SexType::FEMALE)
        return "female";

    return "";
}

QMUD::SexType QMUD::sexTypeFromString(QString str)
{
    if (str.compare("male", Qt::CaseInsensitive) == 0)
        return SexType::MALE;
    else if (str.compare("female", Qt::CaseInsensitive) == 0)
        return SexType::FEMALE;

    return SexType::UNKNOWN;
}

void QMUD::sexTypeJsRegistration(QQmlEngine& engine)
{
    engine.globalObject().setProperty("SEX_MALE", QJSValue(static_cast<int>(SexType::MALE)));
    engine.globalObject().setProperty("SEX_FEMALE", QJSValue(static_cast<int>(SexType::FEMALE)));
}

void QMUD::raceTypeJsRegistration(QQmlEngine &engine)
{
    for (int i = 0; i < static_cast<int>(RaceType::RACE_TYPE_MAX); ++i)
        engine.globalObject().setProperty(raceJsString[i], QJSValue(i));
}

QMUD::RaceType QMUD::Race::fromString(QString str, bool *isOk)
{
    if (isOk)
        *isOk = true;

    for (int i = 0; i < static_cast<int>(RaceType::RACE_TYPE_MAX); ++i)
        if (raceString[i].compare(str, Qt::CaseInsensitive) == 0)
            return static_cast<RaceType>(i);

    if (isOk)
        *isOk = false;

    return RaceType::HUMAN;
}

QString QMUD::Race::toString(QMUD::RaceType race)
{
    return raceString[static_cast<int>(race)];
}

QString QMUD::Race::toReadableString(QMUD::RaceType race)
{
    return raceReadableString[static_cast<int>(race)];
}

int QMUD::Race::hitDiceBonus(QMUD::RaceType race)
{
    if (race == RaceType::HUMAN)            return 1;
    else if (race == RaceType::DWARF)       return 1;
    else if (race == RaceType::GNOME)       return 0;
    else if (race == RaceType::HALF_ORC)    return 2;
    else if (race == RaceType::HIGH_ELF)    return 1;
    else if (race == RaceType::HALF_ELF)    return 1;
    else if (race < RaceType::RACE_TYPE_PC_MAX)
    {
        Q_ASSERT(0);
        return 0;
    }
    else
        return 0;
}

int QMUD::Race::manaDiceBonus(QMUD::RaceType race)
{
    if (race == RaceType::HUMAN)            return 1;
    else if (race == RaceType::DWARF)       return 1;
    else if (race == RaceType::GNOME)       return 2;
    else if (race == RaceType::HALF_ORC)    return 0;
    else if (race == RaceType::HIGH_ELF)    return 1;
    else if (race == RaceType::HALF_ELF)    return 1;
    else if (race < RaceType::RACE_TYPE_PC_MAX)
    {
        Q_ASSERT(0);
        return 0;
    }
    else
        return 0;
}

int QMUD::Race::basePCInt(QMUD::RaceType race)
{
    if (race < RaceType::RACE_TYPE_PC_MAX)
        return raceStatsPC[static_cast<int>(race)][0];
    else
        return 6;
}

int QMUD::Race::basePCWis(QMUD::RaceType race)
{
    if (race < RaceType::RACE_TYPE_PC_MAX)
        return raceStatsPC[static_cast<int>(race)][1];
    else
        return 6;
}

int QMUD::Race::basePCCon(QMUD::RaceType race)
{
    if (race < RaceType::RACE_TYPE_PC_MAX)
        return raceStatsPC[static_cast<int>(race)][2];
    else
        return 6;
}

int QMUD::Race::basePCStr(QMUD::RaceType race)
{
    if (race < RaceType::RACE_TYPE_PC_MAX)
        return raceStatsPC[static_cast<int>(race)][3];
    else
        return 6;
}

int QMUD::Race::basePCDex(QMUD::RaceType race)
{
    if (race < RaceType::RACE_TYPE_PC_MAX)
        return raceStatsPC[static_cast<int>(race)][4];
    else
        return 6;
}

int QMUD::Race::baseNPCInt(QMUD::RaceType race)
{
    return raceStatsNPC[static_cast<int>(race)][0];
}

int QMUD::Race::baseNPCWis(QMUD::RaceType race)
{
    return raceStatsNPC[static_cast<int>(race)][1];
}

int QMUD::Race::baseNPCCon(QMUD::RaceType race)
{
    return raceStatsNPC[static_cast<int>(race)][2];
}

int QMUD::Race::baseNPCStr(QMUD::RaceType race)
{
    return raceStatsNPC[static_cast<int>(race)][3];
}

int QMUD::Race::baseNPCDex(QMUD::RaceType race)
{
    return raceStatsNPC[static_cast<int>(race)][4];
}

QMUD::DamageType QMUD::Race::baseDamageType(QMUD::RaceType race)
{
    return raceDamageType[static_cast<int>(race)];
}

QString QMUD::classTypeToString(QMUD::ClassType type)
{
    switch(type)
    {
    case ClassType::WARRIOR:    return "warrior";
    case ClassType::MAGE:       return "mage";
    case ClassType::BARBARIAN:  return "barbarian";
    case ClassType::DRUID:      return "druid";
    case ClassType::MONK:       return "monk";
    case ClassType::PRIEST:     return "priest";
    case ClassType::ROGUE:      return "rogue";
    case ClassType::UNKNOWN:
    case ClassType::MAX_VALUE:
        Q_ASSERT(false);
        return "warrior";
    }

    Q_ASSERT(false);
    return "warrior";
}

QMUD::ClassType QMUD::classTypeFromString(QString str)
{
    if (str.compare("warrior", Qt::CaseInsensitive) == 0)   return ClassType::WARRIOR;
    else if (str.compare("mage", Qt::CaseInsensitive) == 0)      return ClassType::MAGE;
    else if (str.compare("barbarian", Qt::CaseInsensitive) == 0) return ClassType::BARBARIAN;
    else if (str.compare("druid", Qt::CaseInsensitive) == 0)     return ClassType::DRUID;
    else if (str.compare("monk", Qt::CaseInsensitive) == 0)      return ClassType::MONK;
    else if (str.compare("priest", Qt::CaseInsensitive) == 0)    return ClassType::PRIEST;
    else if (str.compare("rogue", Qt::CaseInsensitive) == 0)     return ClassType::ROGUE;
    else return QMUD::ClassType::UNKNOWN;
}

QString QMUD::classTypeToReadableString(ClassType type)
{
    switch(type)
    {
    case ClassType::WARRIOR:    return QObject::tr("Guerriero");
    case ClassType::MAGE:       return QObject::tr("Mago");
    case ClassType::BARBARIAN:  return QObject::tr("Barbaro");
    case ClassType::DRUID:      return QObject::tr("Druido");
    case ClassType::MONK:       return QObject::tr("Monaco");
    case ClassType::PRIEST:     return QObject::tr("Chierico");
    case ClassType::ROGUE:      return QObject::tr("Ladro");
    case ClassType::UNKNOWN:
    case ClassType::MAX_VALUE:
        Q_ASSERT(false);
        return "";
    }

    Q_ASSERT(false);
    return "";
}

bool QMUD::classTypeIsCaster(QMUD::ClassType type)
{
    switch(type)
    {
    case ClassType::MAGE:
    case ClassType::PRIEST:
    case ClassType::DRUID:
        return true;

    case ClassType::WARRIOR:
    case ClassType::ROGUE:
    case ClassType::BARBARIAN:
    case ClassType::MONK:
        return false;

    case ClassType::UNKNOWN:
    case ClassType::MAX_VALUE:
        return false;
    }

    Q_ASSERT(false);
    return false;
}

void QMUD::classTypeJsRegistration(QQmlEngine &engine)
{
    engine.globalObject().setProperty("CLASS_BARBARIAN", QJSValue(static_cast<int>(ClassType::BARBARIAN)));
    engine.globalObject().setProperty("CLASS_DRUID", QJSValue(static_cast<int>(ClassType::DRUID)));
    engine.globalObject().setProperty("CLASS_MONK", QJSValue(static_cast<int>(ClassType::MONK)));
    engine.globalObject().setProperty("CLASS_PRIEST", QJSValue(static_cast<int>(ClassType::PRIEST)));
    engine.globalObject().setProperty("CLASS_ROGUE", QJSValue(static_cast<int>(ClassType::ROGUE)));
    engine.globalObject().setProperty("CLASS_WARRIOR", QJSValue(static_cast<int>(ClassType::WARRIOR)));
    engine.globalObject().setProperty("CLASS_MAGE", QJSValue(static_cast<int>(ClassType::MAGE)));
}

QMUD::ItemClassType QMUD::itemClassTypeFromChClass(QMUD::ClassType classType)
{
    switch (classType)
    {
    case QMUD::ClassType::WARRIOR:  return QMUD::ItemClassType::DPS_STR;
    case QMUD::ClassType::MAGE:     return QMUD::ItemClassType::CASTER_INT;
    case QMUD::ClassType::ROGUE:    return QMUD::ItemClassType::DPS_DEX;
    case QMUD::ClassType::PRIEST:   return QMUD::ItemClassType::CASTER_WIS;
    case QMUD::ClassType::DRUID:    return QMUD::ItemClassType::CASTER_WIS;
    case QMUD::ClassType::BARBARIAN:return QMUD::ItemClassType::TANK;
    case QMUD::ClassType::MONK:     return QMUD::ItemClassType::DPS_STR;
    case ClassType::UNKNOWN:
    case ClassType::MAX_VALUE:
        Q_ASSERT(false);
        return QMUD::ItemClassType::UNKNOWN;
    }

    Q_ASSERT(false);
    return QMUD::ItemClassType::UNKNOWN;
}

void QMUD::classTypeToMaxStatistics(QMUD::ClassType classType,
                                    int& maxInt,
                                    int& maxWis,
                                    int& maxStr,
                                    int& maxCon,
                                    int& maxDex)
{
    if (classType == QMUD::ClassType::WARRIOR)
    {
        maxInt = 16;
        maxWis = 16;
        maxStr = 18;
        maxCon = 18;
        maxDex = 16;
    }
    else if (classType == QMUD::ClassType::MAGE)
    {
        maxInt = 18;
        maxWis = 18;
        maxStr = 16;
        maxCon = 16;
        maxDex = 16;
    }
    else if (classType == QMUD::ClassType::BARBARIAN)
    {
        maxInt = 16;
        maxWis = 16;
        maxStr = 18;
        maxCon = 18;
        maxDex = 16;
    }
    else if (classType == QMUD::ClassType::DRUID)
    {
        maxInt = 18;
        maxWis = 18;
        maxStr = 16;
        maxCon = 16;
        maxDex = 16;
    }
    else if (classType == QMUD::ClassType::MONK)
    {
        maxInt = 16;
        maxWis = 16;
        maxStr = 18;
        maxCon = 16;
        maxDex = 18;
    }
    else if (classType == QMUD::ClassType::PRIEST)
    {
        maxInt = 18;
        maxWis = 18;
        maxStr = 16;
        maxCon = 16;
        maxDex = 16;
    }
    else if (classType == QMUD::ClassType::ROGUE)
    {
        maxInt = 16;
        maxWis = 16;
        maxStr = 18;
        maxCon = 16;
        maxDex = 18;
    }
    else
    {
        maxInt = 16;
        maxWis = 16;
        maxStr = 16;
        maxCon = 16;
        maxDex = 16;

        Q_ASSERT(false);
    }
}

QString QMUD::statusToReadableString(QMUD::TemporaryStatusType status)
{
    switch(status)
    {
    case TemporaryStatusType::ACTIONLAG:          return QObject::tr("lag");
    case TemporaryStatusType::STUNNED:            return QObject::tr("stordito");
    case TemporaryStatusType::BLINDED:            return QObject::tr("accecato");
    case TemporaryStatusType::SANCTIFY:           return QObject::tr("santificazione");
    case TemporaryStatusType::SHIELD:             return QObject::tr("scudo");
    case TemporaryStatusType::STRENGHT:           return QObject::tr("forza");
    case TemporaryStatusType::DIVINE_SHIELD:      return QObject::tr("scudo divino");
    case TemporaryStatusType::FLY:                return QObject::tr("volo");
    case TemporaryStatusType::INVISIBILITY:       return QObject::tr("invisibilita");
    case TemporaryStatusType::SEE_INVISIBILITY:   return QObject::tr("vedi invisibile");
    case TemporaryStatusType::FIRE_SHIELD:        return QObject::tr("scudo di fuoco");
    case TemporaryStatusType::ICE_SHIELD:         return QObject::tr("scudo di ghiaccio");
    case TemporaryStatusType::BLADES_SHIELD:      return QObject::tr("scudo di lame");
    case TemporaryStatusType::ILLUSORY_IMAGES_1:  return QObject::tr("immagini illusorie (1)");
    case TemporaryStatusType::ILLUSORY_IMAGES_2:  return QObject::tr("immagini illusorie (2)");
    case TemporaryStatusType::ILLUSORY_IMAGES_3:  return QObject::tr("immagini illusorie (3)");
    case TemporaryStatusType::ILLUSORY_IMAGES_4:  return QObject::tr("immagini illusorie (4)");
    case TemporaryStatusType::ILLUSORY_IMAGES_5:  return QObject::tr("immagini illusorie (5)");
    case TemporaryStatusType::UNKNOWN:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QString QMUD::statusToString(QMUD::TemporaryStatusType status)
{
    switch(status)
    {
    case TemporaryStatusType::ACTIONLAG:          return "action_lag";
    case TemporaryStatusType::STUNNED:            return "stunned";
    case TemporaryStatusType::BLINDED:            return "blinded";
    case TemporaryStatusType::SANCTIFY:           return "sanctify";
    case TemporaryStatusType::SHIELD:             return "shield";
    case TemporaryStatusType::STRENGHT:           return "strenght";
    case TemporaryStatusType::DIVINE_SHIELD:      return "divine_shield";
    case TemporaryStatusType::FLY:                return "fly";
    case TemporaryStatusType::INVISIBILITY:       return "invisibility";
    case TemporaryStatusType::SEE_INVISIBILITY:   return "see_invisibility";
    case TemporaryStatusType::FIRE_SHIELD:        return "fire_shield";
    case TemporaryStatusType::ICE_SHIELD:         return "ice_shield";
    case TemporaryStatusType::BLADES_SHIELD:      return "blades_shield";
    case TemporaryStatusType::ILLUSORY_IMAGES_1:  return "illusory_images_1";
    case TemporaryStatusType::ILLUSORY_IMAGES_2:  return "illusory_images_2";
    case TemporaryStatusType::ILLUSORY_IMAGES_3:  return "illusory_images_3";
    case TemporaryStatusType::ILLUSORY_IMAGES_4:  return "illusory_images_4";
    case TemporaryStatusType::ILLUSORY_IMAGES_5:  return "illusory_images_5";
    case TemporaryStatusType::UNKNOWN:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::TemporaryStatusType QMUD::statusFromString(QString status, bool& ok)
{
    ok = true;

    if (status == "action_lag")         return TemporaryStatusType::ACTIONLAG;
    if (status == "stunned")            return TemporaryStatusType::STUNNED;
    if (status == "blinded")            return TemporaryStatusType::BLINDED;
    if (status == "sanctify")           return TemporaryStatusType::SANCTIFY;
    if (status == "shield")             return TemporaryStatusType::SHIELD;
    if (status == "strenght")           return TemporaryStatusType::STRENGHT;
    if (status == "divine_shield")      return TemporaryStatusType::DIVINE_SHIELD;
    if (status == "fly")                return TemporaryStatusType::FLY;
    if (status == "invisibility")       return TemporaryStatusType::INVISIBILITY;
    if (status == "see_invisibility")   return TemporaryStatusType::SEE_INVISIBILITY;
    if (status == "fire_shield")        return TemporaryStatusType::FIRE_SHIELD;
    if (status == "ice_shield")         return TemporaryStatusType::ICE_SHIELD;
    if (status == "blades_shield")      return TemporaryStatusType::BLADES_SHIELD;
    if (status == "illusory_images_1")  return TemporaryStatusType::ILLUSORY_IMAGES_1;
    if (status == "illusory_images_2")  return TemporaryStatusType::ILLUSORY_IMAGES_2;
    if (status == "illusory_images_3")  return TemporaryStatusType::ILLUSORY_IMAGES_3;
    if (status == "illusory_images_4")  return TemporaryStatusType::ILLUSORY_IMAGES_4;
    if (status == "illusory_images_5")  return TemporaryStatusType::ILLUSORY_IMAGES_5;

    ok = false;
    return TemporaryStatusType::UNKNOWN;
}

QMUD::PcCharacterBaseInfo::PcCharacterBaseInfo() :
    m_eRace(QMUD::RaceType::HUMAN),
    m_eClassType(QMUD::ClassType::WARRIOR),
    m_iLevel(-1)
{

}

QMUD::PcCharacterBaseInfo::PcCharacterBaseInfo(QString name, QMUD::RaceType race, QMUD::ClassType classType, int level) :
    m_strName(name),
    m_eRace(race),
    m_eClassType(classType),
    m_iLevel(level)
{

}

bool QMUD::PcCharacterBaseInfo::operator==(const QMUD::PcCharacterBaseInfo &other) const
{
    return m_strName == other.m_strName &&
            m_iLevel == other.m_iLevel &&
            m_eClassType == other.m_eClassType &&
            m_eRace == other.m_eRace;
}

void QMUD::PcCharacterBaseInfo::serialize(QJsonObject &obj) const
{
    obj["name"] = m_strName;
    obj["race"] = QMUD::Race::toString(m_eRace);
    obj["class"] = QMUD::classTypeToString(m_eClassType);
    obj["level"] = m_iLevel;
}

void QMUD::PcCharacterBaseInfo::deserialize(const QJsonObject &obj)
{
    m_strName = obj["name"].toString();
    m_eRace = QMUD::Race::fromString(obj["race"].toString());
    m_eClassType = QMUD::classTypeFromString(obj["class"].toString());
    m_iLevel = obj["level"].toInt();
}

QString QMUD::PcCharacterBaseInfo::name() const
{
    return m_strName;
}

int QMUD::PcCharacterBaseInfo::level() const
{
    return m_iLevel;
}

QMUD::ClassType QMUD::PcCharacterBaseInfo::classType() const
{
    return m_eClassType;
}

QMUD::RaceType QMUD::PcCharacterBaseInfo::race() const
{
    return m_eRace;
}

QString QMUD::roomFlagToString(QMUD::RoomFlagType flag)
{
    switch (flag)
    {
    case RoomFlagType::PACE:           return "pace";
    case RoomFlagType::DIFFICULT:      return "difficult";
    case RoomFlagType::IMPERVIOUS:     return "impervious";
    case RoomFlagType::NO_RESOURCES:   return "no_resources";
    case RoomFlagType::IMPOSSIBLE:     return "impossible";
    case RoomFlagType::WATER:          return "water";
    case RoomFlagType::OUTSIDE:        return "outside";
    case RoomFlagType::UNKNOWN:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::RoomFlagType QMUD::roomFlagFromString(QString str)
{
    if (str.compare("pace", Qt::CaseInsensitive) == 0)
        return RoomFlagType::PACE;
    else if (str.compare("difficult", Qt::CaseInsensitive) == 0)
        return RoomFlagType::DIFFICULT;
    else if (str.compare("impervious", Qt::CaseInsensitive) == 0)
        return RoomFlagType::IMPERVIOUS;
    else if (str.compare("impossible", Qt::CaseInsensitive) == 0)
        return RoomFlagType::IMPOSSIBLE;
    else if (str.compare("water", Qt::CaseInsensitive) == 0)
        return RoomFlagType::WATER;
    else if (str.compare("no_resources", Qt::CaseInsensitive) == 0)
        return RoomFlagType::NO_RESOURCES;
    else if (str.compare("outside", Qt::CaseInsensitive) == 0)
        return RoomFlagType::OUTSIDE;
    else
        return RoomFlagType::UNKNOWN;
}

QString QMUD::resourceToString(QMUD::ResourceType resource)
{
    switch (resource)
    {
    case ResourceType::COINS:                        return "coins";
    case ResourceType::WOOD:                         return "wood";
    case ResourceType::LEATHER:                      return "leather";
    case ResourceType::IRON_BAR:                     return "iron_bar";
    case ResourceType::TANNED_LEATHER:               return "tanned_leather";
    case ResourceType::RAW_MEAT:                     return "raw_meat";
    case ResourceType::SCALE:                        return "scale";
    case ResourceType::GARLIC:                       return "garlic";
    case ResourceType::MARIGOLD:                     return "marigold";
    case ResourceType::HYSSOP:                       return "hyssop";
    case ResourceType::PRIMULA:                      return "primula";
    case ResourceType::RED_CURRANT:                  return "red_currant";
    case ResourceType::GOOSEBERRY:                   return "gooseberry";
    case ResourceType::GINGER:                       return "ginger";
    case ResourceType::LAVENDER:                     return "lavender";
    case ResourceType::POPPY:                        return "poppy";
    case ResourceType::BLACK_CURRANT:                return "black_currant";
    case ResourceType::TILIA:                        return "tilia";
    case ResourceType::HOP:                          return "hop";
    case ResourceType::WITCH_HAZEL:                  return "witch_hazel";
    case ResourceType::SAFFLOWER:                    return "safflower";
    case ResourceType::GINKGO:                       return "ginkgo";
    case ResourceType::VERBENA:                      return "verbena";
    case ResourceType::ANGELICA:                     return "angelica";
    case ResourceType::ANISE:                        return "anise";
    case ResourceType::EDELWEISS:                    return "edelweiss";
    case ResourceType::HELICHRYSUM:                  return "helichrysum";
    case ResourceType::HOLLY:                        return "holly";
    case ResourceType::ILEX:                         return "ilex";
    case ResourceType::ELDER:                        return "elder";
    case ResourceType::FOXGLOVE:                     return "foxglove";
    case ResourceType::AGAVE:                        return "agave";
    case ResourceType::AMBROSIA:                     return "ambrosia";
    case ResourceType::SUNDEW:                       return "sundew";
    case ResourceType::HAWTHORN:                     return "hawthorn";
    case ResourceType::LOTUS:                        return "lotus";
    case ResourceType::MORINGA:                      return "moringa";
    case ResourceType::ALCHEMILLA:                   return "alchemilla";
    case ResourceType::ROBINIA:                      return "robinia";
    case ResourceType::ESCOLZIA:                     return "escolzia";
    case ResourceType::NETTLE:                       return "nettle";
    case ResourceType::STRAMONIUM:                   return "stramonium";
    case ResourceType::COPPER:                       return "copper";
    case ResourceType::TIN:                          return "tin";
    case ResourceType::IRON:                         return "iron";
    case ResourceType::ZINC:                         return "zinc";
    case ResourceType::ANTIMONY:                     return "antimony";
    case ResourceType::ALUMINIUM:                    return "aluminium";
    case ResourceType::CHROME:                       return "chrome";
    case ResourceType::LEAD:                         return "lead";
    case ResourceType::CADMIUM:                      return "cadmium";
    case ResourceType::SILVER:                       return "silver";
    case ResourceType::NICKEL:                       return "nickel";
    case ResourceType::BISMUTH:                      return "bismuth";
    case ResourceType::GOLD:                         return "gold";
    case ResourceType::TITANIUM:                     return "titanium";
    case ResourceType::PLATINUM:                     return "platinum";
    case ResourceType::IRIDIUM:                      return "iridium";
    case ResourceType::RHODIUM:                      return "rhodium";
    case ResourceType::OSMIUM:                       return "osmium";
    case ResourceType::BRONZE:                       return "bronze";
    case ResourceType::BRASS:                        return "brass";
    case ResourceType::PEWTER:                       return "pewter";
    case ResourceType::DURALLUMINIUM:                return "duralluminium";
    case ResourceType::ALPACCA:                      return "alpacca";
    case ResourceType::NICROSIL:                     return "nicrosil";
    case ResourceType::CORROBEND:                    return "corrobend";
    case ResourceType::ELECTRUM:                     return "electrum";
    case ResourceType::PLATINUM_MAX:                 return "platinum_max";
    case ResourceType::LUCENS:                       return "lucens";
    case ResourceType::ALTIUM:                       return "altium";
    case ResourceType::ESSENCE_LIGHT:                return "essence_light";
    case ResourceType::ESSENCE_DARKNESS:             return "essence_darkness";
    case ResourceType::ESSENCE_FIRE:                 return "essence_fire";
    case ResourceType::ESSENCE_AIR:                  return "essence_air";
    case ResourceType::ESSENCE_WATER:                return "essence_water";
    case ResourceType::ESSENCE_EARTH:                return "essence_earth";
    case ResourceType::ESSENCE_SUPERIOR_LIGHT:       return "essence_superior_light";
    case ResourceType::ESSENCE_SUPERIOR_DARKNESS:    return "essence_superior_darkness";
    case ResourceType::ESSENCE_SUPERIOR_FIRE:        return "essence_superior_fire";
    case ResourceType::ESSENCE_SUPERIOR_AIR:         return "essence_superior_air";
    case ResourceType::ESSENCE_SUPERIOR_WATER:       return "essence_superior_water";
    case ResourceType::ESSENCE_SUPERIOR_EARTH:       return "essence_superior_earth";
    case ResourceType::ESSENCE_SUPREME_LIGHT:        return "essence_supreme_light";
    case ResourceType::ESSENCE_SUPREME_DARKNESS:     return "essence_supreme_darkness";
    case ResourceType::ESSENCE_SUPREME_FIRE:         return "essence_supreme_fire";
    case ResourceType::ESSENCE_SUPREME_AIR:          return "essence_supreme_air";
    case ResourceType::ESSENCE_SUPREME_WATER:        return "essence_supreme_water";
    case ResourceType::ESSENCE_SUPREME_EARTH:        return "essence_supreme_earth";
    case ResourceType::ESSENCE_PERFECT_LIGHT:        return "essence_perfect_light";
    case ResourceType::ESSENCE_PERFECT_DARKNESS:     return "essence_perfect_darkness";
    case ResourceType::ESSENCE_PERFECT_FIRE:         return "essence_perfect_fire";
    case ResourceType::ESSENCE_PERFECT_AIR:          return "essence_perfect_air";
    case ResourceType::ESSENCE_PERFECT_WATER:        return "essence_perfect_water";
    case ResourceType::ESSENCE_PERFECT_EARTH:        return "essence_perfect_earth";
    case ResourceType::ESSENCE_LEGENDARY_LIGHT:      return "essence_legendary_light";
    case ResourceType::ESSENCE_LEGENDARY_DARKNESS:   return "essence_legendary_darkness";
    case ResourceType::ESSENCE_LEGENDARY_FIRE:       return "essence_legendary_fire";
    case ResourceType::ESSENCE_LEGENDARY_AIR:        return "essence_legendary_air";
    case ResourceType::ESSENCE_LEGENDARY_WATER:      return "essence_legendary_water";
    case ResourceType::ESSENCE_LEGENDARY_EARTH:      return "essence_legendary_earth";
    case ResourceType::UNKNOWN:
    case ResourceType::MAX_VALUE:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ResourceType QMUD::resourceFromString(QString resource)
{
    if (resource.compare("coins", Qt::CaseInsensitive) == 0)                             return ResourceType::COINS;
    else if (resource.compare("iron", Qt::CaseInsensitive) == 0)                         return ResourceType::IRON;
    else if (resource.compare("wood", Qt::CaseInsensitive) == 0)                         return ResourceType::WOOD;
    else if (resource.compare("leather", Qt::CaseInsensitive) == 0)                      return ResourceType::LEATHER;
    else if (resource.compare("iron_bar", Qt::CaseInsensitive) == 0)                     return ResourceType::IRON_BAR;
    else if (resource.compare("tanned_leather", Qt::CaseInsensitive) == 0)               return ResourceType::TANNED_LEATHER;
    else if (resource.compare("raw_meat", Qt::CaseInsensitive) == 0)                     return ResourceType::RAW_MEAT;
    else if (resource.compare("scale", Qt::CaseInsensitive) == 0)                        return ResourceType::SCALE;
    else if (resource.compare("garlic", Qt::CaseInsensitive) == 0)                       return ResourceType::GARLIC;
    else if (resource.compare("marigold", Qt::CaseInsensitive) == 0)                     return ResourceType::MARIGOLD;
    else if (resource.compare("hyssop", Qt::CaseInsensitive) == 0)                       return ResourceType::HYSSOP;
    else if (resource.compare("primula", Qt::CaseInsensitive) == 0)                      return ResourceType::PRIMULA;
    else if (resource.compare("red_currant", Qt::CaseInsensitive) == 0)                  return ResourceType::RED_CURRANT;
    else if (resource.compare("gooseberry", Qt::CaseInsensitive) == 0)                   return ResourceType::GOOSEBERRY;
    else if (resource.compare("ginger", Qt::CaseInsensitive) == 0)                       return ResourceType::GINGER;
    else if (resource.compare("lavender", Qt::CaseInsensitive) == 0)                     return ResourceType::LAVENDER;
    else if (resource.compare("poppy", Qt::CaseInsensitive) == 0)                        return ResourceType::POPPY;
    else if (resource.compare("black_currant", Qt::CaseInsensitive) == 0)                return ResourceType::BLACK_CURRANT;
    else if (resource.compare("tilia", Qt::CaseInsensitive) == 0)                        return ResourceType::TILIA;
    else if (resource.compare("hop", Qt::CaseInsensitive) == 0)                          return ResourceType::HOP;
    else if (resource.compare("witch_hazel", Qt::CaseSensitive) == 0)                    return ResourceType::WITCH_HAZEL;
    else if (resource.compare("safflower", Qt::CaseSensitive) == 0)                      return ResourceType::SAFFLOWER;
    else if (resource.compare("ginkgo", Qt::CaseSensitive) == 0)                         return ResourceType::GINKGO;
    else if (resource.compare("verbena", Qt::CaseSensitive) == 0)                        return ResourceType::VERBENA;
    else if (resource.compare("angelica", Qt::CaseSensitive) == 0)                       return ResourceType::ANGELICA;
    else if (resource.compare("anise", Qt::CaseSensitive) == 0)                          return ResourceType::ANISE;
    else if (resource.compare("edelweiss", Qt::CaseSensitive) == 0)                      return ResourceType::EDELWEISS;
    else if (resource.compare("helichrysum", Qt::CaseSensitive) == 0)                    return ResourceType::HELICHRYSUM;
    else if (resource.compare("holly", Qt::CaseSensitive) == 0)                          return ResourceType::HOLLY;
    else if (resource.compare("ilex", Qt::CaseSensitive) == 0)                           return ResourceType::ILEX;
    else if (resource.compare("elder", Qt::CaseSensitive) == 0)                          return ResourceType::ELDER;
    else if (resource.compare("foxglove", Qt::CaseSensitive) == 0)                       return ResourceType::FOXGLOVE;
    else if (resource.compare("agave", Qt::CaseSensitive) == 0)                          return ResourceType::AGAVE;
    else if (resource.compare("ambrosia", Qt::CaseSensitive) == 0)                       return ResourceType::AMBROSIA;
    else if (resource.compare("sundew", Qt::CaseSensitive) == 0)                         return ResourceType::SUNDEW;
    else if (resource.compare("hawthorn", Qt::CaseSensitive) == 0)                       return ResourceType::HAWTHORN;
    else if (resource.compare("lotus", Qt::CaseSensitive) == 0)                          return ResourceType::LOTUS;
    else if (resource.compare("moringa", Qt::CaseSensitive) == 0)                        return ResourceType::MORINGA;
    else if (resource.compare("alchemilla", Qt::CaseSensitive) == 0)                     return ResourceType::ALCHEMILLA;
    else if (resource.compare("robinia", Qt::CaseSensitive) == 0)                        return ResourceType::ROBINIA;
    else if (resource.compare("escolzia", Qt::CaseSensitive) == 0)                       return ResourceType::ESCOLZIA;
    else if (resource.compare("nettle", Qt::CaseSensitive) == 0)                         return ResourceType::NETTLE;
    else if (resource.compare("stramonium", Qt::CaseSensitive) == 0)                     return ResourceType::STRAMONIUM;
    else if (resource.compare("copper", Qt::CaseSensitive) == 0)                         return ResourceType::COPPER;
    else if (resource.compare("tin", Qt::CaseSensitive) == 0)                            return ResourceType::TIN;
    else if (resource.compare("iron", Qt::CaseSensitive) == 0)                           return ResourceType::IRON;
    else if (resource.compare("zinc", Qt::CaseSensitive) == 0)                           return ResourceType::ZINC;
    else if (resource.compare("antimony", Qt::CaseSensitive) == 0)                       return ResourceType::ANTIMONY;
    else if (resource.compare("aluminium", Qt::CaseSensitive) == 0)                      return ResourceType::ALUMINIUM;
    else if (resource.compare("chrome", Qt::CaseSensitive) == 0)                         return ResourceType::CHROME;
    else if (resource.compare("lead", Qt::CaseSensitive) == 0)                           return ResourceType::LEAD;
    else if (resource.compare("cadmium", Qt::CaseSensitive) == 0)                        return ResourceType::CADMIUM;
    else if (resource.compare("silver", Qt::CaseSensitive) == 0)                         return ResourceType::SILVER;
    else if (resource.compare("nickel", Qt::CaseSensitive) == 0)                         return ResourceType::NICKEL;
    else if (resource.compare("bismuth", Qt::CaseSensitive) == 0)                        return ResourceType::BISMUTH;
    else if (resource.compare("gold", Qt::CaseSensitive) == 0)                           return ResourceType::GOLD;
    else if (resource.compare("titanium", Qt::CaseSensitive) == 0)                       return ResourceType::TITANIUM;
    else if (resource.compare("platinum", Qt::CaseSensitive) == 0)                       return ResourceType::PLATINUM;
    else if (resource.compare("iridium", Qt::CaseSensitive) == 0)                        return ResourceType::IRIDIUM;
    else if (resource.compare("rhodium", Qt::CaseSensitive) == 0)                        return ResourceType::RHODIUM;
    else if (resource.compare("osmium", Qt::CaseSensitive) == 0)                         return ResourceType::OSMIUM;
    else if (resource.compare("bronze", Qt::CaseSensitive) == 0)                         return ResourceType::BRONZE;
    else if (resource.compare("brass", Qt::CaseSensitive) == 0)                          return ResourceType::BRASS;
    else if (resource.compare("pewter", Qt::CaseSensitive) == 0)                         return ResourceType::PEWTER;
    else if (resource.compare("duralluminium", Qt::CaseSensitive) == 0)                  return ResourceType::DURALLUMINIUM;
    else if (resource.compare("alpacca", Qt::CaseSensitive) == 0)                        return ResourceType::ALPACCA;
    else if (resource.compare("nicrosil", Qt::CaseSensitive) == 0)                       return ResourceType::NICROSIL;
    else if (resource.compare("corrobend", Qt::CaseSensitive) == 0)                      return ResourceType::CORROBEND;
    else if (resource.compare("electrum", Qt::CaseSensitive) == 0)                       return ResourceType::ELECTRUM;
    else if (resource.compare("platinum_max", Qt::CaseSensitive) == 0)                   return ResourceType::PLATINUM_MAX;
    else if (resource.compare("lucens", Qt::CaseSensitive) == 0)                         return ResourceType::LUCENS;
    else if (resource.compare("altium", Qt::CaseSensitive) == 0)                         return ResourceType::ALTIUM;
    else if (resource.compare("essence_light", Qt::CaseSensitive) == 0)                  return ResourceType::ESSENCE_LIGHT;
    else if (resource.compare("essence_darkness", Qt::CaseSensitive) == 0)               return ResourceType::ESSENCE_DARKNESS;
    else if (resource.compare("essence_fire", Qt::CaseSensitive) == 0)                   return ResourceType::ESSENCE_FIRE;
    else if (resource.compare("essence_air", Qt::CaseSensitive) == 0)                    return ResourceType::ESSENCE_AIR;
    else if (resource.compare("essence_water", Qt::CaseSensitive) == 0)                  return ResourceType::ESSENCE_WATER;
    else if (resource.compare("essence_earth", Qt::CaseSensitive) == 0)                  return ResourceType::ESSENCE_EARTH;
    else if (resource.compare("essence_superior_light", Qt::CaseSensitive) == 0)         return ResourceType::ESSENCE_SUPERIOR_LIGHT;
    else if (resource.compare("essence_superior_darkness", Qt::CaseSensitive) == 0)      return ResourceType::ESSENCE_SUPERIOR_DARKNESS;
    else if (resource.compare("essence_superior_fire", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_SUPERIOR_FIRE;
    else if (resource.compare("essence_superior_air", Qt::CaseSensitive) == 0)           return ResourceType::ESSENCE_SUPERIOR_AIR;
    else if (resource.compare("essence_superior_water", Qt::CaseSensitive) == 0)         return ResourceType::ESSENCE_SUPERIOR_WATER;
    else if (resource.compare("essence_superior_earth", Qt::CaseSensitive) == 0)         return ResourceType::ESSENCE_SUPERIOR_EARTH;
    else if (resource.compare("essence_supreme_light", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_SUPREME_LIGHT;
    else if (resource.compare("essence_supreme_darkness", Qt::CaseSensitive) == 0)       return ResourceType::ESSENCE_SUPREME_DARKNESS;
    else if (resource.compare("essence_supreme_fire", Qt::CaseSensitive) == 0)           return ResourceType::ESSENCE_SUPREME_FIRE;
    else if (resource.compare("essence_supreme_air", Qt::CaseSensitive) == 0)            return ResourceType::ESSENCE_SUPREME_AIR;
    else if (resource.compare("essence_supreme_water", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_SUPREME_WATER;
    else if (resource.compare("essence_supreme_earth", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_SUPREME_EARTH;
    else if (resource.compare("essence_perfect_light", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_PERFECT_LIGHT;
    else if (resource.compare("essence_perfect_darkness", Qt::CaseSensitive) == 0)       return ResourceType::ESSENCE_PERFECT_DARKNESS;
    else if (resource.compare("essence_perfect_fire", Qt::CaseSensitive) == 0)           return ResourceType::ESSENCE_PERFECT_FIRE;
    else if (resource.compare("essence_perfect_air", Qt::CaseSensitive) == 0)            return ResourceType::ESSENCE_PERFECT_AIR;
    else if (resource.compare("essence_perfect_water", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_PERFECT_WATER;
    else if (resource.compare("essence_perfect_earth", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_PERFECT_EARTH;
    else if (resource.compare("essence_legendary_light", Qt::CaseSensitive) == 0)        return ResourceType::ESSENCE_LEGENDARY_LIGHT;
    else if (resource.compare("essence_legendary_darkness", Qt::CaseSensitive) == 0)     return ResourceType::ESSENCE_LEGENDARY_DARKNESS;
    else if (resource.compare("essence_legendary_fire", Qt::CaseSensitive) == 0)         return ResourceType::ESSENCE_LEGENDARY_FIRE;
    else if (resource.compare("essence_legendary_air", Qt::CaseSensitive) == 0)          return ResourceType::ESSENCE_LEGENDARY_AIR;
    else if (resource.compare("essence_legendary_water", Qt::CaseSensitive) == 0)        return ResourceType::ESSENCE_LEGENDARY_WATER;
    else if (resource.compare("essence_legendary_earth", Qt::CaseSensitive) == 0)        return ResourceType::ESSENCE_LEGENDARY_EARTH;
    else
    {
        return ResourceType::UNKNOWN;
    }
}

QString QMUD::resourceToReadableString(QMUD::ResourceType resource)
{
    switch (resource)
    {
    case ResourceType::COINS:                        return QObject::tr("aurei");
    case ResourceType::WOOD:                         return QObject::tr("legno");
    case ResourceType::LEATHER:                      return QObject::tr("pelli");
    case ResourceType::IRON_BAR:                     return QObject::tr("lingotto di ferro");
    case ResourceType::TANNED_LEATHER:               return QObject::tr("pelle conciata");
    case ResourceType::RAW_MEAT:                     return QObject::tr("carne cruda");
    case ResourceType::SCALE:                        return QObject::tr("scaglie");
    case ResourceType::GARLIC:                       return QObject::tr("aglio");
    case ResourceType::MARIGOLD:                     return QObject::tr("calendula");
    case ResourceType::HYSSOP:                       return QObject::tr("issopo");
    case ResourceType::PRIMULA:                      return QObject::tr("primula");
    case ResourceType::RED_CURRANT:                  return QObject::tr("ribes rosso");
    case ResourceType::GOOSEBERRY:                   return QObject::tr("uva spina");
    case ResourceType::GINGER:                       return QObject::tr("zenzero");
    case ResourceType::LAVENDER:                     return QObject::tr("lavanda");
    case ResourceType::POPPY:                        return QObject::tr("papavero");
    case ResourceType::BLACK_CURRANT:                return QObject::tr("ribes nero");
    case ResourceType::TILIA:                        return QObject::tr("tiglio");
    case ResourceType::HOP:                          return QObject::tr("luppolo");
    case ResourceType::WITCH_HAZEL:                  return QObject::tr("amamelide");
    case ResourceType::SAFFLOWER:                    return QObject::tr("cartamo");
    case ResourceType::GINKGO:                       return QObject::tr("ginkgo");
    case ResourceType::VERBENA:                      return QObject::tr("verbena");
    case ResourceType::ANGELICA:                     return QObject::tr("angelica");
    case ResourceType::ANISE:                        return QObject::tr("anice");
    case ResourceType::EDELWEISS:                    return QObject::tr("lentopodium");
    case ResourceType::HELICHRYSUM:                  return QObject::tr("elicriso");
    case ResourceType::HOLLY:                        return QObject::tr("pungitopo");
    case ResourceType::ILEX:                         return QObject::tr("agrifoglio");
    case ResourceType::ELDER:                        return QObject::tr("sambuco");
    case ResourceType::FOXGLOVE:                     return QObject::tr("digitalis purpurea");
    case ResourceType::AGAVE:                        return QObject::tr("agave");
    case ResourceType::AMBROSIA:                     return QObject::tr("ambrosia");
    case ResourceType::SUNDEW:                       return QObject::tr("drosera");
    case ResourceType::HAWTHORN:                     return QObject::tr("biancospino");
    case ResourceType::LOTUS:                        return QObject::tr("fior di loto");
    case ResourceType::MORINGA:                      return QObject::tr("moringa");
    case ResourceType::ALCHEMILLA:                   return QObject::tr("alchemilla");
    case ResourceType::ROBINIA:                      return QObject::tr("robinia");
    case ResourceType::ESCOLZIA:                     return QObject::tr("escolzia");
    case ResourceType::NETTLE:                       return QObject::tr("ortica");
    case ResourceType::STRAMONIUM:                   return QObject::tr("stramonio");
    case ResourceType::COPPER:                       return QObject::tr("rame");
    case ResourceType::TIN:                          return QObject::tr("stagno");
    case ResourceType::IRON:                         return QObject::tr("ferro");
    case ResourceType::ZINC:                         return QObject::tr("calamina");
    case ResourceType::ANTIMONY:                     return QObject::tr("ammon");
    case ResourceType::ALUMINIUM:                    return QObject::tr("alluminio");
    case ResourceType::CHROME:                       return QObject::tr("piombo Rosso");
    case ResourceType::LEAD:                         return QObject::tr("piombo");
    case ResourceType::CADMIUM:                      return QObject::tr("argento nero");
    case ResourceType::SILVER:                       return QObject::tr("argento");
    case ResourceType::NICKEL:                       return QObject::tr("rame bianco");
    case ResourceType::BISMUTH:                      return QObject::tr("argento alchemico");
    case ResourceType::GOLD:                         return QObject::tr("oro");
    case ResourceType::TITANIUM:                     return QObject::tr("titanio");
    case ResourceType::PLATINUM:                     return QObject::tr("platino");
    case ResourceType::IRIDIUM:                      return QObject::tr("iride");
    case ResourceType::RHODIUM:                      return QObject::tr("oro alchemico");
    case ResourceType::OSMIUM:                       return QObject::tr("osmiridio");
    case ResourceType::BRONZE:                       return QObject::tr("bronzo");
    case ResourceType::BRASS:                        return QObject::tr("ottone");
    case ResourceType::PEWTER:                       return QObject::tr("peltro");
    case ResourceType::DURALLUMINIUM:                return QObject::tr("duralluminio");
    case ResourceType::ALPACCA:                      return QObject::tr("argentite");
    case ResourceType::NICROSIL:                     return QObject::tr("nicrosil");
    case ResourceType::CORROBEND:                    return QObject::tr("corrobend");
    case ResourceType::ELECTRUM:                     return QObject::tr("electrum");
    case ResourceType::PLATINUM_MAX:                 return QObject::tr("diamantite");
    case ResourceType::LUCENS:                       return QObject::tr("lucens");
    case ResourceType::ALTIUM:                       return QObject::tr("altium");
    case ResourceType::ESSENCE_LIGHT:                return QObject::tr("essenza di luce");
    case ResourceType::ESSENCE_DARKNESS:             return QObject::tr("essenza oscura");
    case ResourceType::ESSENCE_FIRE:                 return QObject::tr("essenza del fuoco");
    case ResourceType::ESSENCE_AIR:                  return QObject::tr("essenza dell'aria");
    case ResourceType::ESSENCE_WATER:                return QObject::tr("essenza dell'acqua");
    case ResourceType::ESSENCE_EARTH:                return QObject::tr("essenza della terra");
    case ResourceType::ESSENCE_SUPERIOR_LIGHT:       return QObject::tr("essenza superiore di luce");
    case ResourceType::ESSENCE_SUPERIOR_DARKNESS:    return QObject::tr("essenza superiore oscura");
    case ResourceType::ESSENCE_SUPERIOR_FIRE:        return QObject::tr("essenza superiore del fuoco");
    case ResourceType::ESSENCE_SUPERIOR_AIR:         return QObject::tr("essenza superiore dell'aria");
    case ResourceType::ESSENCE_SUPERIOR_WATER:       return QObject::tr("essenza superiore dell'acqua");
    case ResourceType::ESSENCE_SUPERIOR_EARTH:       return QObject::tr("essenza superiore della terra");
    case ResourceType::ESSENCE_SUPREME_LIGHT:        return QObject::tr("essenza suprema di luce");
    case ResourceType::ESSENCE_SUPREME_DARKNESS:     return QObject::tr("essenza suprema oscura");
    case ResourceType::ESSENCE_SUPREME_FIRE:         return QObject::tr("essenza suprema del fuoco");
    case ResourceType::ESSENCE_SUPREME_AIR:          return QObject::tr("essenza suprema dell'aria");
    case ResourceType::ESSENCE_SUPREME_WATER:        return QObject::tr("essenza suprema dell'acqua");
    case ResourceType::ESSENCE_SUPREME_EARTH:        return QObject::tr("essenza suprema della terra");
    case ResourceType::ESSENCE_PERFECT_LIGHT:        return QObject::tr("essenza perfetta di luce");
    case ResourceType::ESSENCE_PERFECT_DARKNESS:     return QObject::tr("essenza perfetta oscura");
    case ResourceType::ESSENCE_PERFECT_FIRE:         return QObject::tr("essenza perfetta del fuoco");
    case ResourceType::ESSENCE_PERFECT_AIR:          return QObject::tr("essenza perfetta dell'aria");
    case ResourceType::ESSENCE_PERFECT_WATER:        return QObject::tr("essenza perfetta dell'acqua");
    case ResourceType::ESSENCE_PERFECT_EARTH:        return QObject::tr("essenza perfetta della terra");
    case ResourceType::ESSENCE_LEGENDARY_LIGHT:      return QObject::tr("essenza leggendaria di luce");
    case ResourceType::ESSENCE_LEGENDARY_DARKNESS:   return QObject::tr("essenza leggendaria oscura");
    case ResourceType::ESSENCE_LEGENDARY_FIRE:       return QObject::tr("essenza leggendaria del fuoco");
    case ResourceType::ESSENCE_LEGENDARY_AIR:        return QObject::tr("essenza leggendaria dell'aria");
    case ResourceType::ESSENCE_LEGENDARY_WATER:      return QObject::tr("essenza leggendaria dell'acqua");
    case ResourceType::ESSENCE_LEGENDARY_EARTH:      return QObject::tr("essenza leggendaria della terra");
    case ResourceType::UNKNOWN:
    case ResourceType::MAX_VALUE:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

bool QMUD::resourceIsForMining(QMUD::ResourceType resource)
{
    return resource >= ResourceType::RESOURCE_MINING_FIRST && resource < ResourceType::RESOURCE_MINING_MAX_VALUE;
}

bool QMUD::resourceIsForErbalism(QMUD::ResourceType resource)
{
    return resource >= ResourceType::RESOURCE_HERBALISM_FIRST && resource < ResourceType::RESOURCE_HERBALISM_MAX_VALUE;
}

int QMUD::resourceToLevel(QMUD::ResourceType resource)
{
    switch (resource)
    {
    case ResourceType::WOOD:
    case ResourceType::GARLIC:
    case ResourceType::MARIGOLD:
    case ResourceType::HYSSOP:
    case ResourceType::PRIMULA:
    case ResourceType::RED_CURRANT:
    case ResourceType::GOOSEBERRY:
    case ResourceType::GINGER:
    case ResourceType::LAVENDER:
    case ResourceType::POPPY:
    case ResourceType::BLACK_CURRANT:
    case ResourceType::TILIA:
    case ResourceType::HOP:
    case ResourceType::COPPER:
    case ResourceType::TIN:
    case ResourceType::IRON:
    case ResourceType::BRONZE:
    case ResourceType::ESSENCE_LIGHT:
    case ResourceType::ESSENCE_DARKNESS:
    case ResourceType::ESSENCE_FIRE:
    case ResourceType::ESSENCE_AIR:
    case ResourceType::ESSENCE_WATER:
    case ResourceType::ESSENCE_EARTH:
        return 1;

    case ResourceType::WITCH_HAZEL:
    case ResourceType::SAFFLOWER:
    case ResourceType::GINKGO:
    case ResourceType::VERBENA:
    case ResourceType::ANGELICA:
    case ResourceType::ANISE:
    case ResourceType::EDELWEISS:
    case ResourceType::ZINC:
    case ResourceType::ANTIMONY:
    case ResourceType::BRASS:
    case ResourceType::PEWTER:
    case ResourceType::ESSENCE_SUPERIOR_LIGHT:
    case ResourceType::ESSENCE_SUPERIOR_DARKNESS:
    case ResourceType::ESSENCE_SUPERIOR_FIRE:
    case ResourceType::ESSENCE_SUPERIOR_AIR:
    case ResourceType::ESSENCE_SUPERIOR_WATER:
    case ResourceType::ESSENCE_SUPERIOR_EARTH:
        return 2;

    case ResourceType::HELICHRYSUM:
    case ResourceType::HOLLY:
    case ResourceType::ILEX:
    case ResourceType::ELDER:
    case ResourceType::FOXGLOVE:
    case ResourceType::ALUMINIUM:
    case ResourceType::CHROME:
    case ResourceType::LEAD:
    case ResourceType::CADMIUM:
    case ResourceType::SILVER:
    case ResourceType::DURALLUMINIUM:
    case ResourceType::ALPACCA:
    case ResourceType::ESSENCE_SUPREME_LIGHT:
    case ResourceType::ESSENCE_SUPREME_DARKNESS:
    case ResourceType::ESSENCE_SUPREME_FIRE:
    case ResourceType::ESSENCE_SUPREME_AIR:
    case ResourceType::ESSENCE_SUPREME_WATER:
    case ResourceType::ESSENCE_SUPREME_EARTH:
        return 3;

    case ResourceType::AGAVE:
    case ResourceType::AMBROSIA:
    case ResourceType::SUNDEW:
    case ResourceType::HAWTHORN:
    case ResourceType::LOTUS:
    case ResourceType::MORINGA:
    case ResourceType::NICKEL:
    case ResourceType::BISMUTH:
    case ResourceType::GOLD:
    case ResourceType::TITANIUM:
    case ResourceType::NICROSIL:
    case ResourceType::CORROBEND:
    case ResourceType::ESSENCE_PERFECT_LIGHT:
    case ResourceType::ESSENCE_PERFECT_DARKNESS:
    case ResourceType::ESSENCE_PERFECT_FIRE:
    case ResourceType::ESSENCE_PERFECT_AIR:
    case ResourceType::ESSENCE_PERFECT_WATER:
    case ResourceType::ESSENCE_PERFECT_EARTH:
        return 4;

    case ResourceType::ALCHEMILLA:
    case ResourceType::ROBINIA:
    case ResourceType::ESCOLZIA:
    case ResourceType::NETTLE:
    case ResourceType::STRAMONIUM:
    case ResourceType::PLATINUM:
    case ResourceType::IRIDIUM:
    case ResourceType::RHODIUM:
    case ResourceType::OSMIUM:
    case ResourceType::ELECTRUM:
    case ResourceType::PLATINUM_MAX:
    case ResourceType::LUCENS:
    case ResourceType::ALTIUM:
    case ResourceType::ESSENCE_LEGENDARY_LIGHT:
    case ResourceType::ESSENCE_LEGENDARY_DARKNESS:
    case ResourceType::ESSENCE_LEGENDARY_FIRE:
    case ResourceType::ESSENCE_LEGENDARY_AIR:
    case ResourceType::ESSENCE_LEGENDARY_WATER:
    case ResourceType::ESSENCE_LEGENDARY_EARTH:
        return 5;

    case ResourceType::COINS:
    case ResourceType::LEATHER:
    case ResourceType::IRON_BAR:
    case ResourceType::TANNED_LEATHER:
    case ResourceType::RAW_MEAT:
    case ResourceType::SCALE:
    case ResourceType::UNKNOWN:
    case ResourceType::MAX_VALUE:
        return 0;
    }

    Q_ASSERT(false);
    return 0;
}

QString QMUD::resourceRarityToString(QMUD::ResourceRarityType rarity)
{
    switch (rarity)
    {
    case QMUD::ResourceRarityType::NONE:            return "none";
    case QMUD::ResourceRarityType::NORMAL:          return "normal";
    case QMUD::ResourceRarityType::UNCOMMON:        return "uncommon";
    case QMUD::ResourceRarityType::RARE:            return "rare";
    case QMUD::ResourceRarityType::VERY_RARE:       return "very_rare";
    case QMUD::ResourceRarityType::EXTREMELY_RARE:  return "extremely_rare";
    case QMUD::ResourceRarityType::UNKNOWN:
        Q_ASSERT(false);
        return "none";
    }

    Q_ASSERT(false);
    return "none";
}

QMUD::ResourceRarityType QMUD::resourceRarityFromString(QString rarity)
{
    if (rarity.compare("none", Qt::CaseInsensitive) == 0)                   return QMUD::ResourceRarityType::NONE;
    else if (rarity.compare("normal", Qt::CaseInsensitive) == 0)            return QMUD::ResourceRarityType::NORMAL;
    else if (rarity.compare("uncommon", Qt::CaseInsensitive) == 0)          return QMUD::ResourceRarityType::UNCOMMON;
    else if (rarity.compare("rare", Qt::CaseInsensitive) == 0)              return QMUD::ResourceRarityType::RARE;
    else if (rarity.compare("very_rare", Qt::CaseInsensitive) == 0)         return QMUD::ResourceRarityType::VERY_RARE;
    else if (rarity.compare("extremely_rare", Qt::CaseInsensitive) == 0)    return QMUD::ResourceRarityType::EXTREMELY_RARE;
    else
        return QMUD::ResourceRarityType::UNKNOWN;
}

QString QMUD::professionToString(ProfessionType profession)
{
    switch (profession)
    {
    case ProfessionType::BLACKSMITHING:  return "blacksmithing";
    case ProfessionType::COOKING:        return "cooking";
    case ProfessionType::TAILORING:      return "tailoring";
    case ProfessionType::JEWELCRAFTING:  return "jewelcrafting";
    case ProfessionType::ENCHANTING:     return "enchanting";
    case ProfessionType::ALCHEMY:        return "alchemy";
    case ProfessionType::UNKNOWN:
    case ProfessionType::MAX_VALUE:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ProfessionType QMUD::professionFromString(QString profession)
{
    if (profession.compare("blacksmithing", Qt::CaseInsensitive) == 0)      return ProfessionType::BLACKSMITHING;
    else if (profession.compare("cooking", Qt::CaseInsensitive) == 0)       return ProfessionType::COOKING;
    else if (profession.compare("tailoring", Qt::CaseInsensitive) == 0)     return ProfessionType::TAILORING;
    else if (profession.compare("jewelcrafting", Qt::CaseInsensitive) == 0) return ProfessionType::JEWELCRAFTING;
    else if (profession.compare("enchanting", Qt::CaseInsensitive) == 0)    return ProfessionType::ENCHANTING;
    else if (profession.compare("alchemy", Qt::CaseInsensitive) == 0)       return ProfessionType::ALCHEMY;
    else
    {
        return ProfessionType::UNKNOWN;
    }
}

QString QMUD::professionToReadableString(ProfessionType profession)
{
    switch (profession)
    {
    case ProfessionType::BLACKSMITHING:  return QObject::tr("forgiatura");
    case ProfessionType::COOKING:        return QObject::tr("cucina");
    case ProfessionType::TAILORING:      return QObject::tr("sartoria");
    case ProfessionType::JEWELCRAFTING:  return QObject::tr("oreficeria");
    case ProfessionType::ENCHANTING:     return QObject::tr("incantamento");
    case ProfessionType::ALCHEMY:        return QObject::tr("alchimia");
    case ProfessionType::UNKNOWN:
    case ProfessionType::MAX_VALUE:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ActionType QMUD::actionFromString(QString str)
{
    if (str.compare("open", Qt::CaseInsensitive) == 0) return ActionType::OPEN;
    else if (str.compare("close", Qt::CaseInsensitive) == 0) return ActionType::CLOSE;
    else if (str.compare("move", Qt::CaseInsensitive) == 0) return ActionType::MOVE;
    else return ActionType::UNKNOWN;
}

QString QMUD::actionToString(QMUD::ActionType action)
{
    switch (action)
    {
    case ActionType::OPEN:   return "open";
    case ActionType::CLOSE:  return "close";
    case ActionType::MOVE:   return "move";
    case ActionType::UNKNOWN:
        return "";
    }

    Q_ASSERT(false);
    return "";
}

QMUD::ClientData::ClientData(QMUD::ClientDataType type, bool isValid) :
    m_eType(type),
    m_bIsValid(isValid)
{

}

QMUD::ClientData::~ClientData()
{

}

QMUD::ClientDataType QMUD::ClientData::type() const
{
    return m_eType;
}

bool QMUD::ClientData::isValid() const
{
    return m_bIsValid;
}

QByteArray QMUD::ClientData::serialize() const
{
    QJsonObject obj;
    obj["_"] = static_cast<int>(m_eType);

    serializePrivate(obj);

    QByteArray data = QJsonDocument(obj).toJson();
    data.push_front(29);
    data.push_back(29);
    return data;
}

QMUD::ClientData *QMUD::ClientData::deserialize(QByteArray data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    if (!obj.contains("_"))
        return Q_NULLPTR;

    ClientDataType dataType = static_cast<ClientDataType>(obj["_"].toInt());
    ClientData* cData = Q_NULLPTR;

    switch (dataType)
    {
    case ClientDataType::LIFE:
        cData = new ClientDataLife();
        break;

    case ClientDataType::BASIC_INFO:
        cData = new ClientDataBasicInfo();
        break;

    case ClientDataType::TARGET_INFO:
        cData = new ClientDataTargetInfo();
        break;

    case ClientDataType::ACTION_LAG:
        cData = new ClientDataActionLag();
        break;

    case ClientDataType::LOOT:
        cData = new ClientDataLoot();
        break;

    case ClientDataType::STATUS:
        cData = new ClientDataStatus();
        break;

    case ClientDataType::AGGRO:
        cData = new ClientDataTargetAggro();
        break;

    case ClientDataType::ROOM_INFO:
        cData = new ClientDataRoomInfo();
        break;

    case ClientDataType::ROOM_CONTENT:
        cData = new ClientDataRoomContent();
        break;

    case ClientDataType::ATTACK_INFO:
        cData = new ClientDataAttackInfo();
        break;

    case ClientDataType::NEW_COMBAT_ROUND:
        cData = new ClientDataNewCombatRound();
        break;

    case ClientDataType::REQUEST:
        cData = new ClientDataRequest();
        break;

    case ClientDataType::CDT_MESSAGE:
        cData = new ClientDataMessage();
        break;

    case ClientDataType::CHARACTERS_LIST:
        cData = new ClientDataCharactersList();
        break;
    }

    if (cData)
    {
        cData->deserializePrivate(obj);
        cData->m_bIsValid = true;
    }

    return cData;
}

void QMUD::ClientData::operator=(const QMUD::ClientData &o)
{
    m_bIsValid = o.m_bIsValid;
}

bool QMUD::ClientData::operator==(const QMUD::ClientData &o) const
{
    return m_bIsValid == o.m_bIsValid;
}

bool QMUD::ClientData::operator!=(const QMUD::ClientData &o) const
{
    return !(operator==(o));
}

QMUD::ClientDataLife::ClientDataLife() :
    ClientData(ClientDataType::LIFE, false),
    m_iHpCurrent(0),
    m_iHpCurrentMaximum(0),
    m_iMpCurrent(0),
    m_iMpCurrentMaximum(0),
    m_iApCurrent(0),
    m_iApCurrentMaximum(0)
{

}

QMUD::ClientDataLife::ClientDataLife(int hpCurrent, int hpCurrentMaximum, int mpCurrent, int mpCurrentMaximum, int apCurrent, int apCurrentMaximum) :
    ClientData(ClientDataType::LIFE, true),
    m_iHpCurrent(hpCurrent),
    m_iHpCurrentMaximum(hpCurrentMaximum),
    m_iMpCurrent(mpCurrent),
    m_iMpCurrentMaximum(mpCurrentMaximum),
    m_iApCurrent(apCurrent),
    m_iApCurrentMaximum(apCurrentMaximum)
{

}

QMUD::ClientDataLife::~ClientDataLife()
{

}

void QMUD::ClientDataLife::operator=(const QMUD::ClientDataLife &o)
{
    ClientData::operator=(o);

    m_iHpCurrent = o.m_iHpCurrent;
    m_iHpCurrentMaximum = o.m_iHpCurrentMaximum;
    m_iMpCurrent = o.m_iMpCurrent;
    m_iMpCurrentMaximum = o.m_iMpCurrentMaximum;
    m_iApCurrent = o.m_iApCurrent;
    m_iApCurrentMaximum = o.m_iApCurrentMaximum;
}

bool QMUD::ClientDataLife::operator==(const QMUD::ClientDataLife &o) const
{
    return ClientData::operator==(o) &&
            m_iHpCurrent == o.m_iHpCurrent &&
            m_iHpCurrentMaximum == o.m_iHpCurrentMaximum &&
            m_iMpCurrent == o.m_iMpCurrent &&
            m_iMpCurrentMaximum == o.m_iMpCurrentMaximum &&
            m_iApCurrent == o.m_iApCurrent &&
            m_iApCurrentMaximum == o.m_iApCurrentMaximum;
}

bool QMUD::ClientDataLife::operator!=(const QMUD::ClientDataLife &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataLife::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_HP_CURRENT] = m_iHpCurrent;
    obj[KEY_HP_CURRENT_MAX] = m_iHpCurrentMaximum;
    obj[KEY_MP_CURRENT] = m_iMpCurrent;
    obj[KEY_MP_CURRENT_MAX] = m_iMpCurrentMaximum;
    obj[KEY_AP_CURRENT] = m_iApCurrent;
    obj[KEY_AP_CURRENT_MAX] = m_iApCurrentMaximum;
}

void QMUD::ClientDataLife::deserializePrivate(const QJsonObject &obj)
{
    m_iHpCurrent = obj[KEY_HP_CURRENT].toInt();
    m_iHpCurrentMaximum = obj[KEY_HP_CURRENT_MAX].toInt();
    m_iMpCurrent = obj[KEY_MP_CURRENT].toInt();
    m_iMpCurrentMaximum = obj[KEY_MP_CURRENT_MAX].toInt();
    m_iApCurrent = obj[KEY_AP_CURRENT].toInt();
    m_iApCurrentMaximum = obj[KEY_AP_CURRENT_MAX].toInt();
}

QMUD::ClientDataTargetInfo::ClientDataTargetInfo() :
    ClientData(ClientDataType::TARGET_INFO, false),
    m_iHpCurrent(0),
    m_iHpCurrentMaximum(0),
    m_iMpCurrent(0),
    m_iMpCurrentMaximum(0),
    m_iApCurrent(0),
    m_iApCurrentMaximum(0),
    m_iId(ULLONG_MAX),
    m_iLevel(0)
{

}

QMUD::ClientDataTargetInfo::ClientDataTargetInfo(QString name, int level, int hpCurrent, int hpCurrentMaximum, int mpCurrent,
                                                 int mpCurrentMaximum, int apCurrent, int apCurrentMaximum, IdType id) :
    ClientData(ClientDataType::TARGET_INFO, true),
    m_iHpCurrent(hpCurrent),
    m_iHpCurrentMaximum(hpCurrentMaximum),
    m_iMpCurrent(mpCurrent),
    m_iMpCurrentMaximum(mpCurrentMaximum),
    m_iApCurrent(apCurrent),
    m_iApCurrentMaximum(apCurrentMaximum),
    m_strName(name),
    m_iId(id),
    m_iLevel(level)
{

}

QMUD::ClientDataTargetInfo::~ClientDataTargetInfo()
{

}

void QMUD::ClientDataTargetInfo::operator=(const QMUD::ClientDataTargetInfo &o)
{
    ClientData::operator=(o);

    m_iHpCurrent = o.m_iHpCurrent;
    m_iHpCurrentMaximum = o.m_iHpCurrentMaximum;
    m_iMpCurrent = o.m_iMpCurrent;
    m_iMpCurrentMaximum = o.m_iMpCurrentMaximum;
    m_iApCurrent = o.m_iApCurrent;
    m_iApCurrentMaximum = o.m_iApCurrentMaximum;
    m_strName = o.m_strName;
    m_iId = o.m_iId;
    m_iLevel = o.m_iLevel;
}

bool QMUD::ClientDataTargetInfo::operator==(const QMUD::ClientDataTargetInfo &o) const
{
    return ClientData::operator==(o) &&
            m_iHpCurrent == o.m_iHpCurrent &&
            m_iHpCurrentMaximum == o.m_iHpCurrentMaximum &&
            m_iMpCurrent == o.m_iMpCurrent &&
            m_iMpCurrentMaximum == o.m_iMpCurrentMaximum &&
            m_iApCurrent == o.m_iApCurrent &&
            m_iApCurrentMaximum == o.m_iApCurrentMaximum &&
            m_strName == o.m_strName &&
            m_iId == o.m_iId &&
            m_iLevel == o.m_iLevel;
}

bool QMUD::ClientDataTargetInfo::operator!=(const QMUD::ClientDataTargetInfo &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataTargetInfo::serializePrivate(QJsonObject &obj) const
{
    if (!m_strName.isEmpty())
    {
        obj[KEY_HP_CURRENT] = m_iHpCurrent;
        obj[KEY_HP_CURRENT_MAX] = m_iHpCurrentMaximum;
        obj[KEY_MP_CURRENT] = m_iMpCurrent;
        obj[KEY_MP_CURRENT_MAX] = m_iMpCurrentMaximum;
        obj[KEY_AP_CURRENT] = m_iApCurrent;
        obj[KEY_AP_CURRENT_MAX] = m_iApCurrentMaximum;
        obj[KEY_NAME] = m_strName;
        obj[KEY_ID] = QString::number(m_iId);
        obj[KEY_LEVEL] = m_iLevel;
    }
}

void QMUD::ClientDataTargetInfo::deserializePrivate(const QJsonObject &obj)
{
    if (obj.contains(KEY_NAME))
    {
        m_iHpCurrent = obj[KEY_HP_CURRENT].toInt();
        m_iHpCurrentMaximum = obj[KEY_HP_CURRENT_MAX].toInt();
        m_iMpCurrent = obj[KEY_MP_CURRENT].toInt();
        m_iMpCurrentMaximum = obj[KEY_MP_CURRENT_MAX].toInt();
        m_iApCurrent = obj[KEY_AP_CURRENT].toInt();
        m_iApCurrentMaximum = obj[KEY_AP_CURRENT_MAX].toInt();
        m_strName = obj[KEY_NAME].toString();
        m_iId = obj[KEY_ID].toString().toULongLong();
        m_iLevel = obj[KEY_LEVEL].toInt();
    }
    else
    {
        m_iHpCurrent = 0;
        m_iHpCurrentMaximum = 0;
        m_iMpCurrent = 0;
        m_iMpCurrentMaximum = 0;
        m_iApCurrent = 0;
        m_iApCurrentMaximum = 0;
        m_iId = ULLONG_MAX;
        m_iLevel = 0;
        m_strName.clear();
    }
}

QMUD::ClientDataBasicInfo::ClientDataBasicInfo() :
    ClientData(ClientDataType::BASIC_INFO, false),
    m_eClassType(ClassType::WARRIOR),
    m_iLevel(0),
    m_iExperience(0),
    m_iExperienceToLevelUp(0),
    m_iId(ULLONG_MAX)
{

}

QMUD::ClientDataBasicInfo::ClientDataBasicInfo(QString name, QMUD::RaceType race, ClassType classType,
                                               int level, qint64 experience, qint64 experienceToLevelUp, IdType id) :
    ClientData(ClientDataType::BASIC_INFO, true),
    m_strName(name),
    m_eRace(race),
    m_eClassType(classType),
    m_iLevel(level),
    m_iExperience(experience),
    m_iExperienceToLevelUp(experienceToLevelUp),
    m_iId(id)
{

}

QMUD::ClientDataBasicInfo::~ClientDataBasicInfo()
{

}

void QMUD::ClientDataBasicInfo::operator=(const QMUD::ClientDataBasicInfo &o)
{
    ClientData::operator=(o);

    m_strName = o.m_strName;
    m_eRace = o.m_eRace;
    m_eClassType = o.m_eClassType;
    m_iLevel = o.m_iLevel;
    m_iExperience = o.m_iExperience;
    m_iExperienceToLevelUp = o.m_iExperienceToLevelUp;
    m_iId = o.m_iId;
}

bool QMUD::ClientDataBasicInfo::operator==(const QMUD::ClientDataBasicInfo &o) const
{
    return ClientData::operator==(o) &&
            m_strName == o.m_strName &&
            m_eRace == o.m_eRace &&
            m_eClassType == o.m_eClassType &&
            m_iLevel == o.m_iLevel &&
            m_iExperience == o.m_iExperience &&
            m_iExperienceToLevelUp == o.m_iExperienceToLevelUp &&
            m_iId == o.m_iId;
}

bool QMUD::ClientDataBasicInfo::operator!=(const QMUD::ClientDataBasicInfo &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataBasicInfo::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_NAME] = m_strName;
    obj[KEY_RACE] = static_cast<int>(m_eRace);
    obj[KEY_CLASS] = static_cast<int>(m_eClassType);
    obj[KEY_LEVEL] = m_iLevel;
    obj[KEY_EXPERIENCE] = m_iExperience;
    obj[KEY_EXPERIENCE_TO_LEVEL_UP] = m_iExperienceToLevelUp;
    obj[KEY_ID] = QString::number(m_iId);
}

void QMUD::ClientDataBasicInfo::deserializePrivate(const QJsonObject &obj)
{
    m_strName = obj[KEY_NAME].toString();
    m_eRace = static_cast<RaceType>(obj[KEY_RACE].toInt());
    m_eClassType = static_cast<ClassType>(obj[KEY_CLASS].toInt());
    m_iLevel = obj[KEY_LEVEL].toInt();
    m_iExperience = obj[KEY_EXPERIENCE].toVariant().toLongLong();
    m_iExperienceToLevelUp = obj[KEY_EXPERIENCE_TO_LEVEL_UP].toVariant().toLongLong();
    m_iId = obj[KEY_ID].toString().toULongLong();
}

QMUD::ClientDataActionLag::ClientDataActionLag() :
    ClientData(ClientDataType::ACTION_LAG, false),
    m_iActionLag(0),
    m_bUpdate(false)
{

}

QMUD::ClientDataActionLag::ClientDataActionLag(int actionLag, bool update) :
    ClientData(ClientDataType::ACTION_LAG, true),
    m_iActionLag(actionLag),
    m_bUpdate(update)
{

}

QMUD::ClientDataActionLag::~ClientDataActionLag()
{

}

void QMUD::ClientDataActionLag::operator=(const QMUD::ClientDataActionLag &o)
{
    ClientData::operator=(o);

    m_iActionLag = o.m_iActionLag;
    m_bUpdate = o.m_bUpdate;
}

bool QMUD::ClientDataActionLag::operator==(const QMUD::ClientDataActionLag &o) const
{
    return ClientData::operator==(o) &&
            m_iActionLag == o.m_iActionLag &&
            m_bUpdate == o.m_bUpdate;
}

bool QMUD::ClientDataActionLag::operator!=(const QMUD::ClientDataActionLag &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataActionLag::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_ACTION_LAG] = m_iActionLag;
    obj[KEY_UPDATE] = static_cast<int>(m_bUpdate);
}

void QMUD::ClientDataActionLag::deserializePrivate(const QJsonObject &obj)
{
    m_iActionLag = obj[KEY_ACTION_LAG].toInt();
    m_bUpdate = !!obj[KEY_UPDATE].toInt();
}

QMUD::ClientDataLoot::ClientDataLoot() :
    ClientData(ClientDataType::LOOT, false)
{

}

QMUD::ClientDataLoot::ClientDataLoot(QVector<QMUD::ClientDataLoot::ItemData> loot) :
    ClientData(ClientDataType::LOOT, true),
    m_vLootList(loot)
{

}

QMUD::ClientDataLoot::~ClientDataLoot()
{

}

void QMUD::ClientDataLoot::operator=(const QMUD::ClientDataLoot &o)
{
    ClientData::operator=(o);

    m_vLootList = o.m_vLootList;
}

bool QMUD::ClientDataLoot::operator==(const QMUD::ClientDataLoot &o) const
{
    return ClientData::operator==(o) &&
            m_vLootList == o.m_vLootList;
}

bool QMUD::ClientDataLoot::operator!=(const QMUD::ClientDataLoot &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataLoot::serializePrivate(QJsonObject &obj) const
{
    QJsonArray array;

    for (const auto& item : m_vLootList)
    {
        QJsonObject objItem;
        objItem[KEY_NAME] = item.name;
        objItem[KEY_TO_SELL] = static_cast<int>(item.toSell);
        objItem[KEY_QUALITY] = static_cast<int>(item.quality);
        array.push_back(objItem);
    }

    obj[KEY_LOOT_LIST] = array;
}

void QMUD::ClientDataLoot::deserializePrivate(const QJsonObject &obj)
{
    m_vLootList.clear();

    QJsonArray array = obj[KEY_LOOT_LIST].toArray();

    m_vLootList.reserve(array.size());

    for (auto e : array)
    {
        QJsonObject objItem = e.toObject();
        ItemData data;
        data.name = objItem[KEY_NAME].toString();
        data.toSell = !!objItem[KEY_TO_SELL].toInt();
        data.quality = static_cast<ItemQualityType>(objItem[KEY_QUALITY].toInt());
        m_vLootList.push_back(data);
    }
}

QMUD::ClientDataLoot::ItemData::ItemData() :
    toSell(false),
    quality(ItemQualityType::JUNK)
{

}

bool QMUD::ClientDataLoot::ItemData::operator==(const ItemData &o) const
{
    return name == o.name &&
            toSell == o.toSell &&
            quality == o.quality;
}

QMUD::ClientDataStatus::StatusData::StatusData() :
    status(TemporaryStatusType::SHIELD),
    time(0)
{

}

bool QMUD::ClientDataStatus::StatusData::operator==(const QMUD::ClientDataStatus::StatusData &o) const
{
    return status == o.status &&
            time == o.time;
}

QMUD::ClientDataStatus::ClientDataStatus() :
    ClientData(ClientDataType::STATUS, false)
{

}

QMUD::ClientDataStatus::ClientDataStatus(QVector<QMUD::ClientDataStatus::StatusData> status) :
    ClientData(ClientDataType::STATUS, true),
    m_vStatusList(status)
{

}

QMUD::ClientDataStatus::~ClientDataStatus()
{

}

void QMUD::ClientDataStatus::operator=(const QMUD::ClientDataStatus &o)
{
    ClientData::operator=(o);

    m_vStatusList = o.m_vStatusList;
}

bool QMUD::ClientDataStatus::operator==(const QMUD::ClientDataStatus &o) const
{
    return ClientData::operator==(o) &&
            m_vStatusList == o.m_vStatusList;
}

bool QMUD::ClientDataStatus::operator!=(const QMUD::ClientDataStatus &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataStatus::serializePrivate(QJsonObject &obj) const
{
    QJsonArray array;

    for (const auto& status : m_vStatusList)
    {
        QJsonObject objStatus;
        objStatus[KEY_STATUS] = static_cast<int>(status.status);
        objStatus[KEY_TIME] = status.time;
        array.push_back(objStatus);
    }

    obj[KEY_STATUS_LIST] = array;
}

void QMUD::ClientDataStatus::deserializePrivate(const QJsonObject &obj)
{
    m_vStatusList.clear();

    QJsonArray array = obj[KEY_STATUS_LIST].toArray();

    m_vStatusList.reserve(array.size());

    for (auto e : array)
    {
        QJsonObject objStatus = e.toObject();
        StatusData data;
        data.time = objStatus[KEY_TIME].toInt();
        data.status = static_cast<TemporaryStatusType>(objStatus[KEY_STATUS].toInt());
        m_vStatusList.push_back(data);
    }
}

QMUD::ClientDataTargetAggro::AggroData::AggroData() :
    aggro(0)
{

}

bool QMUD::ClientDataTargetAggro::AggroData::operator==(const QMUD::ClientDataTargetAggro::AggroData &o) const
{
    return aggro == o.aggro &&
            name == o.name;
}


QMUD::ClientDataTargetAggro::ClientDataTargetAggro() :
    ClientData(ClientDataType::AGGRO, false)
{

}

QMUD::ClientDataTargetAggro::ClientDataTargetAggro(QVector<QMUD::ClientDataTargetAggro::AggroData> aggroList) :
    ClientData(ClientDataType::AGGRO, true),
    m_vAggroList(aggroList)
{

}

QMUD::ClientDataTargetAggro::~ClientDataTargetAggro()
{

}

void QMUD::ClientDataTargetAggro::operator=(const QMUD::ClientDataTargetAggro &o)
{
    m_vAggroList = o.m_vAggroList;
}

bool QMUD::ClientDataTargetAggro::operator==(const QMUD::ClientDataTargetAggro &o) const
{
    return m_vAggroList == o.m_vAggroList;
}

bool QMUD::ClientDataTargetAggro::operator!=(const QMUD::ClientDataTargetAggro &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataTargetAggro::serializePrivate(QJsonObject &obj) const
{
    QJsonArray array;

    for (const auto& aggro : m_vAggroList)
    {
        QJsonObject objAggro;
        objAggro[KEY_AGGRO] = aggro.aggro;
        objAggro[KEY_NAME] = aggro.name;
        array.push_back(objAggro);
    }

    obj[KEY_AGGRO_LIST] = array;
}

void QMUD::ClientDataTargetAggro::deserializePrivate(const QJsonObject &obj)
{
    m_vAggroList.clear();

    QJsonArray array = obj[KEY_AGGRO_LIST].toArray();

    m_vAggroList.reserve(array.size());

    for (auto e : array)
    {
        QJsonObject objAggro = e.toObject();
        AggroData data;
        data.aggro = objAggro[KEY_AGGRO].toVariant().toLongLong();
        data.name = objAggro[KEY_NAME].toString();
        m_vAggroList.push_back(data);
    }
}

QMUD::ClientDataRoomInfo::ClientDataRoomInfo() :
    ClientData(ClientDataType::ROOM_INFO, false),
    m_iMap(QMUD::StaticIdInvalid),
    m_iX(-1),
    m_iY(-1)
{

}

QMUD::ClientDataRoomInfo::ClientDataRoomInfo(QString name, QString description,
                                             StaticIdType map, StaticIdType index, int x, int y,
                                     QVector<ChInfo> chs) :
    ClientData(ClientDataType::ROOM_INFO, true),
    m_strName(name),
    m_strDescription(description),
    m_iMap(map),
    m_iIndex(index),
    m_iX(x),
    m_iY(y),
    m_vChs(chs)
{

}

QMUD::ClientDataRoomInfo::~ClientDataRoomInfo()
{

}

void QMUD::ClientDataRoomInfo::operator=(const QMUD::ClientDataRoomInfo &o)
{
    m_strName = o.m_strName;
    m_strDescription = o.m_strDescription;
    m_iMap = o.m_iMap;
    m_iIndex = o.m_iIndex;
    m_iX = o.m_iX;
    m_iY = o.m_iY;
    m_vChs = o.m_vChs;
}

bool QMUD::ClientDataRoomInfo::operator==(const QMUD::ClientDataRoomInfo &o) const
{
    return m_strName == o.m_strName &&
        m_strDescription == o.m_strDescription &&
        m_iMap == o.m_iMap &&
        m_iIndex == o.m_iIndex &&
        m_iX == o.m_iX &&
        m_iY == o.m_iY &&
        m_vChs == o.m_vChs;
}

bool QMUD::ClientDataRoomInfo::operator!=(const QMUD::ClientDataRoomInfo &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataRoomInfo::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_NAME] = m_strName;
    obj[KEY_DESCRIPTION] = m_strDescription;
    obj[KEY_MAP] = QString::number(m_iMap);
    obj[KEY_INDEX] = QString::number(m_iIndex);
    obj[KEY_X] = m_iX;
    obj[KEY_Y] = m_iY;

    QJsonArray npcs;

    for (auto elem : m_vChs)
    {
        QJsonObject npc;
        npc[KEY_CHS_X] = elem.x;
        npc[KEY_CHS_Y] = elem.y;
        npc[KEY_CHS_NAME] = elem.name;
        npc[KEY_CHS_RACE] = static_cast<int>(elem.race);
        npc[KEY_CHS_ID] = QString::number(elem.id);
        npc[KEY_CHS_TYPE] = static_cast<int>(elem.type);
        npcs.push_back(npc);
    }

    obj[KEY_CHS] = npcs;
}

void QMUD::ClientDataRoomInfo::deserializePrivate(const QJsonObject &obj)
{
    m_strName = obj[KEY_NAME].toString();
    m_strDescription = obj[KEY_DESCRIPTION].toString();
    m_iMap = obj[KEY_MAP].toString().toUInt();
    m_iIndex = obj[KEY_INDEX].toString().toUInt();
    m_iX = obj[KEY_X].toInt();
    m_iY = obj[KEY_Y].toInt();

    auto objNpcs = obj[KEY_CHS];

    if (objNpcs.isArray())
    {
        for (auto elem : objNpcs.toArray())
        {
            QJsonObject objNpc = elem.toObject();
            ChInfo npc;
            npc.x = objNpc[KEY_CHS_X].toInt();
            npc.y = objNpc[KEY_CHS_Y].toInt();
            npc.name = objNpc[KEY_CHS_NAME].toString();
            npc.race = static_cast<QMUD::RaceType>(objNpc[KEY_CHS_RACE].toInt());
            npc.id = objNpc[KEY_CHS_ID].toString().toULongLong();
            npc.type = static_cast<ChType>(objNpc[KEY_CHS_TYPE].toInt());
            m_vChs.push_back(npc);
        }
    }
}


QMUD::ClientDataRoomContent::ClientDataRoomContent() :
    ClientData(ClientDataType::ROOM_CONTENT, false)
{

}

QMUD::ClientDataRoomContent::ClientDataRoomContent(QVector<CharacterData> npcs) :
    ClientData(ClientDataType::ROOM_CONTENT, true),
    m_vCharacters(npcs)
{

}

QMUD::ClientDataRoomContent::~ClientDataRoomContent()
{

}

void QMUD::ClientDataRoomContent::operator=(const QMUD::ClientDataRoomContent &o)
{
    m_vCharacters = o.m_vCharacters;
}

bool QMUD::ClientDataRoomContent::operator==(const QMUD::ClientDataRoomContent &o) const
{
    return m_vCharacters == o.m_vCharacters;
}

bool QMUD::ClientDataRoomContent::operator!=(const QMUD::ClientDataRoomContent &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataRoomContent::serializePrivate(QJsonObject &obj) const
{
    QJsonArray npcs;

    for (auto elem : m_vCharacters)
    {
        QJsonObject objNpc;
        objNpc[KEY_CHDATA_NAME] = elem.name;
        objNpc[KEY_CHDATA_LEVEL] = elem.level;
        objNpc[KEY_CHDATA_IS_NPC] = elem.isNpc;
        objNpc[KEY_CHDATA_HP] = elem.hpCurrent;
        objNpc[KEY_CHDATA_HP_MAX] = elem.hpMax;
        objNpc[KEY_CHDATA_MP] = elem.mpCurrent;
        objNpc[KEY_CHDATA_MP_MAX] = elem.mpMax;
        objNpc[KEY_CHDATA_ID] = QString::number(elem.id);
        npcs.push_back(objNpc);
    }

    obj[KEY_CHDATA] = npcs;
}

void QMUD::ClientDataRoomContent::deserializePrivate(const QJsonObject &obj)
{
    auto objNpcs = obj[KEY_CHDATA];

    if (objNpcs.isArray())
    {
        for (auto elem : objNpcs.toArray())
        {
            QJsonObject npcObj = elem.toObject();

            CharacterData npc;
            npc.name =      npcObj[KEY_CHDATA_NAME].toString();
            npc.level =     npcObj[KEY_CHDATA_LEVEL].toInt();
            npc.isNpc =     npcObj[KEY_CHDATA_IS_NPC].toBool();
            npc.hpCurrent = npcObj[KEY_CHDATA_HP].toInt();
            npc.hpMax =     npcObj[KEY_CHDATA_HP_MAX].toInt();
            npc.mpCurrent = npcObj[KEY_CHDATA_MP].toInt();
            npc.mpMax =     npcObj[KEY_CHDATA_MP_MAX].toInt();
            npc.id =        npcObj[KEY_CHDATA_ID].toString().toULongLong();

            m_vCharacters.push_back(npc);
        }
    }
}

QMUD::ClientDataAttackInfo::ClientDataAttackInfo() :
    ClientData(ClientDataType::ATTACK_INFO, false),
    m_iValue(VALUE_NONE),
    m_bIsDamage(false),
    m_iSuitorId(IdInvalid),
    m_iTargetId(IdInvalid),
    m_eDamageType(QMUD::DamageType::HANDS),
    m_eAbility(QMUD::AbilityType::UNKNOWN),
    m_eSpell(QMUD::SpellType::UNKNOWN)
{

}

QMUD::ClientDataAttackInfo::ClientDataAttackInfo(int value, IdType suitorId, IdType targetId, bool is_damage, DamageType damageType, QMUD::AbilityType ability, QMUD::SpellType spell) :
    ClientData(ClientDataType::ATTACK_INFO, true),
    m_iValue(value),
    m_bIsDamage(is_damage),
    m_iSuitorId(suitorId),
    m_iTargetId(targetId),
    m_eDamageType(damageType),
    m_eAbility(ability),
    m_eSpell(spell)
{

}

QMUD::ClientDataAttackInfo::~ClientDataAttackInfo()
{

}

void QMUD::ClientDataAttackInfo::operator=(const QMUD::ClientDataAttackInfo &o)
{
    m_iValue = o.m_iValue;
    m_bIsDamage = o.m_bIsDamage;
    m_iSuitorId = o.m_iSuitorId;
    m_iTargetId = o.m_iTargetId;
    m_eDamageType = o.m_eDamageType;
    m_eAbility = o.m_eAbility;
    m_eSpell = o.m_eSpell;
}

bool QMUD::ClientDataAttackInfo::operator==(const QMUD::ClientDataAttackInfo &o) const
{
    return m_iValue == o.m_iValue &&
        m_bIsDamage == o.m_bIsDamage &&
        m_iSuitorId == o.m_iSuitorId &&
        m_iTargetId == o.m_iTargetId &&
        m_eDamageType == o.m_eDamageType &&
        m_eAbility == o.m_eAbility &&
        m_eSpell == o.m_eSpell;
}

bool QMUD::ClientDataAttackInfo::operator!=(const QMUD::ClientDataAttackInfo &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataAttackInfo::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_VALUE] = m_iValue;
    obj[KEY_IS_DAMAGE] = m_bIsDamage;
    obj[KEY_SUITOR_ID] = QString::number(m_iSuitorId);
    obj[KEY_TARGET_ID] = QString::number(m_iTargetId);
    obj[KEY_DAMAGE_TYPE] = static_cast<int>(m_eDamageType);
    obj[KEY_ABILITY] = static_cast<int>(m_eAbility);
    obj[KEY_SPELL] = static_cast<int>(m_eSpell);
}

void QMUD::ClientDataAttackInfo::deserializePrivate(const QJsonObject &obj)
{
    m_iValue = obj[KEY_VALUE].toInt();
    m_bIsDamage = obj[KEY_IS_DAMAGE].toBool();
    m_iSuitorId = obj[KEY_SUITOR_ID].toString().toULongLong();
    m_iTargetId = obj[KEY_TARGET_ID].toString().toULongLong();
    m_eDamageType = static_cast<QMUD::DamageType>(obj[KEY_DAMAGE_TYPE].toInt());
    m_eAbility = static_cast<QMUD::AbilityType>(obj[KEY_ABILITY].toInt());
    m_eSpell = static_cast<QMUD::SpellType>(obj[KEY_SPELL].toInt());
}

QMUD::ClientDataNewCombatRound::ClientDataNewCombatRound() :
    ClientData(ClientDataType::NEW_COMBAT_ROUND, true)
{

}

QMUD::ClientDataNewCombatRound::~ClientDataNewCombatRound()
{

}

void QMUD::ClientDataNewCombatRound::serializePrivate(QJsonObject &obj) const
{

}

void QMUD::ClientDataNewCombatRound::deserializePrivate(const QJsonObject &obj)
{

}


QMUD::ClientDataRequest::ClientDataRequest() :
    ClientData(ClientDataType::REQUEST, false)
{

}

QMUD::ClientDataRequest::ClientDataRequest(QString request, QStringList options) :
    ClientData(ClientDataType::REQUEST, true),
    m_vOptions(options),
    m_strRequest(request)
{

}

QMUD::ClientDataRequest::~ClientDataRequest()
{

}

void QMUD::ClientDataRequest::operator=(const QMUD::ClientDataRequest &o)
{
    m_vOptions = o.m_vOptions;
    m_strRequest = o.m_strRequest;
}

bool QMUD::ClientDataRequest::operator==(const QMUD::ClientDataRequest &o) const
{
    return m_vOptions == o.m_vOptions &&
            m_strRequest == o.m_strRequest;
}

bool QMUD::ClientDataRequest::operator!=(const QMUD::ClientDataRequest &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataRequest::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_OPTIONS] = QJsonArray::fromStringList(m_vOptions);
    obj[KEY_REQUEST] = m_strRequest;
}

void QMUD::ClientDataRequest::deserializePrivate(const QJsonObject &obj)
{
    m_vOptions = obj[KEY_OPTIONS].toVariant().toStringList();
    m_strRequest = obj[KEY_REQUEST].toString();
}


QMUD::ClientDataMessage::ClientDataMessage() :
    ClientData(ClientDataType::CDT_MESSAGE, false)
{

}

QMUD::ClientDataMessage::ClientDataMessage(Message error) :
    ClientData(ClientDataType::CDT_MESSAGE, true),
    m_eMessage(error)
{

}

QMUD::ClientDataMessage::~ClientDataMessage()
{

}

void QMUD::ClientDataMessage::operator=(const QMUD::ClientDataMessage &o)
{
    m_eMessage = o.m_eMessage;
}

bool QMUD::ClientDataMessage::operator==(const QMUD::ClientDataMessage &o) const
{
    return m_eMessage == o.m_eMessage;
}

bool QMUD::ClientDataMessage::operator!=(const QMUD::ClientDataMessage &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataMessage::serializePrivate(QJsonObject &obj) const
{
    obj[KEY_MESSAGE] = static_cast<int>(m_eMessage);
}

void QMUD::ClientDataMessage::deserializePrivate(const QJsonObject &obj)
{
    m_eMessage = static_cast<Message>(obj[KEY_MESSAGE].toInt());
}

QMUD::ClientDataCharactersList::ClientDataCharactersList() :
    ClientData(ClientDataType::CHARACTERS_LIST, false)
{

}

QMUD::ClientDataCharactersList::ClientDataCharactersList(QVector<PcCharacterBaseInfo> chsList) :
    ClientData(ClientDataType::CHARACTERS_LIST, true),
    m_vChsList(chsList)
{

}

QMUD::ClientDataCharactersList::~ClientDataCharactersList()
{

}

void QMUD::ClientDataCharactersList::operator=(const QMUD::ClientDataCharactersList &o)
{
    m_vChsList = o.m_vChsList;
}

bool QMUD::ClientDataCharactersList::operator==(const QMUD::ClientDataCharactersList &o) const
{
    return m_vChsList == o.m_vChsList;
}

bool QMUD::ClientDataCharactersList::operator!=(const QMUD::ClientDataCharactersList &o) const
{
    return !(operator==(o));
}

void QMUD::ClientDataCharactersList::serializePrivate(QJsonObject &obj) const
{
    QJsonArray arrayData;

    for (auto elem : m_vChsList)
    {
        QJsonObject elemObj;
        elem.serialize(elemObj);
        arrayData.push_back(elemObj);
    }

    obj[KEY_LIST] = arrayData;
}

void QMUD::ClientDataCharactersList::deserializePrivate(const QJsonObject &obj)
{
    m_vChsList.clear();

    QJsonArray arrayData = obj[KEY_LIST].toArray();

    for (auto it = arrayData.begin(); it != arrayData.end(); ++it)
    {
        QJsonObject elemObj = it->toObject();
        QMUD::PcCharacterBaseInfo baseInfo;
        baseInfo.deserialize(elemObj);
        m_vChsList.push_back(baseInfo);
    }
}

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif
