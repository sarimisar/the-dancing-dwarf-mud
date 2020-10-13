#include "quest.h"
#include "world.h"
#include "../global.h"
#include "../globaldebug.h"

#include <QObject>

using namespace CORE;

Quest::QuestIdType Quest::QuestIdInvalid = QuestIdType(QMUD::StaticIdInvalid, QMUD::StaticIdInvalid);

Quest::TargetData::TargetData() :
    m_bNpcToTalkToEnabled(false),
    m_iNpcToTalkToId(0),
    m_iNpcToTalkToMapId(0),
    m_bNpcToKillEnabled(false),
    m_iNpcToKillId(0),
    m_iNpcToKillMapId(0),
    m_iNpcToKillCount(1),
    m_bNpcToKillGiveItemEnabled(false),
    m_iNpcToKillGiveItemProbability(1)
{ }

int Quest::TargetData::npcToKillGiveItemProbability() const
{
    return m_iNpcToKillGiveItemProbability;
}

void Quest::TargetData::setNpcToKillGiveItemProbability(int iNpcToKillGiveItemProbability)
{
    m_iNpcToKillGiveItemProbability = iNpcToKillGiveItemProbability;
}

bool Quest::TargetData::read(const QJsonObject &obj)
{
    if (!QMUD::fileJsonRead(obj, "description", m_strDescription))
        return false;

    m_strDescription = m_strDescription.simplified();

    if (!QMUD::fileJsonRead(obj, "completeDescription", m_strCompleteDescription))
        return false;

    if (!QMUD::fileJsonRead(obj, "npcToTalkToEnabled", m_bNpcToTalkToEnabled))
        return false;

    if (m_bNpcToTalkToEnabled)
    {
        if (!QMUD::fileJsonReadStaticId(obj, "npcToTalkToId", m_iNpcToTalkToId))
            return false;

        if (!QMUD::fileJsonReadStaticId(obj, "npcToTalkToMapId", m_iNpcToTalkToMapId))
            return false;
    }

    if (!QMUD::fileJsonRead(obj, "npcToKillEnabled", m_bNpcToKillEnabled))
        return false;

    if (m_bNpcToKillEnabled)
    {
        if (!QMUD::fileJsonReadStaticId(obj, "npcToKillId", m_iNpcToKillId))
            return false;

        if (!QMUD::fileJsonReadStaticId(obj, "npcToKillMapId", m_iNpcToKillMapId))
            return false;

        if (!QMUD::fileJsonRead(obj, "npcToKillCount", m_iNpcToKillCount))
            return false;

        if (!QMUD::fileJsonRead(obj, "npcToKillGiveItemEnabled", m_bNpcToKillGiveItemEnabled))
            return false;

        if (m_bNpcToKillGiveItemEnabled)
        {
            if (!QMUD::fileJsonRead(obj, "npcToKillGiveItemName", m_strNpcToKillGiveItemName))
                return false;

            if (!QMUD::fileJsonRead(obj, "npcToKillGiveItemProbability", m_iNpcToKillGiveItemProbability))
                return false;
        }
    }

    return true;
}

QString Quest::TargetData::npcToKillGiveItemName() const
{
    return m_strNpcToKillGiveItemName;
}

void Quest::TargetData::setNpcToKillGiveItemName(const QString &strNpcToKillGiveItemName)
{
    m_strNpcToKillGiveItemName = strNpcToKillGiveItemName;
}

bool Quest::TargetData::npcToKillGiveItemEnabled() const
{
    return m_bNpcToKillGiveItemEnabled;
}

void Quest::TargetData::setNpcToKillGiveItemEnabled(bool bNpcToKillGiveItemEnabled)
{
    m_bNpcToKillGiveItemEnabled = bNpcToKillGiveItemEnabled;
}

int Quest::TargetData::npcToKillCount() const
{
    return m_iNpcToKillCount;
}

void Quest::TargetData::setNpcToKillCount(int iNpcToKillCount)
{
    m_iNpcToKillCount = iNpcToKillCount;
}

QMUD::StaticIdType Quest::TargetData::npcToKillMapId() const
{
    return m_iNpcToKillMapId;
}

void Quest::TargetData::setNpcToKillMapId(const QMUD::StaticIdType &iNpcToKillMapId)
{
    m_iNpcToKillMapId = iNpcToKillMapId;
}

