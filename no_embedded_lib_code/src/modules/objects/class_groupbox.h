#ifndef _CLASS_GROUPBOX_H_
#define _CLASS_GROUPBOX_H_
//=============================================================================
//
//   File : class_groupbox.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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



#include <kvi_tal_groupbox.h>
#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_groupbox : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_groupbox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	
	bool setTitle(KviKvsObjectFunctionCall *c);
	bool title(KviKvsObjectFunctionCall *c);
	bool setFlat(KviKvsObjectFunctionCall *c);
	bool isFlat(KviKvsObjectFunctionCall *c);
	bool setCheckable(KviKvsObjectFunctionCall *c);
	bool isCheckable(KviKvsObjectFunctionCall *c);
	bool setInsideMargin(KviKvsObjectFunctionCall *c);
	bool insideMargin(KviKvsObjectFunctionCall *c);
	bool setInsideSpacing(KviKvsObjectFunctionCall *c);
	bool insideSpacing(KviKvsObjectFunctionCall *c);
	bool addSpace(KviKvsObjectFunctionCall *c);
	bool alignment(KviKvsObjectFunctionCall *c);
	bool setAlignment(KviKvsObjectFunctionCall *c);
	bool setOrientation(KviKvsObjectFunctionCall *c);
	bool isChecked(KviKvsObjectFunctionCall *c);
	bool setChecked(KviKvsObjectFunctionCall *c);

};
#endif //!_CLASS_GROUPBOX_H_
