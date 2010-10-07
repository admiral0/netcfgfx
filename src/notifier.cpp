#include "notifier.h"
#include <QDebug>

notifier::notifier(QDBusConnection &conn,QObject *parent) :
    QSystemTrayIcon(parent)
{
    notify=new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications","/org/freedesktop/Notifications",conn,parent);
}
void notifier::showMessage(const QString &title, const QString &msg, MessageIcon icon, int msecs){
    QString desktopicon;
    if(icon==notifier::Information){
        desktopicon="info";
    }else if(icon==notifier::Warning){
        desktopicon="important";
    }else if(icon==notifier::Critical){
        desktopicon="error";
    }else{
        desktopicon="image-missing";
    }
    //Actually notify event via DBUS
    notify->Notify("netcfgfx",0,desktopicon,title,msg,QStringList(),QVariantMap(),msecs);
}

void notifier::showMessage(const QString &title, const QString &msg, const QString &icon, int msecs){
    //Pipe to dbus notifications
    notify->Notify("netcfgfx",0,icon,title,msg,QStringList(),QVariantMap(),msecs);
}
