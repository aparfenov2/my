#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "OscilImpl.h"
#include "DMEButtonVM.h"
#include "AppModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<OscilImpl>("MyLib", 1, 0, "OscilImpl");
//    qmlRegisterType<AppModel>("MyLib", 1, 0, "AppModel");
//    qmlRegisterType<DMEModel>();

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/myviqt22/main.qml"));

//    AppModel appModel;
//    viewer.rootContext()->setContextProperty("appModel",&appModel);
    viewer.showExpanded();

    return app.exec();
}
