//===============================================================================
//
//   File : setupwizard.cpp
//   Creation date : Sat Oct  6 02:06:53 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//==============================================================================

bool g_bFoundMirc;

#include "setupwizard.h"

#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_fileutils.h"
#include "kvi_defaults.h"
#include "kvi_msgbox.h"
#include "kvi_tal_filedialog.h"
#include "kvi_qstring.h"
#include "kvi_env.h"
#include "kvi_options.h"
#include "kvi_config.h"
#include <kvi_tal_textedit.h>

#include <QMessageBox>
#include <QDir>
#include <QPushButton>
#include <QValidator>
#include <QTextCodec>
#include <QWidget>
#include <QHBoxLayout>

#ifdef COMPILE_ON_WINDOWS
	#include <windows.h>
	#include <winnls.h>  // for MultiByteToWideChar
	#include <objbase.h> // CoCreateInstance , CoInitialize & CO.
	#include <shlobj.h>  // IShellLink IPersistFile & CO.
#else
	#include <unistd.h>  // for symlink()
#endif

// libkvisetup.cpp
extern QString g_szChoosenIncomingDirectory;
//extern int g_iThemeToApply;
extern bool bNeedToApplyDefaults;
extern unsigned int uPort;
extern QString szHost;
extern QString szUrl;
extern QString szMircServers;
extern QString szMircIni;

#ifdef COMPILE_ON_WINDOWS
	#define KVI_LOCAL_KVIRC_SUBDIRECTORY_NAME "KVIrc"
#else
	#define KVI_LOCAL_KVIRC_SUBDIRECTORY_NAME ".kvirc"
#endif

KviSetupPage::KviSetupPage(KviSetupWizard * w)
: KviTalHBox(w)
{
	// we need this to set localized text on buttons (see QT doc/ KviTalWizard class)
	w->KviTalWizard::backButton()->setText(__tr2qs("< &Back"));
	w->KviTalWizard::nextButton()->setText(__tr2qs("&Next >"));
	w->KviTalWizard::finishButton()->setText(__tr2qs("Finish"));
	w->KviTalWizard::cancelButton()->setText(__tr2qs("Cancel"));
	//w->KviTalWizard::helpButton()->setText(__tr2qs("Help"));

	setSpacing(8);

	m_pPixmapLabel = new QLabel(this);

	m_pPixmapLabel->setPixmap(*(w->m_pLabelPixmap));
	m_pPixmapLabel->setFixedSize(w->m_pLabelPixmap->size());
	m_pPixmapLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	m_pPixmapLabel->setMargin(0);

	m_pVBox = new QWidget(this);
	QVBoxLayout * pVLayout = new QVBoxLayout(this);
	pVLayout->setSpacing(8);
	m_pVBox->setLayout(pVLayout);
	

	QLabel * l = new QLabel(m_pVBox);
	l->setAlignment(Qt::AlignAuto | Qt::AlignTop);
	QString szHeader = "<table width=\"100%\"><tr><td bgcolor=\"#303030\">" \
			"<h1><font color=\"#FFFFFF\">KVIrc " KVI_VERSION "</font></h1>" \
			"</td></tr></table>";
	l->setText(szHeader);
	pVLayout->addWidget(l);

	m_pTextLabel = new QLabel(m_pVBox);
	m_pTextLabel->setWordWrap(true);
	m_pTextLabel->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
	pVLayout->addWidget(m_pTextLabel);
	pVLayout->setStretchFactor(m_pTextLabel,1);
}

KviSetupPage::~KviSetupPage()
{
}


KviSetupWizard::KviSetupWizard()
: KviTalWizard(0,0,true)
{
	g_bFoundMirc = false;
	QString szLabelText;

	QString szImagePath;
	g_pApp->getGlobalKvircDirectory(szImagePath,KviApp::Pics,"kvi_setup_label.png");

	m_pLabelPixmap = new QPixmap(szImagePath);
	if(m_pLabelPixmap->isNull())
	{
		delete m_pLabelPixmap;
		m_pLabelPixmap = new QPixmap(180,400);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Welcome

	m_pWelcome = new KviSetupPage(this);

	// here we go...
	QString szText  = __tr2qs("<p>" \
			"<h2>Welcome :)</h2>" \
			"This is your first time running this version of KVIrc.<br>" \
			"This wizard will guide you through the few steps " \
			"required to complete the setup.<br><br>" \
			"If you had a previous version of KVIrc installed, no worries. You will " \
			"have a chance to preserve the old configuration." \
			"</p>" \
			"<p>Click \"<b>Next</b>\" to proceed.</p>");

	m_pWelcome->m_pTextLabel->setText(szText);

	addPage(m_pWelcome,__tr2qs("Welcome to KVIrc"));

#ifdef COMPILE_ON_WINDOWS
	m_pCreateUrlHandlers = new QCheckBox(__tr2qs("Make KVIrc default IRC client"),m_pWelcome->m_pVBox);
	m_pCreateUrlHandlers->setChecked(true);
#endif
#ifdef COMPILE_KDE_SUPPORT
	m_pCreateDesktopShortcut = new QCheckBox(__tr2qs("Create desktop shortcut"),m_pWelcome->m_pVBox);
	m_pCreateDesktopShortcut->setChecked(true);
#endif
	setBackEnabled(m_pWelcome,false);
	setHelpEnabled(m_pWelcome,false);



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// License
#ifndef COMPILE_ON_WINDOWS //it have been already shown by installer
	KviSetupPage * m_pLicense = new KviSetupPage(this);
	m_pLicense->m_pTextLabel->setText(__tr2qs( \
			"<p>All of the files in this distribution are covered by the GPL. " \
			"In human terms this can be read as follows:<br>" \
			"<ul>" \
			"<li><b>KVIrc is free</b>, use it, have fun! <b>:)</b></li>" \
			"<li>If you use <b>any</b> part of KVIrc in your own project, you <b>must</b> release that project under the same license.</li>" \
			"</ul></p>" \
			"<p>The \"legalese\" version of the license is shown in the box below.</p>"));

	KviTalTextEdit * ed = new KviTalTextEdit(m_pLicense->m_pVBox);
	ed->setReadOnly(true);
	ed->setWordWrap(KviTalTextEdit::NoWrap);
	QString szLicense;
	QString szLicensePath;
	g_pApp->getGlobalKvircDirectory(szLicensePath,KviApp::License,"COPYING");
	if(!KviFileUtils::loadFile(szLicensePath,szLicense))
	{
		szLicense = __tr("Oops... can't find the license file.\n" \
						"It MUST be included in the distribution...\n" \
						"Please report to <pragma at kvirc dot net>");
	}
	ed->setText(szLicense);
	
	//m_pLicense->pVLayout->setStretchFactor(ed,1);

	addPage(m_pLicense,__tr2qs("Dreaded License Agreement"));

	setHelpEnabled(m_pLicense,false);

	setCaption(__tr2qs("KVIrc Setup"));
#else
	m_pLicense = 0;
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Directories

	m_pDirectory = new KviSetupPage(this);


	m_pDirectory->m_pTextLabel->setText(__tr2qs("<p>Please choose a folder for " \
		"KVIrc to store its settings and other data, and another for downloaded files. " \
		"Make sure that you have permission to write to both folders.<br><br>" \
		"The suggested locations are fine in most cases, so if you don't know what " \
		"is this about, just click \"<b>Next</b>\".<br><br>" \
		"If you had a previous version of KVIrc installed, you can choose " \
		"the existing directory for the settings, and your configuration " \
		"will be preserved." \
		"</p>"));

	addPage(m_pDirectory,__tr2qs("Application Folders"));

	QString tmp;

	m_pDirButtonGroup = new QButtonGroup(m_pDirectory->m_pVBox);
	m_pDirButtonGroup->setObjectName(__tr2qs("Store configuration in folder"));
	m_pDirUsePrev = new QRadioButton(__tr2qs("Use settings folder from previous installation"),this);
	connect(m_pDirUsePrev,SIGNAL(clicked()),this,SLOT(oldDirClicked()));
	m_pDirButtonGroup->addButton(m_pDirUsePrev);
	
	m_pOldPathBox = new QWidget(this);
	QHBoxLayout * pHLayout = new QHBoxLayout(this);
	m_pOldPathBox->setLayout(pHLayout);

	m_pOldDataPathEdit = new QLineEdit(m_pOldPathBox);
	connect(m_pOldDataPathEdit,SIGNAL(textChanged ( const QString & )),this,SLOT(oldDataTextChanged ( const QString & )));
	pHLayout->addWidget(m_pOldDataPathEdit);
	
	QPushButton * pb = new QPushButton(__tr2qs("&Browse..."),m_pOldPathBox);
	connect(pb,SIGNAL(clicked()),this,SLOT(chooseOldDataPath()));
	pHLayout->setSpacing(3);
	pHLayout->setStretchFactor(m_pOldDataPathEdit,1);
	pHLayout->addWidget(pb);
	
	m_pDirUseNew = new QRadioButton(__tr2qs("Use new settings folder"),this);
	connect(m_pDirUseNew,SIGNAL(clicked()),this,SLOT(newDirClicked()));
	m_pDirButtonGroup->addButton(m_pDirUseNew);
	
	QLabel* l = new QLabel(this);
	l->setText(__tr2qs("Settings folder:"));

	m_pNewPathBox = new QWidget(this);
	QHBoxLayout * pH2Layout = new QHBoxLayout(this);
	m_pNewPathBox->setLayout(pH2Layout);

	m_pDataPathEdit = new QLineEdit(m_pNewPathBox);
	pH2Layout->addWidget(m_pDataPathEdit);
	
	pb = new QPushButton(__tr2qs("&Browse..."),m_pNewPathBox);
	connect(pb,SIGNAL(clicked()),this,SLOT(chooseDataPath()));
	pH2Layout->addWidget(pb);

	pH2Layout->setSpacing(3);
	pH2Layout->setStretchFactor(m_pDataPathEdit,1);

#ifdef COMPILE_ON_WINDOWS
	tmp = QTextCodec::codecForLocale()->toUnicode(getenv( "APPDATA" ));
	if(tmp.isEmpty()) tmp = QDir::homeDirPath();
#else
	tmp = QDir::homeDirPath();
#endif //COMPILE_ON_WINDOWS
	KviQString::ensureLastCharIs(tmp,KVI_PATH_SEPARATOR_CHAR);
	tmp.append(KVI_LOCAL_KVIRC_SUBDIRECTORY_NAME);
	KviFileUtils::adjustFilePath(tmp);
	m_pDataPathEdit->setText(tmp);



	l = new QLabel(this);
	l->setText(__tr2qs("Download files to folder:"));


	m_pNewIncomingBox = new QWidget(this);
	QHBoxLayout * pH3Layout = new QHBoxLayout(this);
	m_pNewIncomingBox->setLayout(pH3Layout);

	m_pIncomingPathEdit = new QLineEdit(m_pNewIncomingBox);
	pH3Layout->addWidget(m_pIncomingPathEdit);
	
	pb = new QPushButton(__tr2qs("&Browse..."),m_pNewIncomingBox);
	connect(pb,SIGNAL(clicked()),this,SLOT(chooseIncomingPath()));
	pH3Layout->addWidget(pb);

	pH3Layout->setSpacing(3);
	pH3Layout->setStretchFactor(m_pIncomingPathEdit,1);

	tmp = QDir::homeDirPath();
	KviQString::ensureLastCharIs(tmp,KVI_PATH_SEPARATOR_CHAR);
	tmp.append(KVI_DEFAULT_INCOMING_SUBDIRECTORY_NAME);
	KviFileUtils::adjustFilePath(tmp);
	m_pIncomingPathEdit->setText(tmp);
	
	m_pDirUseNew->toggle();
	newDirClicked();

#ifdef COMPILE_ON_WINDOWS
	m_pDirMakePortable = new QRadioButton(__tr2qs("All settings in  shared program folder (portable)"),this);
	m_pDirButtonGroup->addButton(m_pDirMakePortable);
#endif
	m_pDirRestore = new QRadioButton(__tr2qs("Restore from backup archive"),this);
	m_pDirRestore->setEnabled(FALSE);
	m_pDirButtonGroup->addButton(m_pDirRestore);
	
	l = new QLabel(m_pDirectory->m_pVBox,"<b> </b>");

	//m_pDirectory->m_pVBox->setStretchFactor(m_pDirectory->m_pTextLabel,1);

	setHelpEnabled(m_pDirectory,false);

	connect(m_pDataPathEdit,SIGNAL(textChanged ( const QString & )),this,SLOT(newDataTextChanged ( const QString & )));
	connect(m_pIncomingPathEdit,SIGNAL(textChanged ( const QString & )),this,SLOT(newIncomingTextChanged ( const QString & )));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Identity

	m_pIdentity = new KviSetupPage(this);

	m_pIdentity->m_pTextLabel->setText(__tr2qs("Please choose a Nickname.<br><br>" \
		"Your nickname is the name that other IRC users will know you by. " \
		"It can't contain spaces or punctuation. Some IRC networks will shorten your nickname if it is more than 32 characters " \
		"long.<br><br>"
		"If in doubt, just enter the first nick that comes to mind. " \
		"You will be able to change it later in the Identity properties, or with the /NICK command."));

	QString nick;
	char * nnn = kvi_getenv("USER");
	if(nnn)nick = nnn;
	else nick = "newbie";
	if(nick.isEmpty())nick = "newbie";
	if(nick == "root")nick = "newbie";
//m_pIdentity->m_pVBox
	KviTalGroupBox * gbox = new KviTalGroupBox(1,Qt::Horizontal,__tr2qs("Basic Properties"),m_pIdentity->m_pVBox);

	m_pNickSelector = new KviStringSelector(gbox,__tr2qs("Nickname:"),&(KVI_OPTION_STRING(KviOption_stringNickname1)),true);
	m_pNickSelector->setMinimumLabelWidth(120);
	
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"),gbox);
	m_pNickSelector->setValidator(v);
	
	m_pRealNameSelector = new KviStringSelector(gbox,__tr2qs("Real name:"),&(KVI_OPTION_STRING(KviOption_stringRealname)),true);
	m_pRealNameSelector->setMinimumLabelWidth(120);


	QString szOptionalCtcpUserInfo = __tr2qs("This field is optional and will appear as part of the CTCP USERINFO reply.");
	QString szCenterBegin("<center>");
	QString szCenterEnd("</center>");
	QString szTrailing = "<br><br>" + szOptionalCtcpUserInfo + szCenterEnd;

	gbox = new KviTalGroupBox(1,Qt::Horizontal,__tr2qs("Profile"),m_pIdentity->m_pVBox);

	QWidget * hb = new QWidget(gbox);
	QHBoxLayout * pH4Layout = new QHBoxLayout(gbox);
	hb->setLayout(pH4Layout);
	pH4Layout->setSpacing(4);
	
	l = new QLabel(__tr2qs("Age:"),hb);
	l->setMinimumWidth(120);
	pH4Layout->addWidget(l);
	
	m_pAgeCombo = new QComboBox(hb);
	m_pAgeCombo->insertItem(__tr2qs("Unspecified"));
	unsigned int i;
	for(i=1;i<120;i++)
	{
		QString tmp;
		tmp.setNum(i);
		m_pAgeCombo->insertItem(tmp);
	}

	bool bOk;
	i = KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge).toUInt(&bOk);
	if(!bOk)i = 0;
	if(i > 120)i = 120;
	m_pAgeCombo->setCurrentItem(i);
	pH4Layout->addWidget(m_pAgeCombo);
	pH4Layout->setStretchFactor(m_pAgeCombo,1);

	hb = new QWidget(gbox);
	QHBoxLayout * pH5Layout = new QHBoxLayout(gbox);
	hb->setLayout(pH5Layout);
	pH5Layout->setSpacing(4);
	
	l = new QLabel(__tr2qs("Gender:"),hb);
	l->setMinimumWidth(120);
	pH5Layout->addWidget(l);

	m_pGenderCombo = new QComboBox(hb);
	m_pGenderCombo->insertItem(__tr2qs("Unspecified"));
	m_pGenderCombo->insertItem(__tr2qs("Female"));
	m_pGenderCombo->insertItem(__tr2qs("Male"));

	if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender),"Male"))
		m_pGenderCombo->setCurrentItem(2);
	else if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender),"Female"))
		m_pGenderCombo->setCurrentItem(1);
	else
		m_pGenderCombo->setCurrentItem(0);

	pH5Layout->addWidget(m_pGenderCombo);
	pH5Layout->setStretchFactor(m_pGenderCombo,1);

	m_pLocationSelector = new KviStringSelector(gbox,__tr2qs("Location:"),&(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLocation)),true);
	m_pLocationSelector->setMinimumLabelWidth(120);

	m_pLanguagesSelector = new KviStringSelector(gbox,__tr2qs("Languages:"),&(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLanguages)),true);
	m_pLanguagesSelector->setMinimumLabelWidth(120);

	m_pOtherInfoSelector = new KviStringSelector(gbox,__tr2qs("Other:"),&(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoOther)),true);
	m_pOtherInfoSelector->setMinimumLabelWidth(120);

	addPage(m_pIdentity,__tr2qs("Identity"));

	l = new QLabel(m_pIdentity->m_pVBox,"<b> </b>");

	setHelpEnabled(m_pIdentity,false);

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Default theme
/*	m_pTheme = new KviSetupPage(this);
	m_pTheme->m_pTextLabel->setText(__tr2qs( \
		"<p>Here you can choose the default appearance of KVIrc.<br><br>" \
		"The Fancy Theme uses icons, a transparent background and a lot of colors. " \
		"The Minimalist Theme is designed for low-color displays " \
		"or for \"console\" extremists; it's more or less white text on a black background.<br><br>" \
		"If you had a previous version of KVIrc installed, you can choose to not apply any theme " \
		"in order to preserve your current visual settings.<br><br>" \
		"If you don't know what to choose, just use the default.</p>"));

	m_pThemeButtonGroup = new KviTalVButtonGroup(m_pTheme->m_pVBox);
	
	KviTalHBox* pThemesHb = new KviTalHBox(m_pThemeButtonGroup);

	KviTalVBox* pThemesVb = new KviTalVBox(pThemesHb);
	
	QString szThemeImagePath;
	g_pApp->getGlobalKvircDirectory(szThemeImagePath,KviApp::Pics,"kvi_setup_theme_hires.png");
	
	QPixmap* pHiResPixmap = new QPixmap(szThemeImagePath);
	if(pHiResPixmap->isNull())
	{
		delete pHiResPixmap;
		pHiResPixmap= new QPixmap(250,200);
	}
	
	QLabel* pPixmapLabelHiRes = new QLabel(pThemesVb);

	pPixmapLabelHiRes->setPixmap(*pHiResPixmap);
	pPixmapLabelHiRes->setFixedSize(pHiResPixmap->size());
	pPixmapLabelHiRes->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	pPixmapLabelHiRes->setMargin(0);
	
	m_pThemeHiRes = new QRadioButton(__tr2qs("&Fancy Theme"),pThemesVb);
	
	pThemesVb = new KviTalVBox(pThemesHb);
	g_pApp->getGlobalKvircDirectory(szThemeImagePath,KviApp::Pics,"kvi_setup_theme_lowres.png");
	QPixmap* pLowResPixmap = new QPixmap(szThemeImagePath);
	if(pLowResPixmap->isNull())
	{
		delete pLowResPixmap;
		pLowResPixmap= new QPixmap(250,200);
	}
	
	QLabel* pPixmapLabelLowRes = new QLabel(pThemesVb);

	pPixmapLabelLowRes->setPixmap(*pLowResPixmap);
	pPixmapLabelLowRes->setFixedSize(pLowResPixmap->size());
	pPixmapLabelLowRes->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	pPixmapLabelLowRes->setMargin(0);
	
	m_pThemeLoRes  = new QRadioButton(__tr2qs("&Minimalist Theme"),pThemesVb);
	m_pThemeNone = new QRadioButton(__tr2qs("&Don't apply any theme"),m_pThemeButtonGroup);
	m_pThemeButtonGroup->insert(m_pThemeHiRes);
	m_pThemeButtonGroup->insert(m_pThemeLoRes);
	
	m_pThemeHiRes->setChecked(true);

	addPage(m_pTheme,__tr2qs("Default Theme"));

	setHelpEnabled(m_pTheme,false);*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Server config
	m_pServers = new KviSetupPage(this);

	m_pServers->m_pTextLabel->setText(__tr2qs( \
			"Now you should specify an IRC server, to be connected to it"));


	m_pServersButtonGroup = new QButtonGroup(m_pServers->m_pVBox);
	m_pServersButtonGroup->setObjectName(__tr2qs("Choose a server to connect"));
	
	m_pServersChooseFromList = new QRadioButton(__tr2qs("Choose from built-in server list"),this);
	m_pServersButtonGroup->addButton(m_pServersChooseFromList);
	
	m_pServersSpecifyManually = new QRadioButton(__tr2qs("Specify server manually"),this);
	m_pServersButtonGroup->addButton(m_pServersSpecifyManually);

	hb = new QWidget(this);
	QHBoxLayout * pH6Layout = new QHBoxLayout(this);
	hb->setLayout(pH6Layout);
	
	m_uServerPort=6667;
	m_pServerHostSelector = new KviStringSelector(hb,__tr2qs("Server:"),&m_szServerHost,true);
	m_pServerPortSelector = new KviUIntSelector(hb,__tr2qs("Port:"),&m_uServerPort,1,65536,6667,true,false);
	pH6Layout->addWidget(m_pServerHostSelector);
	pH6Layout->addWidget(m_pServerPortSelector);
	
	m_pServersOpenIrcUrl = new QRadioButton(__tr2qs("Open irc:// or irc6:// URL"),this);
	m_pServersButtonGroup->addButton(m_pServersOpenIrcUrl);
	m_szServerUrl="irc://";
	m_pServerUrlSelector = new KviStringSelector(this,__tr2qs("URL:"),&m_szServerUrl,true);

#ifdef COMPILE_ON_WINDOWS
	m_pUseMircServerList = new QRadioButton(__tr2qs("Import server list from mIRC"),this);
	m_pUseMircServerList->setEnabled(false);
	m_pServersButtonGroup->addButton(m_pUseMircServerList);
#endif

	m_pServersLoadConfig = new QRadioButton(__tr2qs("Use server config"),this);
	m_pServersLoadConfig->setEnabled(FALSE);
	m_pServersButtonGroup->addButton(m_pServersLoadConfig);

	m_pServerConfigSelector = new KviFileSelector(this,__tr2qs("Config file:"),&m_szServerConfigFile,true);
	m_pServerConfigSelector->setEnabled(FALSE);

	m_pServersChooseFromList->toggle();
	addPage(m_pServers,__tr2qs("Choose a server to connect"));
	setFinishEnabled(m_pServers,true);
	setHelpEnabled(m_pServers,false);

	// Preconfigured values
#ifdef COMPILE_ON_WINDOWS
	QString szTmp;
	g_pApp->getGlobalKvircDirectory(szTmp,KviApp::Config,"preinstalled.kvc");
	if(KviFileUtils::fileExists(szTmp))
	{
		KviConfig cfg(szTmp,KviConfig::Read);
		cfg.setGroup("Setup");
		if(cfg.readBoolEntry("hideServerList",FALSE)) {
			setAppropriate(m_pServers,false);
			setFinishEnabled(m_pIdentity,true);
			KVI_OPTION_BOOL(KviOption_boolShowChannelsJoinOnIrc) = false;
			KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart) = false;
		}
		int iDir;
		iDir=cfg.readIntEntry("settingsDir",-1);
		if(iDir>0) {
			switch(iDir)
			{
				case 1:
					m_pDirUseNew->toggle();
					setAppropriate(m_pDirectory,false);
					break;
				case 2:
					m_pDirMakePortable->toggle();
					setAppropriate(m_pDirectory,false);
					break;
			}
		}
	}

	//mIRC import
	#define QUERY_BUFFER 2048
	char* buffer;
	DWORD len = QUERY_BUFFER;
	buffer = (char*)malloc(len*sizeof(char));
	HKEY hKey;
	QString szMircDir;

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"ChatFile\\DefaultIcon",0,KEY_READ,&hKey) == ERROR_SUCCESS )
	{
		if( RegQueryValueEx( hKey,0,0,0,(LPBYTE)buffer,&len) == ERROR_SUCCESS)
		{
			szMircDir = QString::fromLocal8Bit(buffer,len);

			szMircDir.remove('"');
			QString szMircFile = KviFileUtils::extractFileName(szMircDir);
			szMircFile = szMircFile.left(szMircFile.length()-4); //cut off ".exe"
			szMircDir = KviFileUtils::extractFilePath(szMircDir);

			szMircIni = szMircDir + "/" + szMircFile + ".ini";

			if(!KviFileUtils::fileExists(szMircIni))
				szMircIni = szMircDir + "/mirc.ini";

			if(!KviFileUtils::fileExists(szMircIni))
				szMircIni = szMircDir + "/pirc.ini";

			if(KviFileUtils::fileExists(szMircIni)){
				KviConfig cfg(szMircIni,KviConfig::Read,true);
				if(cfg.hasGroup("mirc"))
				{
					g_bFoundMirc = true;
					cfg.setGroup("mirc");
					m_pNickSelector->setText(cfg.readQStringEntry("nick",KVI_OPTION_STRING(KviOption_stringNickname1)));
					m_pRealNameSelector->setText(cfg.readQStringEntry("user",KVI_OPTION_STRING(KviOption_stringRealname)));
					KVI_OPTION_STRING(KviOption_stringNickname2) = 
						cfg.readQStringEntry("anick",KVI_OPTION_STRING(KviOption_stringNickname2));
					KVI_OPTION_STRING(KviOption_stringUsername)  = 
						cfg.readQStringEntry("email",KVI_OPTION_STRING(KviOption_stringUsername)).section('@',0,0);

					if(cfg.hasGroup("files"))
					{
						m_szMircServerIniFile = cfg.readQStringEntry("servers","servers.ini");
						m_szMircServerIniFile.prepend('/');
						m_szMircServerIniFile.prepend(szMircDir);
						if(KviFileUtils::fileExists(m_szMircServerIniFile)){
							m_pUseMircServerList->setEnabled(true);
							m_pUseMircServerList->setChecked(true);
						}
					}
					KviMessageBox::information(__tr2qs("Setup found existing mIRC installation. It will try to import "
						"some of mIRC settings and serverlist. If you don't want to do it, unselect import in setup pages"));
				}
			}
		}
	}
	free(buffer);
	
#endif
}


KviSetupWizard::~KviSetupWizard()
{
	delete m_pLabelPixmap;
}

void KviSetupWizard::oldDirClicked()
{
	m_pOldPathBox->setEnabled(true);
	m_pNewPathBox->setEnabled(false);
	m_pNewIncomingBox->setEnabled(false);
	
	if(m_pIdentity) setAppropriate(m_pIdentity,false);
//	if(m_pTheme) setAppropriate(m_pTheme,false);
	if(m_pServers) setAppropriate(m_pServers,false);
	
	if(m_pOldDataPathEdit->text().isEmpty()) setFinishEnabled(m_pDirectory,false);
	else setFinishEnabled(m_pDirectory,true);
}

void KviSetupWizard::oldDataTextChanged ( const QString & str)
{
	setNextEnabled(m_pDirectory,!str.isEmpty());
}

void KviSetupWizard::newDataTextChanged ( const QString & str)
{
	setNextEnabled(m_pDirectory,!str.isEmpty() && !m_pIncomingPathEdit->text().isEmpty());
}

void KviSetupWizard::newIncomingTextChanged ( const QString & str)
{
	setNextEnabled(m_pDirectory,!str.isEmpty() && !m_pDataPathEdit->text().isEmpty());
}

void KviSetupWizard::newDirClicked()
{
	m_pOldPathBox->setEnabled(false);
	m_pNewPathBox->setEnabled(true);
	m_pNewIncomingBox->setEnabled(true);
	
	if(m_pIdentity) setAppropriate(m_pIdentity,true);
//	if(m_pTheme) setAppropriate(m_pTheme,true);
	if(m_pServers) setAppropriate(m_pServers,true);
	
	if(m_pDataPathEdit->text().isEmpty() || m_pIncomingPathEdit->text().isEmpty()) setNextEnabled(m_pDirectory,false);
	else setNextEnabled(m_pDirectory,true);
}
void KviSetupWizard::chooseOldDataPath()
{
	QString szBuffer = KviTalFileDialog::getExistingDirectoryPath(m_pDataPathEdit->text(),__tr2qs("Choose an Old Configuration Folder - KVIrc Setup"),0);
	if(!szBuffer.isEmpty())
	{
		KviQString::ensureLastCharIs(szBuffer,KVI_PATH_SEPARATOR_CHAR);
		if(!g_pApp->checkLocalKvircDirectory(szBuffer))
		{
			if(
				QMessageBox::question(
				this,
				__tr2qs("Do not overwrite folder? - KVIrc"),
				tr("A folder %1 seems to be not a valid KVIrc settings folder."
					"Do you want to use it anyway?")
					.arg( szBuffer ),
				__tr2qs("&Yes"), __tr2qs("&No"),
				QString::null, 0, 1 ) == 0
			) {
				m_pOldDataPathEdit->setText(szBuffer);
			}
		} else {
			m_pOldDataPathEdit->setText(szBuffer);
		}
	}
}

void KviSetupWizard::chooseDataPath()
{
	QString szBuffer = KviTalFileDialog::getExistingDirectoryPath(m_pDataPathEdit->text(),__tr2qs("Choose a Configuration Folder - KVIrc Setup"),0);
	if(!szBuffer.isEmpty())
	{
		KviQString::ensureLastCharIs(szBuffer,KVI_PATH_SEPARATOR_CHAR);
		m_pDataPathEdit->setText(szBuffer);
	}
}

void KviSetupWizard::chooseIncomingPath()
{
	//QString szBuffer = QFileDialog::getExistingDirectory(m_pIncomingPathEdit->text(),0,0,__tr2qs("Choose the download folder"));
	QString szBuffer = KviTalFileDialog::getExistingDirectoryPath(m_pIncomingPathEdit->text(),__tr2qs("Choose a Download Folder - KVIrc Setup"),0);
	if(!szBuffer.isEmpty())
	{
		m_pIncomingPathEdit->setText(szBuffer);
	}
}


void KviSetupWizard::makeLink()
{
#ifdef COMPILE_ON_WINDOWS
	// Let's make a link on the desktop :)
	// You need this horrible snippet of code to create a shortcut!!!!
	//
	// you have to:
	// - dig in the registry , 
	// - trigger the entire COM subsystem
	// - bring up a couple of OLE interfaces....
	// - use some obscure functions like MultiByteToWideChar
	// - and the finally drop the entire OLE+COM interface in the garbadge
	// ...
	// Isn't this horrible ? (compared to symlink()!)
	//

	// Well..let's go (please note that we don't handle most possible errors!
	// otherwise there would be 150 lines for a stupid symlink!)

	HKEY hCU;
	DWORD lpType;
	ULONG ulSize = MAX_PATH;
	char szLink[MAX_PATH];

	// Dig in the registry looking up the Desktop path
	if(RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 
		0,KEY_QUERY_VALUE,&hCU) == ERROR_SUCCESS)
	{
		RegQueryValueEx(hCU,"Desktop",NULL,&lpType,
			(unsigned char *)&szLink,&ulSize);
		RegCloseKey(hCU);
	}

	// Build our paths
	QString szLinkTarget = szLink;
	szLinkTarget.append("\\kvirc.lnk");

	QString szKvircExec = g_pApp->m_szGlobalKvircDir;
	szKvircExec.append("\\kvirc.exe");

	// Trigger a horrible machinery
	CoInitialize(NULL); // we need COM+OLE

	// Fiddle with an obscure shell interface
	IShellLink* psl;

	// Get a pointer to the IShellLink interface: this is kinda ugly :)
	if(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,
		IID_IShellLink,(void **)&psl) == S_OK)
	{
		// just for fun , lookup another shell interface

		IPersistFile* ppf;

		// Query IShellLink for the IPersistFile interface for 
		// saving the shell link in persistent storage.
		if(psl->QueryInterface(IID_IPersistFile,(void **)&ppf) == S_OK)
		{
			WORD wsz[MAX_PATH];
			// Set the path to the shell link target.
			psl->SetPath(QTextCodec::codecForLocale()->fromUnicode(szKvircExec).data());
			// Set the description of the shell link.
			psl->SetDescription("kvirc");
			// Ensure string is ANSI.
			MultiByteToWideChar(CP_ACP,0,QTextCodec::codecForLocale()->fromUnicode(szLinkTarget).data(),-1,(LPWSTR)wsz,MAX_PATH);
			// Save the link via the IPersistFile::Save method.
			ppf->Save((LPCOLESTR)wsz,true);    
			ppf->Release();
		}
		psl->Release();
	}
	// And throw OLE & Co. in the garbadge
	CoUninitialize();
#endif //COMPILE_ON_WINDOWS

#ifdef COMPILE_KDE_SUPPORT
	QString tmp = QDir::homeDirPath();
	KviQString::ensureLastCharIs(tmp,KVI_PATH_SEPARATOR_CHAR);
	tmp.append("Desktop");
	KviQString::ensureLastCharIs(tmp,KVI_PATH_SEPARATOR_CHAR);
	tmp.append("kvirc.desktop");

	QString contents = "[Desktop Entry]\n" \
		"GenericName=IRC Client\n" \
		"Comment=Connect to Internet Relay Chat\n" \
		"Exec=kvirc -m %u\n" \
		"Icon=kvirc\n" \
		"MapNotify=true\n" \
		"Name=KVIrc 3\n" \
		"Terminal=false\n" \
		"Type=Application\n" \
		"X-KDE-SubstituteUID=false\n";

	KviFileUtils::writeFile(tmp,contents,false);
#endif //COMPILE_KDE_SUPPORT

}

void KviSetupWizard::setUrlHandlers()
{
#ifdef COMPILE_ON_WINDOWS
	QString szReg = "REGEDIT4\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc]\r\n" \
		"@=\"URL:IRC Protocol\"\r\n" \
		"\"IRC Protocol\"=\"http://www.kvirc.net/\"\r\n" \
		"\"EditFlags\"=hex:02,00,00,00\r\n" \
		"\"URL Protocol\"=""\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc\\DefaultIcon]\r\n" \
		"@=\"\\\"@KVIRCEXECUTABLE@\\\"\"\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc\\shell]\r\n" \
		"@=\"\"\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc\\shell\\open]\r\n" \
		"\"EditFlags\"=hex:01,00,00,00\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc\\shell\\open\\command]\r\n" \
		"@=\"\\\"@KVIRCEXECUTABLE@\\\" %1\"\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc6]\r\n" \
		"@=\"URL:IRC6 Protocol\"\r\n" \
		"\"IRC6 Protocol\"=\"http://www.kvirc.net/\"\r\n" \
		"\"EditFlags\"=hex:02,00,00,00\r\n" \
		"\"URL Protocol\"=""\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc6\\DefaultIcon]\r\n" \
		"@=\"\\\"@KVIRCEXECUTABLE@\\\"\"\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc6\\shell]\r\n" \
		"@=\"\"\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc6\\shell\\open]\r\n" \
		"\"EditFlags\"=hex:01,00,00,00\r\n" \
		"\r\n" \
		"[HKEY_CLASSES_ROOT\\irc6\\shell\\open\\command]\r\n" \
		"@=\"\\\"@KVIRCEXECUTABLE@\\\" %1\"\r\n";

	QString szKvircExec = g_pApp->m_szGlobalKvircDir;
	szKvircExec.append("\\kvirc.exe");
	szKvircExec.replace("\\","\\\\");

	szReg.replace("@KVIRCEXECUTABLE@",QTextCodec::codecForLocale()->fromUnicode(szKvircExec));

	QString szRegFile = g_pApp->m_szGlobalKvircDir;
	szRegFile.append("\\kvirc.reg");

	KviFileUtils::writeFile(szRegFile,szReg,false);

	QString szCmd = "regedit /S \"";
	szCmd += szRegFile;
	szCmd += "\"";

	WinExec(QTextCodec::codecForLocale()->fromUnicode(szCmd).data(),SW_SHOW);
#endif
}

void KviSetupWizard::reject()
{
	if(QMessageBox::warning(this,__tr2qs("Abort Setup - KVIrc Setup"),
		__tr2qs("You have chosen to abort the setup.<br>KVIrc cannot run until you complete this procedure.<br><br>Do you really wish to abort?"),
		QMessageBox::Yes,QMessageBox::No|QMessageBox::Default|QMessageBox::Escape) != QMessageBox::Yes)return;

	KviTalWizard::reject();
}

void KviSetupWizard::accept()
{
	QString szDir;
	
	if(m_pDirUsePrev->isOn())
	{
		bNeedToApplyDefaults=false;
		g_pApp->m_szLocalKvircDir =  m_pOldDataPathEdit->text();
	} else {
		bNeedToApplyDefaults=true;
		if(m_pDirUseNew->isOn()) {
			szDir = m_pDataPathEdit->text();
		} 
#ifdef COMPILE_ON_WINDOWS
		else { //portable
			szDir = g_pApp->applicationDirPath()+KVI_PATH_SEPARATOR_CHAR+"Settings";
		}
#endif

		KviFileUtils::adjustFilePath(szDir);
		KviQString::ensureLastCharIs(szDir,KVI_PATH_SEPARATOR_CHAR);
		if(!KviFileUtils::directoryExists(szDir))
		{
			if(!KviFileUtils::makeDir(szDir))
			{
				KviMessageBox::warning(__tr("Cannot create directory %s.\n" \
					"You may not have write permission " \
					"for that path. Please go back and choose another directory."));
				showPage(m_pDirectory);
				return;
			}
		} /*else {
			kvi_infoBox(__tr("Kvirc setup"),
				__tr("The directory '%s' already exists.\n" \
				"(maybe from a previous Kvirc installation)\n"\
				"If you experience any problems try deleting the old directory:\n" \
				"the setup program will be started automatically again."),szDir.ptr());
		}*/
	
		g_pApp->m_szLocalKvircDir = szDir;
		KviFileUtils::adjustFilePath(g_pApp->m_szLocalKvircDir);

		if(m_pDirUseNew->isOn()) {
			szDir = m_pIncomingPathEdit->text();
		} 
