#include <QIcon>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>
#include <QLabel>
#include <QProcess>
#include <QTimer>
#include <QInputDialog>
#include <QCloseEvent>
#include "netcfgfx_notifier.h"
#include <QDBusConnection>
#include <iwlib.h>

#include "netcfgfx_tray.h"
#include "ui_netcfgfx_profile.h"
#include "netcfgfx_profile_editor.h"
#include "netcfgfx_options.h"
#include "netcfgfx_network_scanner.h"
#include "version.h"

//-----------------------------------------------------------------------------------------
// Name: netcfgfx(QWidget *parent)
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx::netcfgfx(QWidget *parent)
    : QDialog(parent),ui(new Ui::netcfgfx_profile)
{
    ui->setupUi(this);

    ui->treeWidget->setAlternatingRowColors(true);

    netcfg = new QProcess(this);
    autoWireless = new QProcess(this);
    autoWireless->setProcessChannelMode(QProcess::MergedChannels);
    qualityTimer = new QTimer(this);
    connect(qualityTimer, SIGNAL(timeout()), SLOT(updateConnectionQuality()));

    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(profileSelected()));
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(editSelectedProfile()));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(profileSelectionChanged()));
    connect(ui->editPushButton, SIGNAL(clicked()), this, SLOT(editSelectedProfile()));
    connect(ui->newPushButton, SIGNAL(clicked()), this, SLOT(newProfile()));
    connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeProfile()));
    connect(ui->connectPushButton, SIGNAL(clicked()), this, SLOT(manageProfile()));
    connect(netcfg, SIGNAL(finished(int)), this, SLOT(connectionFinished()));
    connect(autoWireless,SIGNAL(readyRead()), this, SLOT(autoWirelessProcessing()));
    connect(autoWireless,SIGNAL(finished(int)),this, SLOT(cechkForConnectedProfiles()));
    connect(this, SIGNAL(lostConnection(QString,QString)), this, SLOT(onLostConnectionDo(QString,QString)));
    connect(this, SIGNAL(profilesChanged()), this, SLOT(updateProfileList()));
    connect(this, SIGNAL(profilesChanged()), this, SLOT(updateContextMenu()));

    QDir tmpDir("/var/run/network/profiles");
    QStringList profileList = tmpDir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    settings = new QSettings(qApp->applicationDirPath() + "/settings",QSettings::NativeFormat,this);

    if((profileList.isEmpty()) && (settings->value("auto-wireless",true).toBool()) || settings->value("auto-wired",false).toBool())
        if(settings->value("auto-wireless",true).toBool())
            QTimer::singleShot(700, this, SLOT(performAutoWireless()));
        if(settings->value("auto-wired",false).toBool())
            QTimer::singleShot(700, this, SLOT(performAutoWired()));
    else
        QTimer::singleShot(700, this, SLOT(cechkForConnectedProfiles()));

    emit profilesChanged();
    trayIcon->show();
    trayIcon->setToolTip("netcfgfx " + version);
}

//-----------------------------------------------------------------------------------------
// Name: ~netcfgfx()
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx::~netcfgfx()
{
}

//-----------------------------------------------------------------------------------------
// Name: closeEvent(QCloseEvent* event)
// Desc: overloaded close event, for some reason it closes the whole app
//-----------------------------------------------------------------------------------------
void netcfgfx::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}

//-----------------------------------------------------------------------------------------
// Name: iconActivated(QSystemTrayIcon::ActivationReason reason)
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            if(this->isHidden())
                show();
            else
                hide();
            break;
        case QSystemTrayIcon::Context:
            //updateContextMenu();
            break;
        default:
            ;
    }
}

/**
 * Create menus
 */
