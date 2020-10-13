/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandtitle.h"
#include "world.h"
#include "characterclass.h"
#include "datetime.h"
#include "pccharacter.h"

using namespace CORE;

CommandTitle::CommandTitle(QString origCmd) :
    Command(),
    m_strOrigCmd(origCmd)
{

}

CommandTitle::~CommandTitle()
{

}

bool CommandTitle::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("titolo"), cmd);
}

QStringList CommandTitle::commands()
{
    return QStringList() << QObject::tr("titolo");
}

void CommandTitle::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    if (args().size() == 1 && QObject::tr("RESET") == args()[0])
    {
        QString oldTitle = static_cast<PcCharacter*>(ch)->title();

        static_cast<PcCharacter*>(ch)->setTitle("");
        player->sendLine(QObject::tr("Titolo impostato al valore di default: %1").arg(static_cast<PcCharacter*>(ch)->title()));
        player->sendLineNoColorConversion(QObject::tr("Il precedente titolo era: %1").arg(oldTitle));
    }
    else if (args().size() >= 1)
    {
        QStringList strList;
        if (!splitCommandArgs(strList, m_strOrigCmd, 1))
        {
            Q_ASSERT(false);
            return;
        }

        QString title = strList.last();

        QString oldTitle = static_cast<PcCharacter*>(ch)->title();
        static_cast<PcCharacter*>(ch)->setTitle(title);

        player->sendLine(QObject::tr("Nuovo titolo impostato: %1&d").arg(static_cast<PcCharacter*>(ch)->title()));
        player->sendLineNoColorConversion(QObject::tr("Il precedente titolo era: %1").arg(oldTitle));
    }
    else
    {
        player->sendLineNoColorConversion(QObject::tr("Il tuo titolo attuale e': %1").arg(static_cast<PcCharacter*>(ch)->title()));
    }
}

void CommandTitle::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandTitle::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Permette di impostare un titolo personalizzato"));
        }

        player->sendLine(QObject::tr("Uso: titolo <titolo> - imposta un titolo personalizzato"
                                     "     titolo RESET    - imposta il titolo di default"
                                     "     titolo          - mostra il titolo attuale"));
    }
}
