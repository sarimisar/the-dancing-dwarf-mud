/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsay.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "characterclass.h"
#include "map.h"
#include "group.h"

using namespace CORE;

CommandSay::CommandSay(QMUD::SayType sayType, QString origCmd) :
    Command(),
    m_eSayType(sayType),
    m_strOrigCmd(origCmd)
{

}

CommandSay::~CommandSay()
{

}

bool CommandSay::isOf(QString cmd, QMUD::SayType& sayType)
{
    if (Command::isOf(QObject::tr("dico"), cmd))
    {
        sayType = QMUD::SayType::CHARACTER;
        return true;
    }
    else if (Command::isOf(QObject::tr("sdico"), cmd))
    {
        sayType = QMUD::SayType::ROOM;
        return true;
    }
    else if (Command::isOf(QObject::tr("adico"), cmd))
    {
        sayType = QMUD::SayType::AREA;
        return true;
    }
    else if (Command::isOf(QObject::tr("gdico"), cmd))
    {
        sayType = QMUD::SayType::GROUP;
        return true;
    }

    return false;
}

QStringList CommandSay::commands()
{
    return QStringList() << QObject::tr("dico")
                         << QObject::tr("sdico")
                         << QObject::tr("adico")
                         << QObject::tr("gdico");
}

void CommandSay::executePc(Character* ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    if (m_eSayType == QMUD::SayType::CHARACTER)
    {
        if (args().size() <= 1)
        {
            if (player)
                player->sendLine(QObject::tr("Dire che cosa a chi?"));
            return;
        }

        QString targetName;
        int numArgs = 2;

        if (args()[0].compare("a", Qt::CaseInsensitive) == 0)
        {
            if (args().size() < 2)
            {
                if (player)
                    player->sendLine(QObject::tr("Dire che cosa a chi?"));
                return;
            }

            targetName = args()[1];
            numArgs = 3;
        }
        else
            targetName = args()[0];

        bool wrongUsage = false;
        Character *targetCh = findCharacterInMap(targetName, wrongUsage, true);

        if (wrongUsage)
        {
            sendUsage(player);
            return;
        }

        if (!targetCh)
        {
            if (player)
                player->sendLine(QObject::tr("Non c'e' nessun %1").arg(targetName));
            return;
        }

        QStringList strList;
        if (!splitCommandArgs(strList, m_strOrigCmd, numArgs))
        {
            Q_ASSERT(false);
            return;
        }

        QString strSay = strList.last();

        if (player)
        {
            player->sendLine(QObject::tr("&Pdici a %1 \'%2\'&d").arg(targetCh->name(ch)).arg(strSay));
            static_cast<PcCharacter*>(ch)->sendDataMessage(m_eSayType, strSay, ch->name(Q_NULLPTR), targetCh->name(ch));
        }

        if (targetCh->chType() == Character::ChType::CHTYPE_PC)
        {
            static_cast<PcCharacter*>(targetCh)->player()->sendLine(QObject::tr("&P%1 ti dice \'%2\'&d").arg(ch->name(targetCh)).arg(strSay));
            static_cast<PcCharacter*>(targetCh)->sendDataMessage(m_eSayType, strSay, ch->name(targetCh), targetCh->name(Q_NULLPTR));
        }
    }
    else if (m_eSayType == QMUD::SayType::ROOM)
    {
        QString strSay = m_strOrigCmd.mid(command().size()).trimmed();

        if (player)
        {
            player->sendLine(QObject::tr("&W[STANZA] dici \'%1\'&d").arg(strSay));
            static_cast<PcCharacter*>(ch)->sendDataMessage(m_eSayType, strSay, ch->name(Q_NULLPTR), "");
        }

        auto characters = ch->room()->characters();

        for (auto chOther : characters)
        {
            if (chOther == ch)
                continue;

            if (chOther->chType() == Character::ChType::CHTYPE_PC)
            {
                static_cast<PcCharacter*>(chOther)->player()->sendLine(QObject::tr("&W[STANZA] %1 dice \'%2\'&d").arg(ch->name(chOther)).arg(strSay));
                static_cast<PcCharacter*>(chOther)->sendDataMessage(m_eSayType, strSay, ch->name(chOther), "");
            }
        }
    }
    else if (m_eSayType == QMUD::SayType::AREA)
    {
        QString strSay = m_strOrigCmd.mid(command().size()).trimmed();

        if (player)
        {
            player->sendLine(QObject::tr("&W[AREA] dici \'%1\'&d").arg(strSay));
            static_cast<PcCharacter*>(ch)->sendDataMessage(m_eSayType, strSay, ch->name(Q_NULLPTR), "");
        }

        Map* map = ch->room()->map();
        auto characters = map->characters();

        for (auto chOther : characters)
        {
            if (chOther == ch)
                continue;

            if (chOther->chType() == Character::ChType::CHTYPE_PC)
            {
                static_cast<PcCharacter*>(chOther)->player()->sendLine(QObject::tr("&W[AREA] %1 dice \'%2\'&d").arg(ch->name(chOther)).arg(strSay));
                static_cast<PcCharacter*>(chOther)->sendDataMessage(m_eSayType, strSay, ch->name(chOther), "");
            }
        }
    }
    else if (m_eSayType == QMUD::SayType::GROUP)
    {
        if (player)
        {
            PcCharacter *pc = static_cast<PcCharacter*>(ch);

            if (!pc->group())
            {
                player->sendLine(QObject::tr("Non fai parte di nessun gruppo"));
                return;
            }

            QString strSay = m_strOrigCmd.mid(command().size()).trimmed();

            if (player)
            {
                player->sendLine(QObject::tr("&B[GRUPPO] dici \'%1\'&d").arg(strSay));
                static_cast<PcCharacter*>(ch)->sendDataMessage(m_eSayType, strSay, ch->name(Q_NULLPTR), "");
            }

            auto characters = pc->group()->characters();

            for (auto chOther : characters)
            {
                if (chOther == ch)
                    continue;

                static_cast<PcCharacter*>(chOther)->player()->sendLine(QObject::tr("&B[GRUPPO] %1 dice \'%2\'&d").arg(ch->name(chOther)).arg(strSay));
                static_cast<PcCharacter*>(chOther)->sendDataMessage(m_eSayType, strSay, ch->name(chOther), "");
            }
        }
    }
}

void CommandSay::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandSay::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    if (player)
        player->sendLine(QObject::tr("Uso: dico [a] [<n>.]<nome> <frase> - Parlare con qualcuno" QMUD_GLOBAL_NEW_LINE
                                     "     sdico <frase>                 - Parlare a tutti nella stanza" QMUD_GLOBAL_NEW_LINE
                                     "     adico <frase>                 - Parlare a tutti nella mappa" QMUD_GLOBAL_NEW_LINE
                                     "     gdico <frase>                 - Parlare al grupo"));
}
