//=============================================================================
//
//   File : libkvitrayicon.cpp
//   Creation date : Tue Jan 02 2001 14:34:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2007 Alexey Uzhva (wizard at opendoor dot ru)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "libkvitrayicon.h"

#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviModule.h"
#include "KviLocale.h"
#include "KviMemory.h"
#include "KviWindowListBase.h"
#include "KviWindow.h"
#include "KviDynamicToolTip.h"
#include "KviIconManager.h"
#include "KviInternalCommand.h"
#include "KviConsoleWindow.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviOptions.h"
#include "KviIrcView.h"

#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QRegExp>
#include <QWidgetAction>
#include <QMenu>

#include <stdlib.h>
#include <time.h>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#define ICON_SIZE 16
	#include <Windows.h>
#else
	#define ICON_SIZE 22
#endif

extern KVIRC_API KviPointerHashTable<QString,KviWindow> * g_pGlobalWindowDict;
static KviTrayIconWidget * g_pTrayIcon = 0;

static QPixmap * g_pDock1 = 0;
static QPixmap * g_pDock2 = 0;
static QPixmap * g_pDock3 = 0;

KviTrayIconWidget::KviTrayIconWidget()
: QSystemTrayIcon(g_pMainWindow), m_CurrentPixmap(ICON_SIZE,ICON_SIZE)
{
	g_pTrayIcon = this;
    m_pContextPopup = new QMenu(0);
	setContextMenu(m_pContextPopup);

	m_iConsoles = 0;
	m_iChannels = 0;
	m_iQueries  = 0;
	m_iOther    = 0;

	m_pFlashingTimer = new QTimer(this);
	m_pFlashingTimer->setObjectName("flashing_timer");
	connect( m_pFlashingTimer, SIGNAL(timeout()), this, SLOT(flashingTimerShot()) );
	m_bFlashed=0;

	g_pMainWindow->setTrayIcon(this);

	m_pTip = new KviDynamicToolTip(g_pMainWindow,"dock_tooltip");
    m_pAwayPopup = new QMenu(0);

	m_pTitleLabel = new QLabel(__tr2qs("<b>KVIrc</b>"),m_pContextPopup);
	QPalette p;
	m_pTitleLabel->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());
	QWidgetAction * pAction = new QWidgetAction(this);
	pAction->setDefaultWidget(m_pTitleLabel);
	m_pContextPopup->addAction(pAction);

	m_pContextPopup->setWindowTitle(__tr2qs("Context"));
	m_pAwayMenuId = m_pContextPopup->addMenu(m_pAwayPopup);
	m_pAwayMenuId->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Away)));
	m_pAwayMenuId->setText(__tr2qs("Away"));

	QAction* id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Options)),__tr2qs("&Configure KVIrc..."),this,SLOT(executeInternalCommand(bool)));
	id->setData(KVI_INTERNALCOMMAND_OPTIONS_DIALOG);

	id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::KVIrc)),__tr2qs("&About KVIrc"),this,SLOT(executeInternalCommand(bool)));
	id->setData(KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);

    m_pContextPopup->addSeparator();
	m_pToggleFrame = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Raw)),__tr2qs("Hide/Show"),this,SLOT(toggleParentFrame()));

    m_pContextPopup->addSeparator();

	id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::TrayIcon)),__tr2qs("Un&dock"),this,SLOT(executeInternalCommand(bool)));
	id->setData(KVI_INTERNALCOMMAND_TRAYICON_HIDE);

	id = m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::QuitApp)),__tr2qs("&Quit"),g_pMainWindow,SLOT(close()));

	connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(fillContextPopup()));

	setIcon(*g_pDock1);

	connect(this,SIGNAL(activated ( QSystemTrayIcon::ActivationReason )),this,SLOT(activatedSlot ( QSystemTrayIcon::ActivationReason )));
}


