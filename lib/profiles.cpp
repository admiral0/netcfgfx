#include "profiles.h"
#include "config.h"
#include <QDir>
#include <QDebug>
Profiles* Profiles::self(0);
Profiles::Profiles() :
    QObject(0)
{
    profiles=new QList<Profile*>();
    watcher=new QFileSystemWatcher();
    action=new QProcess();
    QDir net(NETCFG_PROFILES_PATH);
    qDebug()<<"Searching profiles in"<<NETCFG_PROFILES_PATH;
    QFileInfoList list= net.entryInfoList(QDir::Files);
    foreach (QFileInfo file, list) {
        Profile *p=new Profile(file.absoluteFilePath());
        profiles->append(p);
        qDebug()<<"Profile:"<<p->getName();
        watcher->addPath(file.absoluteFilePath());
        connect(watcher,SIGNAL(fileChanged(QString)),this,SLOT(notifyChangedProfile(QString)));
    }
    self=this;
}
Profiles::~Profiles(){
    delete watcher;
    delete profiles;
}
void Profiles::notifyChangedProfile(QString file){
    int size=profiles->size();
    for(int i=0;i<size;++i) {
        Profile *p=profiles->at(i);
        if(p->getFilePath()==file){
            p->emitUpdated();
        }
    }

}
Profile* Profiles::getProfile(QString name){
    int size=profiles->size();
    for(int i=0;i<size;++i) {
        Profile *profile=profiles->at(i);
        if(profile->getName()==name)
            return profile;
    }
    return NULL;
}
QList<Profile*>* Profiles::getProfiles(){
    return profiles;
}
Profiles* Profiles::instance(){
	Q_ASSERT(self);
	return self;
}
void Profiles::connectProfile(Profile* p){
	qDebug()<<"Connect"<<p->getName();
	QString name=p->getName();
	actionTarget=p;
	actiondesc="connect";
	action->start("sudo",QStringList()<<"netcfg"<<name);
	connect(action,SIGNAL(finished( int, QProcess::ExitStatus)),this,SLOT(actionFinished( int, QProcess::ExitStatus )));
}
void Profiles::disconnectProfile(Profile* p){
	qDebug()<<"Disconnect"<<p->getName();
	QString name=p->getName();
	actionTarget=p;
	actiondesc="disconnect";
	action->start("sudo",QStringList()<<"netcfg"<<"-d"<<name);
	connect(action,SIGNAL(finished ( int, QProcess::ExitStatus )),this,SLOT(actionFinished ( int, QProcess::ExitStatus )));
}
void Profiles::actionFinished ( int exitCode, QProcess::ExitStatus exitStatus ){
	QString result=action->readAll();
	if(result.contains("DONE")){
		if(actiondesc=="connect"){
			actionTarget->emitConnected();
		}else if(actiondesc=="disconnect"){
			actionTarget->emitDisconnected();
		}
		emit profileChanged(actionTarget->getName(),actiondesc,result);
	}else{
		emit profileChanged(actionTarget->getName(),"error",result);
	}
	qDebug()<<"WTF?"<<result;
	action->kill();
	delete action;
	action=new QProcess();
}
