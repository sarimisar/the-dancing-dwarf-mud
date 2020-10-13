/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef PCCHARACTER_H
#define PCCHARACTER_H

#include "character.h"
#include "characterclass.h"
#include "playerconnection.h"
#include "roomexit.h"
#include "quest.h"
#include "../global.h"
#include "../common.h"

#include <QDir>

namespace CORE
{

class Player;
class Group;
class NpcCharacter;

class PcCharacter : public Character
{
    Q_OBJECT
public:
    struct ActiveQuest
    {
        ActiveQuest(Quest* quest);
        ActiveQuest();

        bool isCompleted() const;

        void questKillCheck(NpcCharacter *npc, Player *player);
        void sendProgress(Player* player) const;

        bool read(const QJsonObject &obj);
        void write(QJsonObject &obj) const;

        const Quest* quest() const;

    private:
        struct TargetProgress
        {
            friend class Quest;

            TargetProgress();

        public:
            bool isCompleted(const Quest::TargetData& targetData) const;

            void setNpcToTalkToCompleted(bool isCompleted);
            void incNpcToKillCount(int count = 1);
            void setNpcToKillCount(int count);
            int npcToKillCount() const;

            bool read(const QJsonObject& obj);
            void write(QJsonObject &obj) const;

        private:
            bool m_bNpcToTalkToCompleted;
            int m_iNpcToKillCount;
        };

        const Quest* m_ptrQuest;
        QVector<TargetProgress> m_vProgress;
    };

    virtual ~PcCharacter() Q_DECL_OVERRIDE;

    static PcCharacter *loadCharacter(QDir rootDir, PlayerConnection *conn, Player *player, QString name, QMUD::FileOpStatus &status, Room *parent);
    static bool createCharacter(QDir rootDir, PlayerConnection *conn, Player *player,
                                QString name, QMUD::SexType sex,
                                QMUD::ClassType chClasses,
                                QMUD::RaceType race,
                                int intelligence,
                                int wis,
                                int str,
                                int con,
                                int dex,
                                QMUD::FileOpStatus &status,
                                bool baseEquipment);

    static bool checkCharacterFiles(QDir rootDir, QMap<QString, QMUD::PcCharacterBaseInfo>& pcBaseInfo);

    virtual void update() Q_DECL_OVERRIDE;

    void save();

    PlayerConnection* connection() const;
    Player* player() const;

    Group* group() const;
    void setGroup(Group*group);

    QVector<LootItemData>& loot();
    void addLoot(const LootItemData& itemData);

    int moveCounter() const;
    void updateMoveCounter();

    void sendDataBasicInfo();
    void sendStatus();
    void sendTargetInfo();
    void sendDataActionLag(bool update);
    void sendDataStatus();
    void sendDataLoot();
    void sendDataMessage(QMUD::SayType type, QString message, QString fromName, QString toName);
    void sendLook();
    void sendRoomInfo();
    void sendRoomContent();
    void sendAttackInfo(int value, int suitorId, int targetId, bool is_damage,
                        QMUD::DamageType damageType,
                        QMUD::AbilityType ability = QMUD::AbilityType::UNKNOWN,
                        QMUD::SpellType spell = QMUD::SpellType::UNKNOWN);
    void sendRequest(QString request, QStringList options = QStringList());
    void sendNewCombatRound();

    QVector<QPair<QMUD::StaticIdType, QMUD::StaticIdType>> recipes() const;
    int professionLevel(QMUD::ProfessionType p) const;
    bool professionLevelInc(QMUD::ProfessionType p);

    void setResourceRadarOn(bool on);
    bool resourceRadarIsOn() const;

    QMUD::StaticIdType lastSessionMapStaticId() const;
    QMUD::StaticIdType lastSessionRoomStaticId() const;

    QString title() const;
    void setTitle(QString title);

    // Tracciatori Wild
    Room* tracedRoom() const;
    QMUD::StaticIdType tracedRoomNpcMapId() const;
    QMUD::StaticIdType tracedRoomNpcId() const;
    void setTracedRoom(Room *room, QMUD::StaticIdType mapId = QMUD::StaticIdInvalid, QMUD::StaticIdType npcId = QMUD::StaticIdInvalid);

    Room* tracedRoomLastFromRoom() const;
    Room* tracedRoomLastRoomExitToTake() const;
    RoomExit::RoomExitType tracedRoomLastRoomExitType() const;
    void setTracedRoomLastData(const Room *lastFromRoom, Room* lastRoomExitToTake, RoomExit::RoomExitType lastRoomExitType);

    // Quest
    QMap<Quest::QuestIdType, ActiveQuest> questListActive() const;
    QMap<Quest::QuestIdType, const Quest*> questListCompleted() const;
    bool questIsSuitabled(Quest* quest);
    void questKillCheck(NpcCharacter* npc);
    bool questAccept(Quest* quest);
    void questComplete(const Quest::QuestIdType &id);

signals:

public slots:
    void onGroupDisbanded(Group* group);
    void onGroupCharacterEnter(Group* group, PcCharacter* ch);
    void onGroupCharacterExit(Group* group, PcCharacter* ch);

protected:
    explicit PcCharacter(PlayerConnection* conn, Player *player, Room *parent = Q_NULLPTR);

private:
    bool readJson(const QJsonObject& obj);
    void writeJson(QJsonObject &obj);
    void updateRecipes();

private:
    PlayerConnection *m_ptrConnection;
    Player *m_ptrPlayer;

    QString m_strFileName;

    Group *m_ptrGroup;

    QVector<LootItemData> m_vLootList;

    QMUD::ClientDataBasicInfo m_clientDataBasicInfoLastSend;
    QMUD::ClientDataLife m_clientDataLifeLastSend;
    QMUD::ClientDataTargetInfo m_clientDataTargetLifeLastSend;
    QMUD::ClientDataActionLag m_clientDataActionLagLastSend;
    QMUD::ClientDataLoot m_clientDataLootLastSend;
    QMUD::ClientDataStatus m_clientDataStatusLastSend;
    QMUD::ClientDataTargetAggro m_clientDatatargetAggroLastSend;
    QMUD::ClientDataRoomInfo m_clientDataRoomInfo;
    QMUD::ClientDataRoomContent m_clientDataRoomContent;

    // Move counter
    int m_iMoveCounter;

    // Recipe
    QVector<QPair<QMUD::StaticIdType, QMUD::StaticIdType>> m_vRecipesByMapAndId;
    QVector<QPair<QMUD::StaticIdType, QMUD::StaticIdType>> m_vRecipesByMapAndIdLearned;
    QMap<QMUD::ProfessionType, int> m_mapProfessionsLevel;

    // Radar risorse
    bool m_bResourceRadarIsOn;

    // Ultima posizione prima di sloggarsi
    QMUD::StaticIdType m_iLastSessionMapStaticId;
    QMUD::StaticIdType m_iLastSessionRoomStaticId;

    // Titolo
    QString m_strTitle;

    // Tracciatori Wild
    Room* m_ptrTracedRoom;
    QMUD::StaticIdType m_iTracedRoomNpcMapId;
    QMUD::StaticIdType m_iTracedRoomNpcId;
    Room* m_ptrTracedRoomLastFromRoom;
    Room* m_ptrTracedRoomLastRoomExitToTake;
    RoomExit::RoomExitType m_eTracedRoomLastRoomExitType;

    // Quest
    QMap<Quest::QuestIdType, const Quest*> m_mapCompletedQuests;
    QMap<Quest::QuestIdType, ActiveQuest> m_mapActiveQuests;
};

} // namespace CORE

#endif // PCCHARACTER_H