#ifdef COMPILE_ON_WINDOWS
		else { //portable
			szDir = g_pApp->applicationDirPath()+KVI_PATH_SEPARATOR_CHAR+"Downloads";
		}
#endif

		KviFileUtils::adjustFilePath(szDir);
		if(!KviFileUtils::directoryExists(szDir))
		{
			if(!KviFileUtils::makeDir(szDir))
			{
				KviMessageBox::warning(__tr("Cannot create directory %s.\n" \
					"You may not have write permission " \
					"for that path. Please go back and choose another directory."));
				showPage(m_pDirectory);
				return;
			}
		}

		g_szChoosenIncomingDirectory = szDir;
	
#ifndef COMPILE_ON_WINDOWS
		// Make local->global link
		QString localPath = QString("%1/global").arg(g_pApp->m_szLocalKvircDir);
		unlink(QTextCodec::codecForLocale()->fromUnicode(localPath).data());
		symlink(QTextCodec::codecForLocale()->fromUnicode(g_pApp->m_szGlobalKvircDir).data(),QTextCodec::codecForLocale()->fromUnicode(localPath).data());
#endif
	
#ifdef COMPILE_KDE_SUPPORT
		if(m_pCreateDesktopShortcut->isChecked())
			makeLink();
#endif
	
#ifdef COMPILE_ON_WINDOWS
		if(m_pCreateUrlHandlers->isChecked())
			setUrlHandlers();
