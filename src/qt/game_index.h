#ifndef __RETROPIA__QT__GAME_INDEX_H__
#define __RETROPIA__QT__GAME_INDEX_H__

#include "../core/game_index.h"
#include "game_info.h"
#include <QtCore/QObject>

namespace retropia
{
    class GameIndex : public QObject
    {
        Q_OBJECT

        public:
            GameIndex(const QString &fileName, unsigned int nThreads, QObject *parent = 0);
            ~GameIndex(void);

            void addSearchPath(const QString &path);
            void removeSearchPath(const QString &path);
            bool refresh(void);

        signals:
            void onRefresh(const GameInfo &) const;

        protected:
            void emitOnRefresh(const GameInfo &info) const;
            static void onRefreshCallback(const r_game_info_t info, void *obj);


            r_game_index_t m_gameIndex;

    };
}

#endif /* __RETROPIA__QT__GAME_INDEX_H__ */
