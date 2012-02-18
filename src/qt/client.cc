#include "client.h"
#include "../config.h"

namespace retropia
{

    Client::Client(int argc, char **argv) :
        QApplication(argc, argv),
        m_settings(NULL),
        m_gameIndex(NULL)
    {
        setApplicationName(PACKAGE_NAME);
        setApplicationVersion(PACKAGE_VERSION);
        setOrganizationName(PACKAGE_ORGANIZATION);
        setOrganizationDomain(PACKAGE_ORGANIZATION_DOMAIN);

        m_settings = new Settings(organizationName(), applicationName());
        m_gameIndex = new GameIndex(m_settings->value("core/db").toString(), m_settings->value("core/nthreads").toUInt());
    }

    Client::~Client(void)
    {
        delete m_gameIndex;
        delete m_settings;
    }

    Settings *Client::settings(void) const
    {
        return m_settings;
    }

    GameIndex *Client::gameIndex(void) const
    {
        return m_gameIndex;
    }

}
