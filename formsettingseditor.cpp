#include "formsettingseditor.h"
#include "ui_formsettingseditor.h"
#include "QVBoxLayout"
#include "csettings.h"
FormSettingsEditor::FormSettingsEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingsEditor),
    settingsTree(new CSettingsTree)
{
    ui->setupUi(this);
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(settingsTree);
    setLayout(layout);

    SettingsPtr settings(new CSettings("middle.ini"));
    settingsTree->setSettingsObject(settings);
}

FormSettingsEditor::~FormSettingsEditor()
{
    delete ui;
}
