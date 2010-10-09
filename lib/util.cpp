#include "util.h"
#include "profile.h"
#include "profiles.h"
#include <QString>
Profile* Util::getProfileByName(QString name){
	Profile *p=NULL;
	int size=Profiles::instance()->getProfiles()->size();
	for(int i=0;i<size;i++){
		Profile *pt=Profiles::instance()->getProfiles()->at(i);
		if(pt->getName()==name)
			return pt;
	}
	return NULL;
}
