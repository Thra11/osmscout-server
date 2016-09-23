#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

/// \brief A wrapper around QSettings allowing to expose it to QML
///
class AppSettings : public QSettings
{
    Q_OBJECT
public:
    AppSettings();
    virtual ~AppSettings() {}

    Q_INVOKABLE void setValue(const QString &key, const QVariant &value)
    { QSettings::setValue(key, value); }

    Q_INVOKABLE int valueInt(const QString &key);
    Q_INVOKABLE int valueFloat(const QString &key);
    Q_INVOKABLE QString valueString(const QString &key);

    void initDefaults(); ///< Initialize settings for configurable parameters on the first start
};

#endif // APPSETTINGS_H