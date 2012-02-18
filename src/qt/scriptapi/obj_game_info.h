#ifndef __RETROPIA__QT__SCRIPTAPI__OBJ_GAME_INFO_H__
#define __RETROPIA__QT__SCRIPTAPI__OBJ_GAME_INFO_H__

#include "../game_index.h"

namespace retropia
{
    namespace scriptapi
    {
        class ObjGameInfo : public QObject
        {
            Q_OBJECT

            Q_PROPERTY(QString title READ title);
            Q_PROPERTY(QString platform READ platform);

            public:
                ObjGameInfo(GameInfo *gameInfo, QObject *parent = 0);
                ~ObjGameInfo(void);

                QString title(void) const;
                QString platform(void) const;


            protected:
                GameInfo *m_impl;

        };
    }
}

#endif /* __RETROPIA__QT__SCRIPTAPI__OBJ_GAME_INFO_H__ */
