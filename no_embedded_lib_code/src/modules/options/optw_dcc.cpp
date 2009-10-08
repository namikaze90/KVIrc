//=============================================================================
//
//   File : optw_dcc.cpp
//   Creation date : Fri Aug 17 22:08:09 2001 GMT by Szymon Stefanek
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

#include "optw_dcc.h"

#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>


KviDccOptionsWidget::KviDccOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
}

KviDccOptionsWidget::~KviDccOptionsWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviDccGeneralOptionsWidget::KviDccGeneralOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("dccgeneral_options_widget");
	createLayout();

	KviBoolSelector * b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Use workaround for firewall","options"),KviOption_boolCantAcceptIncomingDccConnections);

	mergeTip(b,__tr2qs_ctx("<center>Enable this option if you can't accept incoming connections.<br>" \
			"KVIrc will try to use different methods to send and receive files.<br>" \
			"Please note that these methods may NOT work when communicating with a non-KVIrc client.</center>","options"));

	addRowSpacer(0,1,0,1);
}

KviDccGeneralOptionsWidget::~KviDccGeneralOptionsWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviDccAdvancedOptionsWidget::KviDccAdvancedOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("dccgeneral_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("Network Properties","options"));
	
	KviBoolSelector * b = addBoolSelector(g,__tr2qs_ctx("Use user-defined address or network interface","options"),KviOption_boolDccListenOnSpecifiedInterfaceByDefault);


	mergeTip(b,__tr2qs_ctx("<center>Enable this option if you are on a multihost machine and want " \
		"to force one of the available IP addresses to be used for outgoing DCCs.<br>This is especially useful " \
		"when you use IPv6 and IPv4 addresses.<br>You can force KVIrc to always choose the IPv4 interface.</center>","options"));

	KviStringSelector * s = addStringSelector(g,__tr2qs_ctx("Listen on address/interface:","options"),KviOption_stringDccListenDefaultInterface,KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault));

	mergeTip(s,__tr2qs_ctx("<center>This is the IP address or name of the interface to use by default for outgoing DCC transfers.<br>" \
		"On UNIX systems that support it, you can also specify IPv4 interface names (such as <b>ppp0</b>).<br>" \
		"If you set it to <b>0.0.0.0</b>, KVIrc will try to use the first available IPv4 interface</center>","options"));

	connect(b,SIGNAL(toggled(bool)),s,SLOT(setEnabled(bool)));

 	b = addBoolSelector(g,__tr2qs_ctx("Use user-defined port range","options"),KviOption_boolUserDefinedPortRange);

	mergeTip(b,__tr2qs_ctx("<center>Enable this option if you want specify a local port range for DCC.</center>","options"));

	KviTalHBox * hb = new KviTalHBox(g);
	hb->setSpacing(4);
	KviUIntSelector * u = addUIntSelector(hb,__tr2qs_ctx("Lowest port:","options"),KviOption_uintDccMinPort,1,65535,5000,KVI_OPTION_BOOL(KviOption_boolUserDefinedPortRange));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));

	u = addUIntSelector(hb,__tr2qs_ctx("Highest port:","options"),KviOption_uintDccMaxPort,1,65535,30000,KVI_OPTION_BOOL(KviOption_boolUserDefinedPortRange));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));

	b = addBoolSelector(g,__tr2qs_ctx("Send a fixed address in requests","options"),KviOption_boolDccSendFakeAddressByDefault);

	mergeTip(b,__tr2qs_ctx("<center>Enable this option if you want to always send a fake IP address in your DCC requests.<br>" \
		"This might be useful if you're behind a router with a static address that does network address translation (NAT) and forwards all or a range of ports.</center>","options"));

	s = addStringSelector(g,__tr2qs_ctx("Send address/interface:","options"),KviOption_stringDefaultDccFakeAddress,KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault));
	connect(b,SIGNAL(toggled(bool)),s,SLOT(setEnabled(bool)));

	mergeTip(s,__tr2qs_ctx("<center>This is the fixed address that will be sent with all DCC requests if you enable the option above.</center>","options"));

	KviBoolSelector * b2;
	b2 = addBoolSelector(g,__tr2qs_ctx("Guess address from IRC server if unroutable","options"),KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable,!KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault));
	connect(b,SIGNAL(toggled(bool)),b2,SLOT(setNotEnabled(bool)));

	mergeTip(b2,__tr2qs_ctx("<center>You can enable this option if you are behind a router that forwards all or a range of ports.<br>" \
			"KVIrc will try to guess the IP address to use for DCC by looking up the local hostname as seen " \
			"by the IRC server you're connected to.<br>This method is an exclusive alternative to the \"fixed address\" above.<br>" \
			"It might guess the correct address automatically if certain conditions are met (e.g. the IRC server does not mask hostnames).</center>","options"));

	b = addBoolSelector(g,__tr2qs_ctx("Use \"broken bouncer hack\" to detect address","options"),KviOption_boolDccBrokenBouncerHack,KVI_OPTION_BOOL(KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable));
	mergeTip(b,__tr2qs_ctx("<center>When you're behind a dialup router and also tunneling through a psyBNC bouncer, " \
				"you can use a bug in the bouncer to force KVIrc to bind the DCC connections to the dialup router's address.<br>" \
				"It's an ugly hack - use it only if nothing else works.</center>","options"));
									
	connect(b2,SIGNAL(toggled(bool)),b,SLOT(setEnabled(bool)));

	b = addBoolSelector(0,1,1,1,__tr2qs_ctx("Notify failed DCC handshakes to the remote end","options"),KviOption_boolNotifyFailedDccHandshakes);
	mergeTip(b,__tr2qs_ctx("<center>If you enable this option, when a DCC request from a remote " \
					"user can't be satisfied KVIrc will notify him by a CTCP ERRMSG. This is " \
					"a nice feature so it is a good idea to leave it on unless for some reason " \
					"you have deactivated the antiflood system: in this case turning off this option " \
					"might help if you often get attacked by CTCP floods.</center>","options"));

	u = addUIntSelector(0,2,1,2,__tr2qs_ctx("Maximum number of DCC sessions","options"),KviOption_uintMaxDccSlots,0,1000,64);
	mergeTip(u,__tr2qs_ctx("<center>This is the maximum number of concurrent DCC sessions " \
					"and it includes all the DCC types (send,chat,recv...). " \
					"KVIrc will refuse the requests when this limit is reached.</center>","options"));

	addUIntSelector(0,3,1,3,__tr2qs_ctx("DCC socket timeout:","options"),KviOption_uintDccSocketTimeout,10,65536,180);
	u->setSuffix(__tr2qs_ctx(" sec","options"));
	mergeTip(u,__tr2qs_ctx("<center>This is the amount of time that KVIrc will wait for a response before assuming that a DCC has failed " \
		"because the remote client was unable to connect to our listening socket.</center>","options"));

	addRowSpacer(0,4,1,4);
}

