#ifndef PROFILES_H
#define PROFILES_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QList>
#include "profile.h"
#include <QProcess>
class Profiles : public QObject
{
    Q_OBJECT
public:
    explicit Profiles();
    ~Profiles();
    static Profiles* instance();
    Profile* getProfile(QString name);
    QList<Profile*>* getProfiles();
    void connectProfile(Profile *p);
    void disconnectProfile(Profile *p);
signals:
	void profileChanged(QString name,QString what,QString info);
public slots:
    void notifyChangedProfile(QString);
    void actionFinished ( int exitCode, QProcess::ExitStatus exitStatus );
private:
    QFileSystemWatcher *watcher;
    QList<Profile*> *profiles;
    static Profiles* self;
    QProcess* action;
    QString actiondesc;
    Profile* actionTarget;

};

#endif // PROFILES_H
