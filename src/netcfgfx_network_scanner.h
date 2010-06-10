#ifndef NETCFG_NETWORK_SCANNER_H
#define NETCFG_NETWORK_SCANNER_H

#include <QtGui/QDialog>
#include <QtGui/QTreeWidgetItem>
#include <QList>
#include <QProcess>

namespace Ui
{
    class netcfgfx_network_scanner_UI;
}

class netcfgfx_network_scanner : public QDialog
{
    Q_OBJECT

public:
    netcfgfx_network_scanner(QWidget *parent = 0);
    ~netcfgfx_network_scanner();

signals:
    void networkChoosen(QString,QString);

private slots:
    void scanNetwork();
    void networkSelected();
    void networkItemClicked(QTreeWidgetItem*,int);
    void ifconfigFinished();
    void iwlistFinished();

private:
    QString getWirelessInterfaceName();
    QString getWirelessInterfaceStatus(QString interface);
    void parseIwlist(QString buffer);

    QProcess                                *ifconfig;
    QProcess                                *iwlist;
    Ui::netcfgfx_network_scanner_UI        *ui;
};

#endif // NETCFG_NETWORK_SCANNER_H
