#ifndef __RETROPIA__QT__SETTINGS_H__
#define __RETROPIA__QT__SETTINGS_H__

#include <QtCore/QSettings>
#include <QtCore/QMap>

namespace retropia
{
    class Settings : QObject
    {
        Q_OBJECT

        public:
            Settings(const QString &organization, const QString &application);
            Settings(QSettings *qSettings);
            virtual ~Settings(void);

            virtual QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
            virtual void setValue(const QString &key, const QVariant &value);

        protected:
            void populateDefaultSettings(void);

            QSettings *m_qSettings;
            QMap<QString, QVariant> m_defaultSettings;

    };
}

#endif /* __RETROPIA__QT__SETTINGS_H__ */
