#ifndef NETCFGGUI_OPTIONS_H
#define NETCFGGUI_OPTIONS_H

#include <QtGui/QDialog>

namespace Ui
{
    class netcfgfx_options_UI;
}

class netcfgfx_options : public QDialog
{
    Q_OBJECT

public:
    netcfgfx_options(QWidget *parent = 0);
    ~netcfgfx_options();

private slots:
    void saveSettings();
    void loadSettings();

signals:
    void settingsChanged();

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::netcfgfx_options_UI            *ui;
};

#endif // NETCFGGUI_OPTIONS_H