KviDccAdvancedOptionsWidget::~KviDccAdvancedOptionsWidget()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviDccSendOptionsWidget::KviDccSendOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("dccsend_options_widget");
}

KviDccSendOptionsWidget::~KviDccSendOptionsWidget()
{
}


KviDccSendGeneralOptionsWidget::KviDccSendGeneralOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("dccsend_options_widget");
	createLayout();

	KviBoolSelector * b;

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("On Incoming File","options"));
	KviBoolSelector * b2 = addBoolSelector(g,__tr2qs_ctx("Automatically accept","options"),KviOption_boolAutoAcceptDccSend);
	KviBoolSelector * b3 = addBoolSelector(g,__tr2qs_ctx("Open transfer window as minimized when auto-accepted","options"),KviOption_boolCreateMinimizedDccSendWhenAutoAccepted,!KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSend));
	KviBoolSelector * b4 = addBoolSelector(g,__tr2qs_ctx("Automatically resume when auto-accepted","options"),KviOption_boolAutoResumeDccSendWhenAutoAccepted,KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccSend));

	g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("Save Location","options"));
	addDirectorySelector(g,__tr2qs_ctx("Download folder:","options"),KviOption_stringIncomingPath);
	addBoolSelector(g,__tr2qs_ctx("Sort saved files by nicknames (create subfolders)","options"),KviOption_boolSortReceivedByDccFilesByNicks);
	addBoolSelector(g,__tr2qs_ctx("Guess save path from media type","options"),KviOption_boolUseIncomingDccMediaTypeSavePath);


	g = addGroupBox(0,2,0,2,Qt::Horizontal,__tr2qs_ctx("On Download Completion","options"));
	addBoolSelector(g,__tr2qs_ctx("Notify completion in console","options"),KviOption_boolNotifyDccSendSuccessInConsole);
	addBoolSelector(g,__tr2qs_ctx("Notify completion in notifier","options"),KviOption_boolNotifyDccSendSuccessInNotifier);
	b = addBoolSelector(g,__tr2qs_ctx("Automatically clear transfer","options"),KviOption_boolAutoCloseDccSendOnSuccess);
	mergeTip(b,
		__tr2qs_ctx("<center>This option will cause succesfully terminated transfers " \
				"to be automatically removed from the transfer window.</center>","options"));


	KviBoolSelector * b1 = addBoolSelector(0,3,0,3,__tr2qs_ctx("Always open transfer window as minimized","options"),KviOption_boolCreateMinimizedDccSend);

	connect(b1,SIGNAL(toggled(bool)),b3,SLOT(setNotEnabled(bool)));
	connect(b2,SIGNAL(toggled(bool)),b4,SLOT(setEnabled(bool)));

	addRowSpacer(0,4,0,4);
}

KviDccSendGeneralOptionsWidget::~KviDccSendGeneralOptionsWidget()
{
}


KviDccSendAdvancedOptionsWidget::KviDccSendAdvancedOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("dccsend_advanced_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("Bug Compatibility","options"));
	KviBoolSelector * b = addBoolSelector(g,__tr2qs_ctx("Send ACK for byte 0","options"),KviOption_boolSendZeroAckInDccRecv);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes KVIrc to send a zero-byte acknowledge to kick-start " \
				"the DCC transfer with some buggy IRC clients.<br>" \
				"Use it only if your DCC transfers stall just after establishing a connection without sending any data.</center>","options"));

	b = addBoolSelector(g,__tr2qs_ctx("Accept broken RESUME (mIRC file.ext)","options"),KviOption_boolAcceptBrokenFileNameDccResumeRequests);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes KVIrc to accept RESUME requests with invalid filenames.<br>" \
				"Use it if KVIrc fails to accept RESUME requests from other clients (e.g. some versions of mIRC).</center>","options"));

	b = addBoolSelector(g,__tr2qs_ctx("Replace spaces with underscores in outgoing filenames","options"),KviOption_boolDCCFileTransferReplaceOutgoingSpacesWithUnderscores);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes KVIrc to replace spaces with underscores in filenames " \
				"for all the outgoing file transfers. This will fix filename handling with some buggy clients (e.g. some versions of mIRC).","options"));

	g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("Limits","options"));

	KviTalHBox * hb = new KviTalHBox(g);
	b = addBoolSelector(hb,__tr2qs_ctx("Limit upload bandwidth to","options"),KviOption_boolLimitDccSendSpeed);
	KviUIntSelector * u = addUIntSelector(hb,"",KviOption_uintMaxDccSendSpeed,0,0xffffff1,1024,KVI_OPTION_BOOL(KviOption_boolLimitDccSendSpeed));
	u->setSuffix(" " + __tr2qs_ctx("bytes/sec","options"));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));
	
	hb = new KviTalHBox(g);
	b = addBoolSelector(hb,__tr2qs_ctx("Limit download bandwidth to","options"),KviOption_boolLimitDccRecvSpeed);
	u = addUIntSelector(hb,"",KviOption_uintMaxDccRecvSpeed,0,0xffffff1,1024,KVI_OPTION_BOOL(KviOption_boolLimitDccRecvSpeed));
	u->setSuffix(" " + __tr2qs_ctx("bytes/sec","options"));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));


	u = addUIntSelector(g,__tr2qs_ctx("Maximum number of DCC transfers","options"),KviOption_uintMaxDccSendTransfers,0,1000,10);
	mergeTip(u,__tr2qs_ctx("<center>This is the maximum number of concurrent DCC transfers. " \
					"KVIrc will refuse the requests when this limit is reached.</center>","options"));

	g = addGroupBox(0,2,0,2,Qt::Horizontal,__tr2qs_ctx("Tweaks","options"));

	b = addBoolSelector(g,__tr2qs_ctx("Use fast send (send ahead)","options"),KviOption_boolUseFastDccSend);
	mergeTip(b,
		__tr2qs_ctx("<center>The \"send ahead\" DCC method allows data to be sent faster by breaking " \
				"some of the rules of the original DCC SEND protocol specification.<br>" \
				"Most clients can handle this kind of optimisation so disable it only if you have problems.</center>","options"));

	hb = new KviTalHBox(g);

	b = addBoolSelector(hb,__tr2qs_ctx("Force idle step","options"),KviOption_boolDccSendForceIdleStep);
	mergeTip(b,
		__tr2qs_ctx("<center>Enable this option when the dcc file transfers " \
				"tend to block your computer by consuming too much CPU time. " \
				"When this option is enabled the idle interval below will be " \
				"forcibly inserted between each sent/received data packet.</center>","options"));

	u = addUIntSelector(hb,"",KviOption_uintDccSendIdleStepInMSec,1,65536,30,KVI_OPTION_BOOL(KviOption_boolDccSendForceIdleStep));
	connect(b,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));
	u->setSuffix(__tr2qs_ctx(" msec","options"));
	
	mergeTip(u,
		__tr2qs_ctx("<center>This parameter controls the average delay between two packets sent or received.<br>" \
				"A smaller interval will cause you to send data faster but will also " \
				"add load to your CPU, disk and network interface.<br>" \
				"Reasonable values are from 5 to 50 milliseconds.</center>","options"));

	u = addUIntSelector(g,__tr2qs_ctx("Packet size:","options"),KviOption_uintDccSendPacketSize,16,65536,1024);
	u->setSuffix(__tr2qs_ctx(" bytes","options"));
	mergeTip(u,
		__tr2qs_ctx("<center>This parameter controls the packet size used for DCC SEND.<br>" \
				"With bigger packets you will be probably send data faster, but " \
				"you will also saturate your bandwidth and in some cases " \
				"cause more disk activity.<br>" \
				"Reasonable values are from 512 to 4096 bytes.</center>","options"));

	addRowSpacer(0,3,0,4);
}

