#ifndef _KVI_KVS_OBJECT_H_
#define _KVI_KVS_OBJECT_H_
//=============================================================================
//
//   File : kvi_kvs_object.h
//   Creation date : Wed 08 Oct 2003 02:31:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_pointerlist.h"
#include "kvi_kvs_runtimecall.h"
#include "kvi_kvs_parameterprocessor.h"
#include "kvi_kvs_object_functionhandler.h"
#include "kvi_kvs_types.h"

#include <QObject>


class KviKvsObjectFunctionCall;

typedef struct _KviKvsObjectConnection
{
	KviKvsObject * pSourceObject;    // source object (owner of the struct)
	KviKvsObject * pTargetObject;    // target object
	QString        szSignal;         // source signal name
	QString        szSlot;           // target slot function
} KviKvsObjectConnection;

typedef KviPointerList<KviKvsObjectConnection> KviKvsObjectConnectionList;
typedef KviPointerListIterator<KviKvsObjectConnection> KviKvsObjectConnectionListIterator;

class KVIRC_API KviKvsObject : public QObject
{
	friend class KviKvsObjectController;
	friend class KviKvsObjectClass;
	Q_OBJECT
public:
	KviKvsObject(KviKvsObjectClass * pClass,KviKvsObject * pParent,const QString &szName);
	virtual ~KviKvsObject();
protected:
	// main data
	QString                                      m_szName;             // object name
	kvs_hobject_t                                m_hObject;            // global object handle
	KviKvsObjectClass                          * m_pClass;             // the class definition

	KviKvsHash                                 * m_pdataContainer;     // member variables

	KviPointerList<KviKvsObject>                   * m_pChildList;

	KviPointerHashTable<QString,KviKvsObjectFunctionHandler>         * m_pFunctionHandlers;  // our function handlers

	KviPointerHashTable<QString,KviKvsObjectConnectionList>          * m_pSignalDict;        // our signals connected to other object functions

	KviKvsObjectConnectionList                 * m_pConnectionList;    // signals connected to this object functions
	
	// this is valid when processing one of our slots
	kvs_hobject_t                                m_hSignalSender;
	QString                                      m_szSignalName;

	// if this object wraps a qt one, it is here
	QObject                                    * m_pObject;
	bool                                         m_bObjectOwner;       // do we have to destroy it ?

	// internal stuff for die()
	bool                                         m_bInDelayedDeath;
public:
	kvs_hobject_t handle(){ return m_hObject; };

	// the wrapped Qt object (may be 0!)
	QObject * object() const { return m_pObject; };
	void setObject(QObject * o,bool bIsOwned = true);

	const QString & getName(){ return m_szName; };

	KviKvsObject * parentObject(){ return (KviKvsObject *)parent(); };
	QWidget * parentScriptWidget();

	bool connectSignal(const QString &sigName,KviKvsObject * target,const QString &slotName);
	bool disconnectSignal(const QString &sigName,KviKvsObjectConnection * con);
	bool disconnectSignal(const QString &sigName,KviKvsObject * target,const QString & slotName);

	// Emits a signal by calling all the attacched slots in an unspecified order. 
	// Returns the number of slots called (may be 0, if no slot is connected)
	// The parameters are preserved.
	// this is intended to be called from other function calls (the parameters are copied from pOuterCall)
	// since we should NEVER emit totally spontaneous signals: all of them
	// should be generated inside object functions (either from scripting or by core calls)
	int emitSignal(const QString &sigName,KviKvsObjectFunctionCall * pOuterCall,KviKvsVariantList * pParams = 0);

	void setSignalSender(kvs_hobject_t hObject){ m_hSignalSender = hObject; };
	kvs_hobject_t signalSender(){ return m_hSignalSender; };
	void setSignalName(const QString &szSigName){ m_szSignalName = szSigName; };

	KviPointerHashTable<QString,KviKvsObjectFunctionHandler> * functionHandlers(){ return m_pFunctionHandlers; };

	KviKvsHash * dataContainer(){ return m_pdataContainer; };

	bool die();
	bool dieNow();

	KviKvsObjectClass * getExactClass(){ return m_pClass; };
	KviKvsObjectClass * getClass(const QString & classOverride = QString());
	bool inheritsClass(KviKvsObjectClass * pClass);
	bool inheritsClass(const QString &szClass);

	KviKvsObjectFunctionHandler * lookupFunctionHandler(const QString & funcName,const QString & classOverride = QString());

	// Registers a private implementation of a function
	// The function may or may not be already registered in the class
	// If szCode is empty the the private implementation is removed instead
	void registerPrivateImplementation(const QString &szFunctionName,const QString &szCode);
	
	// ONLY pCaller can be zero here!
	// please use one of the wrappers, if possible
	bool callFunction(
		KviKvsObject * pCaller,          // calling object, can be zero (used for the "internal" access list verification)
		const QString & fncName,         // name of the function to call
		const QString & classOverride,   // eventual class override for the functon call, may be QString()
		KviKvsRunTimeContext * pContext, // calling runtime context (you'll have problems with instantiating this... :P )
		KviKvsVariant * pRetVal,         // the return value
		KviKvsVariantList * pParams);    // the parameters for the call
	// a nice and simple wrapper: it accepts a parameter list only (eventually 0)
	bool callFunction(KviKvsObject * pCaller,const QString &fncName,KviKvsVariantList * pParams = 0);
	// this one gets a non null ret val too
	bool callFunction(KviKvsObject * pCaller,const QString &fncName,KviKvsVariant * pRetVal,KviKvsVariantList * pParams = 0);

	KviKvsObject * findChild(const QString &szClass,const QString &szName);
	void killAllChildrenWithClass(KviKvsObjectClass *cl);
protected:
	void registerConnection(KviKvsObjectConnection * con);
	bool unregisterConnection(KviKvsObjectConnection * con);

	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	
	void registerChild(KviKvsObject * c);
	void unregisterChild(KviKvsObject *c);
	
	virtual bool eventFilter(QObject *o,QEvent *e); //necessary ?
	virtual void timerEvent(QTimerEvent *e);
protected:
	bool function_name(KviKvsObjectFunctionCall *c);
	bool function_startTimer(KviKvsObjectFunctionCall *c);
	bool function_killTimer(KviKvsObjectFunctionCall *c);
	bool function_className(KviKvsObjectFunctionCall *c);
	bool function_findChild(KviKvsObjectFunctionCall *c);
	bool function_childCount(KviKvsObjectFunctionCall *c);
	bool function_emit(KviKvsObjectFunctionCall *c);
	bool function_children(KviKvsObjectFunctionCall *c);
	bool function_signalSender(KviKvsObjectFunctionCall *c);
	bool function_signalName(KviKvsObjectFunctionCall *c);
	bool function_destructor(KviKvsObjectFunctionCall *c);
	bool function_parent(KviKvsObjectFunctionCall *c);
	bool function_property(KviKvsObjectFunctionCall *c);
	bool function_setProperty(KviKvsObjectFunctionCall *c);
	bool function_listProperties(KviKvsObjectFunctionCall *c);
protected slots:
	void delayedDie();
	void objectDestroyed();
};


#define KVSO_PARAMETER(a,b,c,d) KVS_PARAMETER(a,b,c,d)

#define KVSO_PARAMETERS_BEGIN(pCall) \
	KVS_PARAMETERS_BEGIN(parameter_format_list)

#define KVSO_PARAMETERS_END(pCall) \
	KVS_PARAMETERS_END \
	if(!KviKvsParameterProcessor::process(pCall->params(),pCall->context(),parameter_format_list))return false;




#endif //!_KVI_KVS_OBJECT_H_