#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGraphicsScene>

#include "connectionhandler.h"
#include "mapquickitem.h"
#include "formloadresources.h"
#include "resourcemanager.h"

int main(int argc, char *argv[])
{
    {
        QApplication app(argc, argv);

        FormLoadResources wdg;
        wdg.show();

        ResourceManager::instance().loadResources(&wdg);

        app.exec();
    }

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QGuiApplication app(argc, argv);

    qRegisterMetaType<QMUD::StaticIdType>("QMUD::StaticIdType");
    qRegisterMetaType<QMUD::IdType>("QMUD::IdType");

    ConnectionHandler::declareQML();
    CharacterListItemModel::declareQML();
    MapQuickItem::declareQML();

    qRegisterMetaType<QMUD::IdType>("QMUD::IdType");
    qRegisterMetaType<QMUD::SexType>("QMUD::SexType");
    qRegisterMetaType<QMUD::RaceType>("QMUD::RaceType");
    qRegisterMetaType<QMUD::ClassType>("QMUD::ClassType");
    qRegisterMetaType<QMUD::DamageType>("QMUD::DamageType");

    ConnectionHandler connectionHandler;

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("connectionHandler", &connectionHandler);

    QMUD::sexTypeJsRegistration(engine);
    QMUD::raceTypeJsRegistration(engine);
    QMUD::classTypeJsRegistration(engine);
    QMUD::damageTypeJsRegistration(engine);

    connectionHandler.declareQMLContext(&engine);

    engine.load(url);

    return app.exec();
}
