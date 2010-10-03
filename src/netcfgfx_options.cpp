#include <QtCore/QFile>
#include <QtGui/QCloseEvent>
#include <QtCore/QSettings>
#include <QtGui/QMessageBox>

#include "netcfgfx_options.h"
#include "ui_netcfgfx_options.h"
#include "version.h"

//-----------------------------------------------------------------------------------------
// Name: netcfgfx_options(QWidget *parent)
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx_options::netcfgfx_options(QWidget *parent)
    : QDialog(parent), ui(new Ui::netcfgfx_options_UI)
{
    ui->setupUi(this);

    connect(ui->aboutQtPushButton, SIGNAL(clicked()),qApp,SLOT(aboutQt()));
    connect(ui->savePushButton, SIGNAL(clicked()), this, SLOT(saveSettings()));

    ui->label_2->setText("netcfgfx " + version + "\na GUI written with QT4 for netcfg2\nby Known,admiral0");

    loadSettings();
}

//-----------------------------------------------------------------------------------------
// Name: ~netcfgfx_options()
// Desc:
//-----------------------------------------------------------------------------------------
netcfgfx_options::~netcfgfx_options()
{
    delete ui;
}

//-----------------------------------------------------------------------------------------
// Name: closeEvent(QCloseEvebt *event)
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_options::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

//-----------------------------------------------------------------------------------------
// Name: saveSettings()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_options::saveSettings()
{
    QSettings settings(qApp->applicationDirPath() + "/settings",QSettings::NativeFormat,this);
    settings.setValue("use-sudo",ui->sudoCheckbox->isChecked());
    settings.setValue("auto-wireless",ui->autoWirelessCheckBox->isChecked());
    settings.setValue("auto-wired",ui->autoWiredCheckStartup->isChecked());
    settings.setValue("quality-refresh-rate",ui->qualityRefreshSpinBox->value());
    settings.setValue("before-exit-deactivate",ui->quitDeactivateCheckBox->isChecked());
    settings.setValue("show-auto-wireless-progress-messages",ui->showMessageCheckBox->isChecked());
    settings.setValue("signal-warning-value",ui->warningLimitSpinBox->value());
    settings.setValue("auto-connect-on-lost-connection",ui->onLostConnectionDoCheckBox->isChecked());

    QMessageBox::information(this, tr("Saved"),tr("Settings have been successfully saved"));
    emit settingsChanged();
    close();
}

//-----------------------------------------------------------------------------------------
// Name: loadSettings()
// Desc:
//-----------------------------------------------------------------------------------------
void netcfgfx_options::loadSettings()
{
    QSettings settings(qApp->applicationDirPath() + "/settings",QSettings::NativeFormat,this);
    ui->sudoCheckbox->setChecked(settings.value("use-sudo",true).toBool());
    ui->autoWirelessCheckBox->setChecked(settings.value("auto-wireless",true).toBool());
    ui->autoWiredCheckStartup->setChecked(settings.value("auto-wired",false).toBool());
    ui->qualityRefreshSpinBox->setValue(settings.value("quality-refresh-rate",30).toInt());
    ui->quitDeactivateCheckBox->setChecked(settings.value("before-exit-deactivate",false).toBool());
    ui->showMessageCheckBox->setChecked(settings.value("show-auto-wireless-progress-messages",true).toBool());
    ui->warningLimitSpinBox->setValue(settings.value("signal-warning-value",15).toInt());
    ui->onLostConnectionDoCheckBox->setChecked(settings.value("auto-connect-on-lost-connection",false).toBool());
}
