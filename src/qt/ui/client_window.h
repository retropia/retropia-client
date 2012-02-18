#ifndef __RETROPIA__UI__CLIENT_WINDOW_H__
#define __RETROPIA__UI__CLIENT_WINDOW_H__

#include <QMainWindow>
#include <QtWebKit/QWebView>

#include "../scriptapi/obj_client.h"

namespace retropia
{
    namespace ui
    {
        class ClientWindow : public QMainWindow
        {

            Q_OBJECT

            public:
                ClientWindow(QWidget *parent = 0);

            protected:
                void constructWebView(void);
                void constructMenuBar(void);

                QWebView *m_webView;
                scriptapi::ObjClient *m_scriptObjClient;

            protected slots:
                void attachJavaScriptInterface(void);
                void triggerSignIn(void);
                void triggerSignOut(void);
        };
    }
}

#endif /* __RETROPIA__UI__CLIENT_WINDOW_H__ */
