#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>
#include <QVariant>
#include <QDebug>

class Settings
{
public:
    void write(const QString&file,const QVariantMap&settings);
    void read(const QString&file,QVariantMap&settings);
    bool isNull(){return cacheSettings.isEmpty();}
    void sync();
    void setValue(QString key,QVariant value);
    QVariant value(QString key);
    QStringList childGroups();
    QStringList childKeys();
    void beginGroup(QString group){
        groupStack.push_back(group);
    }
    void endGroup(){
        if(groupStack.size()>0)
        {
            groupStack.pop_back();
        }
    }

protected:
    QVariantMap cacheSettings;

    QString fileName;
    QStringList groupStack;
};
#endif // SETTINGS_H
