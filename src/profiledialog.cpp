#include "profiledialog.h"
#include <QList>
#include "application.h"
#include "lib/profile.h"

ProfileDialog::ProfileDialog(QWidget *parent) :
    QDialog(parent)
{

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

}
void ProfileDialog::populateList(){
    QTableWidget *w=ui->profiles;
    Application *app=qobject_cast<Application*> qApp;
    QList<Profile*>* profiles=app->getProfiles();
    int size=profiles->size();
    w->setRowCount(size);
    for(int i=0;i<size;i++){
        Profile *p=profiles->at(i);
        QTableWidgetItem *status;
        if(p->isConnected())
        	status=new QTableWidgetItem(QIcon::fromTheme("network-transmit-receive"),"");
        else
        	status=new QTableWidgetItem(QIcon::fromTheme("network-offline"),"");
        w->setItem(i,0,status);
        QTableWidgetItem *profile=new QTableWidgetItem(p->getName());
        w->setItem(i,1,profile);
        QTableWidgetItem *interface=new QTableWidgetItem(p->getProperty("INTERFACE"));
        w->setItem(i,2,interface);
        QTableWidgetItem *type=new QTableWidgetItem(p->getType());
        w->setItem(i,3,type);
    }
}
void ProfileDialog::updateButton(){
	Profile *p;
	QTableWidgetItem *i= ui->profiles->selectedItems().at(0);
	int row=i->row();
	p=Util::instance()->getProfileByName(QString(ui->profiles->item(row,1)->text()));
	if(p->isConnected()){
		ui->activateProfile->setIcon(QIcon::fromTheme("network-offline"));
		ui->activateProfile->setText(tr("Disconnect"));
	}else{
		ui->activateProfile->setIcon(QIcon::fromTheme("network-transmit-receive"));
		ui->activateProfile->setText(tr("Connect"));
	}
}