KviTrayIconWidget::~KviTrayIconWidget()
{
	g_pTrayIcon=0;
	g_pMainWindow->setTrayIcon(0);
	delete m_pAwayPopup;
	delete m_pTitleLabel;
	delete m_pTip;
	delete m_pFlashingTimer;
	delete m_pContextPopup;
}


void KviTrayIconWidget::executeInternalCommand(bool)
{
	int iCmd;
	bool bOk;
	iCmd=(((QAction*)QObject::sender())->data()).toInt(&bOk);
	if(bOk)
		g_pMainWindow->executeInternalCommand(iCmd);
}
void KviTrayIconWidget::die()
{
	delete this;
}

void KviTrayIconWidget::flashingTimerShot()
{
	m_bFlashed=!m_bFlashed;
	refresh();
}

#define NIDLEMSGS 18

static const char * idlemsgs[NIDLEMSGS]=
{
	__tr("Nothing is happening..."),
	__tr("Just idling..."),
	__tr("Dum de dum de dum..."),
	__tr("Hey man... do something!"),
	__tr("Umpf!"),
	__tr("Silence speaking"),
	__tr("Are ya here?"),
	__tr("The world has stopped?"),
	__tr("Everything is all right"),
	__tr("idle()"),
	__tr("It's so cold here..."),
	__tr("Do not disturb... watching TV"),
	__tr("Just vegetating"),
	__tr("Hey... are ya sure that your network is up?"),
	__tr("Seems like the world has stopped spinning"),
	__tr("This silence is freaking me out!"),
	__tr("Mieeeeeowww!"),
	__tr("idle idle idle idle!")
};

bool KviTrayIconWidget::event(QEvent *e)
{
	if(e->type()==QEvent::ToolTip)
	{
		QPoint pos= g_pMainWindow->mapFromGlobal(QCursor::pos());
		QString tmp;

		KviWindowListBase * t = g_pMainWindow->windowListWidget();

		QString line;
		bool first = true;

		for(KviWindowListItem * b = t->firstItem();b;b = t->nextItem())
		{

			if(b->kviWindow()->view())
			{
				if(b->kviWindow()->view()->haveUnreadedMessages())
				{
					line = b->kviWindow()->lastMessageText();
					if(!line.isEmpty())
					{
						if(!first)tmp += "<br><br>\n";
						else first = false;

						line.replace(QChar('&'),"&amp;");
						line.replace(QChar('<'),"&lt;");
						line.replace(QChar('>'),"&gt;");
						tmp += "<b>";
						tmp += b->kviWindow()->plainTextCaption();
						tmp += "</b><br>";
						tmp += line;
					}
				}
			}
		}


		srand(time(0));

		// We use the bad way to generate random numbers :)))))

		if(tmp.isEmpty())tmp = __tr2qs_no_xgettext(idlemsgs[(int)(rand() % NIDLEMSGS)]);

		m_pTip->tip(QRect(pos,QSize(0,0)),tmp);
		return true;
	}
	return false;
}

//int KviTrayIconWidget::message(int,void *)
//{
//	qDebug("Message");
//	update();
//	return 0;
//}

void KviTrayIconWidget::doAway(bool)
{
	int id;
	bool ok;
	QAction * act = (QAction*)QObject::sender();

	if(act)
	{
		id = act->data().toInt(&ok);
		if(!ok)
			return;
	} else {
		return;
	}

	if(id<0)
	{
		KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);
		while(KviWindow * wnd = it.current())
		{
			if(wnd->type()==KviWindow::Console)
			{
				KviConsoleWindow* pConsole=(KviConsoleWindow*)wnd;
				if(pConsole->isConnected())
				{
					if(id==-2)
					{
						pConsole->connection()->sendFmtData("AWAY");
					} else {
						pConsole->connection()->sendFmtData("AWAY :%s",
							pConsole->connection()->encodeText(KVI_OPTION_STRING(KviOption_stringAwayMessage)).data()
							);
					}
				}
			}
 			++it;
		}
	} else {
		KviConsoleWindow* pConsole=g_pApp->findConsole((unsigned int)id);
		if(pConsole)
		{
			if(pConsole->isConnected())
			{
				if(pConsole->connection()->userInfo()->isAway())
				{
					pConsole->connection()->sendFmtData("AWAY");
				} else {
					pConsole->connection()->sendFmtData("AWAY :%s",
						pConsole->connection()->encodeText(KVI_OPTION_STRING(KviOption_stringAwayMessage)).data()
						);
				}
			}
		}
	}
}

