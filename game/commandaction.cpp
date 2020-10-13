/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandaction.h"
#include "world.h"
#include "map.h"
#include "room.h"
#include "../globaldebug.h"
#include "npccharacter.h"

using namespace CORE;

CommandAction::CommandAction(QMUD::ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandAction::~CommandAction()
{

}

bool CommandAction::isOf(QString cmd, QMUD::ActionType &action)
{
    if (Command::isOf(QObject::tr("apri"), cmd) ||
            Command::isOf(QObject::tr("apro"), cmd))
    {
        action = QMUD::ActionType::OPEN;
        return true;
    }
    else if (Command::isOf(QObject::tr("chiudi"), cmd) ||
            Command::isOf(QObject::tr("chiudo"), cmd))
    {
        action = QMUD::ActionType::CLOSE;
        return true;
    }
    else if (Command::isOf(QObject::tr("sposta"), cmd) ||
            Command::isOf(QObject::tr("sposto"), cmd))
    {
        action = QMUD::ActionType::MOVE;
        return true;
    }

    return false;
}

QStringList CommandAction::commands()
{
    return QStringList() << QObject::tr("apro")
                         << QObject::tr("chiudo")
                         << QObject::tr("sposto");
}

void CommandAction::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: [azione] [cosa] [nord/sud/ovest/est/alto/basso]" QMUD_GLOBAL_NEW_LINE
                                 "     Le possibili azioni sono:" QMUD_GLOBAL_NEW_LINE
                                 "     apro chiudo sposto"));
}

