#ifndef _KVI_IRCCONNECTIONSERVERINFO_H_
#define _KVI_IRCCONNECTIONSERVERINFO_H_
//=============================================================================
//
//   File : kvi_ircconnectionserverinfo.h
//   Creation date : Tue 22 Jun 2004 03:57:32 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_inttypes.h"

class KVIRC_API KviBasicIrcServerInfo
{
protected:
	QString m_szServerVersion;
public:
	KviBasicIrcServerInfo(const QString & version = KviQString::empty);
	virtual ~KviBasicIrcServerInfo();
protected:
	virtual const QString & getCustomChannelModeDescription(QChar) { return KviQString::empty; };
	virtual const QString & getCustomUserModeDescription(QChar) { return KviQString::empty; };
private:
	const QString & getBasicChannelModeDescription(QChar mode);
	const QString & getBasicUserModeDescription(QChar mode);
public:
	const QString & getChannelModeDescription(QChar mode);
	const QString & getUserModeDescription(QChar mode);
	virtual char getRegisterModeChar() { return 0; };
};

class KVIRC_API KviUnrealIrcServerInfo : public KviBasicIrcServerInfo
{
public:
	KviUnrealIrcServerInfo(const QString & version = KviQString::empty)
		:KviBasicIrcServerInfo(version) {;};
	virtual char getRegisterModeChar() { return 'r'; };
};

class KVIRC_API KviBahamutIrcServerInfo : public KviBasicIrcServerInfo
{
public:
	KviBahamutIrcServerInfo(const QString & version = KviQString::empty)
		:KviBasicIrcServerInfo(version) {;};
	virtual char getRegisterModeChar() { return 'r'; };
};

class KVIRC_API KviHyperionIrcServerInfo : public KviBasicIrcServerInfo
{
public:
	KviHyperionIrcServerInfo(const QString & version = KviQString::empty)
		:KviBasicIrcServerInfo(version) {;};
	virtual char getRegisterModeChar() { return 'e'; };
};

class KVIRC_API KviIrcConnectionServerInfo
{
	friend class KviConsole; // for now
	friend class KviServerParser;
	friend class KviIrcConnection;
protected:
	KviIrcConnectionServerInfo();
	~KviIrcConnectionServerInfo();
private:
	KviBasicIrcServerInfo * m_pServInfo;
	QString m_szName;                      // the most actual server name (may be the one we specify or the one that the server wants to be known as)
	QString m_szSupportedUserModes;        // the supported user modes
	QString m_szSupportedChannelModes;     // the supported channel modes
	QString m_szSupportedModePrefixes;     // the actually used mode prefixes  @+
	kvi_u32_t * m_pModePrefixTable;        // the mode prefixes above in a table
	unsigned int m_uPrefixes;
	QString m_szSupportedModeFlags;        // the actually used mode flags     ov
	QString m_szSupportedChannelTypes;     // the supported channel types
	bool m_bSupportsModesIe;               // supports the channel modes I and e ?
	bool m_bSupportsWatchList;             // supports the watch list ?
	bool m_bSupportsCodePages;             // supports the /CODEPAGE command ?
	int m_iMaxTopicLen;
	int m_iMaxModeChanges;
	QString m_szListModes; 
	QString m_szPlainModes;
public:
	char  registerModeChar() { return m_pServInfo ?  m_pServInfo->getRegisterModeChar() : 0; };
	const QString & name(){ return m_szName; };
	const QString & supportedUserModes(){ return m_szSupportedUserModes; };
	const QString & supportedChannelModes(){ return m_szSupportedChannelModes; };
	const QString & supportedChannelTypes(){ return m_szSupportedChannelTypes; };
	const QString & supportedModePrefixes(){ return m_szSupportedModePrefixes; };
	const QString & supportedModeFlags(){ return m_szSupportedModeFlags; };
	const QString & supportedListModes(){ return m_szListModes; };
	const QString & supportedPlainModes(){ return m_szPlainModes; };
	bool supportsModesIe(){ return m_bSupportsModesIe; };
	bool supportsWatchList(){ return m_bSupportsWatchList; };
	bool supportsCodePages(){ return m_bSupportsCodePages; };

	int maxTopicLen() { return m_iMaxTopicLen; };
	int maxModeChanges() { return m_iMaxModeChanges; };
	
	void setServerVersion(const QString & version);
	
	const QString & getChannelModeDescription(QChar mode) { return m_pServInfo->getChannelModeDescription(mode); };
	const QString & getUserModeDescription(QChar mode) { return m_pServInfo->getUserModeDescription(mode); };
	
	bool isSupportedModePrefix(QChar c);
	bool isSupportedModeFlag(QChar c);
	QChar modePrefixChar(kvi_u32_t flag);
	QChar modeFlagChar(kvi_u32_t flag);
	kvi_u32_t modeFlagFromPrefixChar(QChar c);
	kvi_u32_t modeFlagFromModeChar(QChar c);
protected:
	void setName(const QString &szName){ m_szName = szName; };
	void setSupportedUserModes(const QString &szSupportedUserModes){ m_szSupportedUserModes = szSupportedUserModes; };
	void setSupportedChannelModes(const QString &szSupportedChannelModes);
	void setSupportedModePrefixes(const QString &szSupportedModePrefixes,const QString &szSupportedModeFlags);
	void setSupportedChannelTypes(const QString &szSupportedChannelTypes){ m_szSupportedChannelTypes = szSupportedChannelTypes; };
	void setSupportsWatchList(bool bSupportsWatchList){ m_bSupportsWatchList = bSupportsWatchList; };
	void setSupportsCodePages(bool bSupportsCodePages){ m_bSupportsCodePages = bSupportsCodePages; };
	void setMaxTopicLen( int iTopLen ) { m_iMaxTopicLen=iTopLen; };
	void setMaxModeChanges(int iModes ) { m_iMaxModeChanges=iModes; };
private:
	void buildModePrefixTable();
};

#endif //!_KVI_IRCCONNECTIONSERVERINFO_H_