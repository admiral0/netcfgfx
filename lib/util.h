#ifndef UTIL_H
#define UTIL_H


class Profile;
class QString;
class QProcess;
class Util
{
public:
    static Profile* getProfileByName(const QString name);
    static QProcess* connectProfileByName(const QString name);
};

#endif // UTIL_H
