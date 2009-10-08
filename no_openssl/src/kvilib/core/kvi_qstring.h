#ifndef _KVI_QSTRING_H_
#define _KVI_QSTRING_H_
//=============================================================================
//
//   File : kvi_qstring.h
//   Creation date : Mon Aug 04 2003 13:36:33 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_qstring.h
* \author Szymon Stefanek
* \brief Helper functions for the QString class
*/

#include "kvi_settings.h"
#include "kvi_inttypes.h"
#include "kvi_stdarg.h"

#include <QString>
#include <QByteArray>

/**
* \namespace KviQString
* \brief A namespace for QString helper functions
*
* This namespace contains several helper functions that are used when dealing
* with QString.
*/
namespace KviQString
{
	extern KVILIB_API QString makeSizeReadable(size_t size);
	extern KVILIB_API bool equalCS(const QString &sz1,const QString &sz2);
	extern KVILIB_API bool equalCI(const QString &sz1,const QString &sz2);
	extern KVILIB_API bool equalCS(const QString &sz1,const char * sz2);
	extern KVILIB_API bool equalCI(const QString &sz1,const char * sz2);
	// sz2 is assumed to be null terminated here!
	extern KVILIB_API bool equalCI(const QString &sz1,const QChar * sz2);
	inline bool equalCS(const char * sz1,const QString &sz2)
		{ return equalCS(sz2,sz1); }
	inline bool equalCI(const char * sz1,const QString &sz2)
		{ return equalCI(sz2,sz1); }
	// sz1 is assumed to be null terminated here!
	inline bool equalCI(const QChar * sz1,const QString &sz2)
		{ return equalCI(sz2,sz1); }

	extern KVILIB_API bool equalCSN(const QString &sz1,const QString &sz2,unsigned int len);
	extern KVILIB_API bool equalCIN(const QString &sz1,const QString &sz2,unsigned int len);
	extern KVILIB_API bool equalCSN(const QString &sz1,const char * sz2,unsigned int len);
	extern KVILIB_API bool equalCIN(const QString &sz1,const char * sz2,unsigned int len);
	// sz2 is assumed to be null terminated here!
	extern KVILIB_API bool equalCIN(const QString &sz1,const QChar * sz2,unsigned int len);
	inline bool equalCSN(const char * sz1,const QString &sz2,unsigned int len)
		{ return equalCSN(sz2,sz1,len); }
	inline bool equalCIN(const char * sz1,const QString &sz2,unsigned int len)
		{ return equalCIN(sz2,sz1,len); }
	// sz1 is assumed to be null terminated here!
	inline bool equalCIN(const QChar * sz1,const QString &sz2,unsigned int len)
		{ return equalCIN(sz2,sz1,len); }

	//note that greater here means that come AFTER in the alphabetic order
	// return < 0 ---> str1 < str2
	// return = 0 ---> str1 = str2
	// return > 0 ---> str1 > str2
	extern KVILIB_API int cmpCI(const QString &sz1,const QString &sz2,bool nonAlphaGreater = false);
	extern KVILIB_API int cmpCIN(const QString &sz1,const QString &sz2,unsigned int len);
	extern KVILIB_API int cmpCS(const QString &sz1,const QString &sz2);

	extern KVILIB_API void detach(QString &sz);

	// this makes the QString sz appear as a null terminated array
	// it MAY RETURN 0 when the QString is null!
	extern KVILIB_API const QChar * nullTerminatedArray(const QString &sz);

	/**
	* \brief Returns true if the string ends with character c
	* \param szString The source string
	* \param c The char to check
	* \return bool
	*/
	inline bool lastCharIs(QString & szString, const QChar & c)
		{ return szString.endsWith(c); }

	extern KVILIB_API void ensureLastCharIs(QString &szString,const QChar &c);

	// wild expression matching
	extern KVILIB_API bool matchWildExpressionsCI(const QString &szM1,const QString &szM2);
	// wild or regexp matching
	extern KVILIB_API bool matchStringCI(const QString &szExp,const QString &szStr,bool bIsRegExp = false,bool bExact = false);
	extern KVILIB_API bool matchStringCS(const QString &szExp,const QString &szStr,bool bIsRegExp = false,bool bExact = false);

	extern KVILIB_API void vsprintf(QString &s,const QString &szFmt,kvi_va_list list);
	extern KVILIB_API QString & sprintf(QString &s,const QString &szFmt,...);
	extern KVILIB_API void stripRightWhiteSpace(QString &s);
	extern KVILIB_API void stripLeft(QString &s,const QChar &c);
	extern KVILIB_API void stripRight(QString &s,const QChar &c);
	extern KVILIB_API void appendFormatted(QString &s,const QString &szFmt,...);
	extern KVILIB_API void appendNumber(QString &s,double dReal);
	extern KVILIB_API void appendNumber(QString &s,kvi_i64_t iInteger);
	extern KVILIB_API void appendNumber(QString &s,int iInteger);
	extern KVILIB_API void appendNumber(QString &s,unsigned int uInteger);
	extern KVILIB_API void appendNumber(QString &s,kvi_u64_t uInteger);

