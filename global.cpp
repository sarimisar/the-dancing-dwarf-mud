/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "global.h"
#include "game/world.h"
#include "game/map.h"
#include "globaldebug.h"

#include <QtMath>

::CORE::World* QMUD::qmudWorld = Q_NULLPTR;

QString QMUD::eventTypeToString(EventType event)
{
    if (event == EventType::ENTER)
        return "ENTER";
    else if (event == EventType::CUSTOM)
        return "CUSTOM";
    else if (event == EventType::ABILITY)
        return "ABILITY";

    return "";
}

QMUD::Recipe::Recipe() :
    m_iId(StaticIdInvalid),
    m_iItemId(StaticIdInvalid),
    m_iMapId(StaticIdInvalid),
    m_eResource(ResourceType::UNKNOWN),
    m_eProfession(ProfessionType::UNKNOWN),
    m_iLevel(0),
    m_iPrice(0)
{

}

bool QMUD::Recipe::isValid() const
{
    return m_iId != StaticIdInvalid;
}

QMUD::StaticIdType QMUD::Recipe::id() const
{
    return m_iId;
}

QMap<QMUD::ResourceType, qint64> QMUD::Recipe::resources() const
{
    return m_mapResources;
}

qint64 QMUD::Recipe::price() const
{
    return m_iPrice;
}

bool QMUD::Recipe::read(const QJsonObject &obj, StaticIdType mapId)
{
    m_iId = StaticIdInvalid;

    if (!QMUD::fileJsonReadStaticId(obj, "itemId", m_iItemId, true))
        return false;

    if (!QMUD::fileJsonRead(obj, "name", m_strName))
        return false;

    if (!QMUD::fileJsonRead(obj, "level", m_iLevel))
        return false;

    QStringList descriptionList;
    if (!QMUD::fileJsonRead(obj, "description", descriptionList, true, false))
        return false;

    m_strDescription = descriptionList.join(QMUD_GLOBAL_NEW_LINE);

    QStringList actionList;
    if (!QMUD::fileJsonRead(obj, "action", actionList, true, false))
        return false;

    m_strAction = actionList.join(QMUD_GLOBAL_NEW_LINE);

    // Risorsa
    QString strTmp;
    if (!QMUD::fileJsonRead(obj, "resource", strTmp, false, true))
        return false;

    if (!strTmp.isEmpty())
        m_eResource = resourceFromString(strTmp);

    if (m_eResource == ResourceType::UNKNOWN && m_iItemId == StaticIdInvalid)
        return false;

    if (m_eResource != ResourceType::UNKNOWN && m_iItemId != StaticIdInvalid)
        return false;

    // Professione
    if (!QMUD::fileJsonRead(obj, "profession", strTmp))
        return false;

    m_eProfession = professionFromString(strTmp);

    if (m_eProfession == ProfessionType::UNKNOWN)
        return false;

    // Resources
    m_mapResources.clear();
    auto itResources = obj.find("resources");

    if (itResources != obj.end())
    {
        if (!itResources.value().isObject())
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave 'resources' non e' un oggetto"));
            return false;
        }

        auto objResources = itResources->toObject();

        for (int i = static_cast<int>(QMUD::ResourceType::UNKNOWN) + 1;
             i < static_cast<int>(QMUD::ResourceType::MAX_VALUE); ++i)
        {
            auto itResource = objResources.find(QMUD::resourceToString(static_cast<QMUD::ResourceType>(i)));

            if (itResource != objResources.end() && itResource.value().isDouble())
                m_mapResources[static_cast<QMUD::ResourceType>(i)] = qRound64(itResource.value().toDouble());
        }
    }

    m_iMapId = mapId;

    if (!QMUD::fileJsonReadStaticId(obj, "id", m_iId))
    {
        m_iId = StaticIdInvalid;
        return false;
    }

    return true;
}

QString QMUD::Recipe::name() const
{
    return m_strName;
}

QString QMUD::Recipe::action() const
{
    return m_strAction;
}

QMUD::ProfessionType QMUD::Recipe::profession() const
{
    return m_eProfession;
}

int QMUD::Recipe::level() const
{
    return m_iLevel;
}

QMUD::StaticIdType QMUD::Recipe::mapId() const
{
    return m_iMapId;
}

QMUD::StaticIdType QMUD::Recipe::itemId() const
{
    return m_iItemId;
}

QMUD::ResourceType QMUD::Recipe::resource() const
{
    return m_eResource;
}

int QMUD::rollDice(int numdice, int die)
{
    if (numdice == 0 || die == 0)
        return 0;

    int total = 0;

    for (int i = 0; i < numdice; i++)
        total += std::rand() % die + 1;

    return total;
}

