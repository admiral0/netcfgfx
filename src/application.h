#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "notifier.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int argc,char **argv);
    notifier* getNotifier();
signals:

public slots:
    void test();
private:
    notifier *tray;
};

#endif // APPLICATION_H
