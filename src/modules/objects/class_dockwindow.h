#ifndef _CLASS_DOCKWINDOW_H_
#define _CLASS_DOCKWINDOW_H_
//=============================================================================
//
//   File : class_dockwindow.h
//   Created on Thu 29 Dec 2005 23:45:11 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "object_macros.h"
#include "class_widget.h"

#include "kvi_settings.h"

#include <QBoxLayout>

class KviKvsObject_dockwindow : public KviKvsObject_widget
{
	Q_OBJECT
private:
	QBoxLayout * m_pLayout;
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_dockwindow)
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	
	bool function_addWidget(KviKvsObjectFunctionCall *c);
	bool function_orientation(KviKvsObjectFunctionCall *c);
	bool function_setOrientation(KviKvsObjectFunctionCall *c);
	bool function_resizeEnabled(KviKvsObjectFunctionCall *c);
	bool function_setResizeEnabled(KviKvsObjectFunctionCall * c);
	bool function_setAllowedDockAreas(KviKvsObjectFunctionCall * c);
	bool function_dock(KviKvsObjectFunctionCall * c);
};

#endif //!_CLASS_DOCKWINDOW_H_
