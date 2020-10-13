/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "game/world.h"
#include "network/server.h"
#include "globaldebug.h"
#include "unixsignalhandler.h"
#include "configfile.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTime>
#include <QFile>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <signal.h>

#ifdef WIN32
#include <Windows.h>
#endif

// Log
static QFile* fpLogFile = Q_NULLPTR;

// Funzione di overwrite per QDebug
void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString txt;

    switch (type)
    {
    case QtDebugMsg:
        txt = QString("[D] %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("[W] %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("[C] %1:%2:%3 - %4").arg(context.file).arg(context.function).arg(context.line).arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("[F] %1:%2:%3 - %4").arg(context.file).arg(context.function).arg(context.line).arg(msg);
        abort();
    case QtInfoMsg:
        txt = QString("[I] %1").arg(msg);
        break;
    }

    if (type == QtDebugMsg)
        std::cout << "\033[1;34m" << "[" << QTime::currentTime().toString("hh:mm:ss:zzz").toStdString() << "]" << "\033[1;36m" << txt.toStdString() << "\033[0m" << std::endl;
    else if (type == QtWarningMsg)
        std::cout << "\033[1;34m" << "[" << QTime::currentTime().toString("hh:mm:ss:zzz").toStdString() << "]" << "\033[0;33m" << txt.toStdString() << "\033[0m" << std::endl;
    else if (type == QtCriticalMsg || type == QtFatalMsg)
        std::cout << "\033[1;34m" << "[" << QTime::currentTime().toString("hh:mm:ss:zzz").toStdString() << "]" << "\033[1;31m" << txt.toStdString() << "\033[0m" << std::endl;
    else
        std::cout << "\033[1;34m" << "[" << QTime::currentTime().toString("hh:mm:ss:zzz").toStdString() << "]" << "\033[0m" << txt.toStdString() << std::endl;

    fpLogFile->write(("[" + QTime::currentTime().toString("hh:mm:ss:zzz") + "]" + txt + "\n").toUtf8());
    fpLogFile->flush();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Apro il file di log
    fpLogFile = new QFile(QString("qmud_log_%1.txt").arg(QDate::currentDate().toString("dd-MM-yyyy")));
    if (!fpLogFile->open(QFile::WriteOnly | QFile::Append))
    {
        std::cout << "Impossibile aprire il log file qmud_log.txt" << std::endl;
        return -1;
    }

#ifdef WIN32
    // Abilito il supporto a VT100 sulla console cmd.exe Windows
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, consoleMode);
#endif

    // Installo il message handler custom per QDebug
    qInstallMessageHandler(myMessageHandler);

    // Inizializzo la funzione rand
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
\
    // Creo il world
    CORE::World world;
    QMUD::qmudWorld = &world;

    ConfigFile config;

    if (!world.init(QDir(config.dataDirectory())))
        return -2;

    // Creo il server
    NET::Server server;

    QObject::connect(&server, SIGNAL(newConnection(NET::NetConnection*)), &world, SLOT(onConnection(NET::NetConnection*)));

    server.listen(QHostAddress::Any, config.port());

    GLOBAL_INFO(QObject::tr("In ascolto sulla porta %1").arg(server.serverPort()));

#ifndef WIN32
    // Connetto i segnali unix
    QObject::connect(new UnixSignalHandler(SIGTERM, qApp), &UnixSignalHandler::raised, qApp, &QCoreApplication::quit);
    QObject::connect(new UnixSignalHandler(SIGINT, qApp), &UnixSignalHandler::raised, qApp, &QCoreApplication::quit);
#endif

    a.exec();

    std::cout << "QMud terminato correttamente" << std::endl;

    return 0;
}
