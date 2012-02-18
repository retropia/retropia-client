#include "obj_game_info.h"

namespace retropia
{
    namespace scriptapi
    {
        ObjGameInfo::ObjGameInfo(GameInfo* gameInfo, QObject* parent) :
            QObject(parent),
            m_impl(gameInfo)
        {
            setObjectName("game_info");
        }

        ObjGameInfo::~ObjGameInfo(void)
        {
        }

        QString ObjGameInfo::title(void) const
        {
            return m_impl->title();
        }

        QString ObjGameInfo::platform(void) const
        {
            return m_impl->platform();
        }

    }
}

