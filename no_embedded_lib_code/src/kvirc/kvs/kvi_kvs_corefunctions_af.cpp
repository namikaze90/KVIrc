//=============================================================================
//
//   File : kvi_kvs_corefunctions_af.cpp
//   Creation date : Fri 31 Oct 2003 01:52:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_kvs_corefunctions.h"
#include "kvi_kvs_kernel.h"
#include "kvi_kvs_object_controller.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_channel.h"
#include "kvi_console.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_mirccntrl.h"
#include "kvi_avatar.h"
#include "kvi_ircuserdb.h"
#include "kvi_time.h"
#include "kvi_frame.h"

#include <QStatusBar>

namespace KviKvsCoreFunctions
{
	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: active
		@type:
			function
		@title:
			$active
		@short:
			Retrieves the window ID of the active window
		@syntax:
			<integer> $active[(<irc context id:integer>)]
		@description:
			Returns the [b]window ID[/b] of the active window
			bound to the IRC context specified by <irc context id>.
			If no window matches the specified IRC context, and invalid
			window ID is returned (0).[br]
			If no <irc context id> is specified, then
			the application active window is returned (the window
			that currently has the input focus). Note that in this
			case the returned window may also belong to another IRC
			context or be not bound to any IRC context at all.
			In some extreme cases you may even get a window that
			has no output widget and thus has its output redirected.
			Using the "global" active window should be used only
			for communicating something REALLY urgent (and maybe
			unrelated to a specific IRC connection) to the user.
		@seealso:
			[fnc]$window[/fnc]
	*/