QMUD::StaticIdType Quest::TargetData::npcToKillId() const
{
    return m_iNpcToKillId;
}

void Quest::TargetData::setNpcToKillId(const QMUD::StaticIdType &iNpcToKillId)
{
    m_iNpcToKillId = iNpcToKillId;
}

bool Quest::TargetData::npcToKillEnabled() const
{
    return m_bNpcToKillEnabled;
}

void Quest::TargetData::setNpcToKillEnabled(bool bNpcToKillEnabled)
{
    m_bNpcToKillEnabled = bNpcToKillEnabled;
}

QMUD::StaticIdType Quest::TargetData::npcToTalkToMapId() const
{
    return m_iNpcToTalkToMapId;
}

void Quest::TargetData::setNpcToTalkToMapId(const QMUD::StaticIdType &iNpcToTalkToMapId)
{
    m_iNpcToTalkToMapId = iNpcToTalkToMapId;
}

QMUD::StaticIdType Quest::TargetData::npcToTalkToId() const
{
    return m_iNpcToTalkToId;
}

void Quest::TargetData::setNpcToTalkToId(const QMUD::StaticIdType &iNpcToTalkToId)
{
    m_iNpcToTalkToId = iNpcToTalkToId;
}

bool Quest::TargetData::npcToTalkToEnabled() const
{
    return m_bNpcToTalkToEnabled;
}

void Quest::TargetData::setNpcToTalkToEnabled(bool bNpcToTalkToEnabled)
{
    m_bNpcToTalkToEnabled = bNpcToTalkToEnabled;
}

QString Quest::TargetData::completeDescription() const
{
    return m_strCompleteDescription;
}

void Quest::TargetData::setCompleteDescription(const QString &strCompleteDescription)
{
    m_strCompleteDescription = strCompleteDescription;
}

QString Quest::TargetData::description() const
{
    return m_strDescription;
}

void Quest::TargetData::setDescription(const QString &strDescription)
{
    m_strDescription = strDescription;
}

Quest::Quest() :
    m_iId(QuestIdInvalid),
    m_iGiveNpcId(0),
    m_iGiveNpcMapId(0),
    m_iCompleteNpcId(0),
    m_iCompleteNpcMapId(0),
    m_iLevel(0),
    m_eDifficulty(QMUD::NpcType::NORMAL),
    m_iGold(0),
    m_bGiftItemEnabled(false),
    m_bSpecificGiftItemEnabled(false),
    m_iSpecificGiftItemId(QMUD::StaticIdInvalid)
{

}

bool Quest::checkIsValid()
{
    auto map = QMUD::qmudWorld->map(m_iGiveNpcMapId);
    if (!map)
    {
        GLOBAL_WARNING(QObject::tr("Quest %1: giveNpcMapId non valido (mappa %2)")
                       .arg(name())
                       .arg(m_iGiveNpcMapId));
        return false;
    }

    map = QMUD::qmudWorld->map(m_iCompleteNpcMapId);
    if (!map)
    {
        GLOBAL_WARNING(QObject::tr("Quest %1: completeNpcMapId non valido (mappa %2)")
                       .arg(name())
                       .arg(m_iCompleteNpcMapId));
        return false;
    }

    if (m_bGiftItemEnabled && m_bSpecificGiftItemEnabled)
    {
        map = QMUD::qmudWorld->map(m_iSpecificGiftItemMapId);
        if (!map)
        {
            GLOBAL_WARNING(QObject::tr("Quest %1: specificGiftItemMapId non valido (mappa %2)")
                           .arg(name())
                           .arg(m_iSpecificGiftItemMapId));
            return false;
        }
    }

    for (auto target : m_vTargetData)
    {
        map = QMUD::qmudWorld->map(target.m_iNpcToTalkToMapId);
        if (!map)
        {
            GLOBAL_WARNING(QObject::tr("Quest %1: npcToTalkToMapId non valido (mappa %2, target %3)")
                           .arg(name())
                           .arg(target.m_iNpcToTalkToMapId)
                           .arg(target.description()));
            return false;
        }

        map = QMUD::qmudWorld->map(target.m_iNpcToKillMapId);
        if (!map)
        {
            GLOBAL_WARNING(QObject::tr("Quest %1: npcToKillMapId non valido (mappa %2, target %3)")
                           .arg(name())
                           .arg(target.m_iNpcToKillMapId)
                           .arg(target.description()));
            return false;
        }
    }

    return true;
}

