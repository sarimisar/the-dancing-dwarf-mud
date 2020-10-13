/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemarmor.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"
#include "world.h"

#include <QJsonArray>

using namespace CORE;

ItemArmor::ItemArmor(Entity *parent) :
    Item(QMUD::ItemType::ARMOR, parent),
    m_eArmorType(QMUD::ArmorType::CHEST),
    m_iArmorClassBonus(0),
    m_eClassType(QMUD::ItemClassType::TANK)
{

}

ItemArmor::~ItemArmor()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemArmor", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemArmor::loadItem(const QJsonObject &obj, Entity *parent, int level, QMUD::ItemQualityType quality)
{
    // ArmorType
    QString tmpStr;
    QMUD::ArmorType tmpArmorType;
    if (!QMUD::fileJsonRead(obj, "armorType", tmpStr))
        return Q_NULLPTR;

    tmpStr = tmpStr.toLower();

    if (tmpStr == "chest")
        tmpArmorType = QMUD::ArmorType::CHEST;
    else if (tmpStr == "shield")
        tmpArmorType = QMUD::ArmorType::SHIELD;
    else if (tmpStr == "gloves")
        tmpArmorType = QMUD::ArmorType::GLOVES;
    else if (tmpStr == "shoulder")
        tmpArmorType = QMUD::ArmorType::SHOULDER;
    else if (tmpStr == "bracers")
        tmpArmorType = QMUD::ArmorType::BRACERS;
    else if (tmpStr == "helm")
        tmpArmorType = QMUD::ArmorType::HELM;
    else if (tmpStr == "leg")
        tmpArmorType = QMUD::ArmorType::LEG;
    else if (tmpStr == "boots")
        tmpArmorType = QMUD::ArmorType::BOOTS;
    else
    {
        GLOBAL_CRITICAL(tr("Chiave obbligatoria 'armorType' non valida: %1").arg(tmpStr));
        return Q_NULLPTR;
    }

    // ItemClassType
    QMUD::ItemClassType tmpClassType;
    if (!QMUD::fileJsonRead(obj, "classType", tmpStr))
        return Q_NULLPTR;

    tmpClassType = QMUD::itemClassFromString(tmpStr);

    if (tmpClassType == QMUD::ItemClassType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave obbligatoria 'classType' non valida: %1").arg(tmpStr));
        return Q_NULLPTR;
    }

    // Base bonus
    QMUD::ItemBaseBonus baseBonus;
    auto itBaseBonus = obj.find("baseBonus");
    if (itBaseBonus != obj.end() && itBaseBonus->isObject())
    {
        if (!baseBonus.load(itBaseBonus->toObject()))
        {
            GLOBAL_CRITICAL(tr("Chiave 'baseBonus' non valida"));
            return Q_NULLPTR;
        }
    }

    // Base temporary status
    QMUD::ItemBaseStatus baseTemporaryStatus;
    auto itBaseTemporaryStatus = obj.find("baseStatus");
    if (itBaseTemporaryStatus != obj.end() && itBaseTemporaryStatus->isObject())
    {
        if (!baseTemporaryStatus.load(itBaseTemporaryStatus->toObject()))
        {
            GLOBAL_CRITICAL(tr("Chiave 'baseStatus' non valida"));
            return Q_NULLPTR;
        }
    }

    updateBaseBonus(level, quality, tmpClassType, baseBonus);

    ItemArmor* item = new ItemArmor(parent);
    item->m_eArmorType = tmpArmorType;
    item->m_iArmorClassBonus = getArmorClassBonus(level, quality, tmpClassType);
    item->m_eClassType = tmpClassType;
    item->m_itemBaseBonus = baseBonus;
    item->m_itemBaseTemporaryStatus = baseTemporaryStatus;

    return item;
}

