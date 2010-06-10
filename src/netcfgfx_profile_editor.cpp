#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

#include "netcfgfx_profile_editor.h"
#include "netcfgfx_network_scanner.h"
#include "ui_netcfgfx_profile_editor.h"
#include "version.h"

//-----------------------------------------------------------------------------------------
// Name: netcfgfx_profile_editor(QWidget *parent)
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx_profile_editor::netcfgfx_profile_editor(QWidget *parent)
    : QDialog(parent), ui(new Ui::netcfgfx_profile_editor_UI)
{
    ui->setupUi(this);

    ui->hostNameLineEdit->setText(getHostname());

    connect(ui->scanPushButton, SIGNAL(clicked()), this, SLOT(getNetwork()));
    connect(ui->savePushButton, SIGNAL(clicked()), this, SLOT(saveProfile()));
    connect(ui->interfaceComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(refreshInterfaceDesc(QString)));
    connect(ui->protocolComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(protocolChanged(QString)));
    connect(ui->browsePushButton, SIGNAL(clicked()), this, SLOT(getConfigFile()));

    addInterfaceList();
}

//-----------------------------------------------------------------------------------------
// Name: netcfgfx_profile_editor(QString name,QWidget *parent)
// Desc: provided for edit profile function
//-----------------------------------------------------------------------------------------
netcfgfx_profile_editor::netcfgfx_profile_editor(QString name,QWidget *parent)
   : QDialog(parent), ui(new Ui::netcfgfx_profile_editor_UI)
{
    ui->setupUi(this);

    ui->hostNameLineEdit->setText(getHostname());

    connect(ui->scanPushButton, SIGNAL(clicked()), this, SLOT(getNetwork()));
    connect(ui->savePushButton, SIGNAL(clicked()), this, SLOT(saveProfile()));
    connect(ui->interfaceComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(refreshInterfaceDesc(QString)));
    connect(ui->protocolComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(protocolChanged(QString)));
    connect(ui->browsePushButton, SIGNAL(clicked()), this, SLOT(getConfigFile()));

    addInterfaceList();

    ui->profileNameLineEdit->setText(name);
    loadProfile(name);
}

//-----------------------------------------------------------------------------------------
// Name: ~netcfgfx_profile_editor()
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx_profile_editor::~netcfgfx_profile_editor()
{
    delete ui;
}

//-----------------------------------------------------------------------------------------
// Name: refreshInterfaceDesc(QString interface)
// Desc: refresh information of choosen interface in label
//-----------------------------------------------------------------------------------------
void netcfgfx_profile_editor::refreshInterfaceDesc(QString interface)
{
    if(isWifiInterface(interface))
    {
        ui->interfaceDescLabel->setText(tr("wireless interface"));
        ui->tabWidget->setTabEnabled(2,true);
    }
    else
    {
        ui->interfaceDescLabel->setText(tr("ethernet interface"));
        ui->tabWidget->setTabEnabled(2,false);
    }
}

//-----------------------------------------------------------------------------------------
// Name: getNetwork()
// Desc: open scan dialog
//-----------------------------------------------------------------------------------------
void netcfgfx_profile_editor::getNetwork()
{
    netcfgfx_network_scanner scan_dialog(this);
    connect(&scan_dialog,SIGNAL(networkChoosen(QString,QString)), this, SLOT(wirelessNetworkChoosen(QString,QString)));

    scan_dialog.exec();
}

//-----------------------------------------------------------------------------------------
// Name: wirelessNetworkChoosen(QString essid)
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_profile_editor::wirelessNetworkChoosen(QString essid, QString protocol)
{
    if(protocol == "-")
        ui->protocolComboBox->setCurrentIndex(ui->protocolComboBox->findText("NONE"));
    else
        ui->protocolComboBox->setCurrentIndex(ui->protocolComboBox->findText(protocol));

    ui->essidLineEdit->setText(essid);
}