void netcfgfx::createActions()
 {
     //create profile-action group
     QDir dir("/etc/network.d/");
     QStringList profileList = dir.entryList(QDir::Files);

     if(profileList.isEmpty())
     {
        QAction *action = new QAction(tr("No profile(s) found"),this);
        QFont font;
        font.setItalic(true);
        action->setFont(font);
        actionList.append(action);
     }

     for(int i=0; i<profileList.size();i++)
     {
        QString profileName = profileList.at(i);
        QString interface = getInterface(profileName);

        QAction *action = new QAction(QString("%1 (%2)").arg(profileName).arg(interface), this);
        if(isProfileConnected(profileList.at(i)))
            action->setIcon(QIcon(":/up.png"));
        else
            action->setIcon(QIcon(":/down.png"));

        action->setData(QVariant(QStringList() << profileName << interface));
        connect(action, SIGNAL(triggered()), this, SLOT(manageProfileAction()));

        actionList.append(action);
     }


     autoWirelessAction = new QAction(QIcon(":cr22-action-wireless.png"),tr("&Auto Wireless"),this);
     connect(autoWirelessAction, SIGNAL(triggered()), this, SLOT(performAutoWireless()));

     autoWiredAction = new QAction(QIcon(":cr22-action-nm_device_wired.png"),tr("&Auto Wired"),this);
     connect(autoWiredAction, SIGNAL(triggered()), this, SLOT(performAutoWired()));

     networkScanAction = new QAction(QIcon(":/wifi_radar_22x22.png"),tr("&Network Scan"),this);
     connect(networkScanAction, SIGNAL(triggered()), this, SLOT(startNetworkScanner()));

     optionsAction = new QAction(QIcon(":/configure.png"), tr("&Options"),this);
     connect(optionsAction, SIGNAL(triggered()), this, SLOT(openOptionsDialog()));

     quitAction = new QAction(QIcon(":/exit.png"),tr("&Quit"), this);
     quitAction->setMenuRole(QAction::QuitRole);
     connect(quitAction, SIGNAL(triggered()), this, SLOT(quitActionSlot()));
 }

//-----------------------------------------------------------------------------------------
// Name: createTrayIcon()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::createTrayIcon()
 {
     trayIconMenu = new QMenu;

     trayIconMenu->addAction(autoWirelessAction);
     trayIconMenu->addAction(autoWiredAction);
     trayIconMenu->addAction(networkScanAction);
     trayIconMenu->addSeparator()->setText(tr("Profile(s)"));
     trayIconMenu->addActions(actionList);
     seperator = trayIconMenu->addSeparator();
     trayIconMenu->addAction(optionsAction);
     trayIconMenu->addAction(quitAction);
     QDBusConnection session=QDBusConnection::connectToBus(QDBusConnection::SessionBus,"session");
     trayIcon = new netcfgfx_notifier(session);
     trayIcon->setContextMenu(trayIconMenu);
     trayIcon->setIcon(QIcon(":/netcfgfx_main_icon.png"));
 }

 //-----------------------------------------------------------------------------------------
// Name: updateProfileList()
// Desc: refresh tree widget items
//-----------------------------------------------------------------------------------------
 void netcfgfx::updateProfileList()
 {
    ui->treeWidget->clear();
    QDir dir("/etc/network.d/");

    QStringList profileList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    for (int i = 0; i < profileList.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, profileList.at(i));

        QDir tmpDir("/var/run/network/profiles");
        if(tmpDir.exists(profileList.at(i)))
            item->setText(1, tr("Activated"));
        else
            item->setText(1, " - ");

        QFile file("/etc/network.d/" + profileList.at(i));
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        QString line;

        do
        {
            line = in.readLine();

            if(line.contains("INTERFACE=",Qt::CaseInsensitive))
                item->setText(2, line.toLower().remove("INTERFACE=",Qt::CaseInsensitive).remove("\""));

            if(line.contains("CONNECTION=",Qt::CaseInsensitive))
                item->setText(3, line.remove("CONNECTION=",Qt::CaseInsensitive).remove("\""));
        }
        while(!in.atEnd());

        file.close();
    }

    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(3);
 }

