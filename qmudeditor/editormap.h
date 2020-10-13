/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef EDITORMAP_H
#define EDITORMAP_H

#include <QString>
#include <QObject>
#include <QPixmap>

#include "editornpc.h"
#include "editoritem.h"

class FormTilesMapEditor;

class EditorMap : public QObject
{
    Q_OBJECT

public:
    struct MapTilesData
    {
        MapTilesData() :
            m_iMapIndex(-1)
        { }

        MapTilesData(QString mapName, int mapIndex, QPixmap pixmapTerrain, QString terrain, QString objects) :
            m_strMapName(mapName),
            m_iMapIndex(mapIndex),
            m_pixmapTerrain(pixmapTerrain),
            m_strTerrain(terrain),
            m_strObjects(objects)
        { }

        bool isNull() const { return m_iMapIndex == -1; }

        QString mapName() const { return m_strMapName; }
        int mapIndex() const { return m_iMapIndex; }
        QPixmap pixmapTerrain() const { return m_pixmapTerrain; }
        QString terrain() const { return m_strTerrain; }
        QString objects() const { return m_strObjects; }

    private:
        QString m_strMapName;
        int m_iMapIndex;
        QPixmap m_pixmapTerrain;
        QString m_strTerrain;
        QString m_strObjects;
    };

    struct MapCharInfo
    {
        QChar key;
        QChar value;
        QString color;
        QString name;
        bool is_room;
        QVector<QMUD::RoomFlagType> flags;
    };

    struct ResourceInfo
    {
        QMUD::ResourceType resource;
        QMUD::ResourceRarityType rarity;
    };

    struct QuestData
    {
        struct TargetData
        {
            TargetData() :
                npcToTalkToEnabled(false),
                npcToTalkToId(0),
                npcToTalkToMapId(0),
                npcToKillEnabled(false),
                npcToKillId(0),
                npcToKillMapId(0),
                npcToKillCount(1),
                npcToKillGiveItemEnabled(false),
                npcToKillGiveItemProbability(1)
            { }

            QString description;
            QString completeDescription;

            // (Opzionale) Parlare con un NPC
            bool npcToTalkToEnabled;
            int npcToTalkToId;
            int npcToTalkToMapId;

            // (Opzionale) NPC da uccidere
            bool npcToKillEnabled;
            int npcToKillId;
            int npcToKillMapId;
            int npcToKillCount;
            // (Opzionale) Recuperare un oggetto dall'NPC ucciso
            bool npcToKillGiveItemEnabled;
            QString npcToKillGiveItemName;
            int npcToKillGiveItemProbability;
        };

        QuestData() :
            id(0),
            giveNpcId(0),
            giveNpcMapId(0),
            completeNpcId(0),
            completeNpcMapId(0),
            level(0),
            difficulty(QMUD::NpcType::NORMAL),
            gold(0),
            giftItemEnabled(false),
            specificGiftItemEnabled(false),
            specificGiftItemId(0),
            specificGiftItemMapId(0)
        {}

        int id;

        int giveNpcId;
        int giveNpcMapId;
        int completeNpcId;
        int completeNpcMapId;
        int level;
        QMUD::NpcType difficulty;
        int gold;
        bool giftItemEnabled;
        bool specificGiftItemEnabled;
        int specificGiftItemId;
        int specificGiftItemMapId;
        QString name;
        QString giveDescription;
        QString completeDescription;

        QVector<TargetData> targetData;
    };

    EditorMap(QObject* parent = Q_NULLPTR);

    bool read();
    bool write(FormTilesMapEditor *editor) const;

    int unusedNpcId() const;
    int unusedItemId() const;
    int unusedQuestId() const;
    int unusedMapTilesId() const;

    QString filePath() const;
    void setFilePath(const QString &filePath);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    QVector<EditorNpc *> npcs() const;
    void setNpcs(QVector<EditorNpc *> npcs);
    void addNpc(EditorNpc *npc);

    QVector<EditorItem *> items() const;
    void setItems(QVector<EditorItem *> items);
    void addItem(EditorItem *item);

    QVector<QVector<QChar>> wildMap() const;
    void setWildMap(QVector<QVector<QChar>> map);

    QVector<MapCharInfo> wildMapChars() const;
    void setWildMapChars(QVector<MapCharInfo> chars);

    QVector<ResourceInfo> resources() const;
    void setResources(QVector<ResourceInfo> resources);

    QVector<QuestData> quests() const;
    void setQuests(QVector<QuestData> quests);

    QVector<MapTilesData>& mapTiles();
    QVector<MapTilesData> mapTiles() const;
    void setMapTiles(QVector<MapTilesData> mapTiles);
    void addMapTiles(const MapTilesData& map);

private:
    static bool loadMaps(QString path, QVector<int> ids, QVector<QString> names, QVector<EditorMap::MapTilesData>& maps);
    static bool saveMaps(QString path, QVector<EditorMap::MapTilesData> maps);

    QString m_filePath;

    int m_id;
    QString m_name;

    QVector<EditorNpc*> m_npcs;
    QVector<EditorItem*> m_items;
    QVector<QVector<QChar>> m_wildMap;
    QVector<MapCharInfo> m_vWildMapChars;
    QVector<ResourceInfo> m_vResources;
    QVector<QuestData> m_vQuests;

    QVector<MapTilesData> m_vMapTiles;
};

Q_DECLARE_METATYPE(EditorMap*)
Q_DECLARE_METATYPE(EditorMap::QuestData)
Q_DECLARE_METATYPE(EditorMap::QuestData::TargetData)
Q_DECLARE_METATYPE(EditorMap::MapTilesData)

#endif // EDITORMAP_H
