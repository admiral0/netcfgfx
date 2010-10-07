#include "application.h"
#include <QTimer>
int main(int argc, char **argv){
    Application a(argc,argv);
    //Don't close app when all windows are destroyed
    Application::setQuitOnLastWindowClosed(false);
    return a.exec();
}
