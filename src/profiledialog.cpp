#include "profiledialog.h"
#include <QList>
#include "lib/profile.h"
#include "lib/util.h"
#include "lib/profiles.h"
#include <QMenu>
#include "notifier.h"

ProfileDialog::ProfileDialog(QWidget *parent) :
    QDialog(parent)
{
	/*
	 * Start re QApp
	 */
		qDebug()<<"Starting application";
	    QDBusConnection session=QDBusConnection::connectToBus(QDBusConnection::SessionBus,"session");
	    profiles=new Profiles();
	    tray=new notifier(session);
	    //dlg=new ProfileDialog();
	    context=new QMenu();
	    tray->setIcon(QIcon(":/global/netcfgfx.png"));
	    tray->show();
	    //Connect all signals
	    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayAction(QSystemTrayIcon::ActivationReason)));
	    //populateMenu();
	    qDebug()<<Util::getProfileByName(profiles->getProfiles()->at(0)->getName());
	/*
	 * End
	 */
    ui=new Ui_Profiles();
    ui->setupUi(this);
    ui->newProfile->setIcon(QIcon::fromTheme("list-add"));
    ui->removeProfile->setIcon(QIcon::fromTheme("list-remove"));
    ui->editProfile->setIcon(QIcon::fromTheme("stock_view-details"));
    // Other icon is "network-offline"
    ui->activateProfile->setIcon(QIcon::fromTheme("network-transmit-receive"));
    ui->close->setIcon(QIcon::fromTheme("window-close"));
    connect(ui->close,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->profiles,SIGNAL(itemSelectionChanged()),this,SLOT(updateButton()));
    setWindowIcon(QIcon(":/global/netcfgfx.png"));

    //Add elements to list
    populateList();
    ui->profiles->resizeColumnsToContents();
    ui->profiles->selectRow(0);

    connect(Profiles::instance(),SIGNAL(profileChanged(QString,QString,QString)),this,SLOT(notifyOnTray(QString,QString,QString)));
}
void ProfileDialog::populateList(){
    QTableWidget *w=ui->profiles;
    QList<Profile*>* profiles=this->profiles->getProfiles();
    int size=profiles->size();
    w->setRowCount(size);
    for(int i=0;i<size;i++){
        Profile *p=profiles->at(i);
        QTableWidgetItem *status;
        if(p->isProfileConnected())
        	status=new QTableWidgetItem(QIcon::fromTheme("network-transmit-receive"),"");
        else
        	status=new QTableWidgetItem(QIcon::fromTheme("network-offline"),"");
        w->setItem(i,0,status);
        QTableWidgetItem *profile=new QTableWidgetItem(p->getName());
        w->setItem(i,1,profile);
        QTableWidgetItem *interface=new QTableWidgetItem(p->getProfileProperty("INTERFACE"));
        w->setItem(i,2,interface);
        QTableWidgetItem *type=new QTableWidgetItem(p->getType());
        w->setItem(i,3,type);
    }
}
void ProfileDialog::updateButton(){
	int row=ui->profiles->currentRow();
	Profile *p;
	p=Util::getProfileByName(QString(ui->profiles->item(row,1)->text()));
	disconnect(ui->activateProfile,SIGNAL(released()),0,0);
	if(p->isProfileConnected()){
		connect(ui->activateProfile,SIGNAL(released()),p,SLOT(disconnectProfile()));
		ui->activateProfile->setIcon(QIcon::fromTheme("network-offline"));
		ui->activateProfile->setText(tr("Disconnect"));
	}else{
		connect(ui->activateProfile,SIGNAL(released()),p,SLOT(connectProfile()));
		ui->activateProfile->setIcon(QIcon::fromTheme("network-transmit-receive"));
		ui->activateProfile->setText(tr("Connect"));
	}
}
void ProfileDialog::trayAction(QSystemTrayIcon::ActivationReason reason){
    if(reason==notifier::DoubleClick)
        this->show();
    if(reason==notifier::Context){
    	qDebug()<<"Ciao";
    }
}
void ProfileDialog::notifyOnTray(QString name,QString what,QString info){
	populateList();
	updateButton();
	tray->showMessage(name,what+info,notifier::Information);
}
