#ifndef NETCFGGUI_TRAY_H
#define NETCFGGUI_TRAY_H


#include <QDialog>
#include <QAction>
#include <QMenu>
#include <QSettings>

#include "netcfgfx_notifier.h"
class QProcess;

namespace Ui
{
    class netcfgfx_profile;
}

class netcfgfx : public QDialog
{
    Q_OBJECT

public:
    netcfgfx(QWidget *parent = 0);
    ~netcfgfx();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void profileSelected();
    void editSelectedProfile();
    void newProfile();
    void removeProfile();
    void manageProfile();
    void updateProfileList();
    void connectionFinished();
    void cechkForConnectedProfiles();
    void openOptionsDialog();
    void quitActionSlot();
    void updateConnectionQuality();
    void performAutoWireless();
    void autoWirelessProcessing();
    void onLostConnectionDo(QString interface, QString profileName);
    void reloadSettings();
    void profileSelectionChanged();
    void startNetworkScanner();
    void createQuickNetworkProfile(QString ssid, QString protocol);
    void activateProfile(QString profileName, QString interface);
    void deactivateProfile(QString profileName, QString interface);
    void manageProfileAction();
    void updateContextMenu();

signals:
    void lostConnection(QString interface,QString profileName);
    void profilesChanged();

protected:
    void closeEvent(QCloseEvent* event);

private:
    void createActions();
    void createTrayIcon();
    QStringList profilesRunning();
    int validInterfaceType(QString interfaceName);
    void clearNetcfgFolder(QString interface, QString profileName);
    QString getWirelessInterfaceName();
    QString getWirelessInterfaceStatus(QString interface);
    QString getInterface(QString profileName);

    bool isProfileConnected(QString);
    bool isInterfaceActive(QString);
    bool isWirelessInterface(QString);

    QAction                 *quitAction;
    QAction                 *optionsAction;
    QAction                 *autoConnectAction;
    QAction                 *networkScanAction;
    QAction                 *seperator;         // to have a point to start updating menu and insterting new items

    QList<QAction*>         actionList;
    netcfgfx_notifier         *trayIcon;
    QMenu                   *trayIconMenu;
    QProcess                *netcfg;
    QProcess                *autoWireless;
    QTimer                  *qualityTimer;
    QString                 tmpProfileName;
    QString                 tmpInterface;
    QSettings               *settings;

    Ui::netcfgfx_profile   *ui;
};

#endif // NETCFGGUI_TRAY_H
