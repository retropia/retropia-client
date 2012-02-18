#include "obj_client.h"

namespace retropia
{
    namespace scriptapi
    {
        ObjClient::ObjClient(Client* client, QObject *parent) :
            QObject(parent),
            m_impl(client),
            m_objGameIndex(NULL)
        {
            setObjectName("client");

            m_objGameIndex = new ObjGameIndex(client->gameIndex(), this);
        }

        ObjClient::~ObjClient(void)
        {
            delete m_objGameIndex;
        }

        QString ObjClient::version(void) const
        {
            return m_impl->applicationVersion();
        }

    }
}
