#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "settings.h"
#include "mock_qsettings.h"
#include <QtCore/QVariant>
#include <QtCore/QDebug>

using testing::Return;
using testing::_;
using testing::Eq;

namespace retropia
{
    class TestSettings : public testing::Test
    {
        protected:
            TestSettings() :
                m_mockQSettings(NULL),
                m_settings(NULL)
            {
                m_mockQSettings = new MockQSettings("A.C.M.E", "Thinga-Ma-Bob");
                m_settings = new Settings(m_mockQSettings);
            }

            virtual ~TestSettings()
            {
                delete m_settings; // m_settings owns m_mockQSettings
            }

        MockQSettings *m_mockQSettings;
        Settings *m_settings;

    };

    TEST_F(TestSettings, value_sets_and_returns_an_internal_default) {
        QString key("core/db");
        QVariant invalid;

        EXPECT_CALL(*m_mockQSettings, value(Eq(key), Eq(invalid))).Times(1).WillOnce(Return(invalid));
        EXPECT_CALL(*m_mockQSettings, setValue(Eq(key), _)).Times(1);

        m_settings->value(key);
    }

    TEST_F(TestSettings, value_returns_a_provided_default) {
        QString key("core/db");
        QVariant def("foo");

        EXPECT_CALL(*m_mockQSettings, value(Eq(key), Eq(def))).Times(1).WillOnce(Return(def));

        ASSERT_EQ(def, m_settings->value(key, def));

    }

}

