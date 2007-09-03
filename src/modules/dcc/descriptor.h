#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_
//=============================================================================
//
//   File : src/modules/dcc/descriptor.h
//   Creation date : Tue Jul 23 01:11:52 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2004 Szymon Stefanek (oragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_string.h"
#include "kvi_console.h"




class KviDccWindow;
class KviDccFileTransfer;

class KviDccDescriptor
{
public:
	KviDccDescriptor(KviConsole * pConsole);
	//KviDccDescriptor(const KviDccDescriptor & src);
	~KviDccDescriptor();
protected:
	KviConsole         * m_pConsole;

	// mIrc zero port reverse send/chat extension
	KviStr               m_szZeroPortRequestTag;

	unsigned int         m_uId; // this dcc session ID
	QString              m_szId;
	
	KviDccWindow       * m_pDccWindow;   // 0 if it has no window
	KviDccFileTransfer * m_pDccTransfer; // 0 if it is not a transfer
	
	bool                 m_bCreationEventTriggered;
public:
	// A console that this DCC is bound to (might be replaced while we wait for user acknowledge in dialogs)
	KviConsole * console() const { return m_pConsole; };
	void setConsole(KviConsole * c){ m_pConsole = c; };

	KviDccWindow * window() const { return m_pDccWindow; };
	void setWindow(KviDccWindow * w){ m_pDccWindow = w; };
	
	KviDccFileTransfer * transfer() const { return m_pDccTransfer; };
	void setTransfer(KviDccFileTransfer * t){ m_pDccTransfer = t; };

	// mIrc zero port reverse send/chat extension
	bool isZeroPortRequest() const { return m_szZeroPortRequestTag.hasData(); };
	const char * zeroPortRequestTag() const { return m_szZeroPortRequestTag.ptr(); };
	void setZeroPortRequestTag(const KviStr &szTag){ m_szZeroPortRequestTag = szTag; };

	unsigned int id() const { return m_uId; };
	const QString & idString() const { return m_szId; };
	static KviDccDescriptor * find(unsigned int uId);
	static QHash<int,KviDccDescriptor*> * descriptorDict();

	void triggerCreationEvent(); // this MUST be called by the creator of the descriptor!
//private:
//	void copyFrom(const KviDccDescriptor &src);
public:
	// Generic parameters
	QString         szType;            // DCC protocol : CHAT , SCHAT , SEND , TSSEND....

	bool           bActive;           // active or passive connection ?

	QString         szNick;            // remote user nickname
	QString         szUser;            // remote user name (unknown for passive dcc)
	QString         szHost;            // remote user host (unknown for passive dcc)

	QString         szLocalNick;       // local user nickname (always from irc)
	QString         szLocalUser;       // local user username (always from irc)
	QString         szLocalHost;       // local user hostname (always from irc)

	QString         szIp;              // remote user ip (active dcc only)
	QString         szPort;            // remote user port (active dcc only)

	QString         szListenIp;        // passive only : ip to listen on
	QString         szListenPort;      // passive only : port to listen on

	bool           bSendRequest;      // passive only : true if we have to send the CTCP request

	QString         szFakeIp;          // passive only : fake ip to send in the CTCP
	QString         szFakePort;        // passive only : fake port to send in the CTCP

	bool           bDoTimeout;        // the marshall has to setup a timeout ?

	bool           bIsTdcc;           // is this a TDCC ?

	bool           bOverrideMinimize; // Override the default minimize option ?
	bool           bShowMinimized;    // Show minimized ? (valid if bOverrideMinimize is true)

	bool           bAutoAccept;       // Auto accepted dcc send/chat ?
#ifdef COMPILE_SSL_SUPPORT
	bool           bIsSSL;            // do we have to use SSL ?
#endif
	// Specific parameters

	// DCC SEND/RECV

	QString         szFileName;        // RECVFILE: incoming file name, SENDFILE: filename sent to the remote end
	QString         szFileSize;        // RECVFILE: incoming file size, SENDFILE: remote resume size

	QString         szLocalFileName;   // RECVFILE: save file name selected, SENDFILE: file to send
	QString         szLocalFileSize;   // RECVFILE: local file size (to resume), SENDFILE: file to send size

	bool           bRecvFile;         // do we have to RECEIVE the file or SEND it ?

	bool           bResume;           // do we want to resume ?
	bool           bNoAcks;           // blind dcc send ? (do not receive nor send acknowledges)

	bool           bIsIncomingAvatar; // It is an Incoming Avatar DCC SEND ?

	// DCC VOICE

	KviStr         szCodec;           // codec name
	int            iSampleRate;       // Sample rate
public:
	// new interface... but should be converted to QString...
	QString protocol(){ return szType; };
	bool isActive(){ return bActive; };
	QString remoteNick(){ return szNick; };
	QString remoteUser(){ return szUser; };
	QString remoteHost(){ return szHost; };
	QString remoteIp(){ return szIp; };
	QString remotePort(){ return szPort; };
	QString remoteFileName(){ return szFileName; };
	QString remoteFileSize(){ return szFileSize; };
	QString localNick(){ return szLocalNick; };
	QString localUser(){ return szLocalUser; };
	QString localHost(){ return szLocalHost; };
	QString localIp(){ return szIp; };
	QString localPort(){ return szPort; };
	QString localFileName(){ return szLocalFileName; };
	QString localFileSize(){ return szLocalFileSize; };
	bool isFileUpload();
	bool isFileDownload();
	bool isDccChat();
	bool isFileTransfer(){ return (isFileUpload() || isFileDownload()); };
};




#endif //_DESCRIPTOR_H_
