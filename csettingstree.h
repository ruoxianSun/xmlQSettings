#ifndef CSETTINGSTREE_H
#define CSETTINGSTREE_H

#include <QObject>
#include <QTreeWidget>
#include "csettings.h"
#include <QWidget>
#include <QTimer>
class CSettingsTree : public QTreeWidget
{
    Q_OBJECT

public:
    typedef QSharedPointer<CSettings> SettingsPtr;

    CSettingsTree(QWidget *parent = 0);
    ~CSettingsTree();

    void setSettingsObject(const SettingsPtr &settings);
    QSize sizeHint() const override;

public slots:
    void setAutoRefresh(bool autoRefresh);
    void setFallbacksEnabled(bool enabled);
    void maybeRefresh();
    void refresh();

protected:
    bool event(QEvent *event) override;

private slots:
    void updateSetting(QTreeWidgetItem *item);

private:
    void updateChildItems(QTreeWidgetItem *parent);
    QTreeWidgetItem *createItem(const QString &text, QTreeWidgetItem *parent,
                                int index);
    QTreeWidgetItem *childAt(QTreeWidgetItem *parent, int index);
    int childCount(QTreeWidgetItem *parent);
    int findChild(QTreeWidgetItem *parent, const QString &text, int startIndex);
    void moveItemForward(QTreeWidgetItem *parent, int oldIndex, int newIndex);

    SettingsPtr settings;
    QTimer refreshTimer;
    bool autoRefresh;
    QIcon groupIcon;
    QIcon keyIcon;
};

#endif // CSETTINGSTREE_H
