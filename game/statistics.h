#ifndef STATISTICS_H
#define STATISTICS_H

#include "../global.h"

#include <QString>
#include <QVector>
#include <QMap>

namespace CORE
{

class GlobalStatistics
{
public:
    GlobalStatistics();

    bool load(QString rootDir);
    bool save(QString rootDir) const;

    GlobalStatistics& operator+=(const GlobalStatistics& other);

    quint64 numberOfKilledPC() const;
    void addKilledPC(quint64 count);
    quint64 numberOfKilledNPC() const;
    void addKilledNPC(quint64 count);
    quint64 numberOfConnections() const;
    void addConnection(quint64 count);
    QMap<QMUD::ResourceType, quint64> resources() const;
    void addResource(QMUD::ResourceType type, quint64 count);
    QMap<QMUD::ResourceType, quint64> spentResources() const;
    void addSpentResource(QMUD::ResourceType type, quint64 count);

private:
    quint64 m_uiNumberOfKilledPC;
    quint64 m_uiNumberOfKilledNPC;
    quint64 m_uiNumberOfConnections;

    QMap<QMUD::ResourceType, quint64> m_mapResources;
    QMap<QMUD::ResourceType, quint64> m_mapSpentResources;
};

}

#endif // STATISTICS_H
