#include "application.h"
#include <QDebug>
#include <QDBusConnection>
#include "config.h"
#include "lib/profile.h"
#include "lib/util.h"

Application::Application(int argc, char **argv) : QApplication(argc,argv){
    qDebug()<<"Starting application";
    QDBusConnection session=QDBusConnection::connectToBus(QDBusConnection::SessionBus,"session");
    profiles=new Profiles();
    util=new Util(profiles->getProfiles());
    tray=new notifier(session);
    dlg=new ProfileDialog();
    context=new QMenu();
    tray->setIcon(QIcon(":/global/netcfgfx.png"));
    tray->show();
    //Connect all signals
    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayAction(QSystemTrayIcon::ActivationReason)));
    populateMenu();
}
Application::~Application(){
    delete tray;
    delete dlg;
}
QList<Profile*>* Application::getProfiles(){
    return profiles->getProfiles();
}

notifier* Application::getNotifier(){
    return tray;
}
void Application::trayAction(notifier::ActivationReason reason){
    if(reason==notifier::DoubleClick)
        dlg->show();
    if(reason==notifier::Context){
        showMenu();
    }
}
void Application::showMenu(){

}
void Application::populateMenu(){
}