bool Quest::read(const QJsonObject &obj, QMUD::StaticIdType mapId)
{
    QMUD::StaticIdType id;
    if (!QMUD::fileJsonReadStaticId(obj, "id", id))
        return false;

    m_iId = QuestIdType(mapId, id);

    if (!QMUD::fileJsonReadStaticId(obj, "giveNpcId", m_iGiveNpcId))
        return false;

    if (!QMUD::fileJsonReadStaticId(obj, "giveNpcMapId", m_iGiveNpcMapId))
        return false;

    if (!QMUD::fileJsonReadStaticId(obj, "completeNpcId", m_iCompleteNpcId))
        return false;

    if (!QMUD::fileJsonReadStaticId(obj, "completeNpcMapId", m_iCompleteNpcMapId))
        return false;

    if (!QMUD::fileJsonRead(obj, "level", m_iLevel))
        return false;

    if (!QMUD::fileJsonRead(obj, "name", m_strName))
        return false;

    if (!QMUD::fileJsonRead(obj, "giveDescription", m_strGiveDescription))
        return false;

    if (!QMUD::fileJsonRead(obj, "completeDescription", m_strCompleteDescription))
        return false;

    QString tmpDifficulty;
    if (!QMUD::fileJsonRead(obj, "difficulty", tmpDifficulty))
        return false;

    m_eDifficulty = QMUD::npcTypeFromString(tmpDifficulty);

    if (m_eDifficulty == QMUD::NpcType::UNKNOWN)
    {
        GLOBAL_CRITICAL(QObject::tr("Chiave obbligatoria 'difficulty' non valida").arg(tmpDifficulty));
        return false;
    }

    if (!QMUD::fileJsonRead(obj, "gold", m_iGold))
        return false;

    if (!QMUD::fileJsonRead(obj, "giftItemEnabled", m_bGiftItemEnabled, true, false))
        return false;

    if (m_bGiftItemEnabled)
    {
        if (!QMUD::fileJsonRead(obj, "specificGiftItemEnabled", m_bSpecificGiftItemEnabled, true, false))
            return false;

        if (m_bSpecificGiftItemEnabled)
        {
            if (!QMUD::fileJsonReadStaticId(obj, "specificGiftItemId", m_iSpecificGiftItemId))
                return false;

            if (!QMUD::fileJsonReadStaticId(obj, "specificGiftItemMapId", m_iSpecificGiftItemMapId))
                return false;
        }

    }

    auto itTargets = obj.find("targets");

    if (itTargets != obj.end())
    {
        if (!itTargets.value().isArray())
        {
            GLOBAL_CRITICAL(QObject::tr("Chiave 'targets' non valida, deve essere un array"));
            return false;
        }

        QJsonArray arrayTargets = itTargets.value().toArray();

        for (auto elem : arrayTargets)
        {
            if (!elem.isObject())
            {
                GLOBAL_CRITICAL(QObject::tr("Chiave 'targets' non valida, deve essere un array di oggetti"));
                return false;
            }

            QJsonObject objTarget = elem.toObject();

            TargetData targetData;

            if (!targetData.read(objTarget))
                return false;

            m_vTargetData.push_back(targetData);
        }
    }

    return true;
}

Quest::QuestIdType Quest::id() const
{
    return m_iId;
}

QJsonValue Quest::idToJsonValue() const
{
    QJsonArray array;
    array.push_back(static_cast<qint64>(m_iId.first));
    array.push_back(static_cast<qint64>(m_iId.second));
    return array;
}

QString Quest::idToString() const
{
    return idToString(m_iId);
}

QString Quest::idToString(const Quest::QuestIdType &id)
{
    return QString("%1.%2").arg(id.first).arg(id.second);
}

Quest::QuestIdType Quest::idFromString(QString id)
{
    auto list = id.split(".");

    if (list.size() != 2)
        return QuestIdInvalid;

    QMUD::StaticIdType mapId = list[0].toUInt();
    QMUD::StaticIdType questId = list[1].toUInt();

    return QuestIdType(mapId, questId);
}

