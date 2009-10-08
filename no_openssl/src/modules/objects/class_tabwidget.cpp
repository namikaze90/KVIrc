//=============================================================================
//
//   File : class_tabwidget.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
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

#include "class_tabwidget.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include <QTabWidget>

/*
	@doc:	tabwidget
	@keyterms:
		tabwidget object class,
	@title:
		tabwidget class
	@type:
		class
	@short:
		Provides a Tab Widget.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		The tabwidget class provides a stack of tabbed widgets.[br]
		Each tab is associated with a different widget (called a `page').[br]
		Only the current tab's page is shown in the page area; all the others tabs pages are hidden.[br]
		The user can show a different page by clicking on its tab.[br]
	@functions:
		!fn: $addTab(<widget:object>,<text_label:string>,[<icon_id>])
		Adds another page to the tab view with the text label and optional icon.
		!fn: $insertTab(<widget:object>,<tex_label:string>,<index:uinteger>)
		Inserts another tab and page to the tab view with name and label-text specified.[br]
		If index is not specified, the tab is simply appended.[br]
		Otherwise the new tab is inserted at the specified position.
		!fn: $setTabToolTip(<tab_widget:object>,<tooltip:string>)
		Sets the tab tool tip for the tabwidget to <tooltip>.
		!fn: $removeTabToolTip(<tab_widget:object>)
		Removes the tab tool tip for the tab. If the page does not have a tip, nothing happens.
		!fn: $setTabLabel(<tab_widget>, <label:string>)
		Sets the tab label for <tab_widget>  to label.
		!fn: $changeTab(<tab_widget:object>,<label:text>,[<icon_id>])
		Defines a new label, and optional icon, for the tab .
		!fn: $setCurrentPage(<index:uinteger>)
		Sets the index position of the current tab page to <index>.
		!fn: <integer> $currentPageIndex()
		Returns the current page index.
		!fn: <string> $tabLabel(<index:uinteger>)
		Returns the tab label of the <index>.
		!fn: <string> $currentTabLabel()
		Returns the label of the current tab.
		!fn: $removePage(<tab_widget:object>)
		Remove the page <tab_widget>.
		!fn: $setTabPosition(<tab_position:string>)
		Sets TabPosition. Valid parameters are : Top, Bottom.
		!fn: <integer> $count()
		This property holds the number of tabs in the tab bar.
		!fn: $currentChangedEvent()
		This function is invoched when the current tab was changed.[br]
		The default implementation emits the [classfnc]$currentChange[/classfnc]() signal.
	@examples:
		[example]
			# First we'll create the main tabwidget.
			%Tabwidget=$new(tabwidget)
			%Tabwidget->$setToolTip("Example of TabWidget class")
			%Tabwidget->$setMargin(30)
			%Tabwidget->$setTabPosition(Top)

			# Now we'll create the new widgets and put they in to the main tabwidget.
			%firsttab=$new(widget,%Tabwidget)
			%secondtab=$new(widget,%Tabwidget)

			# Now we'll create the item to put in to tab's pages.
			%layoutfirsttab=$new(layout,%firsttab)
			%labelbt=$new(label,%firsttab)
			%labelbt->$settext(Botton Tab)
			%labeltt=$new(label,%firsttab)
			%labeltt->$settext(Top Tab)
			%buttontb=$new(button,%firsttab)
			%buttontb->$settext("To &Botton")
			%buttontt=$new(button,%firsttab)
			%buttontt->$settext("To &Top")

			# Now we'll give a layout to all items.
			# This also allows to use privateimpl without making buttons global variables
			%layoutfirsttab->$addwidget(%labelbt,0,0)
			%layoutfirsttab->$addwidget(%labeltt,0,1)
			%layoutfirsttab->$addwidget(%buttontb,1,0)
			%layoutfirsttab->$addwidget(%buttontt,1,1)

			# Add the page to the main tab.
			%Tabwidget->$addTab(%firsttab,Top&Button,33)

			# Implementing the action to do when the user click the buttons.
			privateimpl(%buttontb,mousepressevent)
			{
				%Tabwidget->$setTabPosition(Bottom)
			}
			privateimpl(%buttontt,mousepressevent)
			{
				%Tabwidget->$setTabPosition(Top)
			}

			# We do the same work  with the second tab's page.
			%layoutsecondtab=$new(layout,%secondtab)
			%labelwp=$new(label,%secondtab)
			%labelwp->$settext("Enjoy the new Class provided by")
			%layoutsecondtab->$addwidget(%labelwp,0,0)
			%labelwp->$setalignment("Center")
			%labelgen=$new(label,%secondtab)
			%labelgen->$settext(Grifisx \& Noldor)
			%labelgen->$setalignment("Center")
			%layoutsecondtab->$addwidget(%labelgen,1,0)
			%Tabwidget->$addTab(%secondtab,&About,50)

			# Let's show our example.
			%Tabwidget->$show()
		[/example]
	@signals:
		!sg: $currentChange()
		This signal is emitted by the default implementation of [classfnc]$currentChangedEvent[/classfnc]().[br]
		If you reimplement that function you will have to emit the signal manually (if you still need it).[br]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_tabwidget,"tabwidget","widget")

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,addTab)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,insertTab)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,setTabToolTip)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,removeTabToolTip)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,setTabLabel)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,changeTab)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,setCurrentPage)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,currentPageIndex)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,tabLabel)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,currentTabLabel)

	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,count)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,removePage)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,setTabPosition)

	// events
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_tabwidget,currentChangedEvent)



KVSO_END_REGISTERCLASS(KviKvsObject_tabwidget)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_tabwidget,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_tabwidget)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_tabwidget)

KVSO_END_CONSTRUCTOR(KviKvsObject_tabwidget)

bool KviKvsObject_tabwidget::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QTabWidget)
	connect(widget(),SIGNAL(currentChanged(int)),this,SLOT(slotCurrentChanged(int)));
	return true;
}
KVSO_CLASS_FUNCTION(tabwidget,addTab)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	QString szLabel,szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	QPixmap * pix = g_pIconManager->getImage(szIcon);

	if(pix){
		((QTabWidget *)widget())->addTab(((QWidget *)(pObject->object())),QIcon(*pix),szLabel);
	}
	else((QTabWidget *)widget())->addTab(((QWidget *)(pObject->object())),szLabel);
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,insertTab)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	QString szLabel,szIcon;
	kvs_int_t iIndex;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix){
			((QTabWidget *)widget())->insertTab(iIndex,(QWidget *)(pObject->object()),QIcon(*pix),szLabel);
	}
	else ((QTabWidget *)widget())->insertTab(iIndex,((QWidget *)(pObject->object())),szLabel);
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,setCurrentPage)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_int_t iIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,iIndex)
	KVSO_PARAMETERS_END(c)
	((QTabWidget *)widget())->setCurrentIndex(iIndex);
    return true;
}
KVSO_CLASS_FUNCTION(tabwidget,setTabToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	QString szTooltip;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("tooltip",KVS_PT_STRING,0,szTooltip)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int ctrl = ((QTabWidget *)widget())->indexOf (((QWidget *)(pObject->object())));
	if (ctrl == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ","objects"));
		return true;
	}
	((QTabWidget *)widget())->setTabToolTip(ctrl,szTooltip);
	return true;
}
KVSO_CLASS_FUNCTION(tabwidget,removeTabToolTip)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int ctrl = ((QTabWidget *)widget())->indexOf (((QWidget *)(pObject->object()))) ;
	if (ctrl==-1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ","objects"));
		return true;
	}
	((QTabWidget *)widget())->setTabToolTip(ctrl,QString());
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,setTabLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	QString szLabel;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_widget",KVS_PT_HOBJECT,0, hObject)
		KVSO_PARAMETER("tooltip",KVS_PT_STRING,0,szLabel)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int ctrl = ((QTabWidget *)widget())->indexOf (((QWidget *)(pObject->object())));
	if (ctrl == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ","objects"));
		return true;
	}
	((QTabWidget *)widget())->setTabText(ctrl,szLabel);
	return true;
}
KVSO_CLASS_FUNCTION(tabwidget,currentPageIndex)
{
	CHECK_INTERNAL_POINTER(widget())
	int index=((QTabWidget *)widget())->currentIndex();
	c->returnValue()->setInteger(index);
	return true;
}
KVSO_CLASS_FUNCTION(tabwidget,tabLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_INT,0,uIndex)
	KVSO_PARAMETERS_END(c)
	QString label=((QTabWidget *)widget())->tabText(uIndex);
	c->returnValue()->setString(label);
    return true;
}


KVSO_CLASS_FUNCTION(tabwidget,count)
{
	CHECK_INTERNAL_POINTER(widget())
	int count = ((QTabWidget *)widget())->count();
	c->returnValue()->setInteger(count);
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,currentTabLabel)
{
	CHECK_INTERNAL_POINTER(widget())
	int i= ((QTabWidget *)widget())->currentIndex();
	QString label=((QTabWidget *)widget())->tabText(i);
	c->returnValue()->setString(label);
    return true;
}
KVSO_CLASS_FUNCTION(tabwidget,removePage)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int ctrl = ((QTabWidget *)widget())->indexOf (((QWidget *)(pObject->object())));
	if (ctrl == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab ","objects"));
		return true;
	}
	((QTabWidget *)widget())->removeTab(ctrl);
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,changeTab)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *pObject;
	QString szLabel,szIcon;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("icon_id",KVS_PT_STRING,KVS_PF_OPTIONAL,szIcon)
	KVSO_PARAMETERS_END(c)
	pObject=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	int ctrl = ((QTabWidget *)widget())->indexOf (((QWidget *)(pObject->object())));
	if (ctrl == -1)
	{
		c->warning(__tr2qs_ctx("Can't find the tab","objects"));
		return true;
	}
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)((QTabWidget *)widget())->setTabIcon(ctrl,QIcon(*pix));
	((QTabWidget *)widget())->setTabText(ctrl,szLabel);
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,setTabPosition)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPos;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("tab_position",KVS_PT_STRING,0,szPos)
	KVSO_PARAMETERS_END(c)
	if(KviQString::equalCI(szPos,"Top"))
		((QTabWidget *)widget())->setTabPosition(QTabWidget::North);
	else if(KviQString::equalCI(szPos,"Bottom"))
		((QTabWidget *)widget())->setTabPosition(QTabWidget::South);
	else c->warning( __tr2qs_ctx("Unknown position '%Q'","objects"),&szPos);
	return true;
}

KVSO_CLASS_FUNCTION(tabwidget,currentChangedEvent)
{

	emitSignal("valueChanged",c,c->params());
	return true;
}

void KviKvsObject_tabwidget::slotCurrentChanged(int value)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)value));
	callFunction(this,"currentChangedEvent",&params);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_tabwidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
