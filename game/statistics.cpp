#include "statistics.h"
#include "../globaldebug.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>


using namespace CORE;

GlobalStatistics::GlobalStatistics() :
    m_uiNumberOfKilledPC(0),
    m_uiNumberOfKilledNPC(0),
    m_uiNumberOfConnections(0)
{

}

bool GlobalStatistics::load(QString rootDir)
{
    QString file = QFileInfo(rootDir + "/statistics.json").filePath();

    if (QFileInfo(file).exists())
    {
        // Carico le statistiche
        QFile fpStatistics(file);
        if (!fpStatistics.open(QFile::ReadOnly))
        {
            GLOBAL_CRITICAL(QObject::tr("Impossibile aprire il file delle statistiche 'statistics.json'"));
            return false;
        }

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(fpStatistics.readAll(), &error);
        fpStatistics.close();

        if (error.error != QJsonParseError::NoError)
        {
            GLOBAL_CRITICAL(QObject::tr("Il file delle statistiche 'statistics.json' sembra corrotto"));
            return false;
        }

        QJsonObject obj = doc.object();

        m_uiNumberOfKilledPC = obj["numberOfKilledPC"].toString().toULongLong();
        m_uiNumberOfKilledNPC = obj["numberOfKilledNPC"].toString().toULongLong();
        m_uiNumberOfConnections = obj["numberOfConnections"].toString().toULongLong();

        auto resources = obj["resources"].toObject();

        for (auto it = resources.begin(); it != resources.end(); ++it)
            m_mapResources[QMUD::resourceFromString(it.key())] = it.value().toString().toULongLong();

        auto spentResources = obj["spentResources"].toObject();

        for (auto it = spentResources.begin(); it != spentResources.end(); ++it)
            m_mapSpentResources[QMUD::resourceFromString(it.key())] = it.value().toString().toULongLong();
    }

    return true;
}

bool GlobalStatistics::save(QString rootDir) const
{
    QJsonObject obj;

    obj["numberOfKilledPC"] = QString::number(m_uiNumberOfKilledPC);
    obj["numberOfKilledNPC"] = QString::number(m_uiNumberOfKilledNPC);
    obj["numberOfConnections"] = QString::number(m_uiNumberOfConnections);

    QJsonObject resources;

    for (auto it = m_mapResources.begin(); it != m_mapResources.end(); ++it)
        resources[QMUD::resourceToString(it.key())] = QString::number(it.value());

    obj["resources"] = resources;

    QJsonObject spentResources;

    for (auto it = m_mapSpentResources.begin(); it != m_mapSpentResources.end(); ++it)
        spentResources[QMUD::resourceToString(it.key())] = QString::number(it.value());

    obj["spentResources"] = spentResources;

    QString file = QFileInfo(rootDir + "/statistics.json").filePath();
    QJsonDocument doc(obj);
    QFile fp(file);

    if (!fp.open(QFile::WriteOnly))
    {
        GLOBAL_CRITICAL(QObject::tr("Impossibile aprile il file delle statistiche '%1'").arg(file));
        return false;
    }

    fp.write(doc.toJson(QJsonDocument::Indented));
    fp.close();

    return true;
}

GlobalStatistics& GlobalStatistics::operator+=(const GlobalStatistics &other)
{
    m_uiNumberOfKilledNPC += other.m_uiNumberOfKilledNPC;
    m_uiNumberOfConnections += other.m_uiNumberOfConnections;

    for (auto it = other.m_mapResources.begin(); it != other.m_mapResources.end(); ++it)
        m_mapResources[it.key()] += it.value();

    for (auto it = other.m_mapSpentResources.begin(); it != other.m_mapSpentResources.end(); ++it)
        m_mapSpentResources[it.key()] += it.value();

    return *this;
}

quint64 GlobalStatistics::numberOfKilledPC() const
{
    return m_uiNumberOfKilledPC;
}

void GlobalStatistics::addKilledPC(quint64 count)
{
    m_uiNumberOfKilledPC += count;
}

quint64 GlobalStatistics::numberOfKilledNPC() const
{
    return m_uiNumberOfKilledNPC;
}

void GlobalStatistics::addKilledNPC(quint64 count)
{
    m_uiNumberOfKilledNPC += count;
}

quint64 GlobalStatistics::numberOfConnections() const
{
    return m_uiNumberOfConnections;
}

void GlobalStatistics::addConnection(quint64 count)
{
    m_uiNumberOfConnections += count;
}

QMap<QMUD::ResourceType, quint64> GlobalStatistics::resources() const
{
    return m_mapResources;
}

void GlobalStatistics::addResource(QMUD::ResourceType type, quint64 count)
{
    m_mapResources[type] += count;
}

QMap<QMUD::ResourceType, quint64> GlobalStatistics::spentResources() const
{
    return m_mapSpentResources;
}

void GlobalStatistics::addSpentResource(QMUD::ResourceType type, quint64 count)
{
    m_mapSpentResources[type] += count;
}
