#ifndef FORMSETTINGSEDITOR_H
#define FORMSETTINGSEDITOR_H

#include <QWidget>
#include "csettingstree.h"
namespace Ui {
class FormSettingsEditor;
}

class FormSettingsEditor : public QWidget
{
    Q_OBJECT

public:
    typedef QSharedPointer<CSettings> SettingsPtr;
    explicit FormSettingsEditor(QWidget *parent = nullptr);
    ~FormSettingsEditor();

private:
    Ui::FormSettingsEditor *ui;
    CSettingsTree*settingsTree;
};

#endif // FORMSETTINGSEDITOR_H
