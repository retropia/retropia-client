#ifndef __RETROPIA__QT__CLIENT_H__
#define __RETROPIA__QT__CLIENT_H__

#include "game_index.h"
#include "settings.h"
#include <QApplication>

#define R_CLIENT (static_cast< ::retropia::Client * >(QCoreApplication::instance()))

namespace retropia
{
    class Client : public QApplication
    {
        Q_OBJECT

        public:
            Client(int argc, char **argv);
            ~Client(void);

            Settings *settings(void) const;
            GameIndex *gameIndex(void) const;

        protected:
            Settings *m_settings;
            GameIndex *m_gameIndex;
    };
}

#endif /* __RETROPIA__QT__CLIENT_H__ */
