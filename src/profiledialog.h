#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>

#include "ui_profiles.h"

class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfileDialog(QWidget *parent = 0);

signals:

public slots:
	void updateButton();
private:
    void populateList();
    Ui_Profiles *ui;
};

#endif // PROFILEDIALOG_H
