/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef QUEST_H
#define QUEST_H

#include <QString>
#include <QVector>

#include "../global.h"

namespace CORE
{

class Quest
{
public:
    struct TargetData
    {
        friend class Quest;

        TargetData();

    public:
        QString description() const;
        void setDescription(const QString &strDescription);

        QString completeDescription() const;
        void setCompleteDescription(const QString &strCompleteDescription);

        bool npcToTalkToEnabled() const;
        void setNpcToTalkToEnabled(bool bNpcToTalkToEnabled);

        QMUD::StaticIdType npcToTalkToId() const;
        void setNpcToTalkToId(const QMUD::StaticIdType &iNpcToTalkToId);

        QMUD::StaticIdType npcToTalkToMapId() const;
        void setNpcToTalkToMapId(const QMUD::StaticIdType &iNpcToTalkToMapId);

        bool npcToKillEnabled() const;
        void setNpcToKillEnabled(bool bNpcToKillEnabled);

        QMUD::StaticIdType npcToKillId() const;
        void setNpcToKillId(const QMUD::StaticIdType &iNpcToKillId);

        QMUD::StaticIdType npcToKillMapId() const;
        void setNpcToKillMapId(const QMUD::StaticIdType &iNpcToKillMapId);

        int npcToKillCount() const;
        void setNpcToKillCount(int iNpcToKillCount);

        bool npcToKillGiveItemEnabled() const;
        void setNpcToKillGiveItemEnabled(bool bNpcToKillGiveItemEnabled);

        QString npcToKillGiveItemName() const;
        void setNpcToKillGiveItemName(const QString &strNpcToKillGiveItemName);

        int npcToKillGiveItemProbability() const;
        void setNpcToKillGiveItemProbability(int iNpcToKillGiveItemProbability);

    private:
        bool read(const QJsonObject& obj);

        QString m_strDescription;
        QString m_strCompleteDescription;

        // Parlare con un NPC
        bool m_bNpcToTalkToEnabled;
        QMUD::StaticIdType m_iNpcToTalkToId;
        QMUD::StaticIdType m_iNpcToTalkToMapId;

        // NPC da uccidere
        bool m_bNpcToKillEnabled;
        QMUD::StaticIdType m_iNpcToKillId;
        QMUD::StaticIdType m_iNpcToKillMapId;
        int m_iNpcToKillCount;

        // Recuperare un oggetto dall'NPC ucciso
        bool m_bNpcToKillGiveItemEnabled;
        QString m_strNpcToKillGiveItemName;
        int m_iNpcToKillGiveItemProbability;
    };

    typedef QPair<QMUD::StaticIdType, QMUD::StaticIdType> QuestIdType;
    static QuestIdType QuestIdInvalid;

    Quest();

    bool checkIsValid();

    bool read(const QJsonObject& obj, QMUD::StaticIdType mapId);

    QuestIdType id() const;
    QJsonValue idToJsonValue() const;
    QString idToString() const;
    static QString idToString(const QuestIdType& id);
    static QuestIdType idFromString(QString id);
    static QuestIdType idFromJsonValue(const QJsonValue &value, bool &ok);

    QMUD::StaticIdType mapId() const;
    QMUD::StaticIdType questId() const;
    void setId(const QuestIdType &id);

    QMUD::StaticIdType giveNpcId() const;
    void setGiveNpcId(QMUD::StaticIdType iGiveNpcId);

    QMUD::StaticIdType giveNpcMapId() const;
    void setGiveNpcMapId(QMUD::StaticIdType iGiveNpcMapId);

    QMUD::StaticIdType completeNpcId() const;
    void setCompleteNpcId(QMUD::StaticIdType iCompleteNpcId);

    QMUD::StaticIdType completeNpcMapId() const;
    void setCompleteNpcMapId(QMUD::StaticIdType iCompleteNpcMapId);

    int level() const;
    void setLevel(int iLevel);

    QString name() const;
    void setName(const QString &strName);

    QString giveDescription() const;
    void setGiveDescription(const QString &strGiveDescription);

    QString completeDescription() const;
    void setCompleteDescription(const QString &strCompleteDescription);

    QVector<TargetData> targetData() const;
    void setTargetData(const QVector<TargetData> &vTargetData);

    QMUD::NpcType difficulty() const;
    void setDifficulty(const QMUD::NpcType &eDifficulty);

    int gold() const;
    void setGold(int iGold);

    bool giftItemEnabled() const;
    void setGiftItemEnabled(bool bGiftItemEnabled);

    bool specificGiftItemEnabled() const;
    void setSpecificGiftItemEnabled(bool bSpecificGiftItemEnabled);

    QMUD::StaticIdType specificGiftItemId() const;
    void setSpecificGiftItemId(const QMUD::StaticIdType &iSpecificGiftItemId);

    QMUD::StaticIdType specificGiftItemMapId() const;
    void setSpecificGiftItemMapId(const QMUD::StaticIdType &iSpecificGiftItemMapId);

private:
    QuestIdType m_iId;

    QMUD::StaticIdType m_iGiveNpcId;
    QMUD::StaticIdType m_iGiveNpcMapId;
    QMUD::StaticIdType m_iCompleteNpcId;
    QMUD::StaticIdType m_iCompleteNpcMapId;
    int m_iLevel;
    QMUD::NpcType m_eDifficulty;
    int m_iGold;
    bool m_bGiftItemEnabled;
    bool m_bSpecificGiftItemEnabled;
    QMUD::StaticIdType m_iSpecificGiftItemId;
    QMUD::StaticIdType m_iSpecificGiftItemMapId;
    QString m_strName;
    QString m_strGiveDescription;
    QString m_strCompleteDescription;

    QVector<TargetData> m_vTargetData;
};

}

#endif // QUEST_H
