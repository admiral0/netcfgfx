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
