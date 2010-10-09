#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "notifier.h"
#include "ui_profiles.h"
class Profiles;
class QMenu;
class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfileDialog(QWidget *parent = 0);

signals:

public slots:
	void updateButton();
	void trayAction(QSystemTrayIcon::ActivationReason);
private:
    void populateList();
    Ui_Profiles *ui;
    Profiles *profiles;
    notifier *tray;
    QMenu *context;
};

#endif // PROFILEDIALOG_H