//-----------------------------------------------------------------------------------------
// Name: saveProfile()
// Desc: write profile to /etc/network.d/
//-----------------------------------------------------------------------------------------
void netcfgfx_profile_editor::saveProfile()
{
    QString profileName = ui->profileNameLineEdit->text();
    if(!profileName.isEmpty())
    {
        QDir dir("/etc/network.d/");
        if(dir.exists(profileName))
            dir.remove(profileName);

        QFile file("/etc/network.d/" + profileName);

        if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QMessageBox::critical(this, tr("No write privileges"), tr("Please run %1 with root privileges to save profiles").arg("netcfgfx"),
                                  QMessageBox::Ok);
            return;
        }

        QTextStream out(&file);

        if(ui->quirksGroupBox->isChecked())
            out << "QUIRKS=(" + ui->quirksLineEdit->text() + ")" << endl;

        if(isWifiInterface(ui->interfaceComboBox->currentText()))
            out << "CONNECTION=\"wireless\"" << endl;
        else
            out << "CONNECTION=\"ethernet\"" << endl;

        out << "DESCRIPTION=\"" + ui->descriptionLineEdit->text() + "\"" << endl;
        out << "INTERFACE=" + ui->interfaceComboBox->currentText() << endl;

        if(!ui->hostNameLineEdit->text().isEmpty())
            out << "HOSTNAME=" + ui->hostNameLineEdit->text() << endl;

        if(ui->dhcpRadioButton->isChecked())
        {
            out << "IP=\"dhcp\"" << endl;
            out << "DHCP_TIMEOUT=" + QString::number(ui->timeoutSpinBox->value()) << endl;
        }
        else
        {
            out << "IP=\"static\"" << endl;
            out << "IFOPTS=\"" + ui->ipLineEdit->text() + " netmask " + ui->netmaskLineEdit->text() + " broadcast " + ui->broadcastLineEdit->text() + "\"" << endl;
            out << "GATEWAY=\"" + ui->gatewayLineEdit->text() + "\"" << endl;

            // DNS Settings
            if(!ui->domainLineEdit->text().isEmpty())
                out << "DOMAIN=" + ui->domainLineEdit->text() << endl;

            out << "DNS1=" + ui->dns1LineEdit->text() << endl;
            out << "DNS2=" + ui->dns2LineEdit->text() << endl;
        }

        // Wireless Settings
        if(isWifiInterface(ui->interfaceComboBox->currentText()))
        {
            out << "ESSID=\"" + ui->essidLineEdit->text() + "\"" << endl;
            out << "SECURITY=\"" + ui->protocolComboBox->currentText() + "\"" << endl;
            out << "KEY=\"" + ui->keyLineEdit->text().toLower() + "\"" << endl;

            if(ui->scanComboBox->currentText() == "yes")
                out << "SCAN=\"YES\"" << endl;
            else
                out << "SCAN=\"NO\"" << endl;

            out << "TIMEOUT=" + QString::number(ui->spinBox->value()) << endl;

            if(ui->iwoptsGroupBox->isChecked())
                out << "IWOPTS=\"" + ui->iwoptsLineEdit->text() + "\"" << endl;

            if(ui->protocolComboBox->currentText() == "WPA-CONFIG")
            {
                out << "WPA_CONF=\"" + ui->wpaConfLineEdit->text() + "\"" << endl;
                if(!ui->wpaOptsLineEdit->text().isEmpty())
                    out << "WPA_OPTS=\"" + ui->wpaOptsLineEdit->text() + "\"" << endl;
            }
        }

        if(ui->preupCheckBox->isChecked())
            out << "PRE_UP=\"" + ui->preupLineEdit->text() + "\"" << endl;
        if(ui->postupCheckBox->isChecked())
            out << "POST_UP=\"" + ui->postupLineEdit->text() + "\"" << endl;
        if(ui->predownCheckBox->isChecked())
            out << "PRE_DOWN=\"" + ui->predownLineEdit->text() + "\"" << endl;
        if(ui->postdownCheckBox->isChecked())
            out << "POST_DOWN=\"" + ui->postdownLineEdit->text() + "\"" << endl;

        file.close();

        QMessageBox::information(this, tr("Saved"), tr("Profile has been saved successfuly"),QMessageBox::Ok);

        emit newProfileCreated();

        close();
    }
    else
    {
        QMessageBox::information(this, tr("No profile name entered"), tr("Please enter a profile name to save the current profile"),
                                 QMessageBox::Ok);
        return;
    }
}

