//=============================================================================
//
//   File : class_popupmenu.cpp
//   Creation date : Fri Mar 18 21:30:48 CEST 2005
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


#include "class_popupmenu.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_tal_popupmenu.h"

#include <QCursor>
#include <QMenu>
#include <QAction>



/*
	@doc:   popupmenu
	@keyterms:
		popupmenu object class,
	@title:
		popupmenu class
	@type:
		class
	@short:
		Provides a popup menu.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget can be used to display a popup menu.Technically, a popup menu consists of a list of menu items.[br]
		You add items with insertItem(). An item is either a string. In addition, items can have an optional icon drawn on the very left side.[br]
	@functions:
		!fn: $insertItem(<text:string>,[icon_id:string])
		Inserts menu items into a popup menu with optional icon and return the popup identifier.
		!fn: $insertWidget(<widget:object>)[br]
		Inserts widget items int a popup menu and return the popup identifier.[br]
		The widget is treats as a separator; this means that the item is not selectable and you can, for example, simply insert a label if you need a popup menu with a title.
		!fn: $insertHandle(<text_label:string>,<popupmenu:object>,[icon])
		Inserts a submenu with optional icon into the popup menu.
		!fn: $exec([<widget:objects>,<x:uinteger>,<y:integer>])
		If called without paramaters show the popup menu at the current pointer position.[br]
		With the optional parameters show the popup menu at the coordinate x,y widget parameter relative.
		!fn: $removeItem(<popup_id:Uinteger>)
		Removes the menu item that has the identifier id.
		!fn: $removeItemAt(<index:uinteger>)
		Removes the menu item at position index.
		!fn: $insertSeparator(<index:uinteger>)
		Inserts a separator at position index.[br]
		If the index is negative the separator becomes the last menu item.
		!fn: $activatedEvent(<popup_id:uinteger>)
		This function is called when a menu item and return the the item id.[br]
		The default implementation emits the [classfnc]$activated[/classfnc]() signal.
		!fn: $highligthtedEvent(<popup_id:uinteger>)
		This function is called when a menu item is highlighted and return the item id.[br]
		The default implementation emits the [classfnc]$highlighted[/classfnc]() signal.
	@signals:
		!sg: $activated()
		This signal is emitted by the default implementation of [classfnc]$activatedEvent[/classfnc]().
		!sg: $highlighted()
		This signal is emitted by the default implementation of [classfnc]$highlightedEvent[/classfnc]().
		[br]
		|-EXAMPLE POPUP MENU-|[br]
		|--Start:
		# First of all we create an array wich will be used to create random colors.[br]
		%Hex[]=$array(0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F)[br]
		[br]
		class (ws,widget)[br]
		{[br]
		#In the constructor we create everything that belong to the single widget.[br]
			constructor[br]
			{[br]
				#Geometry of the widget and setting-up of popupmenu [br]
				$$->$setGeometry(%X,%Y,100,100)[br]
			    $$->%lay=$new(layout,$$)[br]
				#Here we generate a cicle to create our labels inside the widget.[br]
				%i=0[br]
				while (%i<10)[br]
				{[br]
					$$->%label=$new(label,$$)[br]
					$$->%label->$settext("Another class by N\&G")[br]
					#We set our foreground's coulor using the hex arrey in random way.[br]
					%color=%Hex[$rand(14)]%Hex[$rand(14)]%Hex[$rand(14)]%Hex[$rand(14)]%Hex[$rand(14)]%Hex[$rand(14)][br]
					$$->%label->$setforegroundcolor(%color)[br]
					$$->%label->$setautoresize(1)[br]
					#We add the label to the widget's popupmenu.[br]
					$$->%lay->$addwidget($$->%label,%i,0)[br]
					%i++;[br]
				}[br]
				#We make the popupmenu relative to this widget: this is merely demonstrative.[br]
				#because it creates 25 identical popups.[br]
				$$->%Popupmenu=$new(popupmenu,$$)[br]
				#we create the label widget wich will be used as popup's title[br]
				$$->%Popuptitle=$new(label)[br]
				$$->%Popuptitle->$settext(<B><U>"Windows Options"</B></U>)[br]
				$$->%Popuptitle->$setAlignment(Center)[br]
				#and we add it.[br]
				%A=$$->%Popuptitle[br]
				$$->%Popupmenu->$insertwidget(%A)[br]
				#Here we keep the various IDs in the arrays[br]
				%Tile[%I]=$$->%Popupmenu->$insertItem("Tile",118)[br]
				%Cascade[%I]=$$->%Popupmenu->$insertItem("Cascade",115)[br]
				$$->%Popupmenu->$insertSeparator(3)[br]
				%Closeactw[%I]=$$->%Popupmenu->$insertItem("Close Active Window",08)[br]
				%Closeallw[%I]=$$->%Popupmenu->$insertItem("Close All Window",58)[br]
				$$->$show()[br]
				privateimpl($$->%Popupmenu,activatedEvent)[br]
				{[br]
					%id=$0[br]
					%i = 0[br]
					#with this cicle we control wich of the items has been called  comparing the id given back by the event with our arrays created before;[br]
					while (%i<20)[br]
					{[br]
						if (%id == %Tile[%i]) return %Workspace->$tile()[br]
						if (%id == %Cascade[%i]) return %Workspace->$cascade()[br]
						if (%id == %Closeactw[%i]) return %Workspace->$closeactivewindow()[br]
						if (%id == %Closeallw[%i]) return %Workspace->$closeallwindows()[br]
						%i ++[br]
					}[br]
				}[br]
			}[br]
		   #we activate the popup pushing the right mouse button on the widget[br]
		  mousepressevent[br]
			{[br]
			   if ($0 == 1) $$->%Popupmenu->$exec()[br]
			}[br]
		}[br]
		%Workspace=$new(workspace)[br]
		%Workspace->$resize(640,480)[br]
		%I=0[br]
		%Cicle=1[br]
		while (%I<20)[br]
		{[br]
			%X=$rand(500)[br]
			%Y=$rand(480)[br]
			%Widget=$new(ws,%Workspace)[br]
			%I++[br]
		}[br]
		#Let's show!
		%Workspace->$show[br]
		|--End.[br][br]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_popupmenu,"popupmenu","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,insertItem)
	//KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,"insertWidget", functioninsertWidget)
//	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,"setTitle", functionsetTitle)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,exec)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,insertSeparator)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,removeItem)

	// events
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,highlightedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_popupmenu,activatedEvent)

KVSO_END_REGISTERCLASS(KviKvsObject_popupmenu)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_popupmenu,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_popupmenu)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_popupmenu)

KVSO_END_CONSTRUCTOR(KviKvsObject_popupmenu)

bool KviKvsObject_popupmenu::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	identifier=0;
	SET_OBJECT(QMenu)
	connect(widget(),SIGNAL(triggered(QAction *)),this,SLOT(slottriggered(QAction *)));
	connect(widget(),SIGNAL(highlighted(int)),this,SLOT(slothighlighted(int)));
	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,insertItem)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szItem,szIcon;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szItem)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSO_PARAMETERS_END(c)
	QPixmap *pix = 0;
	QAction * action = 0;
	if(!szIcon.isEmpty())
	{
		pix = g_pIconManager->getImage(szIcon);
		if (pix) action=((QMenu *)widget())->addAction(*pix,szItem);
		else c->warning(__tr2qs_ctx("Icon '%Q' doesn't exist","objects"),&szIcon);
	}
	else
		action=((QMenu *)widget())->addAction(szItem);
	actionsDict[identifier]=action;
	c->returnValue()->setInteger(identifier);
	identifier++;
	return true;
}
/*
KVSO_CLASS_FUNCTION(popupmenu,setTitle)
{
	QString szTitle;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("title",KVS_PT_STRING,0,szTitle)
	KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	((QMenu *)widget())->setTitle(szTitle);

	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,insertWidget)
{
	KviKvsObject *pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if (!pObject)
	{
                c->warning(__tr2qs_ctx("Widget parameter is not an object","objects"));
		return true;
	}
	if (!pObject->object())
	{
                c->warning(__tr2qs_ctx("Widget parameter is not a valid object","objects"));
		return true;
	}
	if(!pObject->object()->isWidgetType())
	{
                c->warning(__tr2qs_ctx("Can't add a non-widget object","objects"));
		return TRUE;
	}
	if (widget()) ((KviTalPopupMenu *)widget())->insertItem(((KviTalPopupMenu  *)(pObject->object())));
	return true;
}


KVSO_CLASS_FUNCTION(popupmenu,insertHandle)
{
	KviKvsObject *ob;
	QString szLabel,szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text_label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->inheritsClass("popupmenu"))
	{
                c->warning(__tr2qs_ctx("Can't add a non - popupmenu  object","objects"));
		return TRUE;
	}
	if(!widget())return true;
	QPixmap *pix = 0;
	int id=0;
	if(!szIcon.isEmpty())
	{
		pix = g_pIconManager->getImage(szIcon);
		if (pix) id=((KviTalPopupMenu *)widget())->insertItem(*pix,szLabel,((KviTalPopupMenu  *)(ob->object())));
                else c->warning(__tr2qs_ctx("pix '%Q' doesn't exists","objects"),&szIcon);
	}
	else
		id=((KviTalPopupMenu *)widget())->insertItem(szLabel,((KviTalPopupMenu  *)(ob->object())));
	c->returnValue()->setInteger(id);
	return true;
}
*/
KVSO_CLASS_FUNCTION(popupmenu,exec)
{
	CHECK_INTERNAL_POINTER(widget())
	if(!c->params()->count())
	{
		((QMenu *)widget())->exec(QCursor::pos());
		return true;
	}

	KviKvsObject *pObject;
	kvs_uint_t iX,iY;
	QString szLabel,szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("x",KVS_PT_UNSIGNEDINTEGER,0,iX)
		KVSO_PARAMETER("y",KVS_PT_UNSIGNEDINTEGER,0,iY)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	((QMenu *)widget())->exec(((QWidget *)(pObject->object()))->mapToGlobal(QPoint(iX,iY)) );

	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,removeItem)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uItem;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("item_id",KVS_PT_UNSIGNEDINTEGER,0,uItem)
	KVSO_PARAMETERS_END(c)
	QAction * action=actionsDict.value(uItem);
	if(action)
	{
		((QMenu *)widget())->removeAction(action);
		identifier--;
	}
	return true;
}

KVSO_CLASS_FUNCTION(popupmenu,insertSeparator)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	QAction * action=actionsDict.value(iIndex);
	if(action)((QMenu *)widget())->insertSeparator(action);
	return true;
}

void KviKvsObject_popupmenu::slothighlighted(int i)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)i));
	callFunction(this,"highlightedEvent",&params);
}

KVSO_CLASS_FUNCTION(popupmenu,highlightedEvent)
{
	emitSignal("highlighted",c,c->params());
	return true;
}

void KviKvsObject_popupmenu::slottriggered(QAction *a)
{
	QHashIterator<int, QAction *> i(actionsDict);
	kvs_int_t count=0;
	while (i.hasNext())
	{
		i.next();
		if (i.value()!= a) count++;
		else break;
	}
	KviKvsVariantList params(new KviKvsVariant(count));
	callFunction(this,"activatedEvent",&params);
}

KVSO_CLASS_FUNCTION(popupmenu,activatedEvent)
{
	emitSignal("activated",c,c->params());
	return true;
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_popupmenu.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES