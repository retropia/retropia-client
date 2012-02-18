#ifndef __RETROPIA__QT__SCRIPTAPI__OBJ_CLIENT_H__
#define __RETROPIA__QT__SCRIPTAPI__OBJ_CLIENT_H__

#include <QObject>
#include "obj_game_index.h"
#include "../client.h"

namespace retropia
{
    namespace scriptapi
    {
        class ObjClient : public QObject
        {
            Q_OBJECT

            Q_PROPERTY(QString version READ version);

            public:
                ObjClient(Client *client, QObject *parent = 0);
                ~ObjClient(void);

            protected:
                QString version(void) const;

                ObjGameIndex *m_objGameIndex;
                Client *m_impl;

        };
    }
}

#endif /* __RETROPIA__QT__SCRIPTAPI__OBJ_CLIENT_H__ */
