#include "notifier.h"
#include <QDebug>

notifier::notifier(QDBusConnection &conn,QObject *parent) :
    QSystemTrayIcon(parent)
{
    notify=new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications","/org/freedesktop/Notifications",conn,parent);
}
void notifier::showMessage(const QString &title, const QString &msg, MessageIcon icon, int msecs){

    notify->Notify("netcfgfx",0,"server",title,msg,QStringList(),QVariantMap(),msecs);
}

void notifier::showMessage(const QString &title, const QString &msg, const QString &icon, int msecs){

    notify->Notify("netcfgfx",0,icon,title,msg,QStringList(),QVariantMap(),msecs);
}
