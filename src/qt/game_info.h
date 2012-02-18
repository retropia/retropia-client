#ifndef __RETROPIA__QT__GAME_INFO_H__
#define __RETROPIA__QT__GAME_INFO_H__

#include "../core/game_info.h"
#include <QtCore/QObject>

namespace retropia
{
    class GameInfo : public QObject
    {
        Q_OBJECT

        public:
            GameInfo(const r_game_info_t info, QObject *parent = 0);
            ~GameInfo(void);

            QString title(void) const;
            QString platform(void) const;

        protected:
            r_game_info_t m_info;

    };
}

#endif /* __RETROPIA__QT__GAME_INFO_H__ */