//-----------------------------------------------------------------------------------------
// Name: profileSelected()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::profileSelected()
 {
    ui->editPushButton->setEnabled(true);
    ui->connectPushButton->setEnabled(true);
    ui->removePushButton->setEnabled(true);

    if(isProfileConnected(ui->treeWidget->currentItem()->text(0)))
        ui->connectPushButton->setText(tr("Deactivate"));
    else
        ui->connectPushButton->setText(tr("Activate  "));
 }

//-----------------------------------------------------------------------------------------
// Name: editSelectedProfile()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::editSelectedProfile()
 {
    netcfgfx_profile_editor profile(ui->treeWidget->currentItem()->text(0), this);
    profile.exec();
 }

//-----------------------------------------------------------------------------------------
// Name: newProfile()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::newProfile()
 {
    netcfgfx_profile_editor profile(this);
    connect(&profile, SIGNAL(newProfileCreated()), this, SLOT(updateProfileList()));
    connect(&profile, SIGNAL(newProfileCreated()), this, SLOT(updateContextMenu()));
    profile.exec();
 }

//-----------------------------------------------------------------------------------------
// Name: removeProfile()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::removeProfile()
 {
     QDir dir("/etc/network.d/");
    if(!dir.remove(ui->treeWidget->currentItem()->text(0)))
        QMessageBox::critical(this, tr("Error removing profile"), tr("Error removing profile %1").arg(ui->treeWidget->currentItem()->text(0)),
                                  QMessageBox::Ok);

    emit profilesChanged();
 }

//-----------------------------------------------------------------------------------------
// Name: manageProfile()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::manageProfile()
 {
    QString profileName = ui->treeWidget->currentItem()->text(0);
    QString interface = ui->treeWidget->currentItem()->text(2);



    if(isProfileConnected(profileName))
        deactivateProfile(profileName, interface);
    else
        activateProfile(profileName,interface);
 }

//-----------------------------------------------------------------------------------------
// Name: connectionFinished()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::connectionFinished()
 {
     QByteArray tmpStandart = netcfg->readAll();

     if(!tmpStandart.isEmpty())
        trayIcon->showMessage("netcfgfx " + version,tmpStandart, QSystemTrayIcon::Information, 8000);

    if(isProfileConnected(tmpProfileName) && isWirelessInterface(tmpInterface))
    {
        qualityTimer->start(settings->value("quality-refresh-rate",30).toInt()*1000);
        updateConnectionQuality();
    }

    if(!isProfileConnected(tmpProfileName) && isWirelessInterface(tmpInterface))
    {
             trayIcon->setIcon(QIcon(":/netcfgfx_main_icon.png"));
             trayIcon->setToolTip("netcfgfx " + version);
             qualityTimer->stop();
    }

    emit profilesChanged();
 }

//-----------------------------------------------------------------------------------------
// Name: isProfileConnected(QString profile)
// Desc:
//-----------------------------------------------------------------------------------------
 bool netcfgfx::isProfileConnected(QString profile)
 {
    QDir tmpDir("/var/run/network/profiles");
    if(tmpDir.exists(profile))
        return true;
    else
        return false;
 }

//-----------------------------------------------------------------------------------------
// Name: isWirelessInterface(QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
 bool netcfgfx::isWirelessInterface(QString interface)
 {
    QDir dir("/sys/class/net/" + interface);

    if(dir.exists("wireless"))
        return true;
    else
        return false;
 }

