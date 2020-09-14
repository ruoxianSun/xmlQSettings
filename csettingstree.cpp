#include "csettingstree.h"

#include <QtWidgets>

#include "variantdelegate.h"
#include "csettings.h"
CSettingsTree::CSettingsTree(QWidget *parent)
    : QTreeWidget(parent)
    , autoRefresh(false)
{
    setItemDelegate(new VariantDelegate(this));

    QStringList labels;
    labels << tr("Setting") << tr("Type") << tr("Value");
    setHeaderLabels(labels);
    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(2, QHeaderView::Stretch);

    refreshTimer.setInterval(2000);

    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                        QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                        QIcon::Normal, QIcon::On);
    keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    connect(&refreshTimer, &QTimer::timeout, this, &CSettingsTree::maybeRefresh);
}

CSettingsTree::~CSettingsTree()
{
}

void CSettingsTree::setSettingsObject(const SettingsPtr &newSettings)
{
    settings = newSettings;
    clear();

    if (settings.isNull()) {
        refreshTimer.stop();
    } else {
        refresh();
        if (autoRefresh)
            refreshTimer.start();
    }
}

QSize CSettingsTree::sizeHint() const
{
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    return QSize(availableGeometry.width() * 2 / 3, availableGeometry.height() * 2 / 3);
}

void CSettingsTree::setAutoRefresh(bool autoRefresh)
{
    this->autoRefresh = autoRefresh;
    if (!settings.isNull()) {
        if (autoRefresh) {
            maybeRefresh();
            refreshTimer.start();
        } else {
            refreshTimer.stop();
        }
    }
}

void CSettingsTree::setFallbacksEnabled(bool enabled)
{
    if (!settings.isNull()) {
        settings->setFallbacksEnabled(enabled);
        refresh();
    }
}

void CSettingsTree::maybeRefresh()
{
    if (state() != EditingState)
        refresh();
}

void CSettingsTree::refresh()
{
    if (settings.isNull())
        return;

    disconnect(this, &QTreeWidget::itemChanged,
               this, &CSettingsTree::updateSetting);

    settings->sync();
    updateChildItems(0);

    connect(this, &QTreeWidget::itemChanged,
            this, &CSettingsTree::updateSetting);
}

bool CSettingsTree::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        if (isActiveWindow() && autoRefresh)
            maybeRefresh();
    }
    return QTreeWidget::event(event);
}

void CSettingsTree::updateSetting(QTreeWidgetItem *item)
{
    QString key = item->text(0);
    QTreeWidgetItem *ancestor = item->parent();
    while (ancestor) {
        key.prepend(ancestor->text(0) + QLatin1Char('/'));
        ancestor = ancestor->parent();
    }
    auto rkey=key+"/value";
    settings->setValue(rkey, item->data(2, Qt::UserRole));
    if (autoRefresh)
        refresh();
}

void CSettingsTree::updateChildItems(QTreeWidgetItem *parent)
{
    int dividerIndex = 0;
    foreach (QString group, settings->childGroups()) {
        settings->beginGroup(group);
        if(settings->childGroups().size()==0)
        {
            QTreeWidgetItem *child;
            int childIndex = findChild(parent, group, 0);

            if (childIndex == -1 || childIndex >= dividerIndex) {
                if (childIndex != -1) {
                    child = childAt(parent, childIndex);
                    for (int i = 0; i < child->childCount(); ++i)
                        delete childAt(child, i);
                    moveItemForward(parent, childIndex, dividerIndex);
                } else {
                    child = createItem(group, parent, dividerIndex);
                }
                child->setIcon(0, keyIcon);
                ++dividerIndex;
            } else {
                child = childAt(parent, childIndex);
            }

            QVariant value = settings->value("value");
            if (value.type() == QVariant::Invalid) {
                child->setText(1, "Invalid");
            } else {
                child->setText(1, settings->value("comment").toString());
            }
            child->setText(2, VariantDelegate::displayText(value));
            child->setData(2, Qt::UserRole, value);
        }
        else
        {
            QTreeWidgetItem *child;
            int childIndex = findChild(parent, group, dividerIndex);
            if (childIndex != -1) {
                child = childAt(parent, childIndex);
                child->setText(1, QString());
                child->setText(2, QString());
                child->setData(2, Qt::UserRole, QVariant());
                moveItemForward(parent, childIndex, dividerIndex);
            } else {
                child = createItem(group, parent, dividerIndex);
            }
            child->setIcon(0, groupIcon);
            ++dividerIndex;
            QVariant value = settings->value("comment");
            if (value.type() == QVariant::Invalid) {
//                child->setText(1, "Invalid");
            } else {
                child->setText(1, value.toString());
            }
            updateChildItems(child);
        }
        settings->endGroup();
    }

    while (dividerIndex < childCount(parent))
        delete childAt(parent, dividerIndex);
}

QTreeWidgetItem *CSettingsTree::createItem(const QString &text,
                                          QTreeWidgetItem *parent, int index)
{
    QTreeWidgetItem *after = 0;
    if (index != 0)
        after = childAt(parent, index - 1);

    QTreeWidgetItem *item;
    if (parent)
        item = new QTreeWidgetItem(parent, after);
    else
        item = new QTreeWidgetItem(this, after);

    item->setText(0, text);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    return item;
}

QTreeWidgetItem *CSettingsTree::childAt(QTreeWidgetItem *parent, int index)
{
    if (parent)
        return parent->child(index);
    else
        return topLevelItem(index);
}

int CSettingsTree::childCount(QTreeWidgetItem *parent)
{
    if (parent)
        return parent->childCount();
    else
        return topLevelItemCount();
}

int CSettingsTree::findChild(QTreeWidgetItem *parent, const QString &text,
                            int startIndex)
{
    for (int i = startIndex; i < childCount(parent); ++i) {
        if (childAt(parent, i)->text(0) == text)
            return i;
    }
    return -1;
}

void CSettingsTree::moveItemForward(QTreeWidgetItem *parent, int oldIndex,
                                   int newIndex)
{
    for (int i = 0; i < oldIndex - newIndex; ++i)
        delete childAt(parent, newIndex);
}

