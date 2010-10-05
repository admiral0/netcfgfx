#include "application.h"
#include <QDebug>
#include <QDBusConnection>

Application::Application(int argc, char **argv) : QApplication(argc,argv){
    qDebug()<<"Starting application";
    QDBusConnection session=QDBusConnection::connectToBus(QDBusConnection::SessionBus,"session");
    tray=new notifier(session);
    tray->setIcon(QIcon::fromTheme("server"));
    tray->show();
}
notifier* Application::getNotifier(){
    return tray;
}
void Application::test(){
    Q_ASSERT(tray!=0);
    tray->showMessage(QString("afsdf"),QString("FSDFSAF"),QString("network-wireless-encrypted"));
}
