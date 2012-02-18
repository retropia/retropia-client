#ifndef __RETROPIA__TEST__SUPPORT__MOCKS__QT__MOCK_QSETTINGS_H__
#define __RETROPIA__TEST__SUPPORT__MOCKS__QT__MOCK_QSETTINGS_H__

#include <QtCore/QSettings>
#include <gmock/gmock.h>

namespace retropia
{

    class MockQSettings : public QSettings
    {
        Q_OBJECT

        public:
            MockQSettings(const QString &organization, const QString &application = QString(), QObject *parent = 0) :
                QSettings(organization, application, parent)
            {

            }

            MockQSettings(Scope scope, const QString &organization, const QString &application = QString(), QObject *parent = 0) :
                QSettings(scope, organization, application, parent)
            {

            }

            MockQSettings(Format format, Scope scope, const QString &organization, const QString &application = QString(), QObject *parent = 0) :
                QSettings(format, scope, organization, application, parent)
            {

            }

            MockQSettings(const QString &fileName, Format format, QObject *parent = 0) :
                QSettings(fileName, format, parent)
            {

            }

            MockQSettings(QObject *parent = 0) :
                QSettings(parent)
            {

            }

            MOCK_CONST_METHOD2(value, QVariant (const QString &key, const QVariant &defaultValue));
            MOCK_METHOD2(setValue, void (const QString &key, const QVariant &value));
    };

}

#endif /* __RETROPIA__TEST__SUPPORT__MOCKS__QT__MOCK_QSETTINGS_H__ */
