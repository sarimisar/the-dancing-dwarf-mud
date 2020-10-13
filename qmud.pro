QT += gui
QT += network qml

CONFIG += c++11 console
CONFIG -= app_bundle c++98

TARGET = test45

DEFINES += QT_DEPRECATED_WARNINGS

defineTest(copyToMapsDir) {
    files = $$1

    for(FILE, files) {
        win32:DDIR = $${_PRO_FILE_PWD_}\\qmudroot\\data\\maps
        unix:DDIR = $${_PRO_FILE_PWD_}/qmudroot/data/maps

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$sprintf($${QMAKE_MKDIR_CMD}, $$quote($$DDIR)) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

defineTest(copyToPlayersDir) {
    files = $$1

    for(FILE, files) {
        win32:DDIR = $${_PRO_FILE_PWD_}\\qmudroot\\data\\players
        unix:DDIR = $${_PRO_FILE_PWD_}/qmudroot/data/players

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$sprintf($${QMAKE_MKDIR_CMD}, $$quote($$DDIR)) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

defineTest(copyToCharactersDir) {
    files = $$1

    for(FILE, files) {
        win32:DDIR = $${_PRO_FILE_PWD_}\\qmudroot\\data\\players\\characters
        unix:DDIR = $${_PRO_FILE_PWD_}/qmudroot/data/players/characters

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$sprintf($${QMAKE_MKDIR_CMD}, $$quote($$DDIR)) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToMapsDir($${_PRO_FILE_PWD_}/data/maps/*.*)
#copyToPlayersDir($${_PRO_FILE_PWD_}/data/players/*.json)
#copyToCharactersDir($${_PRO_FILE_PWD_}/data/players/characters/*.json)

SOURCES += \
    game/commandquest.cpp \
    game/commandsuban.cpp \
    game/commandtarget.cpp \
    game/commandtitle.cpp \
    game/connectedcomponentalgorithm.cpp \
    game/mapsgraph.cpp \
    game/quest.cpp \
        main.cpp \
    network/server.cpp \
    network/connection.cpp \
    game/world.cpp \
    game/player.cpp \
    game/playerconnection.cpp \
    game/character.cpp \
    game/entity.cpp \
    game/pccharacter.cpp \
    game/map.cpp \
    game/room.cpp \
    game/command.cpp \
    game/commandempty.cpp \
    game/commandlook.cpp \
    game/commandclose.cpp \
    game/roomexit.cpp \
    game/commandmove.cpp \
    game/color.cpp \
    game/commandwho.cpp \
    game/npccharacter.cpp \
    global.cpp \
    game/commandcondition.cpp \
    game/commandkill.cpp \
    game/commandsukill.cpp \
    game/item.cpp \
    game/commandtake.cpp \
    game/commandinventory.cpp \
    game/itemweapon.cpp \
    game/commandwear.cpp \
    game/commandtakeoff.cpp \
    game/commandequipment.cpp \
    game/commanddrop.cpp \
    game/characterclass.cpp \
    game/characterclasswarrior.cpp \
    game/commandability.cpp \
    game/itemarmor.cpp \
    game/commandsureloadmap.cpp \
    game/characterclassmage.cpp \
    game/group.cpp \
    game/commandgroup.cpp \
    game/commandfollow.cpp \
    game/commandspell.cpp \
    game/commandsay.cpp \
    game/commandinstance.cpp \
    game/commandsuinfo.cpp \
    game/commandloot.cpp \
    game/itemtrinket.cpp \
    game/characterclassbarbarian.cpp \
    game/characterclassdruid.cpp \
    game/characterclassmonk.cpp \
    game/characterclasspriest.cpp \
    game/characterclassrogue.cpp \
    game/commandshop.cpp \
    common.cpp \
    game/commandresources.cpp \
    game/itemfood.cpp \
    game/commandeat.cpp \
    game/itemdrink.cpp \
    game/commandcrafting.cpp \
    game/itemresource.cpp \
    game/commandaction.cpp \
    game/itempotion.cpp \
    unixsignalhandler.cpp \
    game/commandsuset.cpp \
    game/itemtonic.cpp \
    game/commandsugoto.cpp \
    configfile.cpp \
    game/statistics.cpp \
    game/commandstatistics.cpp \
    game/datetime.cpp \
    game/commanddate.cpp \
    game/itemsymbol.cpp \
    game/itemtotem.cpp \
    game/commandinfo.cpp

HEADERS += \
    game/commandquest.h \
    game/commandsuban.h \
    game/commandtarget.h \
    game/commandtitle.h \
    game/connectedcomponentalgorithm.h \
    game/mapsgraph.h \
    game/quest.h \
    network/server.h \
    game/world.h \
    game/player.h \
    game/playerconnection.h \
    game/character.h \
    game/entity.h \
    network/netconnection.h \
    game/pccharacter.h \
    globaldebug.h \
    game/map.h \
    game/room.h \
    game/command.h \
    game/commandempty.h \
    game/commandlook.h \
    game/commandclose.h \
    game/roomexit.h \
    game/commandmove.h \
    game/color.h \
    game/commandwho.h \
    game/npccharacter.h \
    game/util.h \
    game/commandcondition.h \
    game/commandkill.h \
    game/commandsukill.h \
    game/item.h \
    game/commandtake.h \
    game/commandinventory.h \
    game/itemweapon.h \
    game/commandwear.h \
    game/commandtakeoff.h \
    game/commandequipment.h \
    game/commanddrop.h \
    game/characterclass.h \
    game/characterclasswarrior.h \
    game/commandability.h \
    game/itemarmor.h \
    game/commandsureloadmap.h \
    game/characterclassmage.h \
    game/group.h \
    game/commandgroup.h \
    game/commandfollow.h \
    game/commandspell.h \
    game/commandsay.h \
    game/commandinstance.h \
    game/commandsuinfo.h \
    game/commandloot.h \
    game/itemtrinket.h \
    game/characterclassdruid.h \
    game/characterclassbarbarian.h \
    game/characterclassrogue.h \
    game/characterclassmonk.h \
    game/characterclasspriest.h \
    game/commandshop.h \
    common.h \
    game/commandresources.h \
    game/itemfood.h \
    game/commandeat.h \
    game/itemdrink.h \
    game/commandcrafting.h \
    game/itemresource.h \
    game/commandaction.h \
    game/itempotion.h \
    unixsignalhandler.h \
    game/commandsuset.h \
    game/itemtonic.h \
    game/commandsugoto.h \
    configfile.h \
    game/statistics.h \
    game/commandstatistics.h \
    game/datetime.h \
    game/commanddate.h \
    game/itemsymbol.h \
    game/itemtotem.h \
    global.h \
    game/commandinfo.h

DISTFILES += \
    data/maps/casa_degli_dei.json \
    data/maps/villaggio_kinori.json \
    data/players/admin.json \
    data/players/characters/admin.json \
    data/maps/bosco.json \
    data/maps/wilderness.json \
    data/maps/instance_bosco.json \
    data/maps/capitale_saienna.json \
    data/maps/mappa_0.json \
    data/maps/nave_pirati.json \
    data/forbidden_names.txt \
    data/maps/grotta_dei_coboldi.json
