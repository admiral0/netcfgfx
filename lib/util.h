#ifndef UTIL_H
#define UTIL_H


class Profile;
class QString;
class Util
{
public:
    static Profile* getProfileByName(const QString name);
};

#endif // UTIL_H
