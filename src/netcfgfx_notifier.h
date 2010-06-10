#ifndef NETCFGFX_NOTIFIER_H
#define NETCFGFX_NOTIFIER_H

#include <QSystemTrayIcon>
#include "netcfgfx_notifyiface.h"
class netcfgfx_notifier : public QSystemTrayIcon
{
Q_OBJECT
public:
    explicit netcfgfx_notifier(QDBusConnection &conn,QObject *parent = 0);
    void showMessage(const QString &title, const QString &msg, MessageIcon icon=netcfgfx_notifier::Information, int msecs=3000);

signals:

public slots:

private:
    OrgFreedesktopNotificationsInterface *notify;
};

#endif // NETCFGFX_NOTIFIER_H
