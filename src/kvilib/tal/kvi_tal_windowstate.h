#ifndef _KVI_TAL_WINDOWSTATE_H_
#define _KVI_TAL_WINDOWSTATE_H_

//=============================================================================
//
//   File : kvi_tal_windowstate.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"

#ifdef COMPILE_USE_QT4
	#define QT_WINDOWSTATE_FLAGS Qt::WindowState
	
	#define QT_WINDOWSTATE_MAXIMIZED Qt::WindowMaximized
	#define QT_WINDOWSTATE_MINIMIZED Qt::WindowMinimized
#else
	#define QT_WINDOWSTATE_FLAGS Qt::WidgetState

	#define QT_WINDOWSTATE_MAXIMIZED Qt::WState_Maximized
	#define QT_WINDOWSTATE_MINIMIZED Qt::WState_Minimized
#endif

#endif // _KVI_TAL_WINDOWSTATE_H_