//=============================================================================
//
//   File : libkvisnd.cpp
//   Creation date : Thu Dec 27 2002 17:13:12 GMT by Juanjo Alvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez (juanjux at yahoo dot es)
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvisnd.h"

#include "kvi_module.h"
#include "kvi_debug.h"
#include "kvi_fileutils.h"
#include "kvi_malloc.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"

#include <QSound>

#ifdef COMPILE_PHONON_SUPPORT
	#include <Phonon/MediaSource>
	#include <Phonon/MediaObject>
	#include <Phonon/Path>
	Phonon::MediaObject * g_pPhononPlayer=0;
	#include <QUrl>
#endif //!COMPILE_PHONON_SUPPORT

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#include <mmsystem.h>
#else //!COMPILE_ON_WINDOWS

	#include <QFile>
	#include <unistd.h>
	#include <errno.h>

	#ifdef COMPILE_ESD_SUPPORT
		#include <esd.h>
	#endif //COMPILE_ESD_SUPPORT

	#ifdef COMPILE_OSS_SUPPORT
		#include <fcntl.h>
		#include <sys/ioctl.h>
		#ifdef HAVE_LINUX_SOUNDCARD_H
			#include <linux/soundcard.h>
		#else
			// Hint by Andy Fawcett: Thnx :)
			#ifdef HAVE_SYS_SOUNDCARD_H
				#include <sys/soundcard.h>
			#else
				#ifdef HAVE_MACHINE_SOUNDCARD_H
					#include <machine/soundcard.h>
				#else
					#warning "Ops.. have no soundcard.h ? ... we're going to fail here :/"
				#endif
			#endif
		#endif
		#ifdef COMPILE_AUDIOFILE_SUPPORT
			#include <audiofile.h>
		#endif //COMPILE_AUDIOFILE_SUPPORT
	#endif //COMPILE_OSS_SUPPORT
#endif

static KviSoundPlayer * g_pSoundPlayer = 0;

KviSoundPlayer::KviSoundPlayer()
: QObject()
{
	m_pThreadList = new KviPointerList<KviSoundThread>;
	m_pThreadList->setAutoDelete(true);

	m_pSoundSystemDict = new KviPointerHashTable<QString,SoundSystemRoutine>(17,false);
	m_pSoundSystemDict->setAutoDelete(true);
#ifdef COMPILE_PHONON_SUPPORT
	m_pSoundSystemDict->insert("phonon",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playPhonon)));
#endif //!COMPILE_PHONON_SUPPORT
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pSoundSystemDict->insert("winmm",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playWinmm)));
#else //!COMPILE_ON_WINDOWS
	#ifdef COMPILE_OSS_SUPPORT
		#ifdef COMPILE_AUDIOFILE_SUPPORT
			m_pSoundSystemDict->insert("oss+audiofile",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playOssAudiofile)));
		#endif //COMPILE_AUDIOFILE_SUPPORT
		m_pSoundSystemDict->insert("oss",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playOss)));
	#endif //COMPILE_OSS_SUPPORT
	#ifdef COMPILE_ESD_SUPPORT
		m_pSoundSystemDict->insert("esd",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playEsd)));
	#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS

	if(QSound::isAvailable())
		m_pSoundSystemDict->insert("qt",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playQt)));

	m_pSoundSystemDict->insert("null",new SoundSystemRoutine(KVI_PTR2MEMBER(KviSoundPlayer::playNull)));
}

KviSoundPlayer::~KviSoundPlayer()
{
	m_pThreadList->setAutoDelete(false);
	while(KviSoundThread * t = m_pThreadList->first())delete t;
	delete m_pThreadList;
	KviThreadManager::killPendingEvents(this);
	delete m_pSoundSystemDict;

#ifdef COMPILE_PHONON_SUPPORT
	if(g_pPhononPlayer)
		g_pPhononPlayer->deleteLater();
#endif
	g_pSoundPlayer = 0;
}

void KviSoundPlayer::getAvailableSoundSystems(QStringList *l)
{
	KviPointerHashTableIterator<QString,SoundSystemRoutine> it(*m_pSoundSystemDict);
	while(it.current())
	{
		l->append(it.currentKey());
		++it;
	}
}

