#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include "profile.h"
#include <QList>

class Util : public QObject
{
    Q_OBJECT
public:
    explicit Util(QList<Profile*> *profiles,QObject *parent = 0);
    static Util* instance();
    Profile* getProfileByName(const QString name);
signals:
    void disconnectedProfile(QString profile);
    void connectedProfile(QString profile);
public slots:
    void connectProfile(QString profile);
    void disconnectProfile(QString profile);
private:
    QList<Profile*> *profiles;
    static Util *self;
};

#endif // UTIL_H