void KviTrayIconWidget::fillContextPopup()
{
	m_pToggleFrame->setText(g_pMainWindow->isVisible() ? __tr2qs("Hide Window") : __tr2qs("Show Window"));
	if(g_pApp->topmostConnectedConsole())
	{
		m_pAwayMenuId->setVisible(true);
		m_pAwayPopup->clear();

		QAction * pAllAway = m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Console)),__tr2qs("Away on all"),this,SLOT(doAway(bool)));
		pAllAway->setData(-1);

		QAction * pAllUnaway = m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Console)),__tr2qs("Back on all"),this,SLOT(doAway(bool)));
		pAllUnaway->setData(-2);

		QAction* pSeparator=m_pAwayPopup->addSeparator();

		KviPointerHashTableIterator<QString,KviWindow> it(*g_pGlobalWindowDict);
		bool bAllAway=1;
		bool bAllUnaway=1;
		int iNetCount=0;
		while(KviWindow * wnd = it.current())
		{
			if(wnd->type()==KviWindow::Console)
			{
				KviConsoleWindow* pConsole=(KviConsoleWindow*)wnd;
				if(pConsole->isConnected())
				{
					QAction* id;
					if(pConsole->connection()->userInfo()->isAway())
					{
						id=m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Console)),__tr2qs("Back on %1").arg(pConsole->currentNetworkName()),this,SLOT(doAway(bool)));
						id->setData(pConsole->context()->id());
						bAllUnaway=0;
					} else {
						id=m_pAwayPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Console)),__tr2qs("Away on %1").arg(pConsole->currentNetworkName()),this,SLOT(doAway(bool)));
						id->setData(pConsole->context()->id());
						bAllAway=0;
					}
					id->setData(pConsole->context()->id());
					iNetCount++;
				}
			}
 			++it;
		}
		if(iNetCount==1)
		{
			pAllAway->setVisible(false);
			pAllUnaway->setVisible(false);
			pSeparator->setVisible(false);
		} else {
			pAllAway->setVisible(!bAllAway);
			pAllUnaway->setVisible(!bAllUnaway);
		}
	} else {
		m_pAwayMenuId->setVisible(false);
	}
}

void KviTrayIconWidget::toggleParentFrame()
{
	qDebug("TrayIcon::toggleParentFrame()");
	if(g_pMainWindow->isMinimized())
	{
		qDebug("- frame is minimized");
		g_pMainWindow->setWindowState(g_pMainWindow->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));

		if(KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized))
		{
			qDebug("- window was maximized so calling showMaximized()");
			g_pMainWindow->showMaximized();
		} else {
			qDebug("- window wasn't maximized so calling plain show()");
			g_pMainWindow->show();
		}
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		// raise it
		SetForegroundWindow((HWND)g_pMainWindow->winId());
		g_pMainWindow->activateWindow();
#endif
	} else if(!g_pMainWindow->isVisible())
	{
		qDebug("- frame is not visible");
		//restore mainwindow
		if(KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized))
		{
			qDebug("- window was maximized so calling showMaximized()");
			g_pMainWindow->showMaximized();
		} else {
			qDebug("- window wasn't maximized so calling plain show()");
			g_pMainWindow->show();
		}
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		// raise it
		SetForegroundWindow((HWND)g_pMainWindow->winId());
		g_pMainWindow->activateWindow();
