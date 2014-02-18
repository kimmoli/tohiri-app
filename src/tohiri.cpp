
#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QtQml>
#include <QScopedPointer>
#include <QQuickView>
#include <QQmlEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <QCoreApplication>
#include "tohir.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationDomain("KimmoLi");
    QCoreApplication::setOrganizationName("KimmoLi");
    QCoreApplication::setApplicationName("tohiri");
    QCoreApplication::setApplicationVersion("0.1-1");

    qmlRegisterType<TohIR>("tohiri.TohIR", 1, 0, "TohIR");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathTo("qml/tohiri.qml"));
    view->show();

    return app->exec();
}

