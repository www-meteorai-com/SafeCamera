// Qt lib import
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
//#include <QVersionNumber>
#include <QMessageBox>
#include <QVariant>

// JQToolsLibrary import
#include <JQToolsLibrary>

// Project lib import
#include <JQToolsManage>

// Group import
#include <WelcomeGroup>
#include <VideoGroup>
#include <CameraGroup>
#include <SearchGroup>

#include "dataface.h"

#include <QtPlugin>


int main(int argc, char *argv[])
{
    // testDataFace();
    // getVideoDir();
    QCoreApplication::addLibraryPath("./plugins");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    JQToolsManage JQToolsManage;

    JQToolsManage.setQmlApplicationEngine(&engine);


    // Group initialize
    WELCOMEGROUP_INITIALIZA;
    VIDEOGROUP_INITIALIZA;
    CAMERAGROUP_INITIALIZA;
    SEARCHGROUP_INITIALIZA;

    engine.rootContext()->setContextProperty("JQToolsManage", &JQToolsManage);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
