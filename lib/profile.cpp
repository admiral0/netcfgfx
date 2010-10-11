#include "profile.h"
#include "profiles.h"
#include "util.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QRegExp>
#include <QDir>

Profile::Profile(QString path) :
    QObject(0)
{
    this->path=path;
    vars=new QHash<QString,QString>;
    status=false;
    if(!path.isEmpty())
        populate(path,vars);
    type=vars->value("CONNECTION");
}
Profile::~Profile(){
    delete vars;
}
QString Profile::getFilePath(){
    return path;
}
QString Profile::getName(){
    return name;
}
QString Profile::getType(){
    return type;
}

QString Profile::getProfileProperty(QString key){
    return vars->value(key);
}

void Profile::emitUpdated(){
    emit updated();
}
void Profile::emitRemoved(){
    emit removed();
}
void Profile::emitConnected(){
	status=true;
	emit connected();
}
void Profile::emitDisconnected(){
	status=false;
	emit disconnected();
}
void Profile::connectProfile(){
	Profiles::instance()->connectProfile(this);
}
void Profile::disconnectProfile(){
	Profiles::instance()->disconnectProfile(this);
}
bool Profile::isProfileConnected(){
	QDir running("/var/run/network/profiles");
	if(running.entryList().contains(this->getName())){
		return true;
	}
	return false;
}
void Profile::populate(QString profile_path, QHash<QString, QString> *options){
    Q_ASSERT(options);
    qDebug()<<"Reading"<<profile_path;
    QFile profile(profile_path);
    profile.open(QFile::ReadOnly);
    QString vars=profile.readAll();
    QRegExp reg("(.+)=(.+)\\n");
    reg.setMinimal(true);
    int pos=0;
    while ((pos = reg.indexIn(vars, pos)) != -1) {
         QString key=reg.cap(1);
         QString value=reg.cap(2);
         value.replace("\"","");
         value.replace("'","");
         qDebug()<<key<<"="<<value;
         options->insert(key,value);
         pos += reg.matchedLength();
     }
    QString ex_path("/var/run/network/profiles/");
    name=QFileInfo(profile).baseName();
    ex_path.append(name);
    QFile tf(ex_path);
    if(tf.exists()){
    	status=true;
    }
}
