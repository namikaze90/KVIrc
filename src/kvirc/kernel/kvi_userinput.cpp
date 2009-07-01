//=============================================================================
//
//   File : kvi_userinput.cpp
//   Creation date : Sun 25 Sep 2005 05:27:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_userinput.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_console.h"
#include "kvi_kvs_script.h"
#include "kvi_locale.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_kvs_eventtriggers.h"

namespace KviUserInput
{
	bool parse(QString & szData, KviWindow * pWindow, const QString & szContext, bool bUserFriendlyCommandline)
	{
		const QChar * b = KviQString::nullTerminatedArray(szData);
		const QChar * c = b;
		if(!c)return true; // empty
		
		while(c->isSpace()) c++;

		if(!c->unicode())
			return true; // empty
		
		if(c->unicode() == '\\')
		{
			c++;
			if(c->unicode() != '/')
				c--;
		} else {
			if(c->unicode() == '/')
			{
				c++;
				if(c->unicode() != '/')
				{
					szData.remove(0, c - b);
					return parseCommand(szData,pWindow,szContext,bUserFriendlyCommandline);
				} else {
					// C++ comment, probably
					c--;
				}
			}
		}

		if(KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnTextInput,pWindow,szData))
			return true; // halted
		
		if(c != b)
			szData.remove(0,c-b);

		parseNonCommand(szData,pWindow);
		return true;
	}
	
	bool parseCommand(const QString & szData, KviWindow * pWindow, const QString & szContext, bool bUserFriendlyCommandline)
	{
		if(bUserFriendlyCommandline)
		{
			static QString szUserFriendlyCommandlineContext(__tr2qs("commandline::userfriendly"));

			QString szCmd = szData;
			// escape any -$;\%(
			szCmd.replace("\\","\\\\");
			szCmd.replace("\"","\\\"");
			szCmd.replace("$","\\$");
			szCmd.replace("%","\\%");
			szCmd.replace("(","\\(");
			szCmd.replace(";","\\;");
			szCmd.replace("-","\\-");
			szCmd.replace("+","\\+");

			KviKvsScript kvs(szContext.isEmpty() ? szUserFriendlyCommandlineContext : szContext,szCmd);
			return (kvs.run(pWindow,0,0) != KviKvsScript::Error);
		} else {
			static QString szCommandlineContext(__tr2qs("commandline::kvs"));

			KviKvsScript kvs(szContext.isEmpty() ? szCommandlineContext : szContext,szData);
			return (kvs.run(pWindow,0,0/*,KviKvsScript::AssumeLocals*/) != KviKvsScript::Error);
		}
	}
	
	void parseNonCommand(QString & szData, KviWindow * pWindow)
	{
		const QChar * aux = KviQString::nullTerminatedArray(szData);
		const QChar * beg = aux;

		if(!beg)
			return; // empty
	
		while(aux->unicode())
		{
			while(aux->unicode() && (aux->unicode() != '\n'))
				aux++;

			QString buf(beg, aux - beg);
			if(aux->unicode() == '\n')
				aux++;
			beg = aux;
	
			if(buf.isEmpty())
				buf = " "; // avoid "No text to send" (d3vah)
	
			switch(pWindow->type())
			{
				case KVI_WINDOW_TYPE_CONSOLE:
					if(pWindow->connection())
					{
						QByteArray data = pWindow->connection()->encodeText(buf);

						if(((KviConsole *)pWindow)->connection()->sendData(data.data()))
						{
							pWindow->output(KVI_OUT_RAW,"[RAW]: %Q",&buf);
							return;
						}
					}
					pWindow->output(KVI_OUT_PARSERERROR,__tr2qs("You are not connected to a server"));
				break;
				case KVI_WINDOW_TYPE_CHANNEL:
				case KVI_WINDOW_TYPE_QUERY:
					if(pWindow->connection())
					{
						if(KVI_OPTION_BOOL(KviOption_boolExitAwayOnInput))
						{
							if(pWindow->connection()->userInfo()->isAway())
								parseCommand("back",pWindow->console());
						}
					}
					pWindow->ownMessage(buf);
				break;
				case KVI_WINDOW_TYPE_DCCCHAT:
					pWindow->ownMessage(buf);
				break;
				default:
					// FIXME: Should pass the message somewhere ?.. a KviWindow handler ?
				break;
			}
		}
	}
};