QString ItemArmor::getNameByRandomCode(QString randomCode)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return QString();

    int level = elements[1].toInt();
    QMUD::ItemClassType itemClass = QMUD::itemClassFromString(elements[2]);
    QMUD::ItemQualityType quality = QMUD::itemQualityFromString(elements[3]);
    int randomNumber0 = elements[4].toInt();
    QMUD::ArmorType armorType = QMUD::armorTypeFromString(elements[5]);

    QString name;

    switch (armorType)
    {
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    case QMUD::ArmorType::CHEST:
        if (itemClass == QMUD::ItemClassType::CASTER_INT)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "una veste da straccione";
                else if (randomNumber0 == 2)    name = "una vecchia veste malconcia";
                else if (randomNumber0 == 3)    name = "una vecchia veste strappata";
                else if (randomNumber0 == 4)    name = "una veste malridotta";
                else                            name = "una vecchia veste rovinata";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "una veste da incantatore";
                else if (randomNumber0 == 2)    name = "una veste da mistico";
                else if (randomNumber0 == 3)    name = "una veste da principiante";
                else if (randomNumber0 == 4)    name = "una veste da stregone";
                else                            name = "una veste da mago";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "una veste del provetto mago";
                else if (level < 40)    name = "una veste dell'incatatore esperto";
                else if (level < 50)    name = "una veste del mago esperto";
                else if (level < 60)    name = "una veste dello stregone esperto";
                else if (level < 70)    name = "una veste dell'esperto di incantesimi";
                else if (level < 80)    name = "una veste del mistico esperto";
                else if (level < 90)    name = "una veste dell'esperto di magia";
                else                    name = "una veste superiore del mago esperto";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "la veste del maestro mago";
                else if (level < 40)    name = "la veste del maestro incantatore";
                else if (level < 50)    name = "la veste del maestro mago";
                else if (level < 60)    name = "la veste del maestro stregone";
                else if (level < 70)    name = "la veste del maestro degli incantesimi";
                else if (level < 80)    name = "la veste del maestro mistico";
                else if (level < 90)    name = "la veste del maestro di magia";
                else                    name = "la veste del maestro antico";
            }
        }
        else if (itemClass == QMUD::ItemClassType::CASTER_WIS)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "una veste da straccione";
                else if (randomNumber0 == 2)    name = "una vecchia veste malconcia";
                else if (randomNumber0 == 3)    name = "una vecchia veste strappata";
                else if (randomNumber0 == 4)    name = "una veste malridotta";
                else                            name = "una vecchia veste rovinata";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "una veste da sacerdote";
                else if (randomNumber0 == 2)    name = "una veste da mistico";
                else if (randomNumber0 == 3)    name = "una veste di seta";
                else if (randomNumber0 == 4)    name = "una veste da chierico";
                else                            name = "una veste da sapiente";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "una veste da chierico provetto";
                else if (level < 40)    name = "una veste da sacerdote esperto";
                else if (level < 50)    name = "una veste da ministro del culto";
                else if (level < 60)    name = "una veste da divinatore esperto";
                else if (level < 70)    name = "una veste da servitore degli dei";
                else if (level < 80)    name = "una veste da primo adepto";
                else if (level < 90)    name = "una veste da chierico esperto";
                else                    name = "una veste superiore da sacerdote";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "la veste da maestro chierico";
                else if (level < 40)    name = "la veste da maestro del culto";
                else if (level < 50)    name = "la veste da maestro sacerdote";
                else if (level < 60)    name = "la veste del maestro divinatore";
                else if (level < 70)    name = "la veste del maestro degli dei antichi";
                else if (level < 80)    name = "la veste del maestro difensore";
                else if (level < 90)    name = "la veste del maestro del culto antico";
                else                    name = "la veste del difensore degli dei";
            }
        }
        else if (itemClass == QMUD::ItemClassType::DPS_DEX)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "una armatura di pelle logora";
                else if (randomNumber0 == 2)    name = "una vecchia armatura di pelle bucata";
                else if (randomNumber0 == 3)    name = "una armatura di pelle usurata";
                else if (randomNumber0 == 4)    name = "una armatura di pelle mal ridotta";
                else                            name = "una vecchia armatura di pelle rovinata";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "una armatura leggera";
                else if (randomNumber0 == 2)    name = "una armatura di cuoio";
                else if (randomNumber0 == 3)    name = "una armatura da furfante";
                else if (randomNumber0 == 4)    name = "una armatura di pelle";
                else                            name = "una armatura leggera";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "una armatura dello spadaccino provetto";
                else if (level < 40)    name = "una armatura dello spadaccino esperto";
                else if (level < 50)    name = "una armatura leggera del combattente";
                else if (level < 60)    name = "una armatura del furfante esperto";
                else if (level < 70)    name = "una armatura dell'esperto di spada";
                else if (level < 80)    name = "una armatura dell'esperto di battaglia";
                else if (level < 90)    name = "una armatura della morte silenziosa";
                else                    name = "una armatura della lama invisibile";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "l'armatura dello spadaccino abile";
                else if (level < 40)    name = "l'armatura del maestro spadaccino";
                else if (level < 50)    name = "l'armatura leggera del maestro combattente";
                else if (level < 60)    name = "l'armatura del signore dei furfanti";
                else if (level < 70)    name = "l'armatura leggera del maestro di spada";
                else if (level < 80)    name = "l'armatura leggera del maestro di battaglia";
                else if (level < 90)    name = "l'armatura della morte silenziosa";
                else                    name = "l'armatura della morte invisibile";
            }
        }
        else if (itemClass == QMUD::ItemClassType::DPS_STR)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "una armatura di maglia logora";
                else if (randomNumber0 == 2)    name = "una vecchia armatura di maglia arrugginita";
                else if (randomNumber0 == 3)    name = "una armatura di maglia usurata";
                else if (randomNumber0 == 4)    name = "una armatura di maglia mal ridotta";
                else                            name = "una vecchia armatura di maglia rovinata";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "una armatura di maglia";
                else if (randomNumber0 == 2)    name = "una armatura media";
                else if (randomNumber0 == 3)    name = "una armatura da guerriero";
                else if (randomNumber0 == 4)    name = "una armatura di maglia da combattente";
                else                            name = "una armatura di maglia di ferro";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "una armatura del guerriero provetto";
                else if (level < 40)    name = "una armatura del guerriero esperto";
                else if (level < 50)    name = "una armatura di maglia del combattente";
                else if (level < 60)    name = "una armatura del combattente esperto";
                else if (level < 70)    name = "una armatura media dell'esperto di spada";
                else if (level < 80)    name = "una armatura dell'esperto di guerra";
                else if (level < 90)    name = "una armatura dello stratega";
                else                    name = "una armatura del distruttore";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "l'armatura media del guerriero abile";
                else if (level < 40)    name = "l'armatura media del maestro guerriero";
                else if (level < 50)    name = "l'armatura media del maestro combattente";
                else if (level < 60)    name = "l'armatura media del maestro di guerra";
                else if (level < 70)    name = "l'armatura di maglia del maestro di spada";
                else if (level < 80)    name = "l'armatura di maglia del maestro di battaglia";
                else if (level < 90)    name = "l'armatura del condottiero";
                else                    name = "l'armatura del distruttore";
            }
        }
        else if (itemClass == QMUD::ItemClassType::TANK)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "una armatura di piastre logora";
                else if (randomNumber0 == 2)    name = "una vecchia armatura di piastre arrugginita";
                else if (randomNumber0 == 3)    name = "una armatura di piastre usurata";
                else if (randomNumber0 == 4)    name = "una armatura pesante mal ridotta";
                else                            name = "una vecchia armatura pesante rovinata";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "una armatura di piastre";
                else if (randomNumber0 == 2)    name = "una armatura pesante";
                else if (randomNumber0 == 3)    name = "una armatura da maestro d'arme";
                else if (randomNumber0 == 4)    name = "una armatura di piastre da combattente";
                else                            name = "una armatura media";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "una armatura del maestro d'arme provetto";
                else if (level < 40)    name = "una armatura media del guerriero esperto";
                else if (level < 50)    name = "una armatura di piastre del combattente";
                else if (level < 60)    name = "una armatura del difensore esperto";
                else if (level < 70)    name = "una armatura media dell'esperto di spada";
                else if (level < 80)    name = "una armatura del paladino esperto";
                else if (level < 90)    name = "una armatura del custode";
                else                    name = "una armatura del protettore";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "l'armatura pesante del guerriero abile";
                else if (level < 40)    name = "l'armatura pesante del maestro guerriero";
                else if (level < 50)    name = "l'armatura di piastre del maestro combattente";
                else if (level < 60)    name = "l'armatura pesante del maestro di guerra";
                else if (level < 70)    name = "l'armatura pesante del maestro di spada";
                else if (level < 80)    name = "l'armatura pesante del maestro di battaglia";
                else if (level < 90)    name = "l'armatura pesante del custode";
                else                    name = "l'armatura pesante del protettore";
            }
        }
        break;

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    case QMUD::ArmorType::SHIELD:
        if (itemClass == QMUD::ItemClassType::CASTER_INT)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un'asse di legno";
                else if (randomNumber0 == 2)    name = "un scudo rotto";
                else if (randomNumber0 == 3)    name = "un vecchio coperchio da botte";
                else if (randomNumber0 == 4)    name = "uno scudo di ferro arrugginito";
                else                            name = "un vecchio scudo malconcio";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "un piccolo scudo di legno";
                else if (randomNumber0 == 2)    name = "uno scudo buckler";
                else if (randomNumber0 == 3)    name = "un piccolo scudo di ferro";
                else if (randomNumber0 == 4)    name = "un piccolo scudo di legno";
                else                            name = "uno scudo buckler";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "uno scudo del provetto mago";
                else if (level < 40)    name = "uno scudo dell'incatatore esperto";
                else if (level < 50)    name = "uno scudo del mago esperto";
                else if (level < 60)    name = "uno scudo dello stregone esperto";
                else if (level < 70)    name = "uno scudo dell'esperto di incantesimi";
                else if (level < 80)    name = "uno scudo del mistico esperto";
                else if (level < 90)    name = "uno scudo dell'esperto di magia";
                else                    name = "uno scudo superiore del mago esperto";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "lo scudo del maestro mago";
                else if (level < 40)    name = "lo scudo del maestro incantatore";
                else if (level < 50)    name = "lo scudo del maestro divinatore";
                else if (level < 60)    name = "lo scudo del maestro stregone";
                else if (level < 70)    name = "lo scudo del maestro degli incantesimi";
                else if (level < 80)    name = "lo scudo del maestro mistico";
                else if (level < 90)    name = "lo scudo del maestro di magia";
                else                    name = "lo scudo del maestro antico";
            }
        }
        else if (itemClass == QMUD::ItemClassType::CASTER_WIS)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un'asse di legno";
                else if (randomNumber0 == 2)    name = "un scudo rotto";
                else if (randomNumber0 == 3)    name = "un vecchio coperchio da botte";
                else if (randomNumber0 == 4)    name = "uno scudo di ferro arrugginito";
                else                            name = "un vecchio scudo malconcio";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "uno scudo da sacerdote";
                else if (randomNumber0 == 2)    name = "uno scudo da mistico";
                else if (randomNumber0 == 3)    name = "uno scudo di legno";
                else if (randomNumber0 == 4)    name = "uno scudo da chierico";
                else                            name = "uno scudo da sapiente";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "uno scudo da chierico provetto";
                else if (level < 40)    name = "uno scudo da sacerdote esperto";
                else if (level < 50)    name = "uno scudo da ministro del culto";
                else if (level < 60)    name = "uno scudo da divinatore esperto";
                else if (level < 70)    name = "uno scudo da servitore degli dei";
                else if (level < 80)    name = "uno scudo da primo adepto";
                else if (level < 90)    name = "uno scudo da chierico esperto";
                else                    name = "uno scudo superiore da sacerdote";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "lo scudo del maestro chierico";
                else if (level < 40)    name = "lo scudo del maestro del culto";
                else if (level < 50)    name = "lo scudo del maestro sacerdote";
                else if (level < 60)    name = "lo scudo del maestro divinatore";
                else if (level < 70)    name = "lo scudo del maestro degli dei antichi";
                else if (level < 80)    name = "lo scudo del maestro difensore";
                else if (level < 90)    name = "lo scudo del maestro del culto antico";
                else                    name = "lo scudo del difensore degli dei";
            }
        }
        else if (itemClass == QMUD::ItemClassType::DPS_DEX)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un'asse di legno";
                else if (randomNumber0 == 2)    name = "un scudo rotto";
                else if (randomNumber0 == 3)    name = "un vecchio coperchio da botte";
                else if (randomNumber0 == 4)    name = "uno scudo di ferro arrugginito";
                else                            name = "un vecchio scudo malconcio";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "uno scudo leggero";
                else if (randomNumber0 == 2)    name = "uno scudo di cuoio indurito";
                else if (randomNumber0 == 3)    name = "uno scudo da furfante";
                else if (randomNumber0 == 4)    name = "uno scudo di legno";
                else                            name = "uno scudo leggero";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "uno scudo dello spadaccino provetto";
                else if (level < 40)    name = "uno scudo dello spadaccino esperto";
                else if (level < 50)    name = "uno scudo da combattente";
                else if (level < 60)    name = "uno scudo del furfante esperto";
                else if (level < 70)    name = "uno scudo dell'esperto di spada";
                else if (level < 80)    name = "uno scudo dell'esperto di battaglia";
                else if (level < 90)    name = "uno scudo della morte silenziosa";
                else                    name = "uno scudo della lama invisibile";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "lo scudo dello spadaccino abile";
                else if (level < 40)    name = "lo scudo del maestro spadaccino";
                else if (level < 50)    name = "lo scudo leggero del maestro combattente";
                else if (level < 60)    name = "lo scudo del signore dei furfanti";
                else if (level < 70)    name = "lo scudo leggero del maestro di spada";
                else if (level < 80)    name = "lo scudo leggero del maestro di battaglia";
                else if (level < 90)    name = "lo scudo della morte silenziosa";
                else                    name = "lo scudo della morte invisibile";
            }
        }
        else if (itemClass == QMUD::ItemClassType::DPS_STR)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un'asse di legno";
                else if (randomNumber0 == 2)    name = "un scudo rotto";
                else if (randomNumber0 == 3)    name = "un vecchio coperchio da botte";
                else if (randomNumber0 == 4)    name = "uno scudo di ferro arrugginito";
                else                            name = "un vecchio scudo malconcio";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "uno scudo medio";
                else if (randomNumber0 == 2)    name = "uno scudo di ferro";
                else if (randomNumber0 == 3)    name = "uno scudo medio di ferro";
                else if (randomNumber0 == 4)    name = "uno scudo medio di legno";
                else                            name = "uno scudo medio";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "uno scudo del guerriero provetto";
                else if (level < 40)    name = "uno scudo del guerriero esperto";
                else if (level < 50)    name = "uno scudo di ferro del combattente";
                else if (level < 60)    name = "uno scudo del combattente esperto";
                else if (level < 70)    name = "uno scudo medio dell'esperto di spada";
                else if (level < 80)    name = "uno scudo dell'esperto di guerra";
                else if (level < 90)    name = "uno scudo dello stratega";
                else                    name = "uno scudo del distruttore";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "lo scudo medio del guerriero abile";
                else if (level < 40)    name = "lo scudo medio del maestro guerriero";
                else if (level < 50)    name = "lo scudo medio del maestro combattente";
                else if (level < 60)    name = "lo scudo medio del maestro di guerra";
                else if (level < 70)    name = "lo scudo di ferro del maestro di spada";
                else if (level < 80)    name = "lo scudo di ferro del maestro di battaglia";
                else if (level < 90)    name = "lo scudo del condottiero";
                else                    name = "lo scudo del distruttore";
            }
        }
        else if (itemClass == QMUD::ItemClassType::TANK)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un'asse di legno";
                else if (randomNumber0 == 2)    name = "un scudo rotto";
                else if (randomNumber0 == 3)    name = "un vecchio coperchio da botte";
                else if (randomNumber0 == 4)    name = "uno scudo di ferro arrugginito";
                else                            name = "un vecchio scudo malconcio";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "uno scudo pesante di ferro";
                else if (randomNumber0 == 2)    name = "uno scudo pesante";
                else if (randomNumber0 == 3)    name = "uno scudo torre";
                else if (randomNumber0 == 4)    name = "uno scudo torre da combattente";
                else                            name = "uno scudo pesante";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "uno scudo del maestro d'arme provetto";
                else if (level < 40)    name = "uno scudo pesante del guerriero esperto";
                else if (level < 50)    name = "uno scudo torre del combattente";
                else if (level < 60)    name = "uno scudo del difensore esperto";
                else if (level < 70)    name = "uno scudo pesante dell'esperto di spada";
                else if (level < 80)    name = "uno scudo del paladino esperto";
                else if (level < 90)    name = "uno scudo del custode";
                else                    name = "uno scudo del protettore";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "lo scudo torre del guerriero abile";
                else if (level < 40)    name = "lo scudo torre del maestro guerriero";
                else if (level < 50)    name = "lo scudo torre del maestro combattente";
                else if (level < 60)    name = "lo scudo torre del maestro di guerra";
                else if (level < 70)    name = "lo scudo torre del maestro di spada";
                else if (level < 80)    name = "lo scudo torre del maestro di battaglia";
                else if (level < 90)    name = "lo scudo torre del custode";
                else                    name = "lo scudo torre del protettore";
            }
        }
        break;

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    case QMUD::ArmorType::GLOVES:
        if (itemClass == QMUD::ItemClassType::CASTER_INT)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti bucati";
                else if (randomNumber0 == 2)    name = "un vecchio paio di guanti";
                else if (randomNumber0 == 3)    name = "un paio di guanti logori";
                else if (randomNumber0 == 4)    name = "dei guanti malridotti";
                else                            name = "dei guanti rovinati";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti da incantatore";
                else if (randomNumber0 == 2)    name = "un paio di guanti da mistico";
                else if (randomNumber0 == 3)    name = "un paio di guanti di seta";
                else if (randomNumber0 == 4)    name = "un paio di guanti da stregone";
                else                            name = "un paio di guanti da mago";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "un paio di guanti del provetto mago";
                else if (level < 40)    name = "un paio di guanti dell'incatatore esperto";
                else if (level < 50)    name = "un paio di guanti del mago esperto";
                else if (level < 60)    name = "un paio di guanti dello stregone esperto";
                else if (level < 70)    name = "un paio di guanti dell'esperto di incantesimi";
                else if (level < 80)    name = "un paio di guanti del mistico esperto";
                else if (level < 90)    name = "un paio di guanti dell'esperto di magia";
                else                    name = "un paio di guanti superiori del mago esperto";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "un paio di guanti da maestro mago";
                else if (level < 40)    name = "un paio di guanti da maestro incantatore";
                else if (level < 50)    name = "un paio di guanti da maestro mago";
                else if (level < 60)    name = "un paio di guanti da maestro stregone";
                else if (level < 70)    name = "un paio di guanti da maestro degli incantesimi";
                else if (level < 80)    name = "un paio di guanti da maestro mistico";
                else if (level < 90)    name = "un paio di guanti da maestro di magia";
                else                    name = "un paio di guanti da maestro antico";
            }
        }
        else if (itemClass == QMUD::ItemClassType::CASTER_WIS)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti bucati";
                else if (randomNumber0 == 2)    name = "un vecchio paio di guanti";
                else if (randomNumber0 == 3)    name = "un paio di guanti logori";
                else if (randomNumber0 == 4)    name = "dei guanti malridotti";
                else                            name = "dei guanti rovinati";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti da sacerdote";
                else if (randomNumber0 == 2)    name = "un paio di guanti da mistico";
                else if (randomNumber0 == 3)    name = "un paio di guanti di seta";
                else if (randomNumber0 == 4)    name = "un paio di guanti da chierico";
                else                            name = "un paio di guanti da sapiente";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "un paio di guanti da chierico provetto";
                else if (level < 40)    name = "un paio di guanti da sacerdote esperto";
                else if (level < 50)    name = "un paio di guanti da ministro del culto";
                else if (level < 60)    name = "un paio di guanti da divinatore esperto";
                else if (level < 70)    name = "un paio di guanti da servitore degli dei";
                else if (level < 80)    name = "un paio di guanti da primo adepto";
                else if (level < 90)    name = "un paio di guanti da chierico esperto";
                else                    name = "un paio di guanti superiori da sacerdote";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "un paio di guanti da maestro chierico";
                else if (level < 40)    name = "un paio di guanti da maestro del culto";
                else if (level < 50)    name = "un paio di guanti da maestro sacerdote";
                else if (level < 60)    name = "un paio di guanti del maestro divinatore";
                else if (level < 70)    name = "un paio di guanti del maestro degli dei antichi";
                else if (level < 80)    name = "un paio di guanti del maestro difensore";
                else if (level < 90)    name = "un paio di guanti del maestro del culto antico";
                else                    name = "un paio di guanti del difensore degli dei";
            }
        }
        else if (itemClass == QMUD::ItemClassType::DPS_DEX)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti di pelle bucati";
                else if (randomNumber0 == 2)    name = "un vecchio paio di guanti di pelle";
                else if (randomNumber0 == 3)    name = "un paio di guanti di pelle logori";
                else if (randomNumber0 == 4)    name = "dei guanti di pelle malridotti";
                else                            name = "dei guanti di pelle rovinati";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti di pelle rinforzata";
                else if (randomNumber0 == 2)    name = "un paio di guanti di cuoio";
                else if (randomNumber0 == 3)    name = "un paio di guanti di pelle da furfante";
                else if (randomNumber0 == 4)    name = "un paio di guanti di pelle rinforzata";
                else                            name = "un paio di guanti di pelle rinforzata";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "un paio di guanti dello spadaccino provetto";
                else if (level < 40)    name = "un paio di guanti dello spadaccino esperto";
                else if (level < 50)    name = "un paio di guanti leggeri del combattente";
                else if (level < 60)    name = "un paio di guanti del furfante esperto";
                else if (level < 70)    name = "un paio di guanti dell'esperto di spada";
                else if (level < 80)    name = "un paio di guanti dell'esperto di battaglia";
                else if (level < 90)    name = "un paio di guanti della morte silenziosa";
                else                    name = "un paio di guanti della lama invisibile";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "un paio di guanti dello spadaccino abile";
                else if (level < 40)    name = "un paio di guanti del maestro spadaccino";
                else if (level < 50)    name = "un paio di guanti leggeri del maestro combattente";
                else if (level < 60)    name = "un paio di guanti del signore dei furfanti";
                else if (level < 70)    name = "un paio di guanti leggeri del maestro di spada";
                else if (level < 80)    name = "un paio di guanti leggeri del maestro di battaglia";
                else if (level < 90)    name = "un paio di guanti della morte silenziosa";
                else                    name = "un paio di guanti della morte invisibile";
            }
        }
        else if (itemClass == QMUD::ItemClassType::DPS_STR)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti d'arme arrugginiti";
                else if (randomNumber0 == 2)    name = "un vecchio paio di guanti di cuoio rinforzato";
                else if (randomNumber0 == 3)    name = "un paio di guanti d'arme logori";
                else if (randomNumber0 == 4)    name = "dei guanti d'arme malridotti";
                else                            name = "dei guanti d'arme rovinati";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "dei guanti d'arme";
                else if (randomNumber0 == 2)    name = "dei guanti d'arme";
                else if (randomNumber0 == 3)    name = "dei guanti d'arme da guerriero";
                else if (randomNumber0 == 4)    name = "dei guanti d'arme da combattente";
                else                            name = "dei guanti d'arme";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "dei guanti d'arme del guerriero provetto";
                else if (level < 40)    name = "dei guanti d'arme del guerriero esperto";
                else if (level < 50)    name = "dei guanti d'arme del combattente";
                else if (level < 60)    name = "dei guanti d'arme del combattente esperto";
                else if (level < 70)    name = "dei guanti d'arme dell'esperto di spada";
                else if (level < 80)    name = "dei guanti d'arme dell'esperto di guerra";
                else if (level < 90)    name = "dei guanti d'arme dello stratega";
                else                    name = "dei guanti d'arme del distruttore";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "dei guanti d'arme del guerriero abile";
                else if (level < 40)    name = "dei guanti d'arme del maestro guerriero";
                else if (level < 50)    name = "dei guanti d'arme del maestro combattente";
                else if (level < 60)    name = "dei guanti d'arme del maestro di guerra";
                else if (level < 70)    name = "dei guanti d'arme del maestro di spada";
                else if (level < 80)    name = "dei guanti d'arme del maestro di battaglia";
                else if (level < 90)    name = "dei guanti d'arme del condottiero";
                else                    name = "dei guanti d'arme del distruttore";
            }
        }
        else if (itemClass == QMUD::ItemClassType::TANK)
        {
            if (quality == QMUD::ItemQualityType::JUNK)
            {
                if (randomNumber0 == 1)         name = "un paio di guanti corrazzati arrugginiti";
                else if (randomNumber0 == 2)    name = "un vecchio paio di guanti corrazzati";
                else if (randomNumber0 == 3)    name = "un paio di guanti corrazzati logori";
                else if (randomNumber0 == 4)    name = "dei guanti d'arme pesanti malridotti";
                else                            name = "dei guanti d'arme pesanti rovinati";
            }
            else if (quality == QMUD::ItemQualityType::COMMON ||
                     quality == QMUD::ItemQualityType::UNCOMMON)
            {
                if (randomNumber0 == 1)         name = "dei guanti corrazzati";
                else if (randomNumber0 == 2)    name = "dei guanti corrazzati";
                else if (randomNumber0 == 3)    name = "dei guanti da maestro d'arme";
                else if (randomNumber0 == 4)    name = "dei guanti corrazzati da combattente";
                else                            name = "dei guanti corrazzati";
            }
            else if (quality == QMUD::ItemQualityType::VALUABLE)
            {
                if (level < 20)         name = "dei guanti d'arme corrazzati del maestro d'arme provetto";
                else if (level < 40)    name = "dei guanti d'arme corrazzati media del guerriero esperto";
                else if (level < 50)    name = "dei guanti d'arme corrazzati di piastre del combattente";
                else if (level < 60)    name = "dei guanti d'arme corrazzati del difensore esperto";
                else if (level < 70)    name = "dei guanti d'arme corrazzati media dell'esperto di spada";
                else if (level < 80)    name = "dei guanti d'arme corrazzati del paladino esperto";
                else if (level < 90)    name = "dei guanti d'arme corrazzati del custode";
                else                    name = "dei guanti d'arme corrazzati del protettore";
            }
            else if (quality == QMUD::ItemQualityType::RARE ||
                     quality == QMUD::ItemQualityType::EPIC ||
                     quality == QMUD::ItemQualityType::LEGENDARY)
            {
                if (level < 20)         name = "dei guanti d'arme corrazzati del guerriero abile";
                else if (level < 40)    name = "dei guanti d'arme corrazzati del maestro guerriero";
                else if (level < 50)    name = "dei guanti d'arme corrazzati del maestro combattente";
                else if (level < 60)    name = "dei guanti d'arme corrazzati del maestro di guerra";
                else if (level < 70)    name = "dei guanti d'arme corrazzati del maestro di spada";
                else if (level < 80)    name = "dei guanti d'arme corrazzati del maestro di battaglia";
                else if (level < 90)    name = "dei guanti d'arme corrazzati del custode";
                else                    name = "dei guanti d'arme corrazzati del protettore";
            }
        }
        break;

    case QMUD::ArmorType::SHOULDER: name = "un paio di spalline";     break;
    case QMUD::ArmorType::BRACERS:  name = "un paio di bracciali";    break;
    case QMUD::ArmorType::HELM:     name = "un elmo";                 break;
    case QMUD::ArmorType::LEG:      name = "un paio di gambali";      break;
    case QMUD::ArmorType::BOOTS:    name = "un paio di stivali";      break;
    default:
        Q_ASSERT(Q_NULLPTR);
    }

    return name;
}

ItemArmor *ItemArmor::createFromRandomCode(QString randomCode, Entity *parent)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return Q_NULLPTR;

    int level = elements[1].toInt();
    QMUD::ItemClassType itemClass = QMUD::itemClassFromString(elements[2]);
    QMUD::ItemQualityType quality = QMUD::itemQualityFromString(elements[3]);
    QMUD::ArmorType armorType = QMUD::armorTypeFromString(elements[5]);

    QString name = getNameByRandomCode(randomCode);

    QMUD::ItemBaseBonus baseBonus;

    updateBaseBonus(level, quality, itemClass, baseBonus);

    ItemArmor* item = new ItemArmor(Q_NULLPTR);
    item->setParent(parent);
    item->m_iLevel = level;
    item->m_iCoinsValue = getDefaultCoinsValue(level, quality);
    item->m_iStaticId = QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM;
    item->m_iMapStaticId = QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM;
    item->m_strName = name;
    item->m_strDescription = name;
    item->m_eArmorType = armorType;
    item->m_iArmorClassBonus = getArmorClassBonus(level, quality, itemClass);
    item->m_eClassType = itemClass;
    item->m_itemBaseBonus = baseBonus;
    item->m_strRandomCode = randomCode;
    item->m_eQuality = quality;
    item->m_uuid = QUuid::createUuid();

    QMUD::qmudWorld->addItem(item);

    return item;
}