//-----------------------------------------------------------------------------------------
// Name: isWifiInterface(QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
bool netcfgfx_profile_editor::isWifiInterface(QString interface)
{
    QDir dir("/sys/class/net");
    dir.setFilter(QDir::AllDirs);

    if (dir.exists(interface + "/wireless"))
        return true;
    else
        return false;
}

//-----------------------------------------------------------------------------------------
// Name: validInterfaceType(QString interfaceName)
// Desc:
//-----------------------------------------------------------------------------------------
int netcfgfx_profile_editor::validInterfaceType(QString interfaceName)
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
// Name: addInterfaceList()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_profile_editor::addInterfaceList()
{
    QDir dir("/sys/class/net");
    dir.setFilter(QDir::AllDirs);
    QStringList tmpList = dir.entryList();

    for (int i=0 ; i < tmpList.size() ; i++)
    {
        if ( tmpList.at(i) != "." && tmpList.at(i) != ".." )
            if (validInterfaceType(tmpList.at(i)) == 1)
                    ui->interfaceComboBox->addItem(tmpList.at(i));
    }
}

//-----------------------------------------------------------------------------------------
// Name: getHostname()
// Desc:
//-----------------------------------------------------------------------------------------
QString netcfgfx_profile_editor::getHostname()
{
    QString tmpLine;

    QFile file("/etc/rc.conf");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";
    do
    {
        tmpLine = file.readLine();

        if(tmpLine.contains("HOSTNAME="))
            return tmpLine.remove("HOSTNAME=").remove("\"").trimmed();
    }
    while(!file.atEnd());

    return "";
}