KviDccSendAdvancedOptionsWidget::~KviDccSendAdvancedOptionsWidget()
{
}


//KviDccSendOptionsWidget::enableStuff(bool)
//{
//	m_pOpenMinimizedAutoAccept->setEnabled(m_pAutoAccept->isChecked() && !m_pOpenMinimized->isChecked());
//}


KviDccChatOptionsWidget::KviDccChatOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("dccchat_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("On Chat Request","options"));
	KviBoolSelector * b = addBoolSelector(g,__tr2qs_ctx("Automatically accept","options"),KviOption_boolAutoAcceptDccChat);
	b = addBoolSelector(g,__tr2qs_ctx("Open minimized when auto-accepted","options"),KviOption_boolCreateMinimizedDccChatWhenAutoAccepted,!KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat));

	KviBoolSelector * b1 = addBoolSelector(0,1,0,1,__tr2qs_ctx("Always open as minimized","options"),KviOption_boolCreateMinimizedDccChat);

	connect(b1,SIGNAL(toggled(bool)),b,SLOT(setNotEnabled(bool)));

	addRowSpacer(0,2,0,2);
}

KviDccChatOptionsWidget::~KviDccChatOptionsWidget()
{
}


KviDccVoiceOptionsWidget::KviDccVoiceOptionsWidget(QWidget *p):KviOptionsWidget(p)
{
	createLayout();

	KviBoolSelector * b1 = addBoolSelector(0,0,0,0,__tr2qs_ctx("Open all minimized","options"),KviOption_boolCreateMinimizedDccVoice);
	KviTalGroupBox * g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("On Voice Request","options"));
	KviBoolSelector * b = addBoolSelector(g,__tr2qs_ctx("Automatically accept","options"),KviOption_boolAutoAcceptDccVoice);

	b = addBoolSelector(g,__tr2qs_ctx("Open minimized when auto-accepted","options"),KviOption_boolCreateMinimizedDccVoiceWhenAutoAccepted,!KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVoice));
	connect(b1,SIGNAL(toggled(bool)),b,SLOT(setNotEnabled(bool)));

	addBoolSelector(0,2,0,2,__tr2qs_ctx("Force half-duplex mode on sound device","options"), KviOption_boolDccVoiceForceHalfDuplex);
	addBoolSelector(0,3,0,3,__tr2qs_ctx("Volume slider controls PCM, not Master","options"), KviOption_boolDccVoiceVolumeSliderControlsPCM);

	addStringSelector(0,4,0,4,__tr2qs_ctx("Sound device:","options"), KviOption_stringDccVoiceSoundDevice);
	addStringSelector(0,5,0,5,__tr2qs_ctx("Mixer device:","options"), KviOption_stringDccVoiceMixerDevice);

	KviUIntSelector * u = addUIntSelector(0,6,0,6,__tr2qs_ctx("Pre-buffer size:","options"), KviOption_uintDccVoicePreBufferSize, 2048, 65536, 32000);
	u->setSuffix(" bytes");

	addRowSpacer(0,7,0,7);
}

KviDccVoiceOptionsWidget::~KviDccVoiceOptionsWidget()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_dcc.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES