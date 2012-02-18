#ifndef __RETROPIA__QT__SCRIPTAPI__OBJ_GAME_INDEX_H__
#define __RETROPIA__QT__SCRIPTAPI__OBJ_GAME_INDEX_H__

#include "../game_index.h"
#include "obj_game_info.h"

namespace retropia
{
    namespace scriptapi
    {
        class ObjGameIndex : public QObject
        {
            Q_OBJECT

            public:
                ObjGameIndex(GameIndex *gameIndex, QObject *parent = 0);
                ~ObjGameIndex(void);

                Q_INVOKABLE void refresh(void);
                Q_INVOKABLE void addSearchPath(QString path);
                Q_INVOKABLE void removeSearchPath(QString path);

            signals:
                void onRefresh(ObjGameInfo &) const;

            protected:
                GameIndex *m_impl;

            protected slots:
                void onRefreshProxy(GameInfo &) const;

        };
    }
}

#endif /* __RETROPIA__QT__SCRIPTAPI__OBJ_GAME_INDEX_H__ */
