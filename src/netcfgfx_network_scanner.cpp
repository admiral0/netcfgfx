#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QSettings>
//#include <iostream>

#include "netcfgfx_network_scanner.h"
#include "ui_netcfgfx_network_scanner.h"

//-----------------------------------------------------------------------------------------
// Name: netcfgfx_network_scanner(QWidget *parent)
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx_network_scanner::netcfgfx_network_scanner(QWidget *parent)
    : QDialog(parent), ui(new Ui::netcfgfx_network_scanner_UI)
{
    ui->setupUi(this);

    ifconfig = new QProcess(this);
    ifconfig->setProcessChannelMode(QProcess::MergedChannels);

    iwlist = new QProcess(this);
    iwlist->setProcessChannelMode(QProcess::MergedChannels);

    connect(ui->refreshPushButton,SIGNAL(clicked()), this, SLOT(scanNetwork()));
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                        this, SLOT(networkSelected()));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                        this, SLOT(networkItemClicked(QTreeWidgetItem*,int)));
    connect(ui->choosePushButton, SIGNAL(clicked()), this, SLOT(networkSelected()));
    connect(ifconfig, SIGNAL(finished(int)), this, SLOT(ifconfigFinished()));
    connect(iwlist, SIGNAL(finished(int)), this, SLOT(iwlistFinished()));

    scanNetwork();
}

//-----------------------------------------------------------------------------------------
// Name: ~netcfgfx_network_scanner()
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx_network_scanner::~netcfgfx_network_scanner()
{
    delete ui;
}

//-----------------------------------------------------------------------------------------
// Name: scanNetwork()
// Desc: initial scan process + refresh tree widget
//-----------------------------------------------------------------------------------------
void netcfgfx_network_scanner::scanNetwork()
{
    QSettings settings(qApp->applicationDirPath() + "/settings",QSettings::NativeFormat,this);

    QString wirelessInterface = getWirelessInterfaceName();
    QString wirelessInterfaceStatus = getWirelessInterfaceStatus(wirelessInterface);

    if(wirelessInterfaceStatus == "down")
    {
         ifconfig->start("/sbin/ifconfig",QStringList() << wirelessInterface << "up");

        if(!ifconfig->waitForFinished())
        {
            ui->errorLabel->setText(tr("couldn't start ifconfig to activate interface: %1").arg(wirelessInterface));
            return;
        }
    }

    ui->treeWidget->clear();
    QString cmd("");
    QStringList opts;
    if(settings.value("use-sudo").toBool()){
        cmd="sudo";
        opts << "iwlist";
    }else{
        cmd="/usr/sbin/iwlist";
    }
    iwlist->start(cmd, opts << wirelessInterface << "scan");

    if(!iwlist->waitForStarted())
    {
        ui->errorLabel->setText(tr("error starting iwlist process"));
        return;
    }

    // disable GUI elements for user
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->treeWidget->setDisabled(true);
    ui->cancelPushButton->setDisabled(true);
    ui->refreshPushButton->setDisabled(true);
    ui->choosePushButton->setDisabled(true);
}

//-----------------------------------------------------------------------------------------
// Name: networkItemClicked(QTreeWidgetItem*,int)
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_network_scanner::networkItemClicked(QTreeWidgetItem*,int)
{
    ui->choosePushButton->setDisabled(false);
}

//-----------------------------------------------------------------------------------------
// Name: networkSelected()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_network_scanner::networkSelected()
{
    QString networkESSID, protocol;

    networkESSID = ui->treeWidget->currentItem()->text(0);
    protocol = ui->treeWidget->currentItem()->text(4);

    emit networkChoosen(networkESSID, protocol);

    close();
}

//-----------------------------------------------------------------------------------------
// Name: getWirelessInterfaceName()
// Desc:
//-----------------------------------------------------------------------------------------
QString netcfgfx_network_scanner::getWirelessInterfaceName()
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
// Name: getWirelessInterfaceStatus(QString interface)
// Desc:
//-----------------------------------------------------------------------------------------
QString netcfgfx_network_scanner::getWirelessInterfaceStatus(QString interface)
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
// Name: ifconfigFinished()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_network_scanner::ifconfigFinished()
{
    QByteArray output = ifconfig->readAll();
    if(output.contains("Permission denied"))
        ui->errorLabel->setText(tr("couldn' activate wireless interface for scanning, permission denied"));
}

