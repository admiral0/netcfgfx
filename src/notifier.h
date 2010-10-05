#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QSystemTrayIcon>
#include "NotifyIF.h"
class notifier : public QSystemTrayIcon
{
Q_OBJECT
public:
    explicit notifier(QDBusConnection &conn,QObject *parent = 0);
    void showMessage(const QString &title, const QString &msg, MessageIcon icon=notifier::Information, int msecs=3000);
    void showMessage(const QString &title, const QString &msg, const QString &icon, int msecs=3000);
signals:

public slots:

private:
    OrgFreedesktopNotificationsInterface *notify;
};

#endif // NOTIFIER_H