qint64 QMUD::rollDice(int numdice, qint64 die)
{
    if (numdice == 0 || die == 0)
        return 0;

    qint64 total = 0;

    for (int i = 0; i < numdice; i++)
        total += std::rand() % die + 1;

    return total;
}

QString QMUD::coinsToReadableString(qint64 coins)
{
    return QObject::tr("%1 aurei").arg(coins);
}

qint64 QMUD::experienceByLevelBase(int level)
{
    return (qRound64((qPow(level, 4)/1000.0 + 100.0) * level * 3.0));
}

QString QMUD::capitalize(QString str)
{
    if (str.isEmpty())
        return str;
    else
    {
        for (int i=0; i<str.size(); )
        {
            if (str[i] != '&')
            {
                str[i] = str[i].toUpper();
                return str;
            }

            i = i + 2;
        }

        return str;
    }
}

QString QMUD::minutesToString(int minutes)
{
    if (minutes == 0)
        return "meno di un minuto";
    else if (minutes == 1)
        return "un minuto";
    else
        return QString("%1 minuti").arg(minutes);
}


bool QMUD::WildDarkKernel[QMUD_WILD_DESCRIPTION_HEIGHT][QMUD_WILD_DESCRIPTION_WIDTH] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0}
};

bool QMUD::WildAlmostDarkKernel[QMUD_WILD_DESCRIPTION_HEIGHT][QMUD_WILD_DESCRIPTION_WIDTH] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0}
};

bool QMUD::fileJsonRead(const QJsonObject &obj, QString key, QString& value, bool canBeEmpty, bool optional, QString defaultValue)
{
    auto it = obj.find(key);

    if (it == obj.end())
    {
        if (optional)
        {
            value = defaultValue;
            return true;
        }
        else
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non presente").arg(key));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return false;
        }
    }

    QString v = it.value().toString();

    if (canBeEmpty || !v.isEmpty())
    {
        value = v;
        return true;
    }
    else
    {
        GLOBAL_CRITICAL(QObject::tr("Chiave '%1' vuota").arg(key));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return false;
    }
}

bool QMUD::fileJsonRead(const QJsonObject &obj, QString key, int &value, bool optional, int defaultValue)
{
    auto it = obj.find(key);

    if (it == obj.end())
    {
        if (optional)
        {
            value = defaultValue;
            return true;
        }
        else
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non presente").arg(key));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return false;
        }
    }

    value = it.value().toInt();
    return true;
}

bool QMUD::fileJsonRead(const QJsonObject &obj, QString key, double& value, bool optional, double defaultValue)
{
    auto it = obj.find(key);

    if (it == obj.end())
    {
        if (optional)
        {
            value = defaultValue;
            return true;
        }
        else
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non presente").arg(key));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return false;
        }
    }

    value = it.value().toDouble();
    return true;
}

bool QMUD::fileJsonReadStaticId(const QJsonObject &obj, QString key, StaticIdType& value, bool optional, StaticIdType defaultValue)
{
    auto it = obj.find(key);

    if (it == obj.end())
    {
        if (optional)
        {
            value = defaultValue;
            return true;
        }
        else
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non presente").arg(key));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return false;
        }
    }

    if (!it.value().isDouble())
    {
        GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non valida").arg(key));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return false;
    }

    value = static_cast<StaticIdType>(qRound64(it.value().toDouble()));
    return true;
}

bool QMUD::fileJsonRead(const QJsonObject &obj, QString key, QStringList& value, bool simplified, bool lower, bool strCanBeEmpty, bool optional)
{
    auto it = obj.find(key);

    if (it != obj.end())
    {
        if (!it.value().isArray())
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave '%1' non e' un array").arg(key));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return false;
        }

        QJsonArray jsonArray = it.value().toArray();
        value.clear();
        value.reserve(jsonArray.size());

        for (auto e : jsonArray)
        {
            if (!e.isString())
            {
                GLOBAL_CRITICAL(QObject::tr("Chiave '%1' non e' un array di stringhe").arg(key));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                value.clear();
                return false;
            }

            QString str = e.toString();

            if (simplified)
                str = str.simplified();

            if (lower)
                str = str.toLower();

            if (strCanBeEmpty && str.isEmpty())
            {
                GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non e' un array di stringhe non vuote").arg(key));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                value.clear();
                return false;
            }

            value.push_back(str);
        }
    }
    else if (optional)
    {
        value.clear();
    }

    return true;
}

bool QMUD::fileJsonRead(const QJsonObject &obj, QString key, bool &value, bool optional, bool defaultValue)
{
    auto it = obj.find(key);

    if (it == obj.end())
    {
        if (optional)
        {
            value = defaultValue;
            return true;
        }
        else
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria '%1' non presente").arg(key));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return false;
        }
    }

    value = it.value().toBool();
    return true;
}