//-----------------------------------------------------------------------------------------
// Name: iwlistFinished()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_network_scanner::iwlistFinished()
{
    QTextStream stream(iwlist->readAll().data());
    QString buffer = "";
    QString line;
    int cellCount = 0;

    do {

            line = stream.readLine().trimmed();

            if ( line.startsWith("Cell") )
            {
                    if (cellCount > 0)
                            buffer = buffer + "<<#>>"; // cell change

                    cellCount ++;
            }

            if (line.size() > 0)
                    buffer = buffer + line + "<<>>"; // line change

    } while (!line.isNull());

    // Enable GUI elements for user after scanning
    ui->treeWidget->setEnabled(true);
    ui->cancelPushButton->setDisabled(false);
    ui->refreshPushButton->setDisabled(false);
    QApplication::restoreOverrideCursor();

    parseIwlist(buffer);
}

//-----------------------------------------------------------------------------------------
// Name: parseIwlist(QString buffer)
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_network_scanner::parseIwlist(QString buffer)
{
    QStringList bufferLines = buffer.split("<<#>>");
    QString line;

    if (bufferLines.size() > 0)
    {
        if ( bufferLines.at(0).contains("No scan results") )
        {
            ui->errorLabel->setText("No scan results");
            return;
        }
    }

    for (int i=0 ; i < bufferLines.size() ; i++)
    {
            QStringList infoLines = bufferLines.at(i).split("<<>>");
            QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget);
            itm->setText(4,"WEP");
            QString toolTip;

            for (int j=0 ; j < infoLines.size() ; j++)
            {
                    line = infoLines.at(j);

                    if ( line.startsWith ( "Cell" ) && line.contains( "Address:" ) )
                    {
                        QStringList tmp = line.split("Address:");
                        toolTip.append("Address: " + tmp.at(tmp.size()-1).trimmed());
                    }
                    else if((!line.isEmpty()) && (!line.contains("completed")) && (!line.contains("IE: Unknown")))
                        toolTip.append("\n" + line);

                    if ( line.startsWith ( "ESSID:" ) )
                    {
                        QString ssid =line.mid ( line.indexOf ( "\"" ) + 1, line.lastIndexOf ( "\"" ) - line.indexOf ( "\"" ) - 1 );
                        itm->setText(0,ssid);
                        if ((ssid=="") || (ssid==" "))
                                itm->setText(0,tr("(hidden cell)"));
                    }

                    if ( line.startsWith ( "Encryption key:on" ) )
                           itm->setIcon(0, QIcon(":/lock.png"));
                    if ( line.startsWith ( "Encryption key:off" ) )
                           itm->setText(4, "-");

                    if ( line.startsWith ( "Quality:" ) )
                            itm->setText(1,line.mid ( line.indexOf (":") + 1, line.indexOf ("/") - line.indexOf (":") - 1 ));

                    if ( line.startsWith ( "Quality=" ) )
                            itm->setText(1, line.mid (line.indexOf ("=") + 1, line.indexOf ("/") - line.indexOf ("=") - 1 ));

                    if ( line.startsWith("Channel:") )
                            itm->setText(2,line.mid ( line.indexOf (":") + 1, line.size()));

                    if ( line.startsWith("Mode:") )
                            itm->setText(3, line.remove("Mode:",Qt::CaseInsensitive));

                    if ( line.startsWith ( "IE:" ) )
                    {
                        if ( line.toUpper().contains("WPA", Qt::CaseInsensitive) )
                            itm->setText(4,"WPA");
                    }
            }
            itm->setToolTip(0,toolTip);
            itm->setToolTip(1,toolTip);
            itm->setToolTip(2,toolTip);
            itm->setToolTip(3,toolTip);
            itm->setToolTip(4,toolTip);
    }

    ui->treeWidget->sortByColumn(1, Qt::DescendingOrder);
    ui->treeWidget->resizeColumnToContents ( 0 );
    ui->treeWidget->resizeColumnToContents ( 1 );
    ui->treeWidget->resizeColumnToContents ( 2 );
    ui->treeWidget->resizeColumnToContents ( 3 );
    ui->treeWidget->resizeColumnToContents ( 4 );
}
