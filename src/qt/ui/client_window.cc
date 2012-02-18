#include "client_window.h"
#include "../constants.h"
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QtWebKit/QWebFrame>

namespace retropia
{
    namespace ui
    {

        ClientWindow::ClientWindow(QWidget *parent) :
            QMainWindow(parent),
            m_webView(new QWebView(this)),
            m_scriptObjClient(new scriptapi::ObjClient(R_CLIENT, this))
        {
            setWindowTitle(qApp->applicationName());
            setContextMenuPolicy(Qt::NoContextMenu);

            constructMenuBar();

            constructWebView();

            setStatusBar(0);
        }

        void ClientWindow::constructWebView(void)
        {
            QWebFrame *frame;

            frame = m_webView->page()->mainFrame();

            connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachJavaScriptInterface()));

            setCentralWidget(m_webView);
        }

        void ClientWindow::constructMenuBar(void)
        {
            /* File menu */
            QMenu *fileMenu = menuBar()->addMenu("&File");

            QAction *fileMenuNewGame = new QAction("&New game", fileMenu);
            fileMenu->addAction(fileMenuNewGame);

            QAction *fileMenuSignIn = new QAction("&Sign in", fileMenu);
            connect(fileMenuSignIn, SIGNAL(triggered()), this, SLOT(triggerSignIn()));
            fileMenu->addAction(fileMenuSignIn);

            QAction *fileMenuSignOut = new QAction("&Sign out", fileMenu);
            connect(fileMenuSignOut, SIGNAL(triggered()), this, SLOT(triggerSignOut()));
            fileMenu->addAction(fileMenuSignOut);

            QAction *fileMenuExit = new QAction("&Exit", fileMenu);
            connect(fileMenuExit, SIGNAL(triggered()), qApp, SLOT(quit()));
            fileMenu->addAction(fileMenuExit);

            /* Tools menu */
            QMenu *toolsMenu = menuBar()->addMenu("&Tools");

            QAction *toolsMenuPreferences = new QAction("&Preferences", toolsMenu);
            connect(toolsMenuPreferences, SIGNAL(triggered()), this, SLOT(triggerPreferencesWindow()));
            toolsMenu->addAction(toolsMenuPreferences);

            /* Help menu */
            QMenu *helpMenu = menuBar()->addMenu("&Help");

            QAction *helpMenuHelp = new QAction("&Help", helpMenu);
            connect(helpMenuHelp, SIGNAL(triggered()), this, SLOT(triggerPreferencesWindow()));
            helpMenu->addAction(helpMenuHelp);

            QAction *helpMenuAbout = new QAction("&About", helpMenu);
            connect(helpMenuAbout, SIGNAL(triggere()), this, SLOT(triggerAboutWindow()));
            helpMenu->addAction(helpMenuAbout);
        }

        void ClientWindow::attachJavaScriptInterface(void)
        {
            m_webView->page()->mainFrame()->addToJavaScriptWindowObject("client", m_scriptObjClient);
        }

        void ClientWindow::triggerSignIn(void)
        {
            m_webView->load(QUrl(R_SIGN_IN_URL));
        }

        void ClientWindow::triggerSignOut(void)
        {
            m_webView->load(QUrl(R_SIGN_OUT_URL));
        }

    }

}
