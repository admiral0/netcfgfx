#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>
#include <QHash>

class Profile : public QObject
{
    Q_OBJECT
public:
    explicit Profile(QString path = "");
    ~Profile();
    QString getFilePath();
    QString getName();
    QString getType();
    QString getProfileProperty(QString key);
    bool isProfileConnected();
    /*!
      populates QHash with variables from config file
     */
    void populate(QString profile_path,QHash<QString,QString> *options);

signals:
    //void connected();
    //void disconnected();
    void updated();
    void removed();

public slots:
    void emitUpdated();
    void emitRemoved();
private:
    QString name;
    QString path;
    QString type;
    bool status;
    QHash<QString,QString> *vars;
};

#endif // PROFILE_H