void CommandAction::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    Room* room = ch->room();

    if (m_eAction == QMUD::ActionType::OPEN)
    {
        // Esempio: apro porta/nord
        if (args().size() == 1)
        {
            RoomExit* exit = Q_NULLPTR;

            bool done = false;

            if (QObject::tr("nord").startsWith(args()[0]))
                exit = &room->exitNorth();
            else if (QObject::tr("sud").startsWith(args()[0]))
                exit = &room->exitSouth();
            else if (QObject::tr("ovest").startsWith(args()[0]))
                exit = &room->exitWest();
            else if (QObject::tr("est").startsWith(args()[0]))
                exit = &room->exitEast();
            else if (QObject::tr("alto").startsWith(args()[0]))
                exit = &room->exitUp();
            else if (QObject::tr("basso").startsWith(args()[0]))
                exit = &room->exitDown();

            if (exit)
            {
                if (exit->isValid() &&
                        exit->isDoor() &&
                        exit->openAction() == m_eAction &&
                        exit->door().startsWith(args()[0], Qt::CaseInsensitive) == 0)
                {
                    if (exit->isOpen())
                    {
                        if (player)
                            player->sendLine(QObject::tr("E' aperto"));
                    }
                    else
                        toggleExit(ch, player, *exit);
                }
                else
                {
                    if (player)
                        player->sendLine(QObject::tr("Non c'e' nulla da aprire"));
                }
            }
            else
            {
                auto exits = room->exits();

                for (auto exit : exits)
                {
                    if (exit->isDoor() &&
                            !exit->isOpen() &&
                            exit->openAction() == m_eAction &&
                            exit->door().startsWith(args()[0], Qt::CaseInsensitive) == 0)
                    {
                        done = true;
                        toggleExit(ch, player, *exit);
                        break;
                    }
                }

                if (!done)
                {
                    if (player)
                        player->sendLine(QObject::tr("Non c'e' nulla da aprire qui"));
                }
            }
        }
        // Esempio: apro porta nord
        else if (args().size() == 2)
        {
            RoomExit* exit = Q_NULLPTR;

            if (QObject::tr("nord").startsWith(args()[1]))
                exit = &room->exitNorth();
            else if (QObject::tr("sud").startsWith(args()[1]))
                exit = &room->exitSouth();
            else if (QObject::tr("ovest").startsWith(args()[1]))
                exit = &room->exitWest();
            else if (QObject::tr("est").startsWith(args()[1]))
                exit = &room->exitEast();
            else if (QObject::tr("alto").startsWith(args()[1]))
                exit = &room->exitUp();
            else if (QObject::tr("basso").startsWith(args()[1]))
                exit = &room->exitDown();

            if (!exit)
            {
                if (player)
                    sendUsage(player);

                return;
            }

            if (exit->isValid() &&
                    exit->isDoor() &&
                    exit->openAction() == m_eAction &&
                    exit->door().startsWith(args()[0], Qt::CaseInsensitive) == 0)
            {
                if (exit->isOpen())
                {
                    if (player)
                        player->sendLine(QObject::tr("E' aperto"));
                }
                else
                    toggleExit(ch, player, *exit);
            }
            else
            {
                if (player)
                    player->sendLine(QObject::tr("Non c'e' niente di simile in questa direzione"));
            }
        }
        else
        {
            if (player)
                sendUsage(player);
        }
    }
    else if (m_eAction == QMUD::ActionType::CLOSE)
    {
        // Esempio: chiudo porta/nord
        if (args().size() == 1)
        {
            RoomExit* exit = Q_NULLPTR;

            auto exits = room->exits();
            bool done = false;

            if (QObject::tr("nord").startsWith(args()[0]))
                exit = &room->exitNorth();
            else if (QObject::tr("sud").startsWith(args()[0]))
                exit = &room->exitSouth();
            else if (QObject::tr("ovest").startsWith(args()[0]))
                exit = &room->exitWest();
            else if (QObject::tr("est").startsWith(args()[0]))
                exit = &room->exitEast();
            else if (QObject::tr("alto").startsWith(args()[0]))
                exit = &room->exitUp();
            else if (QObject::tr("basso").startsWith(args()[0]))
                exit = &room->exitDown();

            if (exit)
            {
                if (exit->isValid() &&
                    exit->isDoor() &&
                    exit->closeAction() == m_eAction &&
                    exit->door().startsWith(args()[0], Qt::CaseInsensitive) == 0)
                {
                    if (!exit->isOpen())
                    {
                        if (player)
                            player->sendLine(QObject::tr("E' chiuso"));
                    }
                    else
                        toggleExit(ch, player, *exit);
                }
                else
                {
                    if (player)
                        player->sendLine(QObject::tr("Non c'e' nulla da chiudere"));
                }
            }
            else
            {
                for (auto exit : exits)
                {
                    if (exit->isDoor() &&
                            exit->isOpen() &&
                            exit->closeAction() == m_eAction &&
                            exit->door().startsWith(args()[0], Qt::CaseInsensitive) == 0)
                    {
                        done = true;
                        toggleExit(ch, player, *exit);
                        break;
                    }
                }

                if (!done)
                {
                    if (player)
                        player->sendLine(QObject::tr("Non c'e' nulla da chiudere qui"));
                }
            }
        }
        // Esempio: chiudo porta nord
        else if (args().size() == 2)
        {
            RoomExit* exit = Q_NULLPTR;

            if (QObject::tr("nord").startsWith(args()[1]))
                exit = &room->exitNorth();
            else if (QObject::tr("sud").startsWith(args()[1]))
                exit = &room->exitSouth();
            else if (QObject::tr("ovest").startsWith(args()[1]))
                exit = &room->exitWest();
            else if (QObject::tr("est").startsWith(args()[1]))
                exit = &room->exitEast();
            else if (QObject::tr("alto").startsWith(args()[1]))
                exit = &room->exitUp();
            else if (QObject::tr("basso").startsWith(args()[1]))
                exit = &room->exitDown();

            if (!exit)
            {
                if (player)
                    sendUsage(player);

                return;
            }

            if (exit->isValid() &&
                    exit->isDoor() &&
                    exit->closeAction() == m_eAction &&
                    exit->door().startsWith(args()[0], Qt::CaseInsensitive) == 0)
            {
                if (!exit->isOpen())
                {
                    if (player)
                        player->sendLine(QObject::tr("E' chiuso"));
                }
                else
                    toggleExit(ch, player, *exit);
            }
            else
            {
                if (player)
                    player->sendLine(QObject::tr("Non c'e' niente di simile in questa direzione"));
            }
        }
        else
        {
            if (player)
                sendUsage(player);
        }
    }
    else
    {
        // Esempio: sposto rami
        if (args().size() == 1)
        {
            auto exits = room->exits();
            bool done = false;

            for (auto exit : exits)
            {
                if (exit->isDoor() &&
                        ((!exit->isOpen() && exit->openAction() == m_eAction) ||
                         (exit->isOpen() && exit->closeAction() == m_eAction)) &&
                        exit->door().compare(args()[0], Qt::CaseInsensitive) == 0)
                {
                    done = true;
                    toggleExit(ch, player, *exit);
                    break;
                }
            }

            if (!done)
            {
                if (player)
                    player->sendLine(QObject::tr("Non c'e' niente si simile qui"));
            }
        }
        // Esempio: sposto rami nord
        else if (args().size() == 2)
        {
            RoomExit* exit = Q_NULLPTR;

            if (QObject::tr("nord").startsWith(args()[1]))
                exit = &room->exitNorth();
            else if (QObject::tr("sud").startsWith(args()[1]))
                exit = &room->exitSouth();
            else if (QObject::tr("ovest").startsWith(args()[1]))
                exit = &room->exitWest();
            else if (QObject::tr("est").startsWith(args()[1]))
                exit = &room->exitEast();
            else if (QObject::tr("alto").startsWith(args()[1]))
                exit = &room->exitUp();
            else if (QObject::tr("basso").startsWith(args()[1]))
                exit = &room->exitDown();

            if (!exit)
            {
                if (player)
                    sendUsage(player);

                return;
            }

            if (exit->isValid() && exit->isDoor() &&
                    ((!exit->isOpen() && exit->openAction() == m_eAction) ||
                     (exit->isOpen() && exit->closeAction() == m_eAction)) &&
                    exit->door().compare(args()[0], Qt::CaseInsensitive) == 0)
            {
                toggleExit(ch, player, *exit);
            }
            else
            {
                if (player)
                    player->sendLine(QObject::tr("Non c'e' niente di simile in questa direzione"));
            }
        }
        else
        {
            if (player)
                sendUsage(player);
        }
    }
}

void CommandAction::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandAction::toggleExit(Character* ch, Player* player, RoomExit &exit)
{
    Room* room = ch->room();

    QString descClose = exit.descClose();
    QString descCloseOther = exit.descCloseOther();
    QString descOpen = exit.descOpen();
    QString descOpenOther = exit.descOpenOther();

    if (exit.door() == "door")
    {
        if (descOpen.isEmpty())
        {
            if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
                descOpen = QObject::tr("Apri la porta in %1").arg(exit.roomExitToReadableString());
            else
                descOpen = QObject::tr("Apri la porta a %1").arg(exit.roomExitToReadableString());
        }

        if (descClose.isEmpty())
        {
            if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
                descClose = QObject::tr("Chiudi la porta in %1").arg(exit.roomExitToReadableString());
            else
                descClose = QObject::tr("Chiudi la porta a %1").arg(exit.roomExitToReadableString());
        }

        if (descOpenOther.isEmpty())
        {
            if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
                descOpenOther = QObject::tr("%1 apre la porta in ") + exit.roomExitToReadableString();
            else
                descOpenOther = QObject::tr("%1 apre la porta a ") + exit.roomExitToReadableString();
        }

        if (descCloseOther.isEmpty())
        {
            if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
                descCloseOther = QObject::tr("%1 chiude la porta in ") + exit.roomExitToReadableString();
            else
                descCloseOther = QObject::tr("%1 chiude la porta a ") + exit.roomExitToReadableString();
        }
    }

    if (descClose.isEmpty())
    {
        if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
            descClose = QObject::tr("Chiudi il passaggio in %1").arg(exit.roomExitToReadableString());
        else
            descClose = QObject::tr("Chiudi il passaggio a %1").arg(exit.roomExitToReadableString());
    }

    if (descOpen.isEmpty())
    {
        if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
            descOpen = QObject::tr("Apri il passaggio in %1").arg(exit.roomExitToReadableString());
        else
            descOpen = QObject::tr("Apri il passaggio a %1").arg(exit.roomExitToReadableString());
    }

    if (descCloseOther.isEmpty())
    {
        if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
            descCloseOther = QObject::tr("%1 chiude il passaggio in ") + exit.roomExitToReadableString();
        else
            descCloseOther = QObject::tr("%1 chiudi il passaggio a ") + exit.roomExitToReadableString();
    }

    if (descOpenOther.isEmpty())
    {
        if (exit.exitType() == RoomExit::RoomExitType::EXIT_UP || exit.exitType() == RoomExit::RoomExitType::EXIT_DOWN)
            descOpenOther = QObject::tr("%1 apre il passaggio in ") + exit.roomExitToReadableString();
        else
            descOpenOther = QObject::tr("%1 apre il passaggio a ") + exit.roomExitToReadableString();
    }

    if (player)
    {
        if (exit.isOpen())
            player->sendLine(descClose);
        else
            player->sendLine(descOpen);
    }

    auto characters = room->characters();

    for (Character* chInRoom : characters)
    {
        if (chInRoom != ch)
        {
            if (chInRoom->chType() == Character::ChType::CHTYPE_PC &&
                    chInRoom->chFollowed() != ch->id())
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                if (exit.isOpen())
                    playerOther->sendLine(descCloseOther.arg(ch->name(chInRoom)));
                else
                    playerOther->sendLine(descOpenOther.arg(ch->name(chInRoom)));
            }
        }
    }

    exit.toggle();
}
