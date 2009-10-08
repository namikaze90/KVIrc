//=============================================================================
//
//   File : controller.h
//   Creation date : Thu Apr 30 2002 17:13:12 GMT by Juanjo Alvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez (juanjux@yahoo.es)
//   Copyright (C) 2002-2008 Szymon Stefanek (kvirc@tin.it)
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

#include "controller.h"

#include "kvi_window.h"
#include "kvi_console.h"
#include "kvi_mirccntrl.h"
#include "kvi_app.h"
#include "kvi_options.h"

#include <QTimer>
#include <QString>
#include <QStringList>
#include <QClipboard>

extern KviPointerList<SPasteController> * g_pControllerList;

SPasteController::SPasteController(KviWindow * w,int id)
: m_pClipBuff(NULL),m_pFile(NULL),m_pId(id),m_pWindow(w)
{
	g_pControllerList->append(this);
	//m_pWindow = w;
	m_pTimer = new QTimer(this);
}

SPasteController::~SPasteController()
{
	g_pControllerList->removeRef(this);
	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
	}
	if(m_pTimer)
	{
		m_pTimer->stop();
		delete m_pTimer;
	}
	if(m_pClipBuff)
		delete m_pClipBuff;
}

bool SPasteController::pasteFileInit(QString &fileName)
{
	if(m_pClipBuff)return false; // can't paste a file while pasting the clipboard
	if(m_pFile)return false; // can't paste two files at a time
	m_pFile = new QFile(fileName);
	if(!m_pFile->open(QIODevice::ReadOnly))return false;
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(pasteFile()));
	m_pTimer->start(KVI_OPTION_UINT(KviOption_uintPasteDelay));
	return true;
}

bool SPasteController::pasteClipboardInit(void)
{
	if(m_pFile)return false; // can't paste clipboard while pasting a file
	QString tmp(g_pApp->clipboard()->text());
	if(m_pClipBuff)
	{
		(*m_pClipBuff) += tmp.isEmpty()?QStringList():tmp.split("\n",QString::KeepEmptyParts);
	} else {
		m_pClipBuff = new QStringList(tmp.isEmpty()?QStringList():tmp.split("\n",QString::KeepEmptyParts));
		m_clipBuffIterator = m_pClipBuff->begin();
	}
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(pasteClipboard()));
	m_pTimer->start(KVI_OPTION_UINT(KviOption_uintPasteDelay));
	return true;
}

void SPasteController::pasteFile(void)
{
	QString line;
	char data[1024];
	if(m_pFile->readLine(data,sizeof(data)) != -1)
	{
		line = data;
		if(line.isEmpty())
			line = QChar(KVI_TEXT_RESET);

		line.replace('\t',QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput),' ')); //expand tabs to spaces

		if(!g_pApp->windowExists(m_pWindow))
		{
			m_pFile->close();
			delete this;
		} else {
			m_pWindow->ownMessage(line.toAscii());
		}
	} else { //File finished
		m_pFile->close();
		delete this;
	}
}

void SPasteController::pasteClipboard(void)
{
	if(m_clipBuffIterator != m_pClipBuff->end())
	{
		if(!g_pApp->windowExists(m_pWindow))
		{
		  	delete this;
		} else {
			QString line;
			if((*m_clipBuffIterator).isEmpty())
			{
				line = QChar(KVI_TEXT_RESET);
			} else {
				line = *m_clipBuffIterator;
			}

			line.replace('\t',QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput),' ')); //expand tabs to spaces
			m_pWindow->ownMessage(line);
			++m_clipBuffIterator;
		}
	} else delete this;//Clipboard finished
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "controller.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES