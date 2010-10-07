#include "util.h"
Util* Util::self(0);
Util::Util(QList<Profile*> *profiles, QObject *parent) :
    QObject(parent)
{
    this->profiles=profiles;
    self=this;
}
void Util::connectProfile(QString profile){
    int size=profiles->size();
    /* STUB */
}
void Util::disconnectProfile(QString profile){
    int size=profiles->size();
    /* STUB */
}
Util* Util::instance(){
    Q_ASSERT(self);
    return self;
}
Profile* Util::getProfileByName(QString name){
	Profile *p=NULL;
	int size=profiles->size();
	for(int i=0;i<size;i++){
		Profile *pt=profiles->at(i);
		if(pt->getName()==name)
			p=pt;
	}
	return p;
}