	extern KVILIB_API void cutFromFirst(QString &s,const QChar &c,bool bIncluded = true);
	extern KVILIB_API void cutFromLast(QString &s,const QChar &c,bool bIncluded = true);
	extern KVILIB_API void cutToFirst(QString &s,const QChar &c,bool bIncluded = true,bool bClearIfNotFound = false);
	extern KVILIB_API void cutToLast(QString &s,const QChar &c,bool bIncluded = true,bool bClearIfNotFound = false);
	extern KVILIB_API void cutFromFirst(QString &s,const QString &c,bool bIncluded = true);
	extern KVILIB_API void cutFromLast(QString &s,const QString &c,bool bIncluded = true);
	extern KVILIB_API void cutToFirst(QString &s,const QString &c,bool bIncluded = true,bool bClearIfNotFound = false);
	extern KVILIB_API void cutToLast(QString &s,const QString &c,bool bIncluded = true,bool bClearIfNotFound = false);

	extern KVILIB_API QString upperISO88591(const QString &szSrc);
	extern KVILIB_API QString lowerISO88591(const QString &szSrc);
	extern KVILIB_API QString getToken(QString &szString,const QChar &sep);

	extern KVILIB_API void transliterate(QString &s,const QString &szToFind,const QString &szReplacement);

	extern KVILIB_API void bufferToHex(QString &szRetBuffer,const unsigned char * buffer,unsigned int len);

	// a global empty string (note that this is ALSO NULL under Qt 3.x)
	extern KVILIB_API const QString empty;

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made forward starting from index.
	* \param s The source string
	* \param c The character to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int find(const QString & s, QChar c, int index = 0, bool cs = true)
	{
		return s.indexOf(c,index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made forward starting from index.
	* \param s The source string
	* \param c The character to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int find(const QString & s, char c, int index = 0, bool cs = true)
	{
		return s.indexOf(c,index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string
	*
	* The search is made forward starting from index.
	* \param s The source string
	* \param str The string to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int find(const QString & s, const QString & str, int index = 0, bool cs = true)
	{
		return s.indexOf(str,index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string
	*
	* The search is made forward starting from index.
	* \param s The source string
	* \param str The string to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int find(const QString & s, const char * str, int index = 0, bool cs = true)
	{
		return s.indexOf(QString(str),index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string
	*
	* The search is made forward starting from index.
	* \param s The source string
	* \param rx The regexp to match
	* \param index The index to start from
	* \return int
	*/
	inline int find(const QString & s, const QRegExp & rx, int index = 0)
	{
		return s.indexOf(rx,index);
	}

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param s The source string
	* \param c The character to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & s ,QChar c, int index = -1, bool cs = true)
	{
		return s.lastIndexOf(c,index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the character
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param s The source string
	* \param c The character to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & s, char c, int index = -1, bool cs = true)
	{
		return s.lastIndexOf(c,index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string str
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param s The source string
	* \param str The string to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & s, const QString & str, int index = -1, bool cs = true)
	{
		return s.lastIndexOf(str,index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last occurrence of the string str
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param s The source string
	* \param str The string to find
	* \param index The index to start from
	* \param cs Case sensitive search
	* \return int
	*/
	inline int findRev(const QString & s, const char * str, int index = -1, bool cs = true)
	{
		return s.lastIndexOf(QString(str),index,cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
	}

	/**
	* \brief Returns the index position of the last match of the regexp
	*
	* The search is made backward.
	* If index is -1 the search starts at the last character.
	* \param s The source string
	* \param rx The regexp to match
	* \param index The index to start from
	* \return int
	*/
	inline int findRev(const QString & s, const QRegExp & rx, int index = -1)
	{
		return s.lastIndexOf(rx,index);
	}

	/**
	* \brief Return a whitespace-trimmed string
	*
	* Spaces are trimmed at start and end of the string
	* \param s The source string
	* \return QString
	*/
	inline QString trimmed(const QString & s)
	{
		return s.trimmed();
	}

	/**
	* \brief Return a UTF-8 formatted string
	* \param s The source string
	* \return QByteArray
	* \warning: DO NOT USE CONSTRUCTS LIKE char * c = KviQString::toUtf8(something).data();
	* They are dangerous since with many compilers the returned string
	* gets destroyed at the end of the instruction and the c pointer gets
	* thus invalidated.
	* Use
	* QByteArray tmp = KviQString::toUtf8(something);
	* char * c = tmp.data();
	* instead. Yes, I know that it sucks, but it's the only way to
	* transit to Qt 4.x more or less cleanly...
	*/
	inline QByteArray toUtf8(const QString & s)
	{
		return s.toUtf8();
	}

	/**
	* \brief Return the local 8-bit representation of the string
	* \param s The source string
	* \return QByteArray
	*/
	inline QByteArray toLocal8Bit(const QString & s)
	{
		return s.toLocal8Bit();
	}

	/**
	* \brief Return the string converted to a long
	* \param s The source string
	* \param bOk The conversion error handling
	* \return kvi_i64_t
	*/
	inline kvi_i64_t toI64(QString & szNumber, bool * bOk)
	{
#if SYSTEM_SIZE_OF_LONG_INT == 8
		return szNumber.toLong(bOk);
#else
		return szNumber.toLongLong(bOk);
#endif
	}

	/**
	* \brief Return the string converted to an unsigned long
	* \param s The source string
	* \param bOk The conversion error handling
	* \return kvi_u64_t
	*/
	inline kvi_u64_t toU64(QString & szNumber, bool * bOk)
	{
#if SYSTEM_SIZE_OF_LONG_INT == 8
		return szNumber.toULong(bOk);
#else
		return szNumber.toULongLong(bOk);
#endif
	}
}

// QT4SUX: Because QString::null is gone. QString() is SLOWER than QString::null since it invokes a constructor and destructor.

#endif //_KVI_QSTRING_H_