bool KviSoundPlayer::havePlayingSounds()
{
	return (m_pThreadList->count() > 0);
}

void KviSoundPlayer::registerSoundThread(KviSoundThread * t)
{
	m_pThreadList->append(t);
}

void KviSoundPlayer::unregisterSoundThread(KviSoundThread * t)
{
	m_pThreadList->removeRef(t);
}

bool KviSoundPlayer::event(QEvent * e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		KviThread * t = ((KviThreadEvent *)e)->sender();
		if(!t)return true; // huh ?
		delete (KviSoundThread *)t;
		return true;
	}
	return QObject::event(e);
}

void KviSoundPlayer::detectSoundSystem()
{
#ifdef COMPILE_PHONON_SUPPORT
	if(!g_pPhononPlayer) g_pPhononPlayer= Phonon::createPlayer(Phonon::MusicCategory);
	if(g_pPhononPlayer->state() != Phonon::ErrorState)
	{
		KVI_OPTION_STRING(KviOption_stringSoundSystem) = "phonon";
		return;
	}
#endif
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KVI_OPTION_STRING(KviOption_stringSoundSystem) = "winmm";
#else
	#ifdef COMPILE_ESD_SUPPORT
		esd_format_t format = ESD_BITS16 | ESD_STREAM | ESD_PLAY | ESD_MONO;
		int esd_fd = esd_play_stream(format, 8012, NULL, "kvirc");
		if(esd_fd >= 0)
		{
			KVI_OPTION_STRING(KviOption_stringSoundSystem) = "esd";
			return;
		}
	#endif
	#ifdef COMPILE_OSS_SUPPORT
		#ifdef COMPILE_AUDIOFILE_SUPPORT
			KVI_OPTION_STRING(KviOption_stringSoundSystem) = "oss+audiofile";
			return;
		#endif
		KVI_OPTION_STRING(KviOption_stringSoundSystem) = "oss";
	#endif

	if(QSound::isAvailable())
	{
		KVI_OPTION_STRING(KviOption_stringSoundSystem) = "qt";
		return;
	}

	KVI_OPTION_STRING(KviOption_stringSoundSystem) = "null";
#endif
}

#ifdef COMPILE_PHONON_SUPPORT
bool KviSoundPlayer::playPhonon(const QString &szFileName)
{
	if(isMuted()) return true;
	Phonon::MediaSource media(szFileName);
        if(!g_pPhononPlayer)
		g_pPhononPlayer= Phonon::createPlayer(Phonon::MusicCategory,media);
	else g_pPhononPlayer->setCurrentSource(media);
	if(g_pPhononPlayer->state()!=Phonon::ErrorState)
        {
                g_pPhononPlayer->play();
        }
	return true;
}
#endif //!COMPILE_PHONON_SUPPORT
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	bool KviSoundPlayer::playWinmm(const QString &szFileName)
	{
		sndPlaySound(szFileName.toLocal8Bit().data(),SND_ASYNC | SND_NODEFAULT);
		return true;
	}
#else //!COMPILE_ON_WINDOWS
	#ifdef COMPILE_OSS_SUPPORT
		#ifdef COMPILE_AUDIOFILE_SUPPORT
			bool KviSoundPlayer::playOssAudiofile(const QString &szFileName)
			{
				if(isMuted()) return true;
				KviOssAudiofileSoundThread * t = new KviOssAudiofileSoundThread(szFileName);
				if(!t->start())
				{
					delete t;
					return false;
				}
				return true;
			}
		#endif //COMPILE_AUDIOFILE_SUPPORT
		bool KviSoundPlayer::playOss(const QString &szFileName)
		{
			if(isMuted()) return true;
			KviOssSoundThread * t = new KviOssSoundThread(szFileName);
			if(!t->start())
			{
				delete t;
				return false;
			}
			return true;
		}
	#endif //COMPILE_OSS_SUPPORT
	#ifdef COMPILE_ESD_SUPPORT
		bool KviSoundPlayer::playEsd(const QString &szFileName)
		{
			if(isMuted()) return true;
			KviEsdSoundThread * t = new KviEsdSoundThread(szFileName);
			if(!t->start())
			{
				delete t;
				return false;
			}
			return true;
		}
	#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS

bool KviSoundPlayer::playQt(const QString &szFileName)
{
	if(isMuted()) return true;
	QSound::play(szFileName);
	return true;
}

bool KviSoundPlayer::playNull(const QString &)
{
	// null sound system
	return true;
}

bool KviSoundPlayer::play(const QString &szFileName)
{
	if(isMuted()) return true;
	SoundSystemRoutine * r = m_pSoundSystemDict->find(KVI_OPTION_STRING(KviOption_stringSoundSystem));

	if(!r)
	{
		if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringSoundSystem),"unknown"))
		{
			detectSoundSystem();
			r = m_pSoundSystemDict->find(KVI_OPTION_STRING(KviOption_stringSoundSystem));
			if(!r)return false;
		} else {
			return false;
		}
	}

	return (this->*(*r))(szFileName);
}


KviSoundThread::KviSoundThread(const QString &szFileName)
: KviThread()
{
	g_pSoundPlayer->registerSoundThread(this);
	m_szFileName = szFileName;
}

KviSoundThread::~KviSoundThread()
{
	g_pSoundPlayer->unregisterSoundThread(this);
}

void KviSoundThread::play()
{
}

void KviSoundThread::run()
{
	play();
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	postEvent(g_pSoundPlayer,new KviThreadEvent(KVI_THREAD_EVENT_SUCCESS));
#endif
}


