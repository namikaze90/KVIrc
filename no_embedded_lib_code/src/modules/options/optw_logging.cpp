//=============================================================================
//
//   File : optw_logging.cpp
//   Creation date : Mon Oct 29 15:37:16 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_logging.h"

#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>


KviLoggingOptionsWidget::KviLoggingOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("logging_options_widget");

	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("Automatically Log","options"));
	addBoolSelector(g,__tr2qs_ctx("Query windows","options"),KviOption_boolAutoLogQueries);
	addBoolSelector(g,__tr2qs_ctx("Channel windows","options"),KviOption_boolAutoLogChannels);
	addBoolSelector(g,__tr2qs_ctx("DCC Chat windows","options"),KviOption_boolAutoLogDccChat);
	addBoolSelector(g,__tr2qs_ctx("Console windows","options"),KviOption_boolAutoLogConsole);
#ifdef COMPILE_ZLIB_SUPPORT
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Gzip logs","options"),KviOption_boolGzipLogs); 
#endif
	addBoolSelector(0,2,0,2,__tr2qs_ctx("Strip message type numbers in logs","options"),KviOption_boolStripMsgTypeInLogs);
	addBoolSelector(0,3,0,3,__tr2qs_ctx("Strip colors in logs","options"),KviOption_boolStripControlCodesInLogs);
	KviUIntSelector* us = addUIntSelector(0,4,0,4,__tr2qs_ctx("Auto flush logs every","options"),KviOption_uintAutoFlushLogs,0,99999,0);
	us->setSuffix(__tr2qs_ctx(" min","options"));
	mergeTip(us,
		__tr2qs_ctx("<center>Save logs with the current interval<br>" \
			"Set to 0 to disable this feature</center>","options"));
	addRowSpacer(0,5,0,5);
}

KviLoggingOptionsWidget::~KviLoggingOptionsWidget()
{
}



#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_logging.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES