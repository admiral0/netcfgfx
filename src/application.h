#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QMenu>
#include "notifier.h"
#include "profiledialog.h"
#include "lib/profiles.h"
#include "lib/util.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int argc,char **argv);
    ~Application();
    notifier* getNotifier();
    QList<Profile*>* getProfiles();
signals:

public slots:
    void trayAction(QSystemTrayIcon::ActivationReason reason);
private:
    void showMenu();
    void populateMenu();
    QMenu *context;
    notifier *tray;
    Util* util;
    ProfileDialog *dlg;
    Profiles *profiles;
};

#endif // APPLICATION_H