#endif
	} else {
		qDebug("- frame is visible: maximized state=%d, hiding",g_pMainWindow->isMaximized());
		KVI_OPTION_BOOL(KviOption_boolFrameIsMaximized) = g_pMainWindow->isMaximized();
		g_pMainWindow->hide();
	}
}

void KviTrayIconWidget::refresh()
{
	grabActivityInfo();

	if( (m_iChannels == 2) || (m_iQueries == 2) )
	{
		if(!m_pFlashingTimer->isActive() && KVI_OPTION_BOOL(KviOption_boolEnableTrayIconFlashing) )
			m_pFlashingTimer->start(1000);
	} else {
		if(m_pFlashingTimer->isActive()) m_pFlashingTimer->stop();
		m_bFlashed=false;
	}

	m_CurrentPixmap.fill(Qt::transparent);
	QPainter thisRestrictionOfQt4IsNotNice(&m_CurrentPixmap);
	//thisRestrictionOfQt4IsNotNice.drawPixmap(0,0,22,22,*g_pDock1,0,0,22,22);

	if(m_bFlashed)
	{
		thisRestrictionOfQt4IsNotNice.drawPixmap((ICON_SIZE-16)/2,(ICON_SIZE-16)/2,16,16,*(g_pIconManager->getSmallIcon(KviIconManager::Message)),0,0,16,16);
	} else {
		thisRestrictionOfQt4IsNotNice.drawPixmap(0,0,ICON_SIZE/2,ICON_SIZE/2,
			m_iOther ?
				((m_iOther == 2) ? *g_pDock3 : *g_pDock2)
				: *g_pDock1,0,0,ICON_SIZE/2,ICON_SIZE/2);

		thisRestrictionOfQt4IsNotNice.drawPixmap(0,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,
			m_iConsoles ?
				((m_iConsoles == 2) ? *g_pDock3 : *g_pDock2)
				: *g_pDock1,0,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2);

		thisRestrictionOfQt4IsNotNice.drawPixmap(ICON_SIZE/2,0,ICON_SIZE/2,ICON_SIZE/2,
			m_iQueries ?
			((m_iQueries == 2)
			? *g_pDock3 : *g_pDock2)
			: *g_pDock1,ICON_SIZE/2,0,ICON_SIZE/2,ICON_SIZE/2);

		thisRestrictionOfQt4IsNotNice.drawPixmap(ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,
			m_iChannels ?
			((m_iChannels == 2) ? *g_pDock3 : *g_pDock2)
			: *g_pDock1
			,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2,ICON_SIZE/2);

	}
	updateIcon();
}

void KviTrayIconWidget::activatedSlot( QSystemTrayIcon::ActivationReason reason )
{
	switch(reason)
	{
		case QSystemTrayIcon::Trigger:
			// This is single click
#ifdef COMPILE_ON_MAC
			// On MacOSX one can only single-left-click the icon.
			// This activates the context menu and is quite confusing if it *also* hides the kvirc window.
			// So on mac we only _show_ the main window if it's hidden and the CloseInTray option is enabled.
			if((KVI_OPTION_BOOL(KviOption_boolCloseInTray) || KVI_OPTION_BOOL(KviOption_boolMinimizeInTray))
				&& ((!g_pMainWindow->isVisible()) || g_pMainWindow->isMinimized()))
				toggleParentFrame();
#else //!COMPILE_ON_MAC
			// on other platforms we always toggle the window
			toggleParentFrame();
#endif //!COMPILE_ON_MAC
		break;
		default:
			// we do nothing at this time
		break;
	}
}