//-----------------------------------------------------------------------------------------
// Name: cechkForConnectedProfiles()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx::cechkForConnectedProfiles()
 {
    QDir tmpDir("/var/run/network/profiles");
    QStringList profileList = tmpDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    if(!profileList.isEmpty())
    {
        trayIcon->showMessage("netcfgfx " + version, tr("Profile: %1 is connected").arg(profileList.at(0)), QSystemTrayIcon::Information, 8000);
        tmpProfileName = profileList.at(0);

        // Filter interface
        QFile file("/etc/network.d/" + tmpProfileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        QString line;

        do
        {
            line = in.readLine();

            if(line.contains("INTERFACE=",Qt::CaseInsensitive))
                tmpInterface = line.remove("INTERFACE=",Qt::CaseInsensitive).remove("\"");
        }
        while(!in.atEnd());

        file.close();

        qualityTimer->start(settings->value("quality-refresh-rate",30).toInt()*1000);
        updateConnectionQuality();
        emit profilesChanged();
    }
}

//-----------------------------------------------------------------------------------------
// Name: openOptionsDialog()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::openOptionsDialog()
 {
    netcfgfx_options options;
    connect(&options, SIGNAL(settingsChanged()), this, SLOT(reloadSettings()));
    options.exec();
 }

//-----------------------------------------------------------------------------------------
// Name: quitActionSlot()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::quitActionSlot()
 {
    QDir tmpDir("/var/run/network/profiles");
    QStringList profileList = tmpDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    if(!profileList.isEmpty() && settings->value("before-exit-deactivate",true).toBool())
    {
        trayIcon->showMessage("netcfgfx " + version,tr("Disconeccting profile: %1...").arg(profileList.at(0)), QSystemTrayIcon::Information, 3000);

        netcfg->terminate();
        QString cmd("");
        QStringList opts;
        if(settings->value("use-sudo").toBool()){
            cmd="sudo";
            opts << "netcfg";
        }else{
            cmd="/usr/bin/netcfg";
        }
        netcfg->start(cmd,opts << "-d" << profileList.at(0));

        if(!netcfg->waitForFinished())
        {
            trayIcon->showMessage("netcfgfx " + version, tr("An error occured while disconnecting profile %1 with netcfg").arg(profileList.at(0)));
            return;
        }

        trayIcon->setIcon(QIcon(":/netcfgfx_main_icon.png"));
        QTimer::singleShot(3000, qApp, SLOT(quit()));
    }
    else
      qApp->quit();
 }

//-----------------------------------------------------------------------------------------
// Name: updateConnectionQuality()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::updateConnectionQuality()
 {
    iwstats stats;
    iw_range range;
    int has_range;
    int skfd = iw_sockets_open();

    has_range = (iw_get_range_info(skfd, tmpInterface.toLatin1().data(), &range) >= 0);
    if(has_range)
    {
        int exit_stat = iw_get_stats(skfd,tmpInterface.toLatin1().data(),&stats,&range,has_range);

        if(exit_stat == 0)
        {
            int quality = stats.qual.qual;

            if(quality == 0)
            {
                emit lostConnection(tmpInterface, tmpProfileName);
                return;
            }

            int minStrength = settings->value("signal-warning-value",15).toInt();
            if(quality <= minStrength)
                trayIcon->showMessage("netcfgfx " + version, tr("warning, signal strenght is under %1 %").arg(minStrength));

            else if((quality <= 20) && (quality >= 0))
                trayIcon->setIcon(QIcon(":/minimum.png"));
            else if((quality <= 40) && (quality >= 20))
                trayIcon->setIcon(QIcon(":/poor.png"));
            else if((quality <= 60) && (quality >= 40))
                trayIcon->setIcon(QIcon(":/marginal.png"));
            else if((quality <= 80) && (quality >= 60))
                trayIcon->setIcon(QIcon(":/good.png"));
            else if((quality <= 100) && (quality >= 80))
                trayIcon->setIcon(QIcon(":/excellent.png"));

            trayIcon->setToolTip(tr("Active profile: %1\nQuality:%2 %").arg(tmpProfileName).arg(quality));
        }
    }
    iw_sockets_close(skfd);
 }

//-----------------------------------------------------------------------------------------
// Name: validInterfaceType(QString interfaceName)
// Desc:
//-----------------------------------------------------------------------------------------
 int netcfgfx::validInterfaceType(QString interfaceName)
 {
    QString status = "";
    QFile file("/sys/class/net/" + interfaceName + "/type");
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return status.toInt();

        QByteArray line = file.readLine();
        status = line.trimmed();
    }

    return status.toInt();
 }

