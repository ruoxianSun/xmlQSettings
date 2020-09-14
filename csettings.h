#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QObject>
#include <QSettings>

class CSettings : public QSettings
{
    Q_OBJECT
public:
    explicit CSettings(const QString&fileName,QObject *parent = nullptr);

signals:

public slots:
};

#endif // CSETTINGS_H