	KVSCF(active)
	{
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("context_id",KVS_PT_UINT,KVS_PF_OPTIONAL,uContextId)
		KVSCF_PARAMETERS_END

		KviWindow * wnd;
		KviConsole * cons;
		if(KVSCF_pParams->count() > 0)
		{
			cons = g_pApp->findConsole(uContextId);
			if(cons)wnd = cons->activeWindow();
			else wnd = 0;
		} else {
			wnd = g_pActiveWindow;
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(wnd ? wnd->numericId() : 0));
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: ascii
		@type:
			function
		@title:
			$ascii
		@short:
			Returns the UNICODE code of a character
		@syntax:
			<variant> $ascii(<char:string>)
		@description:
			This function has been renamed to $unicode and is present
			only for backward compatibility.
		@seealso:
			[fnc]$cr[/fnc], [fnc]$lf[/fnc], [fnc]$char[/fnc]
	*/

	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: asciiToBase64
		@type:
			function
		@title:
			$asciiToBase64
		@short:
			Returns an encoded base64 string
		@syntax:
			$asciiToBase64(<ascii_string>)
		@description:
			Encodes an ASCII string to its base64 encoded rappresentation
			Please note that since KVS is UNICODE based, this function
			will first encode the string in UTF8 and then base64-encode.
			This means that it is substantially only 7bit safe (ASCII codes below 128).
		@examples:
			[cmd]echo[/cmd] $asciiToBase64("Hello!")
		@seealso:
			[fnc]$base64toascii[/fnc]
	*/

	KVSCF(asciiToBase64)
	{
		QString szAscii;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("ascii_string",KVS_PT_STRING,0,szAscii)
		KVSCF_PARAMETERS_END

		KviStr tmp1(szAscii);
		if(tmp1.len() > 0)
		{
			KviStr tmp2;
			tmp2.bufferToBase64(tmp1.ptr(),tmp1.len());
			KVSCF_pRetBuffer->setString(QString(tmp2.ptr()));
		} else {
			KVSCF_pRetBuffer->setString(QString());
		}
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: asciiToHex
		@type:
			function
		@title:
			$asciiToHex
		@short:
			Returns an encoded hex string
		@syntax:
			<string> $asciiToHex(<ascii_string:string>)
		@description:
			Encodes an ASCII string to its hex encoded rappresentation.
			Please note that since KVS is UNICODE based, this function
			will first encode the string in UTF8 and then hex-encode.
			This means that it is substantially only 7bit safe (ASCII codes below 128).
		@examples:
			[cmd]echo[/cmd] $asciiToHex("Hello!")
		@seealso:
			[fnc]$hextoascii[/fnc]
	*/

	KVSCF(asciiToHex)
	{
		QString szAscii;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("ascii_string",KVS_PT_STRING,0,szAscii)
		KVSCF_PARAMETERS_END

		KviStr tmp1(szAscii);
		if(tmp1.len() > 0)
		{
			KviStr tmp2;
			tmp2.bufferToHex(tmp1.ptr(),tmp1.len());
			KVSCF_pRetBuffer->setString(QString(tmp2.ptr()));
		} else {
			KVSCF_pRetBuffer->setString(QString());
		}
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: array
		@type:
			function
		@title:
			$array
		@short:
			Explicitly creates an array
		@syntax:
			<array> $array(<item:variant>,<item:variant>,<item:variant>,....);
		@description:
			Returns an array with the specified items. The items are indexed starting from 0.
			This is just an explicit way of creating an array with a defined set of items,
			useful for increasing readability.
		@examples:
			[example]
				[cmd]alias[/cmd](test) {
					[cmd]return[/cmd] $array(1,2,3);
				}
				%x = $test();
				[cmd]foreach[/cmd](%y,%x) {
					[cmd]echo[/cmd] %y;
				}
			[/example]
		@seealso:
			[fnc]$hash[/fnc]
	*/

	KVSCF(array)
	{
		KviKvsArray * a = new KviKvsArray();
		kvs_int_t idx = 0;

		for(KviKvsVariant * v = KVSCF_pParams->first();v;v = KVSCF_pParams->next())
		{
			a->set(idx,new KviKvsVariant(*v));
			idx++;
		}

		KVSCF_pRetBuffer->setArray(a);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: away
		@type:
			function
		@title:
			$away
		@short:
			Returns true if the current user is away
		@syntax:
			<boolean> $away
		@description:
			Returns true if the current user is away, else false.
			If the current IRC context is not connected at all, this function returns false.
	*/

	KVSCF(away)
	{
		kvs_uint_t uCntx;

		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("irc_context_id",KVS_PT_UINT,KVS_PF_OPTIONAL,uCntx)
		KVSCF_PARAMETERS_END

		KviConsole * cns;

		if(KVSCF_pParams->count() > 0)
		{
			cns = g_pApp->findConsole(uCntx);
			if(cns)
			{
				if(cns->context()->isConnected())
					KVSCF_pRetBuffer->setBoolean(cns->connection()->userInfo()->isAway());
				else
					KVSCF_pRetBuffer->setNothing();
			} else {
				KVSCF_pRetBuffer->setNothing();
			}
		} else {
			if(KVSCF_pContext->window()->console())
			{
				cns = KVSCF_pContext->window()->console();
				if(cns->context()->isConnected())
					KVSCF_pRetBuffer->setBoolean(cns->connection()->userInfo()->isAway());
				else
					KVSCF_pRetBuffer->setNothing();
			} else {
				KVSCF_pContext->warning(__tr2qs_ctx("This window has no associated IRC context","kvs"));
				KVSCF_pRetBuffer->setNothing();
			}
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: b
		@type:
			function
		@title:
			$b
		@short:
			Returns the BOLD mIRC control character
		@syntax:
			<string> $b
		@description:
			Returns the BOLD mIRC control character (CTRL+B).[br]
		@seealso:
			[fnc]$k[/fnc], [fnc]$u[/fnc]
	*/

	KVSCF(b)
	{
		KVSCF_pRetBuffer->setString(QString(QChar(KVI_TEXT_BOLD)));
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: base64toAscii
		@type:
			function
		@title:
			$base64ToAscii
		@short:
			Returns a decoded base64 string
		@syntax:
			<string> $base64ToAscii(<base_64_encoded_string:string>)
		@description:
			Decodes a base64 encoded string.
		@seealso:
			[fnc]$asciiToBase64[/fnc]
	*/

	KVSCF(base64ToAscii)
	{
		QString szBase64;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("base64_encoded_string",KVS_PT_STRING,0,szBase64)
		KVSCF_PARAMETERS_END

		KviStr tmp1(szBase64);
		char * buf;
		int len = tmp1.base64ToBuffer(&buf,true);
		QString szRet(buf);
		szRet.truncate(len);
		KVSCF_pRetBuffer->setString(szRet);
		KviStr::freeBuffer(buf);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: boolean
		@type:
			function
		@title:
			$boolean
		@short:
			Casts a variable to a boolean
		@syntax:
			<integer> $boolean(<data:variant>)
		@description:
			Forces <data> to be a boolean data type by first casting
			to integer (see [fnc]$int[/fnc]()) and then comparing the result against zero.
			A zero integer will result in a false value while a non-zero one
			will result in a true value.
			This function is similar to the C++ (bool) cast and is internally
			aliased to [fnc]$bool[/fnc] too.
			Note that since KVIrc does most of the casting work automatically
			you shouldn't need to use this function.
		@seealso:
			[fnc]$real[/fnc]
			[fnc]$integer[/fnc]
	*/

	/*
		@doc: bool
		@type:
			function
		@title:
			$bool
		@short:
			Casts a variable to a boolean
		@syntax:
			<integer> $bool(<data:variant>)
		@description:
			This is an internal alias to [fnc]$boolean[/fnc]().
		@seealso:
			[fnc]$real[/fnc]
			[fnc]$integer[/fnc]
	*/

	KVSCF(boolean)
	{
		KviKvsVariant * v;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("data",KVS_PT_VARIANT,0,v)
		KVSCF_PARAMETERS_END

		kvs_int_t iVal;
		v->castToInteger(iVal);
		KVSCF_pRetBuffer->setBoolean(iVal);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: channel
		@type:
			function
		@title:
			$channel
		@short:
			Retrieves the window ID of a specified channel
		@syntax:
			$channel[(<channel name>[,<irc context id>])]
		@description:
			Returns the [b]window ID[/b] of channel matching the
			<channel name> and bound to the connection specified by
			<irc context id>[br]
			If no window matches the specified name or connection, an invalid
			window ID is returned (0).[br]
			If no <irc context id> is specified, this function looks for
			the channel in the current connection context (if any).[br]
			If no <channel name> is specified, this function returns the current
			channel window ID, if executed in a channel, else 0.[br]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$query[/fnc],
			[fnc]$console[/fnc],
			[doc:window_naming_conventions]Window naming conventions[/doc]
	*/

	KVSCF(channel)
	{
		QString szName;
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("channel_name",KVS_PT_NONEMPTYSTRING,KVS_PF_OPTIONAL,szName)
			KVSCF_PARAMETER("context_id",KVS_PT_UINT,KVS_PF_OPTIONAL,uContextId)
		KVSCF_PARAMETERS_END

		KviWindow * wnd = 0;
		if(KVSCF_pParams->count() > 0)
		{
			if(KVSCF_pParams->count() > 1)
			{
				KviConsole * cons = g_pApp->findConsole(uContextId);
				if(!cons)KVSCF_pContext->warning(__tr2qs_ctx("No such IRC context (%u)","kvs"),uContextId);
				else {
					if(cons->connection())
						wnd = cons->connection()->findChannel(szName);
					else
						wnd = 0;
				}
			} else {
				if(KVSCF_pContext->window()->connection())wnd = KVSCF_pContext->window()->connection()->findChannel(szName);
				else {
					if(!KVSCF_pContext->window()->console())
						KVSCF_pContext->warning(__tr2qs_ctx("This window is not associated to an IRC context","kvs"));
					wnd = 0;
				}
			}
		} else {
			if(KVSCF_pContext->window()->type() == KVI_WINDOW_TYPE_CHANNEL)wnd = KVSCF_pContext->window();
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(wnd ? wnd->numericId() : 0));
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: char
		@type:
			function
		@title:
			$char
		@short:
			Returns a character specified by unicode
		@syntax:
			<string> $char(<unicode_value:integer>)
		@description:
			Returns a character corresponding to the UNICODE code <unicode_value>.[br]
			This function can not return NUL character (UNICODE 0). Basically
			you should never need it: if you do, drop me a mail.[br]
			If the <unicode_code> is not a valid UNICODE code (or is 0), this function returns
			an empty string.[br]
		@seealso:
			[fnc]$cr[/fnc], [fnc]$lf[/fnc], [fnc]$unicode[/fnc]
	*/

	KVSCF(charCKEYWORDWORKAROUND)
	{
		kvs_uint_t ac;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("unicode_value",KVS_PT_UINT,0,ac)
		KVSCF_PARAMETERS_END

		if(ac != 0 && ac < 65536)
			KVSCF_pRetBuffer->setString(QString(QChar((unsigned short)ac)));
		else
			KVSCF_pRetBuffer->setString(QString());
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: classdefined
		@type:
			function
		@title:
			$classdefined
		@short:
			Checks if a class is defined
		@syntax:
			$classdefined(<class_name>)
		@description:
			Returns 1 if the class <class_name> is defined, else 0.
	*/

	KVSCF(classDefined)
	{
		// prologue: parameter handling
		QString szClassName;

		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("className",KVS_PT_NONEMPTYSTRING,0,szClassName)
		KVSCF_PARAMETERS_END
		KVSCF_pRetBuffer->setBoolean(KviKvsKernel::instance()->objectController()->lookupClass(szClassName) != 0);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: console
		@type:
			function
		@title:
			$console
		@short:
			Retrieves the window ID of a specified console
		@syntax:
			$console[(<irc context id>)]
		@description:
			Returns the [b]window ID[/b] of the console bound
			to the IRC context specified by <irc context id>.
			If no window matches the specified IRC context, an invalid
			window ID is returned (0).[br]
			If no <irc context id> is specified, this function looks for
			the console in the current IRC context (if any).[br]
		@seealso:
			[fnc]$window[/fnc],
			[fnc]$channel[/fnc],
			[fnc]$query[/fnc],
			[doc:window_naming_conventions]Window naming conventions[/doc]
	*/

	KVSCF(console)
	{
		kvs_uint_t uContextId;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("context_id",KVS_PT_UINT,KVS_PF_OPTIONAL,uContextId)
		KVSCF_PARAMETERS_END

		KviConsole * cons;
		if(KVSCF_pParams->count() > 0)
		{
			cons = g_pApp->findConsole(uContextId);
		} else {
			cons = KVSCF_pContext->window()->console();
			if(!cons)KVSCF_pContext->warning(__tr2qs_ctx("This window is not associated to an IRC context","kvs"));
		}

		KVSCF_pRetBuffer->setInteger((kvs_int_t)(cons ? cons->numericId() : 0));
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: context
		@type:
			function
		@title:
			$context
		@short:
			Retrieves the ID of the specified IRC context
		@syntax:
			$context[(<server>,<nickname>)]
		@description:
			Returns the [b]IRC context ID[/b] of the IRC context that uses
			the specified <server> and local user's <nickname>.[br] This function can
			find only connected IRC contexts.
			If no context matches the server and nickname, and invalid
			[b]IRC context ID[/b] is returned (0).[br]
			If <server> is an empty string, the first context that matches
			the specified nickname is returned. If <nickname> is an empty string
			the first context that uses the specified server is returned.
			If both parameters are missing this function returns the
			id of the current IRC context, or '0' if the
			window in that this call is executed is not bound to any IRC context.
			Please note that in this last case you may find an [b]IRC context[/b]
			that is 'not connected'.
			This can only happen if the current window is a console that is
			in "idle" state, with no connection established yet.[br]
			It is a good idea to take a look at the
			[doc:window_naming_conventions]window naming conventions[/doc].
			This identifier is equivalent to [fnc]$ic[/fnc].[br]
		@seealso:
			[fnc]$window.context[/fnc]
	*/

	KVSCF(context)
	{
		QString szServer,szNick;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("server",KVS_PT_STRING,KVS_PF_OPTIONAL,szServer)
			KVSCF_PARAMETER("nick",KVS_PT_STRING,KVS_PF_OPTIONAL,szNick)
		KVSCF_PARAMETERS_END

		KviConsole * cons;
		if(!(szServer.isEmpty() && szNick.isEmpty()))
		{
			cons = g_pApp->findConsole(szServer,szNick);
		} else {
			cons = KVSCF_pContext->window()->console();
		}

		KVSCF_pRetBuffer->setInteger(cons ? cons->context()->id() : 0);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: countStatusBarItems
		@type:
			function
		@title:
			$countStatusBarItems
		@short:
			Returns the number of items in the statusbar
		@syntax:
			<int> $countStatusBarItems
		@description:
			Returns the number of items in the statusbar
		@seealso:
			[class]widget class[/class]
	*/

	KVSCF(countStatusBarItems)
	{
		QList<QWidget *> widgets = g_pFrame->statusBar()->findChildren<QWidget *>();
		KVSCF_pRetBuffer->setInteger(widgets.count());
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: cr
		@type:
			function
		@title:
			$cr
		@short:
			Returns a carriage return character
		@syntax:
			<string> $cr
		@description:
			Returns a carriage return character
		@seealso:
			[fnc]$lf[/fnc], [fnc]$ascii[/fnc], [fnc]$char[/fnc]
	*/

	KVSCF(cr)
	{
		KVSCF_pRetBuffer->setString(QString(QChar('\r')));
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: date
		@type:
			function
		@title:
			$date
		@short:
			Returns a date/time string using a specified format
		@syntax:
			<string> $date(<format:string>[,<unixtime:integer>])
		@description:
			Returns the string representation of <unixtime> or
			of the current time if <unixtime> is not given,
			based on <format>.[br]
			The <format string> should contain a set of characters
			that will be transformed according to the following rules:[br]
			[br]
			[table]
			[tr][td][b]a[/b][/td][td]The abbreviated weekday name according to the current locale.[/td][/tr]
			[tr][td][b]A[/b][/td][td]The full weekday name according to the current locale.[/td][/tr]
			[tr][td][b]b[/b][/td][td]The abbreviated month name according to the current locale.[/td][/tr]
			[tr][td][b]B[/b][/td][td]The full month name according to the current locale.[/td][/tr]
			[tr][td][b]c[/b][/td][td]The preferred date and time representation for the current locale.[/td][/tr]
			[tr][td][b]C[/b][/td][td]The century number (year/100) as a 2-digit integer. (SU)[/td][/tr]
			[tr][td][b]d[/b][/td][td]The day of the month as a decimal number (range 01 to 31).[/td][/tr]
			[tr][td][b]D[/b][/td][td]Equivalent to m/d/y.[/td][/tr]
			[tr][td][b]e[/b][/td][td]Like d, the day of the month as a decimal number, but a leading
			zero is replaced by a space. (SU)[/td][/tr]
			[tr][td][b]E[/b][/td][td]Modifier: use alternative format, see below. (SU)[/td][/tr]
			[tr][td][b]F[/b][/td][td]Equivalent to Y-m-d (the ISO 8601 date format). (C99)[/td][/tr]
			[tr][td][b]G[/b][/td][td]The ISO 8601 year with century as a decimal number. The 4-digit
			year corresponding  to the ISO week number (see V). This has the
			same format and value as y, except that if the ISO week number
			belongs to the previous or next year, that year is used instead.[/td][/tr]
			[tr][td][b]g[/b][/td][td]Like G, but without century, i.e., with a 2-digit year (00-99).[/td][/tr]
			[tr][td][b]h[/b][/td][td]Equivalent to b.[/td][/tr]
			[tr][td][b]H[/b][/td][td]The hour as a decimal number using a 24-hour clock (range 00 to 23).[/td][/tr]
			[tr][td][b]I[/b][/td][td]The hour as a decimal number using a 12-hour clock (range 01 to 12).[/td][/tr]
			[tr][td][b]j[/b][/td][td]The day of the year as a decimal number (range 001 to 366).[/td][/tr]
			[tr][td][b]k[/b][/td][td]The hour (24-hour clock) as a decimal number (range 0 to 23); sin-
			gle digits are preceded by a blank. (See also H.)[/td][/tr]
			[tr][td][b]l[/b][/td][td]The hour (12-hour clock) as a decimal number (range 1 to 12); sin-
			gle digits are preceded by a blank. (See also I.)[/td][/tr]
			[tr][td][b]m[/b][/td][td]The month as a decimal number (range 01 to 12).[/td][/tr]
			[tr][td][b]M[/b][/td][td]The minute as a decimal number (range 00 to 59).[/td][/tr]
			[tr][td][b]n[/b][/td][td]A newline character. (SU)[/td][/tr]
			[tr][td][b]p[/b][/td][td]Either  `AM' or `PM' according to the given time value, or the cor-
			responding strings for the current locale.  Noon is treated as `pm' and midnight as `am'.[/td][/tr]
			[tr][td][b]r[/b][/td][td]The time in a.m. or p.m. notation.  In the  POSIX  locale  this  is
			equivalent to `I:M:S p'.[/td][/tr]
			[tr][td][b]s[/b][/td][td]The number of seconds  since  the  Epoch,  i.e.,  since  1970-01-01
			00:00:00 UTC.[/td][/tr]
			[tr][td][b]S[/b][/td][td]The second as a decimal number (range 00 to 60).  (The range is up
			to 60 to allow for occasional leap seconds.)[/td][/tr]
			[tr][td][b]t[/b][/td][td]A tab character.[/td][/tr]
			[tr][td][b]T[/b][/td][td]The time in 24-hour notation (H:M:S). (SU)[/td][/tr]
			[tr][td][b]u[/b][/td][td]The day of the week as a decimal, range 1 to  7,  Monday  being  1.
			See also w.[/td][/tr]
			[tr][td][b]U[/b][/td][td]The week number of the current year as a decimal number, range 00
			to 53, starting with the first Sunday as the first day of week  01. See also V and W.[/td][/tr]
			[tr][td][b]V[/b][/td][td]The ISO 8601:1988 week number of the current year as a decimal num-
			ber, range 01 to 53, where week 1 is the first  week  that  has  at
			least  4 days in the current year, and with Monday as the first day
			of the week. See also U and W.[/td][/tr]
			[tr][td][b]w[/b][/td][td]The day of the week as a decimal, range 0 to  6,  Sunday  being  0.[/td][/tr]
			[tr][td][b]W[/b][/td][td]The week number of the current year as a decimal number, range 00
			to 53, starting with the first Monday as the first day of week  01.[/td][/tr]
			[tr][td][b]x[/b][/td][td]The  preferred  date  representation for the current locale without
			the time.[/td][/tr]
			[tr][td][b]X[/b][/td][td]The preferred time representation for the  current  locale  without
			the date.[/td][/tr]
			[tr][td][b]y[/b][/td][td]The year as a decimal number without a century (range 00 to 99).[/td][/tr]
			[tr][td][b]Y[/b][/td][td]The year as a decimal number including the century.[/td][/tr]
			[tr][td][b]z[/b][/td][td]The   time-zone   as  hour  offset  from  GMT.   Required  to  emit
              RFC822-conformant dates (using "a, d b Y H:M:S z").[/td][/tr]
			[tr][td][b]Z[/b][/td][td]The time zone or name or abbreviation.[/td][/tr]
			[tr][td][b]+[/b][/td][td]The date and time in date(1) format. (TZ)[/td][/tr]
			[/table]
			WARNING: Please note that this list is taken from the unix version of the strftime
			function and not all the escape codes may be supported by other platforms.
		@examples:
			[example]
				[cmd]echo[/cmd] $date("d/m/Y H:M:S")
			[/example]
		@seealso:
			[fnc]$unixtime[/fnc], [fnc]$hptimestamp[/fnc]
	*/

	KVSCF(date)
	{
		QString szFormat;
		kvs_int_t iTime;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("format",KVS_PT_NONEMPTYSTRING,0,szFormat)
			KVSCF_PARAMETER("unixtime",KVS_PT_INT,KVS_PF_OPTIONAL,iTime)
		KVSCF_PARAMETERS_END

		KviStr tmpFormat("");

		#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			QString szAllowedCharacters;
			//windows version of strftime()
			//kvirc crashes if other then these characters get an % character in front of them
			szAllowedCharacters = "AaBbcdHIjMmpSUWwXxYyZz";
		#endif

		const QChar * c = KviQString::nullTerminatedArray(szFormat);
		if(c)
		{
			while(c->unicode())
			{
				//Check for right Characters
				#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
					if (szAllowedCharacters.indexOf((char)(c->unicode()),0,Qt::CaseSensitive) >= 0)	tmpFormat += '%';
				#else
					if (c->isLetter()) tmpFormat += '%';
				#endif
				tmpFormat += (char)(c->unicode());
				c++;
			}
		}

		kvi_time_t t;
		if(KVSCF_pParams->count() > 1)
			t = (kvi_time_t)iTime;
		else
			t = kvi_unixTime();

		char buf[256];
		if(strftime(buf,255,tmpFormat.ptr(),localtime(&t))> 0)
		{
			KviStr tmp = buf;
			if(tmp.lastCharIs('\n'))tmp.cutRight(1);
			KVSCF_pRetBuffer->setString(QString(buf));
		} else {
			KVSCF_pContext->warning(__tr2qs_ctx("The specified format string wasn't accepted by the underlying system time formatting function","kvs"));
		}

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: false
		@type:
			function
		@title:
			$false
		@short:
			The boolean false constant
		@syntax:
			<boolean> $false
		@description:
			Evaluates to the false boolean constant. False
			is equivalent to the integer 0 too. This function/constant
			is useful to keep your code readable: when you
			have a variable that can assume boolean values it's
			nicer to use [fnc]$true[/fnc] and $false instead of
			the integer constants 1 and 0. The reader will
			undestand immediately that the variable simply can't
			assume any other value.
		@examples:
			[example]
				%a = $false
				[cmd]echo[/cmd] $typeof(%a)
				[cmd]echo[/cmd] $(%a + 1)
			[/example]
		@seealso:
			[fnc]$true[/fnc]
	*/

	KVSCF(falseCKEYWORDWORKAROUND)
	{
		KVSCF_pRetBuffer->setBoolean(false);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: features
		@type:
			function
		@title:
			$features
		@short:
			Returns the features that KVIrc supports
		@syntax:
			<array> $features()
			<boolean> $features(<test_feature:string>)
		@description:
			The parameterless form returns an array of feature descripton strings that this KVIrc executable supports.[br]
			This function is useful when some part of your script depends on
			an optional KVIrc feature (like SSL support or IPV6 support).[br]
			The returned value may be assigned to a dictionary too: it will be used to simulate an array.[br]
			The form with the [test_feature] parameter returns true if and only if [test_feature] is available.[br]
		@examples:
			[example]
			%myfeats[] = $features
			[cmd]echo[/cmd] %myfeats[]
			%i = %myfeats[]#
			[cmd]while[/cmd](%i > 0)
			{
				[cmd]echo[/cmd] "Supporting feature %myfeats[%i]"
				%i--;
			}
			[/example]
			Nearly the same loop, just really shorter:
			[example]
			[cmd]foreach[/cmd](%f,$features)
				[cmd]echo[/cmd] "Supporting feature %myfeats[%i]"
			[/example]
			You can test for a specific feature in the following way:
			[example]
			[cmd]if[/cmd]($features("SSL"))[cmd]echo[/cmd] "Yes! SSL is available";
			[/example]
			If used in "non-array" context it returns just a comma separated list of entries:[br]
			[example]
			[cmd]echo[/cmd] $features
			[/example]
		@seealso:
			[fnc]$version[/fnc]
	*/

	KVSCF(features)
	{
		QString szFeature;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("test_feature",KVS_PT_STRING,KVS_PF_OPTIONAL,szFeature)
		KVSCF_PARAMETERS_END

		static const char * feature_array[]=
		{
			"IRC",
	#ifdef COMPILE_IPV6_SUPPORT
			"IPv6",
	#endif
	#ifdef COMPILE_CRYPT_SUPPORT
			"Crypt",
	#endif
	#ifdef COMPILE_SSL_SUPPORT
			"SSL",
	#endif
	#ifdef COMPILE_GET_INTERFACE_ADDRESS
			"IfAddr",
	#endif
	#ifndef COMPILE_NO_IPC
			"IPC",
	#endif
	#ifdef COMPILE_KDE_SUPPORT
			"KDE",
	#endif
	#ifdef COMPILE_OSS_SUPPORT
			"OSS",
	#endif
	#ifdef COMPILE_ARTS_SUPPORT
			"ARTS",
	#endif
	#ifdef COMPILE_ESD_SUPPORT
			"ESD",
	#endif
	#ifdef COMPILE_AUDIOFILE_SUPPORT
			"Audiofile",
	#endif
	#ifdef COMPILE_PSEUDO_TRANSPARENCY
			"Transparency",
	#endif
	#ifdef COMPILE_ix86_ASM
			"ix86-ASM",
	#endif
	#ifdef COMPILE_SCRIPTTOOLBAR
			"ScriptToolBar",
	#endif // COMPILE_SCRIPTTOOLBAR
	#ifdef COMPILE_PHONON_SUPPORT
			"Phonon",
	#endif
	#ifdef COMPILE_WEBKIT_SUPPORT
			"Webkit",
	#endif
			"Qt4",
			"KVS",
			0
		};

		if(!szFeature.isEmpty())
		{
			for(int i=0;feature_array[i];i++)
			{
				if(KviQString::equalCI(feature_array[i],szFeature))
				{
					KVSCF_pRetBuffer->setBoolean(true);
					return true;
				}
			}
			KVSCF_pRetBuffer->setBoolean(false);
		} else {
			KviKvsArray * a = new KviKvsArray();
			for(int i=0;feature_array[i];i++)a->set(i,new KviKvsVariant(QString(feature_array[i])));
			KVSCF_pRetBuffer->setArray(a);
		}

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: firstconnectedconsole
		@type:
			function
		@title:
			$firstConnectedConsole
		@short:
			Returns the window id of the first connected console
		@syntax:
			<uint> $firstConnectedConsole()
		@description:
			Returns the window id of the first connected console
			or 0 if no console is actually connected.
	*/

	KVSCF(firstConnectedConsole)
	{
		KviConsole * c = g_pApp->topmostConnectedConsole();
		// FIXME: The window id's should be numeric!!!
		KVSCF_pRetBuffer->setString(c ? c->id() : "0");
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: flatten
		@type:
			function
		@title:
			$flatten
		@short:
			Returns a flattened array of items
		@syntax:
			<array> $flatten(<data1:variant>[,<data2:variant>[,...]])
		@description:
			Returns an array of items built from the passed arguments
			with the following rules:[br]
			[ul]
				[li]If an argument is a scalar value then the argument itself
					is appended to the result.[/li]
				[li]If an argument is an array then each contained item
					is appended to the result.[/li]
				[li]If an argument is a hash then each contained value
					is appended to the result.[/li]
			[/ul]
			A simple example of usage is to merge N arrays into a new one.
			(Please note that for merging one array into another the
			[doc:arrayconcatenation]<+ operator[/doc] is more efficient).
	*/

	KVSCF(flatten)
	{
		KviKvsArray * a = new KviKvsArray();
		KVSCF_pRetBuffer->setArray(a);
		unsigned int uIdx = 0;
		for(KviKvsVariant * v = KVSCF_pParams->first();v;v = KVSCF_pParams->next())
		{
			switch(v->type())
			{
				case KviKvsVariantData::Array:
				{
					KviKvsArray * z = v->array();
					unsigned int uSize = z->size();
					unsigned int uIdx2 = 0;
					while(uIdx2 < uSize)
					{
						KviKvsVariant * pInternal = z->at(uIdx2);
						if(pInternal)
							a->set(uIdx,new KviKvsVariant(*pInternal));
						// else
						//	don't set anything: just leave empty entry (nothing)
						uIdx++;
						uIdx2++;
					}
				}
				break;
				case KviKvsVariantData::Hash:
				{
					KviKvsHash * h = v->hash();
					KviKvsHashIterator it(*(h->dict()));
					while(KviKvsVariant * pInternal = it.current())
					{
						a->set(uIdx,new KviKvsVariant(*pInternal));
						uIdx++;
						++it;
					}
				}
				break;
				default:
					a->set(uIdx,new KviKvsVariant(*v));
					uIdx++;
				break;
			}
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: fmtlink
		@type:
			function
		@title:
			$fmtlink
		@short:
			Returns a formatted link buffer
		@syntax:
			<string> $fmtlink(<link_text:string>,<double_click_command:string>[,<tooltip_text:string>])
		@description:
			Returns a link formatted for the [cmd]echo[/cmd] command.[br]
			If you pass the returned string to the echo command, the string will be displayed
			as a link and will be highlighted when the user moves the mouse over it.[br]
			If the user will leave the mouse for a few seconds over the link, the <tooltip_text>
			will be displayed in a small tooltip window. If <tooltip_text> is not given,
			then no tooltip will be shown.[br]
			The <double_click_command> will be executed when the user will double click on the link.[br]
			Please remember that if <double_click_command> contains identifiers
			that must be evaluated at double-click time, you MUST escape them in the $fmtlink() call
			to prevent the evaluation.[br]
			You might also take a look at [doc:escape_sequences]the escape sequences documentation[/doc]
			to learn more about how the links are implemented and how to create more powerful links (add
			right and middle button actions, use predefined kvirc links etc...)
		@seealso:
			[doc:escape_sequences]the escape sequences documentation[/doc]
	*/

	KVSCF(fmtlink)
	{
		QString szLinkText,szCmd,szToolTip;
		KVSCF_PARAMETERS_BEGIN
			KVSCF_PARAMETER("link_text",KVS_PT_NONEMPTYSTRING,0,szLinkText)
			KVSCF_PARAMETER("double_click_command",KVS_PT_STRING,0,szCmd)
			KVSCF_PARAMETER("tooltip_text",KVS_PT_STRING,KVS_PF_OPTIONAL,szToolTip)
		KVSCF_PARAMETERS_END

		QString szPart;
		KviQString::sprintf(szPart,"[!dbl]%Q",&szCmd);
		if(!szToolTip.isEmpty())KviQString::appendFormatted(szPart,"[!txt]%Q",&szToolTip);
		QString szLink;
		KviQString::sprintf(szLink,"\r!%Q\r%Q\r",&szPart,&szLinkText);
		KVSCF_pRetBuffer->setString(szLink);
		return true;
	}
};