#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	#ifdef COMPILE_OSS_SUPPORT
		#ifdef COMPILE_AUDIOFILE_SUPPORT
			KviOssAudiofileSoundThread::KviOssAudiofileSoundThread(const QString &szFileName)
			: KviSoundThread(szFileName)
			{
			}

			KviOssAudiofileSoundThread::~KviOssAudiofileSoundThread()
			{
			}

			void KviOssAudiofileSoundThread::play()
			{
				#define BUFFER_FRAMES 4096

				AFfilehandle file;
				AFframecount framesRead;
				int sampleFormat, sampleWidth, channelCount, format, freq;
				float frameSize;
				void * buffer;

				file = afOpenFile(m_szFileName.toUtf8().data(),"r",NULL);
				if(!file)
				{
					debug("libaudiofile could not open the file %s",m_szFileName.toUtf8().data());
					debug("giving up playing sound...");
					return; // screwed up
				}

				sampleFormat = -1;

				afGetVirtualSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

				if(sampleFormat == -1)
				{
					debug("libaudiofile couldn't find the sample format for file %s",m_szFileName.toUtf8().data());
					debug("giving up playing sound...");
					afCloseFile(file);
					return; // screwed up
				}

				frameSize = afGetVirtualFrameSize(file, AF_DEFAULT_TRACK, 1);
				channelCount = afGetVirtualChannels(file, AF_DEFAULT_TRACK);
				buffer = kvi_malloc(int(BUFFER_FRAMES * frameSize));

				int audiofd_c = open("/dev/dsp", O_WRONLY | O_EXCL | O_NDELAY);
				QFile audiofd;
				audiofd.open(audiofd_c,QIODevice::WriteOnly);

				if(audiofd_c < 0)
				{
					debug("Could not open audio devive /dev/dsp! [OSS]");
					debug("(the device is probably busy)");
					goto exit_thread;
				}

				if (sampleWidth == 8) format = AFMT_U8;
				else if (sampleWidth == 16)format = AFMT_S16_NE;

				if (ioctl(audiofd.handle(),SNDCTL_DSP_SETFMT, &format) == -1)
				{
					debug("Could not set format width to DSP! [OSS]");
					goto exit_thread;
				}

				if (ioctl(audiofd.handle(), SNDCTL_DSP_CHANNELS, &channelCount) == -1)
				{
					debug("Could not set DSP channels! [OSS]");
					goto exit_thread;
				}

				freq = (int) afGetRate(file, AF_DEFAULT_TRACK);
				if (ioctl(audiofd.handle(), SNDCTL_DSP_SPEED, &freq) == -1)
				{
					debug("Could not set DSP speed %d! [OSS]",freq);
					goto exit_thread;
				}

				framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer, BUFFER_FRAMES);

				while(framesRead > 0)
				{
					audiofd.write((char *)buffer,(int)(framesRead * frameSize));
					framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer,BUFFER_FRAMES);
				}

			exit_thread:
				audiofd.close();
				if(audiofd_c >= 0)close(audiofd_c);
				afCloseFile(file);
				kvi_free(buffer);
			}
		#endif //COMPILE_AUDIOFILE_SUPPORT


		KviOssSoundThread::KviOssSoundThread(const QString &szFileName)
		: KviSoundThread(szFileName)
		{
		}

		KviOssSoundThread::~KviOssSoundThread()
		{
		}

		void KviOssSoundThread::play()
		{
			#define OSS_BUFFER_SIZE 16384

			QFile f(m_szFileName);
			int fd = -1;
			char buf[OSS_BUFFER_SIZE];
			int iDataLen = 0;
			int iSize = 0;

			if(!f.open(QIODevice::WriteOnly))
			{
				debug("Could not open sound file %s! [OSS]",m_szFileName.toUtf8().data());
				return;
			}

			iSize = f.size();

			if(iSize < 24)
			{
				debug("Could not play sound, file %s too small! [OSS]",m_szFileName.toUtf8().data());
				goto exit_thread;
			}

			if(f.read(buf,24) < 24)
			{
				debug("Error while reading the sound file header (%s)! [OSS]",m_szFileName.toUtf8().data());
				goto exit_thread;
			}

			iSize -= 24;

			fd = open("/dev/audio",  O_WRONLY | O_EXCL | O_NDELAY);
			if(fd < 0)
			{
				debug("Could not open device file /dev/audio!");
				debug("Maybe other program is using the device? Hint: fuser -uv /dev/audio");
				goto exit_thread;
			}


			while(iSize > 0)
			{
				int iCanRead = OSS_BUFFER_SIZE - iDataLen;
				if(iCanRead > 0)
				{
					int iToRead = iSize > iCanRead ? iCanRead : iSize;
					int iReaded = f.read(buf + iDataLen,iToRead);
					if(iReaded < 1)
					{
						debug("Error while reading the file data (%s)! [OSS]",m_szFileName.toUtf8().data());
						goto exit_thread;
					}
					iSize -= iReaded;
					iDataLen += iReaded;
				}
				if(iDataLen > 0)
				{
					int iWritten = write(fd,buf,iDataLen);
					if(iWritten < 0)
					{
						if((errno != EINTR) && (errno != EAGAIN))
						{
							debug("Error while writing the audio data (%s)! [OSS]",m_szFileName.toUtf8().data());
							goto exit_thread;
						}
					}
					iDataLen -= iWritten;
				} else {
					// nothing to write ????
					goto exit_thread;
				}
			}

		exit_thread:
			f.close();
			if(fd > 0)close(fd);
		}


	#endif //COMPILE_OSS_SUPPORT

	#ifdef COMPILE_ESD_SUPPORT

		KviEsdSoundThread::KviEsdSoundThread(const QString &szFileName)
		: KviSoundThread(szFileName)
		{
		}

		KviEsdSoundThread::~KviEsdSoundThread()
		{
		}

		void KviEsdSoundThread::play()
		{
			// ESD has a really nice API
			if(!esd_play_file(NULL,m_szFileName.toUtf8().data(),1))
				debug("Could not play sound %s! [ESD]",m_szFileName.toUtf8().data());
		}

	#endif //COMPILE_ESD_SUPPORT
#endif //!COMPILE_ON_WINDOWS


/*
	@doc: snd.play
	@type:
		command
	@title:
		snd.play
	@short:
		Play a sound file from the disk
	@syntax:
		snd.play [-q|quiet] <filename:string>
	@description:
		Play a file, using the sound system specified by the user in the options.[br]
		The supported file formats vary from one sound system to another, but the best
		bet could be Au Law (.au) files. Phonon, EsounD and Linux/OSS with audiofile support also
		support other formats like .wav files but in OSS without audiofile only .au files are
		supported.
		On windows the supported file formats are determined by the drivers installed.
		You should be able to play at least *.wav files.[br]
		(This is a task where the Windows interface is really well done, I must say that :)
	@switches:
		!sw: -q | --quiet
		Causes the command to run quietly (no complains about missing files, invalid formats...
*/