#endif
	
/*		if(m_pTheme)
		{
			if(m_pThemeButtonGroup->selected() == m_pThemeHiRes)
			{
				g_iThemeToApply = THEME_APPLY_HIRES;
			} else if(m_pThemeButtonGroup->selected() == m_pThemeLoRes)
			{
				g_iThemeToApply = THEME_APPLY_LORES;
			} else {
				g_iThemeToApply = THEME_APPLY_NONE;
			}
		}
*/		
		if(m_pIdentity)
		{
			m_pNickSelector->commit();
			m_pRealNameSelector->commit();
			m_pLocationSelector->commit();
			m_pLanguagesSelector->commit();
			m_pOtherInfoSelector->commit();
			
			KVI_OPTION_STRING(KviOption_stringNickname1).stripWhiteSpace();
			KVI_OPTION_STRING(KviOption_stringNickname1).replace(" ","");
			
			if(KVI_OPTION_STRING(KviOption_stringNickname1).length() > 32)
			{
				QString tmp = KVI_OPTION_STRING(KviOption_stringNickname1).left(32);
				KVI_OPTION_STRING(KviOption_stringNickname1) = tmp;
			}
		
			if(KVI_OPTION_STRING(KviOption_stringNickname1).isEmpty())KVI_OPTION_STRING(KviOption_stringNickname1) = "newbie";
		
			QString szNickPart;
			if(KVI_OPTION_STRING(KviOption_stringNickname1).length() < 31)
			{
				szNickPart = KVI_OPTION_STRING(KviOption_stringNickname1);
			} else {
				szNickPart = KVI_OPTION_STRING(KviOption_stringNickname1).left(30);
			}
		
			QString alt = szNickPart;
			alt.prepend("|"); // <-- this is an erroneous nickname on IrcNet :/
			alt.append("|");
			if(!g_bFoundMirc)
				KVI_OPTION_STRING(KviOption_stringNickname2) = alt;
			alt = szNickPart;
			alt.prepend("_");
			alt.append("_");
			KVI_OPTION_STRING(KviOption_stringNickname3) = alt;
			alt = szNickPart;
			alt.append("2");
			KVI_OPTION_STRING(KviOption_stringNickname4) = alt;
			
			int i = m_pAgeCombo->currentItem();
			if(i < 0)i = 0;
			if(i > 120)i = 120;
			if(i <= 0)KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge) = "";
			else KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge).setNum(i);
			
			switch(m_pGenderCombo->currentItem())
			{
				case 1:
					// this should be in english
					KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "Female";
				break;
				case 2:
					// this should be in english
					KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "Male";
				break;
				default:
					KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "";
				break;
			}
			
			m_pServerHostSelector->commit();
			m_pServerUrlSelector->commit();
			m_pServerConfigSelector->commit();
			m_pServerPortSelector->commit();
			
			if(m_pServersSpecifyManually->isOn())
			{
				KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart) = FALSE;
				szHost = m_szServerHost;
				uPort=m_uServerPort;
			} else if(m_pServersOpenIrcUrl->isOn())
			{
				KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart) = FALSE;
				szUrl=m_szServerUrl;
			}
#ifdef COMPILE_ON_WINDOWS			
			else if (m_pUseMircServerList->isOn()) {
				szMircServers = m_szMircServerIniFile;
			}
#endif
		}
	}
#ifdef COMPILE_ON_WINDOWS
	if(m_pDirMakePortable->isOn()) {
		KviFileUtils::writeFile(g_pApp->applicationDirPath()+KVI_PATH_SEPARATOR_CHAR+"portable","true");
	} else {
#endif
		g_pApp->saveKvircDirectory();
#ifdef COMPILE_ON_WINDOWS
	}
#endif
	KviTalWizard::accept();
}
