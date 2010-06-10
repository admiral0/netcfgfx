#ifndef NETCFGGUI_PROFILE_EDITOR_H
#define NETCFGGUI_PROFILE_EDITOR_H

#include <QtGui/QDialog>

namespace Ui
{
    class netcfgfx_profile_editor_UI;
}

class netcfgfx_profile_editor : public QDialog
{
    Q_OBJECT

public:
    netcfgfx_profile_editor(QWidget *parent = 0);
    netcfgfx_profile_editor(QString name,QWidget *parent = 0);
    ~netcfgfx_profile_editor();

signals:
    void newProfileCreated();

private slots:
    void getNetwork();
    void wirelessNetworkChoosen(QString,QString);
    void saveProfile();
    void refreshInterfaceDesc(QString);
    void getConfigFile();
    void protocolChanged(QString);

private:
    Ui::netcfgfx_profile_editor_UI *ui;

    void addInterfaceList();
    int validInterfaceType(QString);
    bool isWifiInterface(QString);
    QString getHostname();
    void loadProfile(QString);

    QString                     profileName;
    QString                     description;
    QString                     interface;
    QString                     hostName;
    QString                     command;
};

#endif // NETCFGGUI_PROFILE_EDITOR_H