QString ItemArmor::generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality, QMUD::ArmorType armorType)
{
    if (armorType == QMUD::ArmorType::UNKNOWN)
        armorType = static_cast<QMUD::ArmorType>(QMUD::rollDice(1, static_cast<int>(QMUD::ArmorType::MAX_VALUE) - 1));

    QString randomCode = "ARMOR-" + QString::number(level) + "-"
            + QMUD::itemClassToString(itemClass) + "-"
            + QMUD::itemQualityToString(quality) + "-"
            + QString::number(QMUD::rollDice(1, 5)) + "-"
            + QMUD::armorTypeToString(armorType);

    return randomCode;
}

bool ItemArmor::fromRandomCodeToValues(QString randomCode, int &level, QString &name, QMUD::ItemClassType &classType, QMUD::ItemQualityType &quality)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return false;

    level = elements[1].toInt();
    classType = QMUD::itemClassFromString(elements[2]);
    quality = QMUD::itemQualityFromString(elements[3]);

    name = getNameByRandomCode(randomCode);

    return true;
}

bool ItemArmor::isValidRandomCode(QString randomCode)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return false;

    return elements[0] == "ARMOR";
}

void ItemArmor::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    if (m_eArmorType == QMUD::ArmorType::CHEST)
        obj["armorType"] = "chest";
    else if (m_eArmorType == QMUD::ArmorType::SHIELD)
        obj["armorType"] = "shield";
    else if (m_eArmorType == QMUD::ArmorType::GLOVES)
        obj["armorType"] = "gloves";
    else if (m_eArmorType == QMUD::ArmorType::SHOULDER)
        obj["armorType"] = "shoulder";
    else if (m_eArmorType == QMUD::ArmorType::BRACERS)
        obj["armorType"] = "bracers";
    else if (m_eArmorType == QMUD::ArmorType::HELM)
        obj["armorType"] = "helm";
    else if (m_eArmorType == QMUD::ArmorType::LEG)
        obj["armorType"] = "leg";
    else if (m_eArmorType == QMUD::ArmorType::BOOTS)
        obj["armorType"] = "boots";
    else
    {
        Q_ASSERT(false);

        obj["armorType"] = "chest";
    }

    obj["classType"] = QMUD::itemClassToString(m_eClassType);

    QJsonObject objBaseBonus;
    m_itemBaseBonus.write(objBaseBonus);
    obj["baseBonus"] = objBaseBonus;

    QJsonObject objBaseStatus;
    m_itemBaseTemporaryStatus.write(objBaseStatus);
    obj["baseStatus"] = objBaseStatus;
}