//-----------------------------------------------------------------------------------------
// Name: isInterfaceActive(QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
 bool netcfgfx::isInterfaceActive(QString interface)
 {
    QString status = "";
    QFile file("/sys/class/net/" + interface + "/operstate");
    if (file.exists())
    {
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                    return false;

            QByteArray line = file.readLine();
            status = line.trimmed();
    }

    if(status == "down")
        return false;
    else
        return true;
 }

//-----------------------------------------------------------------------------------------
// Name: getWirelessInterfaceName()
// Desc:
//-----------------------------------------------------------------------------------------
QString netcfgfx::getWirelessInterfaceName()
{
    QString wifiInterfaceName = "";
    QDir dir("/sys/class/net");
    dir.setFilter(QDir::AllDirs);
    QStringList list = dir.entryList();

    for (int i=0 ; i < list.size() ; i++)
    {
        if ( dir.exists( list.at(i) + "/wireless") )
        {
                wifiInterfaceName = list.at(i);
                break;
        }
    }

    return wifiInterfaceName;
}

//-----------------------------------------------------------------------------------------
// Name: performAutoWireless()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::performAutoWireless()
 {
     QString cmd("");
     QStringList opts;
     if(settings->value("use-sudo").toBool()){
         cmd="sudo";
         opts << "/etc/rc.d/net-auto-wireless";
     }else{
         cmd="/etc/rc.d/net-auto-wireless";
     }
    autoWireless->start(cmd, opts << "start");

    if(!autoWireless->waitForStarted())
    {
        trayIcon->showMessage("netcfgfx " + version + tr(" | auto-wireless process..."), tr("An error occured while trying to start netcfg-auto-wireless"),QSystemTrayIcon::Critical, 8000);
        return;
    }
}
 //-----------------------------------------------------------------------------------------
 // Name: performAutoWired()
 // Desc:
 //-----------------------------------------------------------------------------------------
  void netcfgfx::performAutoWired()
  {
      QString cmd("");
      QStringList opts;
      if(settings->value("use-sudo").toBool()){
          cmd="sudo";
          opts << "/etc/rc.d/net-auto-wired";
      }else{
          cmd="/etc/rc.d/net-auto-wired";
      }
     autoWireless->start(cmd, opts << "start");

     if(!autoWireless->waitForStarted())
     {
         trayIcon->showMessage("netcfgfx " + version + tr(" | auto-wired process..."), tr("An error occured while trying to start net-auto-wired"),QSystemTrayIcon::Critical, 8000);
         return;
     }
 }

//-----------------------------------------------------------------------------------------
// Name: autoWirelessProcessing()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::autoWirelessProcessing()
 {
    QByteArray tmpOutput = autoWireless->readAll();

    if(settings->value("show-auto-wireless-progress-messages",true).toBool())
    trayIcon->showMessage("netcfgfx " + version + tr(" | auto-wireless process..."), tmpOutput,QSystemTrayIcon::Information);
 }

//-----------------------------------------------------------------------------------------
// Name: onLostConnectionDo(QString interface, QString profileName)
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::onLostConnectionDo(QString interface, QString profileName)
 {
    trayIcon->setIcon(QIcon(":/netcfgfx_main_icon.png"));
    trayIcon->showMessage("netcfgfx " + version,tr("Profile: %1 lost connection to access point.").arg(profileName),QSystemTrayIcon::Information);
    qualityTimer->stop();
    clearNetcfgFolder(interface,profileName);
    trayIcon->setToolTip("netcfgfx " + version);
    emit profilesChanged();

    if(settings->value("auto-connect-on-lost-connection",false).toBool())
        performAutoWireless();
 }

