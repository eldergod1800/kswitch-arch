#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "taskmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("ktodo");

    QApplication::setOrganizationName(QStringLiteral("KDE"));
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QApplication::setApplicationName(QStringLiteral("KTodo"));
    QApplication::setApplicationDisplayName(QStringLiteral("KTodo"));
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("view-pim-tasks")));

    QQmlApplicationEngine engine;

    TaskManager taskManager;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty(QStringLiteral("taskManager"), &taskManager);

    engine.load(QStringLiteral("qrc:/qml/main.qml"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