Quest::QuestIdType Quest::idFromJsonValue(const QJsonValue& value, bool& ok)
{
    if (value.isArray())
    {
        auto array = value.toArray();

        if (array.size() == 2 && array[0].isDouble() && array[1].isDouble())
        {
            ok = true;
            return QuestIdType(static_cast<QMUD::StaticIdType>(qRound64(value[0].toDouble())),
                    static_cast<QMUD::StaticIdType>(qRound64(value[1].toDouble())));
        }
    }

    ok = false;
    return QuestIdType(0, 0);
}

void Quest::setId(const QuestIdType &iId)
{
    m_iId = iId;
}

QMUD::StaticIdType Quest::mapId() const
{
    return m_iId.first;
}

QMUD::StaticIdType Quest::questId() const
{
    return m_iId.second;
}

QMUD::StaticIdType Quest::giveNpcId() const
{
    return m_iGiveNpcId;
}

void Quest::setGiveNpcId(QMUD::StaticIdType iGiveNpcId)
{
    m_iGiveNpcId = iGiveNpcId;
}

QMUD::StaticIdType Quest::giveNpcMapId() const
{
    return m_iGiveNpcMapId;
}

void Quest::setGiveNpcMapId(QMUD::StaticIdType iGiveNpcMapId)
{
    m_iGiveNpcMapId = iGiveNpcMapId;
}

QMUD::StaticIdType Quest::completeNpcId() const
{
    return m_iCompleteNpcId;
}

void Quest::setCompleteNpcId(QMUD::StaticIdType iCompleteNpcId)
{
    m_iCompleteNpcId = iCompleteNpcId;
}

QMUD::StaticIdType Quest::completeNpcMapId() const
{
    return m_iCompleteNpcMapId;
}

void Quest::setCompleteNpcMapId(QMUD::StaticIdType iCompleteNpcMapId)
{
    m_iCompleteNpcMapId = iCompleteNpcMapId;
}

int Quest::level() const
{
    return m_iLevel;
}

void Quest::setLevel(int iLevel)
{
    m_iLevel = iLevel;
}

QString Quest::name() const
{
    return m_strName;
}

void Quest::setName(const QString &strName)
{
    m_strName = strName;
}

QString Quest::giveDescription() const
{
    return m_strGiveDescription;
}

void Quest::setGiveDescription(const QString &strGiveDescription)
{
    m_strGiveDescription = strGiveDescription;
}

QString Quest::completeDescription() const
{
    return m_strCompleteDescription;
}

void Quest::setCompleteDescription(const QString &strCompleteDescription)
{
    m_strCompleteDescription = strCompleteDescription;
}

QVector<Quest::TargetData> Quest::targetData() const
{
    return m_vTargetData;
}

void Quest::setTargetData(const QVector<TargetData> &vTargetData)
{
    m_vTargetData = vTargetData;
}

QMUD::NpcType Quest::difficulty() const
{
    return m_eDifficulty;
}

void Quest::setDifficulty(const QMUD::NpcType &eDifficulty)
{
    m_eDifficulty = eDifficulty;
}

int Quest::gold() const
{
    return m_iGold;
}

void Quest::setGold(int iGold)
{
    m_iGold = iGold;
}

bool Quest::giftItemEnabled() const
{
    return m_bGiftItemEnabled;
}

void Quest::setGiftItemEnabled(bool bGiftItemEnabled)
{
    m_bGiftItemEnabled = bGiftItemEnabled;
}

bool Quest::specificGiftItemEnabled() const
{
    return m_bSpecificGiftItemEnabled;
}

void Quest::setSpecificGiftItemEnabled(bool bSpecificGiftItemEnabled)
{
    m_bSpecificGiftItemEnabled = bSpecificGiftItemEnabled;
}

QMUD::StaticIdType Quest::specificGiftItemId() const
{
    return m_iSpecificGiftItemId;
}

void Quest::setSpecificGiftItemId(const QMUD::StaticIdType &iSpecificGiftItemId)
{
    m_iSpecificGiftItemId = iSpecificGiftItemId;
}

QMUD::StaticIdType Quest::specificGiftItemMapId() const
{
    return m_iSpecificGiftItemMapId;
}

void Quest::setSpecificGiftItemMapId(const QMUD::StaticIdType &iSpecificGiftItemMapId)
{
    m_iSpecificGiftItemMapId = iSpecificGiftItemMapId;
}