void KviTrayIconWidget::grabActivityInfo()
{
	KviWindowListBase * t = g_pMainWindow->windowListWidget();

	if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
	{
		if(KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)>5) KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)=5;
		if(KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)>5) KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)=5;

		if(KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)<1) KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)=1;
		if(KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)<1) KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)=1;

		if(KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)<KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage))
			KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)=KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage);
	}

	m_iConsoles = 0;
	m_iChannels = 0;
	m_iQueries  = 0;
	m_iOther    = 0;

	for(KviWindowListItem * b = t->firstItem();b;b = t->nextItem())
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
		{
			unsigned int iLevel = b->highlightLevel();
			switch(b->kviWindow()->type())
			{
				case KviWindow::Console:
					if(m_iConsoles < iLevel) m_iConsoles = iLevel;
				break;
				case KviWindow::Channel:
					if(m_iChannels < iLevel) m_iChannels = iLevel;
				break;
				case KviWindow::Query:
					if(m_iQueries < iLevel) m_iQueries = iLevel;
				break;
				default:
					if(m_iOther < iLevel) m_iOther = iLevel;
				break;
			}
		} else {
			if(b->kviWindow()->view())
			{
				unsigned int iLevel=0;
				if(b->kviWindow()->view()->haveUnreadedHighlightedMessages())
				{
					iLevel=2;
				} else if(b->kviWindow()->view()->haveUnreadedMessages())
				{
					iLevel=1;
				}
				if(iLevel>0)
				switch(b->kviWindow()->type())
				{
					case KviWindow::Console:
						if(m_iConsoles < iLevel) m_iConsoles = iLevel;
					break;
					case KviWindow::Channel:
						if(m_iChannels < iLevel) m_iChannels = iLevel;
					break;
					case KviWindow::Query:
						if(m_iQueries < iLevel) m_iQueries = iLevel;
					break;
					default:
						if(m_iOther < iLevel) m_iOther = iLevel;
					break;
				}
			}
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseLevelBasedTrayNotification))
	{
		if(m_iConsoles >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iConsoles=2;
		else if(m_iConsoles >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iConsoles=1;
		else m_iConsoles=0;

		if(m_iChannels >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iChannels=2;
		else if(m_iChannels >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iChannels=1;
		else m_iChannels=0;

		if(m_iQueries >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iQueries=2;
		else if(m_iQueries >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iQueries=1;
		else m_iQueries=0;

		if(m_iOther >= KVI_OPTION_UINT(KviOption_uintMinTrayHighLevelMessage)) m_iOther=2;
		else if(m_iOther >= KVI_OPTION_UINT(KviOption_uintMinTrayLowLevelMessage)) m_iOther=1;
		else m_iOther=0;
	}
}

void KviTrayIconWidget::updateIcon()
{
	setIcon(QIcon(m_CurrentPixmap));
}

/*
	@doc: trayicon.show
	@type:
		command
	@title:
		trayicon.show
	@short:
		Shows the tray icon (dock widget)
	@keyterms:
		dock widget, system tray
	@syntax:
		trayicon.show
	@description:
		Shows the tray icon (sometimes called dock widget).[br]
		The tray icon is a small widget that docks in the window manager panel.[br]
		It shows a small kvirc icon and eventually displays four squares
		that cover this icon: the bottom left square appears when there is some new
		text in any console window, the square becomes red if the text is highlighted.[br]
		The bottom right square appears when there is some new text in any channel window,
		and it becomes red when the text is highlighted.[br] The upper right square refers to
		query windows and the upper left one to any other kind of window (dcc, links...).[br]
		If you move the mouse over the tray icon a tooltip will show you the last lines
		of the "new" text in all these windows.[br]
		This is useful when you keep the main KVIrc window minimized and you're working on something else:
		if the tray icon shows nothing but the kvirc icon, nothing is happening in the main KVIrc window.
		If the tray icon shows one or more white (or red) squares, you can move the mouse over
		and check what's happened exactly and eventually bring up the main KVIrc window by clicking on the icon.[br]
	@seealso:
		[cmd]trayicon.hide[/cmd]
*/

static bool trayicon_kvs_cmd_show(KviKvsModuleCommandCall *)
{
	if(g_pTrayIcon==0)
	{
		KviTrayIconWidget * w = new KviTrayIconWidget();
		w->show();
	}
	return true;
}

/*
	@doc: trayicon.hide
	@type:
		command
	@title:
		trayicon.hide
	@short:
		Hides the tray icon for the current frame window
	@syntax:
		trayicon.hide
	@description:
		Hides the  tray icon for the current frame window
	@seealso:
		[cmd]trayicon.show[/cmd]
*/

static bool trayicon_kvs_cmd_hide(KviKvsModuleCommandCall *)
{
	if(g_pTrayIcon)
		delete g_pTrayIcon;
	g_pTrayIcon=0;

	// show the parent frame.. otherwise there will be no way to get it back
	if(!g_pMainWindow->isVisible())
		g_pMainWindow->show();

	return true;
}

/*
	@doc: trayicon.hidewindow
	@type:
		command
	@title:
		trayicon.hidewindow
	@short:
		Hides the window, associated with trayicon
	@syntax:
		trayicon.hidewindow
	@description
		Hides the window, associated with trayicon
	@seealso:
		[cmd]trayicon.show[/cmd], [cmd]trayicon.hide[/cmd]
*/

static bool trayicon_kvs_cmd_hidewindow(KviKvsModuleCommandCall *)
{
	if(g_pMainWindow)
		g_pMainWindow->hide();

	return true;
}

/*
	@doc: trayicon.isVisible
	@type:
		function
	@title:
		$trayicon.isVisible
	@short:
		Returns the state of the dock widget
	@syntax:
		$reguser.isVisible()
	@description:
		Returns 1 if the dock widget is actually visible, 0 otherwise.
	@seealso:
		[cmd]trayicon.show[/cmd]
*/

static bool trayicon_kvs_fnc_isvisible(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setBoolean(g_pTrayIcon!=0);
	return true;
}

// =======================================
// init routine
// =======================================
static bool trayicon_module_init(KviModule * m)
{
	QString buffer;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pApp->findImage(buffer,"kvi_dock_win32-0.png");
#elif defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_MAC)
	g_pApp->findImage(buffer,"kvi_dock_mono-0.png");
#else
	g_pApp->findImage(buffer,"kvi_dock_part-0.png");
#endif
	g_pDock1 = new QPixmap(buffer);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pApp->findImage(buffer,"kvi_dock_win32-1.png");
#elif defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_MAC)
	g_pApp->findImage(buffer,"kvi_dock_mono-1.png");
#else
	g_pApp->findImage(buffer,"kvi_dock_part-1.png");
#endif
	g_pDock2 = new QPixmap(buffer);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g_pApp->findImage(buffer,"kvi_dock_win32-2.png");
#elif defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_MAC)
	g_pApp->findImage(buffer,"kvi_dock_mono-2.png");
#else
	g_pApp->findImage(buffer,"kvi_dock_part-2.png");
#endif
	g_pDock3 = new QPixmap(buffer);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"hide",trayicon_kvs_cmd_hide);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"hidewindow",trayicon_kvs_cmd_hidewindow);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"show",trayicon_kvs_cmd_show);
	KVSM_REGISTER_FUNCTION(m,"isVisible",trayicon_kvs_fnc_isvisible);

	return true;
}

static bool trayicon_module_cleanup(KviModule *)
{
	delete g_pTrayIcon;
	g_pTrayIcon = 0;

	delete g_pDock1;
	g_pDock1 = 0;

	delete g_pDock2;
	g_pDock2 = 0;

	delete g_pDock3;
	g_pDock3 = 0;

	return true;
}

static bool trayicon_module_can_unload(KviModule *)
{
	return g_pTrayIcon==0;
}

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
	"KVIrc Tray Icon Implementation",
	"4.0.0",
	"Copyright (C) 2001 Szymon Stefanek <pragma at kvirc dot net>" \
	"Copyright (C) 2007 Alexey Uzhva <alexey at kvirc dot ru>" \
	"Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>",
	"Exports the /trayicon.* interface\n",
	trayicon_module_init,
	trayicon_module_can_unload,
	0,
	trayicon_module_cleanup,
	0
)