QMUD::ArmorType ItemArmor::armorType() const
{
    return m_eArmorType;
}

int ItemArmor::armorClassBonus() const
{
    return m_iArmorClassBonus;
}

QMUD::ItemClassType ItemArmor::classType() const
{
    return m_eClassType;
}

const QMUD::ItemBaseBonus &ItemArmor::baseBonus() const
{
    return m_itemBaseBonus;
}

const QMUD::ItemBaseStatus &ItemArmor::baseTemporaryStatus() const
{
    return m_itemBaseTemporaryStatus;
}

int ItemArmor::getArmorClassBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType)
{
    // Bonus per qualita'
    double qualityMultiplier = 0.6;
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:       qualityMultiplier = 0.6;  break;
        case QMUD::ItemQualityType::COMMON:     qualityMultiplier = 0.7;  break;
        case QMUD::ItemQualityType::UNCOMMON:   qualityMultiplier = 0.85; break;
        case QMUD::ItemQualityType::VALUABLE:   qualityMultiplier = 1.0;  break;
        case QMUD::ItemQualityType::RARE:       qualityMultiplier = 1.15; break;
        case QMUD::ItemQualityType::EPIC:       qualityMultiplier = 1.3;  break;
        case QMUD::ItemQualityType::LEGENDARY:  qualityMultiplier = 1.5;  break;
    }

    // Bonus classe
    double classMultiplier = 0.4;
    switch (classType)
    {
        case QMUD::ItemClassType::CASTER_WIS:   classMultiplier = 0.5; break;
        case QMUD::ItemClassType::CASTER_INT:   classMultiplier = 0.4; break;
        case QMUD::ItemClassType::DPS_DEX:      classMultiplier = 0.7; break;
        case QMUD::ItemClassType::DPS_STR:      classMultiplier = 0.8; break;
        case QMUD::ItemClassType::TANK:         classMultiplier = 1.0; break;
    default:
        break;
    }

    return qMax(1, qRound(level * qualityMultiplier * classMultiplier));
}

void ItemArmor::updateBaseBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType, QMUD::ItemBaseBonus &baseBonus)
{
    double statBonusMultiplier = 0.0;
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:       statBonusMultiplier = 0.0;  break;
        case QMUD::ItemQualityType::COMMON:     statBonusMultiplier = 0.0;  break;
        case QMUD::ItemQualityType::UNCOMMON:   statBonusMultiplier = 0.3;  break;
        case QMUD::ItemQualityType::VALUABLE:   statBonusMultiplier = 0.5;  break;
        case QMUD::ItemQualityType::RARE:       statBonusMultiplier = 0.7;  break;
        case QMUD::ItemQualityType::EPIC:       statBonusMultiplier = 0.9;  break;
        case QMUD::ItemQualityType::LEGENDARY:  statBonusMultiplier = 1.1;  break;
    }

    int statBonusInt = 0;
    int statBonusWis = 0;
    int statBonusStr = 0;
    int statBonusCon = 0;
    int statBonusDex = 0;

    switch (classType)
    {
        case QMUD::ItemClassType::CASTER_INT:
            statBonusInt = basePrimaryStatBonus(level);
            statBonusWis = baseSecondaryStatBonus(level);
            statBonusCon = baseTertiaryStatBonus(level);
        break;

        case QMUD::ItemClassType::CASTER_WIS:
            statBonusWis = basePrimaryStatBonus(level);
            statBonusInt = baseSecondaryStatBonus(level);
            statBonusCon = baseTertiaryStatBonus(level);
        break;

        case QMUD::ItemClassType::DPS_DEX:
            statBonusDex = basePrimaryStatBonus(level);
            statBonusStr = baseSecondaryStatBonus(level);
            statBonusCon = baseTertiaryStatBonus(level);
        break;

        case QMUD::ItemClassType::DPS_STR:
            statBonusStr = basePrimaryStatBonus(level);
            statBonusCon = baseSecondaryStatBonus(level);
            statBonusDex = baseTertiaryStatBonus(level);
        break;

        case QMUD::ItemClassType::TANK:
            statBonusCon = basePrimaryStatBonus(level);
            statBonusStr = baseSecondaryStatBonus(level);
            statBonusDex = baseTertiaryStatBonus(level);
        break;

    default:
        break;
    }

    baseBonus.setIntelligence(baseBonus.intelligence() + qRound(statBonusInt * statBonusMultiplier));
    baseBonus.setWisdom(baseBonus.wisdom() + qRound(statBonusWis * statBonusMultiplier));
    baseBonus.setStrength(baseBonus.strength() + qRound(statBonusStr * statBonusMultiplier));
    baseBonus.setConstitution(baseBonus.constitution() + qRound(statBonusCon * statBonusMultiplier));
    baseBonus.setDexterity(baseBonus.dexterity() + qRound(statBonusDex * statBonusMultiplier));
}
