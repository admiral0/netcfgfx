#include "profile.h"
#include "util.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QRegExp>

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

QString Profile::getProperty(QString key){
    return vars->value(key);
}

void Profile::emitUpdated(){
    emit updated();
}
void Profile::emitRemoved(){
    emit removed();
}
bool Profile::isConnected(){
	return status;
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
