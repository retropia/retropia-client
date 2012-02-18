#include "settings.h"

namespace retropia
{
    Settings::Settings(const QString &organization, const QString &application) :
        m_qSettings(new QSettings(QSettings::IniFormat, QSettings::UserScope, organization, application))
    {
        populateDefaultSettings();
    }

    Settings::Settings(QSettings* qSettings) :
        m_qSettings(qSettings)
    {
        populateDefaultSettings();
    }

    Settings::~Settings(void)
    {
        delete m_qSettings;
    }

    QVariant Settings::value(const QString &key, const QVariant &defaultValue)
    {
        QString path = m_qSettings->fileName();
        m_qSettings->setValue(QString("foo"), QVariant("default"));
        m_qSettings->sync();
        QVariant v = m_qSettings->value(key, defaultValue);
        QVariant d = m_defaultSettings.value(key);

        if (v.isValid() && ( !d.isValid() || v.canConvert(d.type()) ))
        {
            v.convert(d.type());
            return v;
        }
        else
        {
            if (d.isValid())
            {
                m_qSettings->setValue(key, d);
            }

            return d;
        }
    }

    void Settings::setValue(const QString& key, const QVariant& value)
    {
        m_qSettings->setValue(key, value);
    }


    void Settings::populateDefaultSettings(void)
    {
        m_defaultSettings.insert("core/db", QVariant("./client.db"));
        m_defaultSettings.insert("core/nthreads", QVariant(4));
    }

}
