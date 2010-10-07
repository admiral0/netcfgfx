#ifndef PROFILES_H
#define PROFILES_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QList>
#include "profile.h"

class Profiles : public QObject
{
    Q_OBJECT
public:
    explicit Profiles();
    ~Profiles();
    Profile* getProfile(QString name);
    QList<Profile*>* getProfiles();
signals:

public slots:
    void notifyChangedProfile(QString);
private:
    QFileSystemWatcher *watcher;
    QList<Profile*> *profiles;
};

#endif // PROFILES_H