//-----------------------------------------------------------------------------------------
// Name: loadProfile(QString name)
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_profile_editor::loadProfile(QString name)
{
    QFile file("/etc/network.d/" + name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::critical(this, tr("Couldn't read profile"), tr("Error reading profile %1").arg(name),
                                  QMessageBox::Ok);

    QTextStream in(&file);
    QString line;

    do
    {
        line = in.readLine();

        if(line.contains("QUIRKS=",Qt::CaseInsensitive))
        {
            ui->quirksGroupBox->setChecked(true);
            ui->quirksLineEdit->setText(line.remove("QUIRKS=(",Qt::CaseInsensitive).remove(")"));
        }

        if(line.toLower().contains("connection=\"wireless\"",Qt::CaseInsensitive))
            ui->tabWidget->setTabEnabled(2,true);

        if(line.toLower().contains("description=",Qt::CaseInsensitive))
            ui->descriptionLineEdit->setText(line.remove("DESCRIPTION=",Qt::CaseInsensitive).remove("\""));

        if(line.contains("INTERFACE=",Qt::CaseInsensitive))
            ui->interfaceComboBox->setCurrentIndex(ui->interfaceComboBox->findText(line.remove("INTERFACE=",Qt::CaseInsensitive).remove("\"").trimmed()));

        if(line.contains("HOSTNAME=",Qt::CaseInsensitive))
            ui->hostNameLineEdit->setText(line.remove("HOSTNAME=",Qt::CaseInsensitive).remove("\"").trimmed());

        if(line.contains("IP=\"dhcp\"",Qt::CaseInsensitive))
            ui->dhcpRadioButton->setChecked(true);

        if(line.contains("IP=\"static\"",Qt::CaseInsensitive))
        {
            ui->staticRadioButton->setChecked(true);
            ui->groupBox->setEnabled(true);
        }

        if(line.contains("IFOPTS=",Qt::CaseInsensitive))
        {
            int netmaskIndex = line.indexOf("netmask");

            ui->ipLineEdit->setText(line.mid(7, netmaskIndex - 7).remove("\"").trimmed());

            int broadcastIndex = line.indexOf("broadcast");

            ui->netmaskLineEdit->setText(line.mid(netmaskIndex + 7, broadcastIndex - (netmaskIndex + 7)).trimmed());

            int lastIndex = line.lastIndexOf("\"");

            ui->broadcastLineEdit->setText(line.mid(broadcastIndex + 9, lastIndex - (broadcastIndex + 9)).trimmed());
        }

        if(line.contains("GATEWAY=",Qt::CaseInsensitive))
            ui->gatewayLineEdit->setText(line.remove("GATEWAY=",Qt::CaseInsensitive).remove("\"").trimmed());

        if(line.contains("DHCP_TIMEOUT=",Qt::CaseInsensitive))
            ui->timeoutSpinBox->setValue(line.remove("DHCP_TIMEOUT=",Qt::CaseInsensitive).trimmed().toInt());

        if(line.contains("DOMAIN=",Qt::CaseInsensitive))
            ui->domainLineEdit->setText(line.remove("DOMAIN=",Qt::CaseInsensitive).trimmed());

        if(line.contains("DNS1=",Qt::CaseInsensitive))
            ui->dns1LineEdit->setText(line.remove("DNS1=",Qt::CaseInsensitive).trimmed());

        if(line.contains("DNS2=",Qt::CaseInsensitive))
            ui->dns2LineEdit->setText(line.remove("DNS2=",Qt::CaseInsensitive).trimmed());

        if(line.contains("ESSID=",Qt::CaseInsensitive))
            ui->essidLineEdit->setText(line.remove("ESSID=",Qt::CaseInsensitive).remove("\"").trimmed());

        if(line.toLower().contains("security=",Qt::CaseInsensitive))
            ui->protocolComboBox->setCurrentIndex(ui->protocolComboBox->findText(line.remove("SECURITY=",Qt::CaseInsensitive).remove("\"").trimmed().toUpper()));

        if(line.contains("KEY=",Qt::CaseInsensitive))
            ui->keyLineEdit->setText(line.remove("KEY=",Qt::CaseInsensitive).remove("\"").trimmed());

        if(line.contains("SCAN=\"YES\"",Qt::CaseInsensitive))
            ui->scanComboBox->setCurrentIndex(0);

        if(line.contains("SCAN=\"NO\"",Qt::CaseInsensitive))
            ui->scanComboBox->setCurrentIndex(1);

        if(line.contains("TIMEOUT=",Qt::CaseInsensitive))
            ui->spinBox->setValue(line.remove("TIMEOUT=",Qt::CaseInsensitive).trimmed().toInt());

        if(line.contains("IWOPTS=",Qt::CaseInsensitive))
        {
            ui->iwoptsGroupBox->setChecked(true);
            ui->iwoptsLineEdit->setText(line.remove("IWOPTS=",Qt::CaseInsensitive).remove("\""));
        }

        if(line.contains("WPA_CONF=",Qt::CaseInsensitive))
        {
            ui->groupBox_4->setEnabled(true);
            ui->wpaConfLineEdit->setText(line.remove("WPA_CONF=",Qt::CaseInsensitive).remove("\""));
        }

        if(line.contains("WPA_OPTS=",Qt::CaseInsensitive))
        {
            ui->wpaOptsLineEdit->setText(line.remove("WPA_OPTS=",Qt::CaseInsensitive).remove("\""));
        }

        if(line.contains("PRE_UP=",Qt::CaseInsensitive))
        {
            ui->preupCheckBox->setChecked(true);
            ui->preupLineEdit->setText(line.remove("PRE_UP=",Qt::CaseInsensitive).remove("\""));
        }

        if(line.contains("POST_UP=",Qt::CaseInsensitive))
        {
            ui->postupCheckBox->setChecked(true);
            ui->postupLineEdit->setText(line.remove("POST_UP=",Qt::CaseInsensitive).remove("\""));
        }

        if(line.contains("PRE_DOWN=",Qt::CaseInsensitive))
        {
            ui->predownCheckBox->setChecked(true);
            ui->predownLineEdit->setText(line.remove("PRE_DOWN=",Qt::CaseInsensitive).remove("\""));
        }

        if(line.contains("POST_DOWN=",Qt::CaseInsensitive))
        {
            ui->postdownCheckBox->setChecked(true);
            ui->postdownLineEdit->setText(line.remove("POST_DOWN=",Qt::CaseInsensitive).remove("\""));
        }
    }
    while(!in.atEnd());

    file.close();
}

void netcfgfx_profile_editor::getConfigFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select WPA config file"),"/etc/",tr("Config File *.conf"));
    ui->wpaConfLineEdit->setText(file);
}

void netcfgfx_profile_editor::protocolChanged(QString protocol)
{
    if(protocol == "WPA-CONFIG")
        ui->groupBox_4->setEnabled(true);
    else
        ui->groupBox_4->setEnabled(false);
}
