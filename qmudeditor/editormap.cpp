/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "editormap.h"
#include "dialogeditcolors.h"
#include "formtilesmapeditor.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>


EditorMap::EditorMap(QObject *parent) :
    QObject(parent),
    m_id(0),
    m_name(tr("Nuova"))
{

}

bool EditorMap::read()
{
    if (m_filePath.isEmpty())
        return false;

    QFile fp(m_filePath);
    if (!fp.open(QFile::ReadOnly | QFile::ExistingOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(fp.readAll());

    fp.close();

    if (doc.isNull())
        return false;

    QJsonObject obj = doc.object();

    m_id = obj["id"].toInt();
    m_name = obj["name"].toString();

    auto itMap = obj.find("map");

    if (itMap != obj.end())
    {
        auto arrayMap = itMap.value().toArray();

        for (auto row : arrayMap)
        {
            QString rowStr = row.toString();
            QVector<QChar> chars;
            for (auto c : rowStr)
                chars.push_back(c);
            m_wildMap.push_back(chars);
        }
    }

    auto itCustomDesc = obj.find("custom_descriptions");

    if (itCustomDesc != obj.end())
    {
        auto arrayDesc = itCustomDesc.value().toArray();

        for (auto desc : arrayDesc)
        {
            QJsonObject descObj = desc.toObject();

            MapCharInfo charInfo;
            QString keyStr = descObj["key"].toString().simplified();
            QString valueStr = descObj["show"].toString().simplified();

            if (keyStr.isEmpty() || keyStr.size() > 1)
                continue;

            if (valueStr.isEmpty() || valueStr.size() <= 2)
                continue;

            if (valueStr[valueStr.size() - 1] == '&')
                valueStr.remove(valueStr.size() - 1, 1);

            charInfo.color = valueStr.mid(0, valueStr.size() - 1);
            charInfo.key = keyStr[0];
            charInfo.value = valueStr[valueStr.size() - 1];
            charInfo.name = descObj["name"].toString();
            charInfo.is_room = descObj["is_room"].toBool();

            if (descObj.find("flags") != descObj.end())
            {
                QStringList flagsStr = descObj["flags"].toVariant().toStringList();
                for (auto flag : flagsStr)
                    charInfo.flags.push_back(QMUD::roomFlagFromString(flag));
            }

            m_vWildMapChars.push_back(charInfo);
        }
    }

    auto itNpcs = obj.find("npcs");
    if (itNpcs != obj.end())
    {
        auto array = itNpcs->toArray();
        for (auto elem : array)
        {
            EditorNpc *npc = new EditorNpc(this);
            npc->read(elem.toObject());
            m_npcs.push_back(npc);
        }
    }

    auto itItems = obj.find("items");
    if (itItems != obj.end())
    {
        auto array = itItems->toArray();
        for (auto elem : array)
        {
            EditorItem *item = new EditorItem(this);
            item->read(elem.toObject());
            m_items.push_back(item);
        }
    }

    auto itResources = obj.find("resources");

    if (itResources != obj.end())
    {
        auto array = itResources->toArray();
        for (auto elem : array)
        {
            QJsonObject objResource = elem.toObject();
            ResourceInfo info;
            info.resource = QMUD::resourceFromString(objResource["resource"].toString());
            info.rarity = QMUD::resourceRarityFromString(objResource["rarity"].toString());
            m_vResources.push_back(info);
        }
    }

    auto itQuests = obj.find("quests");

    if (itQuests != obj.end())
    {
        auto array = itQuests->toArray();
        for (auto elem : array)
        {
            QJsonObject objQuest = elem.toObject();
            QuestData data;

            data.id = objQuest["id"].toInt();
            data.giveNpcId = objQuest["giveNpcId"].toInt();
            data.giveNpcMapId = objQuest["giveNpcMapId"].toInt();
            data.completeNpcId = objQuest["completeNpcId"].toInt();
            data.completeNpcMapId = objQuest["completeNpcMapId"].toInt();
            data.level = objQuest["level"].toInt();
            data.name = objQuest["name"].toString();
            data.difficulty = QMUD::npcTypeFromString(objQuest["difficulty"].toString());
            data.gold = objQuest["gold"].toInt();
            data.giftItemEnabled = objQuest["giftItemEnabled"].toBool();
            data.specificGiftItemEnabled = objQuest["specificGiftItemEnabled"].toBool();
            data.specificGiftItemId = objQuest["specificGiftItemId"].toInt();
            data.specificGiftItemMapId = objQuest["specificGiftItemMapId"].toBool();
            data.giveDescription = objQuest["giveDescription"].toString();
            data.completeDescription = objQuest["completeDescription"].toString();

            auto itTargets = objQuest.find("targets");

            if (itTargets != objQuest.end())
            {
                auto arrayTarget = itTargets->toArray();
                for (auto target : arrayTarget)
                {
                    QJsonObject objTarget = target.toObject();
                    QuestData::TargetData targetData;

                    targetData.description = objTarget["description"].toString();
                    targetData.completeDescription = objTarget["completeDescription"].toString();
                    targetData.npcToTalkToEnabled = objTarget["npcToTalkToEnabled"].toBool();

                    if (targetData.npcToTalkToEnabled)
                    {
                        targetData.npcToTalkToId = objTarget["npcToTalkToId"].toInt();
                        targetData.npcToTalkToMapId = objTarget["npcToTalkToMapId"].toInt();
                    }

                    targetData.npcToKillEnabled = objTarget["npcToKillEnabled"].toBool();

                    if (targetData.npcToKillEnabled)
                    {
                        targetData.npcToKillId = objTarget["npcToKillId"].toInt();
                        targetData.npcToKillMapId = objTarget["npcToKillMapId"].toInt();
                        targetData.npcToKillCount = objTarget["npcToKillCount"].toInt();

                        if (targetData.npcToKillEnabled)
                        {
                            targetData.npcToKillGiveItemEnabled = objTarget["npcToKillGiveItemEnabled"].toBool();
                            targetData.npcToKillGiveItemName = objTarget["npcToKillGiveItemName"].toString();
                            targetData.npcToKillGiveItemProbability = objTarget["npcToKillGiveItemProbability"].toInt();
                        }
                    }

                    data.targetData.push_back(targetData);
                }
            }

            m_vQuests.push_back(data);
        }
    }

    auto itMaps = obj.find("maps");

    if (itMaps != obj.end())
    {
        QVector<int> ids;
        QVector<QString> names;

        auto arrayMaps = itMaps.value().toArray();

        for (auto it = arrayMaps.begin(); it != arrayMaps.end(); ++it)
        {
            ids.push_back(it->toObject()["index"].toInt());
            names.push_back(it->toObject()["name"].toString());
        }

        if (!loadMaps(m_filePath, ids, names, m_vMapTiles))
            return false;
    }

    return true;
}

bool EditorMap::write(FormTilesMapEditor* editor) const
{
    if (m_filePath.isEmpty())
        return false;

    QFile fp(m_filePath);
    if (!fp.open(QFile::WriteOnly))
        return false;

    QJsonObject obj;

    obj["id"] = static_cast<double>(m_id);
    obj["name"] = m_name;

    if (!m_wildMap.isEmpty())
    {
        QJsonArray arrayMap;
        for (auto row : m_wildMap)
        {
            QString rowData = QString::fromRawData(row.data(), row.size());
            arrayMap.push_back(rowData);
        }

        obj["map"] = arrayMap;
    }

    if (!m_vWildMapChars.isEmpty())
    {
        QJsonArray arrayChars;

        for (auto desc : m_vWildMapChars)
        {
            QJsonObject descObj;

            descObj["key"] = QString(desc.key);
            descObj["show"] = QString("%1%2").arg(desc.color).arg(desc.value == '&' ? "&&" : QString(desc.value));
            descObj["show_no_color"] = QString(desc.value);
            descObj["name"] = desc.name;
            descObj["is_room"] = desc.is_room;

            if (!desc.flags.isEmpty())
            {
                QJsonArray arrayFlags;

                for (auto flag : desc.flags)
                    arrayFlags.push_back(QMUD::roomFlagToString(flag));

                descObj["flags"] = arrayFlags;
            }

            arrayChars.push_back(descObj);
        }

        obj["custom_descriptions"] = arrayChars;
    }

    if (!m_npcs.isEmpty())
    {
        QJsonArray npcs;

        for (auto npc : m_npcs)
        {
            QJsonObject objNpc;
            npc->write(objNpc);
            npcs.push_back(objNpc);
        }

        obj["npcs"] = npcs;
    }

    if (!m_items.isEmpty())
    {
        QJsonArray items;

        for (auto item : m_items)
        {
            QJsonObject objItem;
            item->write(objItem);
            items.push_back(objItem);
        }

        obj["items"] = items;
    }

    if (!m_vResources.isEmpty())
    {
        QJsonArray resources;

        for (auto resource : m_vResources)
        {
            if (resource.rarity != QMUD::ResourceRarityType::NONE &&
                    resource.rarity != QMUD::ResourceRarityType::UNKNOWN)
            {
                QJsonObject objResource;
                objResource["resource"] = QMUD::resourceToString(resource.resource);
                objResource["rarity"] = QMUD::resourceRarityToString(resource.rarity);
                resources.push_back(objResource);
            }
        }

        if (!resources.isEmpty())
            obj["resources"] = resources;
    }

    if (!m_vQuests.isEmpty())
    {
        QJsonArray quests;

        for (auto quest : m_vQuests)
        {
            QJsonObject objQuest;
            objQuest["id"] = quest.id;
            objQuest["giveNpcId"] = quest.giveNpcId;
            objQuest["giveNpcMapId"] = quest.giveNpcMapId;
            objQuest["completeNpcId"] = quest.completeNpcId;
            objQuest["completeNpcMapId"] = quest.completeNpcMapId;
            objQuest["level"] = quest.level;
            objQuest["name"] = quest.name;
            objQuest["difficulty"] = QMUD::npcTypeToString(quest.difficulty);
            objQuest["gold"] = quest.gold;

            if (quest.giftItemEnabled)
            {
                objQuest["giftItemEnabled"] = quest.giftItemEnabled;

                if (quest.specificGiftItemEnabled)
                {
                    objQuest["specificGiftItemEnabled"] = quest.specificGiftItemEnabled;
                    objQuest["specificGiftItemId"] = quest.specificGiftItemId;
                    objQuest["specificGiftItemMapId"] = quest.specificGiftItemMapId;
                }
            }

            objQuest["giveDescription"] = quest.giveDescription;
            objQuest["completeDescription"] = quest.completeDescription;

            QJsonArray targets;

            for (auto target : quest.targetData)
            {
                QJsonObject objTarget;

                objTarget["description"] = target.description;
                objTarget["completeDescription"] = target.completeDescription;
                objTarget["npcToTalkToEnabled"] = target.npcToTalkToEnabled;

                if (target.npcToTalkToEnabled)
                {
                    objTarget["npcToTalkToId"] = target.npcToTalkToId;
                    objTarget["npcToTalkToMapId"] = target.npcToTalkToMapId;
                }

                objTarget["npcToKillEnabled"] = target.npcToKillEnabled;

                if (target.npcToKillEnabled)
                {
                    objTarget["npcToKillId"] = target.npcToKillId;
                    objTarget["npcToKillMapId"] = target.npcToKillMapId;
                    objTarget["npcToKillCount"] = target.npcToKillCount;
                    objTarget["npcToKillGiveItemEnabled"] = target.npcToKillGiveItemEnabled;

                    if (target.npcToKillGiveItemEnabled)
                    {
                        objTarget["npcToKillGiveItemName"] = target.npcToKillGiveItemName;
                        objTarget["npcToKillGiveItemProbability"] = target.npcToKillGiveItemProbability;
                    }
                }

                targets.push_back(objTarget);
            }

            if (!targets.isEmpty())
                objQuest["targets"] = targets;

            quests.push_back(objQuest);
        }

        if (!quests.isEmpty())
            obj["quests"] = quests;
    }

    QVector<QJsonObject> roomsObjs;

    if (!m_vMapTiles.isEmpty())
    {
        QJsonArray arrayMapData;

        for (auto map : m_vMapTiles)
        {
            QJsonObject objMap;

            objMap["name"] = map.mapName();
            objMap["index"] = map.mapIndex();

            if (!editor->writeMapData(map, objMap, roomsObjs))
                return false;

            if (!editor->writeMapImages(map, QFileInfo(m_filePath).path() + "/" + QFileInfo(m_filePath).baseName()))
                return false;

            arrayMapData.push_back(objMap);
        }

        obj["maps"] = arrayMapData;

        if (!saveMaps(m_filePath, m_vMapTiles))
            return false;
    }

    QJsonArray arrayRooms;
    for (auto room : roomsObjs)
        arrayRooms.push_back(room);
    obj["rooms"] = arrayRooms;

    QJsonDocument doc = QJsonDocument(obj);

    fp.write(doc.toJson(QJsonDocument::Indented));
    fp.close();

    return true;
}

int EditorMap::unusedNpcId() const
{
    for (int i=0; ; i++)
    {
        bool found = false;
        for (auto npc : m_npcs)
            if (i == npc->id())
            {
                found = true;
                break;
            }

        if (!found)
            return i;
    }
}

int EditorMap::unusedItemId() const
{
    for (int i=0; ; i++)
    {
        bool found = false;
        for (auto item : m_items)
            if (i == item->id())
            {
                found = true;
                break;
            }

        if (!found)
            return i;
    }
}

int EditorMap::unusedQuestId() const
{
    for (int i=0; ; i++)
    {
        bool found = false;
        for (auto quest : m_vQuests)
            if (i == quest.id)
            {
                found = true;
                break;
            }

        if (!found)
            return i;
    }
}

int EditorMap::unusedMapTilesId() const
{
    for (int i=0; ; i++)
    {
        bool found = false;
        for (auto map : m_vMapTiles)
            if (i == map.mapIndex())
            {
                found = true;
                break;
            }

        if (!found)
            return i;
    }
}

QString EditorMap::name() const
{
    return m_name;
}

void EditorMap::setName(const QString &name)
{
    m_name = name;
}

QVector<EditorNpc *> EditorMap::npcs() const
{
    return m_npcs;
}

void EditorMap::setNpcs(QVector<EditorNpc *> npcs)
{
    m_npcs = npcs;
}

void EditorMap::addNpc(EditorNpc *npc)
{
    m_npcs.push_back(npc);
}

QVector<EditorItem *> EditorMap::items() const
{
    return m_items;
}

void EditorMap::setItems(QVector<EditorItem *> items)
{
    m_items = items;
}

void EditorMap::addItem(EditorItem *item)
{
    m_items.push_back(item);
}

QVector<QVector<QChar> > EditorMap::wildMap() const
{
    return m_wildMap;
}

void EditorMap::setWildMap(QVector<QVector<QChar> > map)
{
    m_wildMap = map;
}

QVector<EditorMap::MapCharInfo> EditorMap::wildMapChars() const
{
    return m_vWildMapChars;
}

void EditorMap::setWildMapChars(QVector<EditorMap::MapCharInfo> chars)
{
    m_vWildMapChars = chars;
}

QVector<EditorMap::ResourceInfo> EditorMap::resources() const
{
    return m_vResources;
}

void EditorMap::setResources(QVector<EditorMap::ResourceInfo> resources)
{
    m_vResources = resources;
}

QVector<EditorMap::QuestData> EditorMap::quests() const
{
    return m_vQuests;
}

void EditorMap::setQuests(QVector<EditorMap::QuestData> quests)
{
    m_vQuests = quests;
}


QVector<EditorMap::MapTilesData>& EditorMap::mapTiles()
{
    return m_vMapTiles;
}

QVector<EditorMap::MapTilesData> EditorMap::mapTiles() const
{
    return m_vMapTiles;
}

void EditorMap::setMapTiles(QVector<EditorMap::MapTilesData> mapTiles)
{
    m_vMapTiles = mapTiles;
}

void EditorMap::addMapTiles(const EditorMap::MapTilesData &map)
{
    m_vMapTiles.push_back(map);
}

bool EditorMap::loadMaps(QString path, QVector<int> ids, QVector<QString> names, QVector<EditorMap::MapTilesData>& maps)
{
    Q_ASSERT(ids.size() == names.size());

    maps.clear();

    for (int i = 0; i < ids.size(); ++i)
    {
        QString fileTerrainPixmap = QFileInfo(path).path() + "/" + QFileInfo(path).baseName() + QString("_%1_terrain_editor.png").arg(ids[i]);
        QString fileTerrain = QFileInfo(path).path() + "/" + QFileInfo(path).baseName() + QString("_%1_terrain_editor.csv").arg(ids[i]);
        QString fileObjects = QFileInfo(path).path() + "/" + QFileInfo(path).baseName() + QString("_%1_objects_editor.csv").arg(ids[i]);

        // Controllo l'esistenza dei file
        if (!QFileInfo(fileTerrainPixmap).exists() ||
            !QFileInfo(fileTerrain).exists() ||
            !QFileInfo(fileObjects).exists())
        {
            break;
        }

        // Carico il terreno
        QPixmap pixmapTerrain = QPixmap(fileTerrainPixmap);

        QFile fpTerrain(fileTerrain);
        if (!fpTerrain.open(QIODevice::ReadOnly))
            return false;

        QString terrain = fpTerrain.readAll();

        // Carico gli oggetti
        QFile fpObjects(fileObjects);
        if (!fpObjects.open(QIODevice::ReadOnly))
            return false;

        QString objects = fpObjects.readAll();

        maps.push_back(EditorMap::MapTilesData(names[i], ids[i], pixmapTerrain, terrain, objects));
    }

    return true;
}

bool EditorMap::saveMaps(QString path, QVector<EditorMap::MapTilesData> maps)
{
    for (auto map : maps)
    {
        QString fileTerrainPixmap = QFileInfo(path).path() + "/" + QFileInfo(path).baseName() + QString("_%1_terrain_editor.png").arg(map.mapIndex());
        QString fileTerrain = QFileInfo(path).path() + "/" + QFileInfo(path).baseName() + QString("_%1_terrain_editor.csv").arg(map.mapIndex());
        QString fileObjects = QFileInfo(path).path() + "/" + QFileInfo(path).baseName() + QString("_%1_objects_editor.csv").arg(map.mapIndex());

        // Salvo il terreno
        {
            if (!map.pixmapTerrain().save(fileTerrainPixmap, "PNG"))
                return false;

            QFile fp(fileTerrain);
            if (!fp.open(QFile::WriteOnly))
                return false;

            QTextStream stream(&fp);
            stream << map.terrain();
        }

        // Salvo gli oggetti
        {
            QFile fp(fileObjects);
            if (!fp.open(QFile::WriteOnly))
                return false;

            QTextStream stream(&fp);
            stream << map.objects();
        }
    }

    return true;
}

QString EditorMap::filePath() const
{
    return m_filePath;
}

void EditorMap::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
}

int EditorMap::id() const
{
    return m_id;
}

void EditorMap::setId(int id)
{
    m_id = id;
}
