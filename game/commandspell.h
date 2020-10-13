/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COMMANDSPELL_H
#define COMMANDSPELL_H

#include "command.h"

namespace CORE
{

class CommandSpell : public Command
{
    friend class Command;
    friend class CommandSpellHandler;

public:
    enum class ActionType
    {
        SPELL,
        SPELL_LIST
    };

    CommandSpell(ActionType type);
    virtual ~CommandSpell();

protected:
    static bool isOf(QString cmd, ActionType &type);
    static QStringList commands();
    static void sendUsage(Player* player, bool help = false);
    void executePc(Character *ch, Player* player);
    void executeNpc(Character* ch);

private:
    // Spell arcani
    void spellMagicMissile(Character *ch, Player* player, Entity* target);
    void spellShield(Character *ch, Player* player, Entity* target);
    void spellStrenght(Character *ch, Player* player, Entity* target);
    void spellIllusoryImages(Character *ch, Player* player, Entity* target);
    void spellFireball(Character *ch, Player* player);
    void spellBurningHands(Character *ch, Player* player);
    void spellFly(Character *ch, Player* player, Entity* target);
    void spellInvisibility(Character *ch, Player* player, Entity* target);
    void spellSeeInvisibility(Character *ch, Player* player, Entity* target);
    void spellFireShield(Character *ch, Player* player, Entity* target);
    void spellIceShield(Character *ch, Player* player, Entity* target);
    void spellBladesShield(Character *ch, Player* player, Entity* target);
    void spellFieryArrow(Character *ch, Player* player, Entity* target);
    void spellIceStorm(Character *ch, Player *player);

    // Spell divini
    void spellSanctify(Character *ch, Player* player, Entity* target);
    void spellCure(Character *ch, Player* player, Entity* target);
    void spellDivineShield(Character *ch, Player* player, Entity* target);
    void spellInflictingWounds(Character *ch, Player* player, Entity* target);

    // Funzioni base
    void spellDamageTarget(Character *ch, Player* player, Entity* target, QMUD::SpellType spell, QMUD::DamageType damageType,
                           int manaPoints, int damage, int lag,
                           QString messageCh,
                           QString messageTarget,
                           QString messageOther);
    void spellDamageZone(Character *ch, Player* player, QMUD::SpellType spell, QMUD::DamageType damageType,
                         int manaPoints, int damage, int lag,
                         QString messageCh,
                         QString messageTarget,
                         QString messageOther);
    void spellCureTarget(Character *ch, Player* player, Entity* target, QMUD::SpellType spell,
                         int manaPoints, int cure, int lag,
                         QString messageCh,
                         QString messageTarget,
                         QString messageOther,
                         QString messageChMe,
                         QString messageOtherMe);
    bool spellAddStatusTarget(Character *ch, Player* player, Entity* target, QMUD::SpellType spell, QMUD::TemporaryStatusType status,
                              int manaPoints, int turns, int lag,
                              QString messageCh,
                              QString messageTarget,
                              QString messageOther,
                              QString messageChMe,
                              QString messageOtherMe);
    bool spellAddStatusShield(Character *ch, Player* player, Entity* target, QMUD::SpellType spell, QMUD::TemporaryStatusType status,
                              int manaPoints, int turns, int lag,
                              QString messageChMe,
                              QString messageOtherMe);

private:
    ActionType m_eActionType;
};

} // namespace CORE

Q_DECLARE_METATYPE(CORE::CommandSpell::ActionType)

namespace CORE
{

class CommandSpellHandler : public CommandHandler
{
public:
    bool isOf(QString cmd, QVector<QVariant>& args)
    {
        CommandSpell::ActionType action;
        bool ret = CommandSpell::isOf(cmd, action);

        args.push_back(QVariant::fromValue(action));

        return ret;
    }

    QStringList commands(Player* player)
    {
        Q_UNUSED(player)

        return CommandSpell::commands();
    }

    bool sendUsage(Player* player, QStringList args)
    {
        Q_UNUSED(args)

        CommandSpell::sendUsage(player, true);
        return true;
    }

    Command::Ptr create(Player* player, QVector<QVariant> args)
    {
        Q_UNUSED(player)

        return Command::Ptr(new CommandSpell(args[0].value<CommandSpell::ActionType>()));
    }
};

} // namespace CORE

#endif // COMMANDSPELL_H
