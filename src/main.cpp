#include <QApplication>
#include "profiledialog.h"

int main(int argc, char **argv){
    QApplication a(argc,argv);
    QApplication::setQuitOnLastWindowClosed(false);
    a.setApplicationName("Netcfgfx");
    ProfileDialog *dlg=new ProfileDialog();
    return a.exec();
}
