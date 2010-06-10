#include "netcfgfx_notifier.h"


netcfgfx_notifier::netcfgfx_notifier(QDBusConnection &conn,QObject *parent) :
    QSystemTrayIcon(parent)
{
    notify=new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications","/",conn,parent);
}
void netcfgfx_notifier::showMessage(const QString &title, const QString &msg, MessageIcon icon, int msecs){
    notify->Notify("netcfgfx",0,"",title,msg,QStringList(),QVariantMap(),msecs);
}
