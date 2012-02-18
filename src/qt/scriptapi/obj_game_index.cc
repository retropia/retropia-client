#include "obj_game_index.h"

namespace retropia
{
    namespace scriptapi
    {
        ObjGameIndex::ObjGameIndex(GameIndex* gameIndex, QObject *parent) :
            QObject(parent),
            m_impl(gameIndex)
        {
            setObjectName("gameIndex");

            connect(m_impl, SIGNAL(onRefresh(GameInfo)), this, SLOT(onRefreshProxy(GameInfo)));
        }

        ObjGameIndex::~ObjGameIndex(void)
        {

        }

        Q_INVOKABLE void ObjGameIndex::refresh(void)
        {
            m_impl->refresh();
        }

        Q_INVOKABLE void ObjGameIndex::addSearchPath(QString path)
        {
            m_impl->addSearchPath(path);
        }

        Q_INVOKABLE void ObjGameIndex::removeSearchPath(QString path)
        {
            m_impl->removeSearchPath(path);
        }

        void ObjGameIndex::onRefreshProxy(GameInfo &info) const
        {
            ObjGameInfo objInfo(&info);

            emit onRefresh(objInfo);
        }

    }
}
