#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtGui/QMessageBox>
#include "netcfgfx_tray.h"

//-----------------------------------------------------------------------------------------
// Name: main(int argc, char *argv[])
// Desc: entry point of the application
//-----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setQuitOnLastWindowClosed(false);
    netcfgfx netcfgfx;


    QTranslator appTranslator;
    appTranslator.load("netcfgfx_" + QLocale::system().name(),
                       qApp->applicationDirPath() + "/translations");
    app.installTranslator(&appTranslator);

    return app.exec();
}