static bool snd_kvs_cmd_play(KviKvsModuleCommandCall * c)
{
	QString szFile;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("file name",KVS_PT_STRING,0,szFile)
	KVSM_PARAMETERS_END(c)
	if(szFile.isEmpty() || (!KviFileUtils::fileExists(szFile)))
	{
		if(!c->hasSwitch('q',"quiet"))c->warning(__tr2qs("Sound file '%Q' not found"),&szFile);
		return true;
	}

	if(!g_pSoundPlayer->play(szFile))
	{
		if(!c->hasSwitch('q',"quiet"))c->warning(__tr2qs("Unable to play sound '%Q'"),&szFile);
	}

	return true;
}

static bool snd_kvs_cmd_autodetect(KviKvsModuleCommandCall * c)
{
	g_pSoundPlayer->detectSoundSystem();
	if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringSoundSystem),"null"))
	{
		c->window()->outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Sorry, I can't find a sound system to use on this machine"));
	} else {
		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Sound system detected to: %s"),KVI_OPTION_STRING(KviOption_stringSoundSystem).toUtf8().data());
	}
	return true;
}


/*
	@doc: snd.mute
	@type:
		command
	@title:
		snd.mute
	@short:
		Mute all sounds
	@syntax:
		snd.mute
	@description:
		Mute all sounds
*/

static bool snd_kvs_cmd_mute(KviKvsModuleCommandCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	g_pSoundPlayer->setMuted(TRUE);
	return true;
}

/*
	@doc: snd.unmute
	@type:
		command
	@title:
		snd.unmute
	@short:
		UnMute all sounds
	@syntax:
		snd.mute
	@description:
		UnMute all sounds
*/

static bool snd_kvs_cmd_unmute(KviKvsModuleCommandCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	g_pSoundPlayer->setMuted(FALSE);
	return true;
}

/*
	@doc: snd.isMuted
	@type:
		function
	@title:
		$snd.isMuted
	@short:
		Returns if the sounds muted
	@syntax:
		<bool> $snd.isMuted()
	@description:
		Returns if the sounds muted
*/


static bool snd_kvs_fnc_ismuted(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setBoolean(g_pSoundPlayer->isMuted());
	return true;
}

static bool snd_module_init(KviModule * m)
{
	g_pSoundPlayer = new KviSoundPlayer();

	KVSM_REGISTER_SIMPLE_COMMAND(m,"autodetect",snd_kvs_cmd_autodetect);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"play",snd_kvs_cmd_play);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"mute",snd_kvs_cmd_mute);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"unmute",snd_kvs_cmd_unmute);

	KVSM_REGISTER_FUNCTION(m,"isMuted",snd_kvs_fnc_ismuted);

	return true;
}

static bool snd_module_cleanup(KviModule *)
{
	delete g_pSoundPlayer;
	g_pSoundPlayer = 0;
	return true;
}

static bool snd_module_can_unload(KviModule *)
{
	return !(g_pSoundPlayer->havePlayingSounds());
}

static bool snd_module_ctrl(KviModule *,const char * operation,void * param)
{
	if(kvi_strEqualCI(operation,"getAvailableSoundSystems"))
	{
		// we expect param to be a pointer to QStringList
		QStringList *l=(QStringList *)param;
		g_pSoundPlayer->getAvailableSoundSystems(l);
		return true;
	}
	if(kvi_strEqualCI(operation,"detectSoundSystem"))
	{
		g_pSoundPlayer->detectSoundSystem();
		return true;
	}
	return false;
}


KVIRC_MODULE(
	"Sound",                                                 // module name
	"4.0.0",                                                // module version
	"(C) 2002 Szymon Stefanek (pragma at kvirc dot net)," \
	"Juanjo Alvarez (juanjux at yahoo dot es)", // author & (C)
	"Sound playing commands",
	snd_module_init,
	snd_module_can_unload,
	snd_module_ctrl,
	snd_module_cleanup
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkvisnd.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES