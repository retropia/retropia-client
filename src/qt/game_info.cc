#include "game_info.h"

namespace retropia
{

    GameInfo::GameInfo(const r_game_info_t info, QObject *parent) :
        QObject(parent),
        m_info(info)
    {

    }

    GameInfo::~GameInfo(void)
    {
        r_game_info_destroy(m_info);
    }

    QString GameInfo::title(void) const
    {
        return QString(m_info->title);
    }

    QString GameInfo::platform(void) const
    {
        return QString(m_info->platform);
    }

}