//-----------------------------------------------------------------------------------------
// Name: clearNetcfgFolder(QString interface, QString profileName)
// Desc: so auto-wireless-connect can reconnect properly
//-----------------------------------------------------------------------------------------
 void netcfgfx::clearNetcfgFolder(QString interface, QString profileName)
 {
    QDir dir("/var/run/network");
    dir.remove("/profiles/" + profileName);
    dir.remove("/interfaces/" + interface);
 }

//-----------------------------------------------------------------------------------------
// Name: getWirelessInterfaceStatus(QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
QString netcfgfx::getWirelessInterfaceStatus(QString interface)
{
    QString status = "";
    QFile file("/sys/class/net/" + interface + "/operstate");
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return status;
                QByteArray line = file.readLine();
                status = line.trimmed();
    }
    return status;
}

//-----------------------------------------------------------------------------------------
// Name: reloadSettings()
// Desc: just stop and restart quality timer
//-----------------------------------------------------------------------------------------
 void netcfgfx::reloadSettings()
 {
    if(qualityTimer->isActive())
    {
        qualityTimer->stop();
        qualityTimer->start(settings->value("quality-refresh-rate",30).toInt() * 1000);
    }
 }

//-----------------------------------------------------------------------------------------
// Name: profileSelectionChanged()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::profileSelectionChanged()
 {
    if(ui->treeWidget->selectedItems().isEmpty())
    {
        ui->removePushButton->setEnabled(false);
        ui->editPushButton->setEnabled(false);
        ui->connectPushButton->setEnabled(false);
    }
 }

//-----------------------------------------------------------------------------------------
// Name: startNetworkScanner()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::startNetworkScanner()
 {
    netcfgfx_network_scanner scanner;
    connect(&scanner, SIGNAL(networkChoosen(QString,QString)), this, SLOT(createQuickNetworkProfile(QString,QString)));
    scanner.exec();
 }

//-----------------------------------------------------------------------------------------
// Name: createQuickNetworkProfile(QString ssid)
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::createQuickNetworkProfile(QString ssid, QString protocol)
 {
     if(protocol == "-")
         protocol = "NONE";
    QDir dir("/etc/network.d/");
    if(dir.exists(ssid))            // use SSID as profile name
    {
        int status = QMessageBox::warning(this,tr("Existing profile"), tr("Profile: %1 allready exists. Overwrite existing profile?").arg(ssid),
                                          QMessageBox::Yes, QMessageBox::No);
        if(status != QMessageBox::Yes)
            return;
    }

    dir.remove(ssid);

    QFile file("/etc/network.d/" + ssid);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("No write privileges"), tr("Please run netcfgfx with root privileges to save profiles"),
                              QMessageBox::Ok);
        return;
    }

    QString key = "";
    bool ok;

    if(protocol != "NONE")
        key = QInputDialog::getText(this, tr("Enter key"), tr("Please enter the matching encryption key"), QLineEdit::Normal, "", &ok);

    QTextStream out(&file);

    // write file
    out << "CONNECTION=\"wireless\"" << endl;
    out << "DESCRIPTION=\"Automatically generated profile\"" << endl;
    out << "INTERFACE=" + getWirelessInterfaceName() << endl;
    out << "IP=\"dhcp\"" << endl;
    out << "ESSID=\"" + ssid + "\"" << endl;
    out << "SECURITY=\"" + protocol + "\"" << endl;
    out << "KEY=\"" + key + "\"" << endl;

    file.close();

    emit profilesChanged();

    if(!ok)
        return;

    activateProfile(ssid,getWirelessInterfaceName());
 }

//-----------------------------------------------------------------------------------------
// Name: activateProfile(QString profileName, QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::activateProfile(QString profileName, QString interface)
 {
    QString cmd("");
    QStringList opts;
    if(settings->value("use-sudo").toBool()){
        cmd="sudo";
        opts << "netcfg";
    }else{
	cmd="/usr/bin/netcfg";
    }
    netcfg->start(cmd,opts << profileName);

    trayIcon->showMessage("netcfgfx " + version, tr("Activating profile: %1...").arg(profileName));

    if(!netcfg->waitForStarted())
    {
        QMessageBox::critical(this, tr("Error starting netcfg2"), tr("An error occured while starting netcfg2 to activate profile: %1").arg(profileName),
                                  QMessageBox::Ok);
        return;
    }

    tmpProfileName = profileName;
    tmpInterface = interface;
 }

//-----------------------------------------------------------------------------------------
// Name: deactivateProfile(QString profileName, QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::deactivateProfile(QString profileName, QString interface)
 {
    QString cmd("");
    QStringList opts;
    if(settings->value("use-sudo").toBool()){
        cmd="sudo";
        opts << "netcfg";
    }else{
        cmd="/usr/bin/netcfg";
    }

   netcfg->start(cmd,opts << "-d" << profileName);

    trayIcon->showMessage("netcfgfx " + version, tr("Deactivating profile: %1...").arg(profileName));

    if(!netcfg->waitForStarted())
    {
        QMessageBox::critical(this, tr("Error starting netcfg"), tr("An error occured while trying to start netcfg to deactivate profile %1").arg(profileName),
                                  QMessageBox::Ok);
        return;
    }

    tmpProfileName = profileName;
    tmpInterface = interface;
 }

//-----------------------------------------------------------------------------------------
// Name: getInterface(QString profileName)
// Desc:
//-----------------------------------------------------------------------------------------
 QString netcfgfx::getInterface(QString profileName)
 {
    QFile file("/etc/network.d/" + profileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString line;

    do
    {
        line = in.readLine();

        if(line.contains("INTERFACE=",Qt::CaseInsensitive))
            return line.remove("INTERFACE=",Qt::CaseInsensitive).remove("\"");
    }
    while(!in.atEnd());

    file.close();
    return "";
 }

//-----------------------------------------------------------------------------------------
// Name: manageProfileAction()
// Desc:
//-----------------------------------------------------------------------------------------
 void netcfgfx::manageProfileAction()
 {
    QAction *action = new QAction(this);
    action = dynamic_cast<QAction*>(QObject::sender());
    QStringList list = action->data().toStringList();
    QString profileName, interface;

    if(list.size() == 2)
    {
        profileName = list.at(0);
        interface = list.at(1);

         if(isProfileConnected(profileName))
            deactivateProfile(profileName, interface);
         else
            activateProfile(profileName,interface);
     }
 }

//-----------------------------------------------------------------------------------------
// Name: updateContextMenu()
// Desc: update main context menu (profiles)
//-----------------------------------------------------------------------------------------
 void netcfgfx::updateContextMenu()
 {
    for(int i=0; i < actionList.size(); i++)
        trayIconMenu->removeAction(actionList.at(i));

    actionList.clear();

     QDir dir("/etc/network.d/");
     QStringList profileList = dir.entryList(QDir::Files);

     if(profileList.isEmpty())
     {
        QAction *action = new QAction(tr("No profile(s) found"),this);
        QFont font;
        font.setItalic(true);
        action->setFont(font);
        actionList.append(action);
     }

     for(int i=0; i<profileList.size();i++)
     {
        QString profileName = profileList.at(i);
        QString interface = getInterface(profileName);

        QAction *action = new QAction(QString("%1 (%2)").arg(profileName).arg(interface), this);
        if(isProfileConnected(profileList.at(i)))
            action->setIcon(QIcon(":/up.png"));
        else
            action->setIcon(QIcon(":/down.png"));

        action->setData(QVariant(QStringList() << profileName << interface));
        connect(action, SIGNAL(triggered()), this, SLOT(manageProfileAction()));

        actionList.append(action);
     }
     trayIconMenu->insertActions(seperator,actionList);
 }

//-----------------------------------------------------------------------------------------
// Name: profilesRunning()
// Desc:
//-----------------------------------------------------------------------------------------
 QStringList netcfgfx::profilesRunning()
 {
    QDir tmpDir("/var/run/network/profiles");
    return tmpDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
 }
