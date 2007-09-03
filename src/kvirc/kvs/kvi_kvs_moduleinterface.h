#ifndef _KVI_KVS_MODULEINTERFACE_H_
#define _KVI_KVS_MODULEINTERFACE_H_
//=============================================================================
//
//   File : kvi_kvs_moduleinterface.h
//   Created on Tue 16 Dec 2003 00:27:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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


#include "kvi_settings.h"



#include "kvi_kvs_runtimecall.h"
#include "kvi_kvs_parameterprocessor.h"
#include "kvi_kvs_switchlist.h"
#include "kvi_kvs_script.h"
#include "kvi_list.h"
#include "kvi_qstring.h"

class KviModule;
class KviKvsTreeNodeDataList;

class KVIRC_API KviKvsModuleRunTimeCall : public KviKvsRunTimeCall
{
protected:
	KviModule             * m_pModule;
public:
	KviKvsModuleRunTimeCall(KviModule * pModule,
					KviKvsRunTimeContext * pContext,
					KviKvsVariantList * pParams)
		: KviKvsRunTimeCall(pContext,pParams), m_pModule(pModule) {}
	~KviKvsModuleRunTimeCall(){};				
public:
	KviModule * module(){ return m_pModule; };
};

class KVIRC_API KviKvsModuleEventCall : public KviKvsModuleRunTimeCall
{
public:
	KviKvsModuleEventCall(KviModule * pModule,
					KviKvsRunTimeContext * pContext,
					KviKvsVariantList * pParams)
		: KviKvsModuleRunTimeCall(pModule,pContext,pParams){};
	~KviKvsModuleEventCall(){};
};

class KVIRC_API KviKvsModuleCommandCall : public KviKvsModuleRunTimeCall
{
protected:
	KviKvsSwitchList * m_pSwitchList;
public:
	KviKvsModuleCommandCall(KviModule * pModule,
					KviKvsRunTimeContext * pContext,
					KviKvsVariantList * pParams,
					KviKvsSwitchList * pSwitches)
		: KviKvsModuleRunTimeCall(pModule,pContext,pParams), m_pSwitchList(pSwitches){};
	~KviKvsModuleCommandCall(){};
public:
	KviKvsSwitchList * switches(){ return m_pSwitchList; };
	KviKvsSwitchList * switchList(){ return m_pSwitchList; };
	
	// forwarders for the switch list
	bool hasSwitch(unsigned short u,const QString &szSwitch){ return (m_pSwitchList->find(u,szSwitch) != 0); };
	KviKvsVariant * getSwitch(unsigned short u,const QString &szSwitch){ return m_pSwitchList->find(u,szSwitch); };
};

class KVIRC_API KviKvsModuleCallbackCommandCall : public KviKvsModuleCommandCall
{
protected:
	const KviKvsScript     * m_pCallback;
	KviKvsTreeNodeDataList * m_pParameterDataList; // core subtree that rappresents the parameter list
public:
	KviKvsModuleCallbackCommandCall(KviModule * pModule,
					KviKvsRunTimeContext * pContext,
					KviKvsVariantList * pParams,
					KviKvsSwitchList * pSwitches,
					const KviKvsScript * pCallback,
					KviKvsTreeNodeDataList * pDataList)
		: KviKvsModuleCommandCall(pModule,pContext,pParams,pSwitches), m_pCallback(pCallback), m_pParameterDataList(pDataList) {};
	~KviKvsModuleCallbackCommandCall(){};
public:
	// Never NULL, but may have empty code
	const KviKvsScript * callback(){ return m_pCallback; };
	virtual bool getParameterCode(unsigned int uParamIdx,QString &szParamBuffer);
};


class KVIRC_API KviKvsModuleFunctionCall : public KviKvsModuleRunTimeCall
{
	friend class KviKvsTreeNodeModuleFunctionCall;
protected:
	KviKvsVariant * m_pResult;
public:
	KviKvsModuleFunctionCall(KviModule * pModule,
					KviKvsRunTimeContext * pContext,
					KviKvsVariantList * pParams,
					KviKvsVariant * pResult)
		: KviKvsModuleRunTimeCall(pModule,pContext,pParams), m_pResult(pResult)
		{};
	~KviKvsModuleFunctionCall(){};
public:
	KviKvsVariant * returnValue(){ return m_pResult; };
};



typedef bool (*KviKvsModuleSimpleCommandExecRoutine)(KviKvsModuleCommandCall * c);
typedef bool (*KviKvsModuleFunctionExecRoutine)(KviKvsModuleFunctionCall * c);
typedef bool (*KviKvsModuleCallbackCommandExecRoutine)(KviKvsModuleCallbackCommandCall * c);
typedef bool (*KviKvsModuleEventHandlerRoutine)(KviKvsModuleEventCall * c);



class KVIRC_API KviKvsModuleInterface
{
	friend class KviKvsModuleManager;
public:
	KviKvsModuleInterface();
	~KviKvsModuleInterface();
protected:
	QHash<QString,KviKvsModuleSimpleCommandExecRoutine*>         * m_pModuleSimpleCommandExecRoutineDict;
	QHash<QString,KviKvsModuleFunctionExecRoutine*>              * m_pModuleFunctionExecRoutineDict;
	QHash<QString,KviKvsModuleCallbackCommandExecRoutine*>       * m_pModuleCallbackCommandExecRoutineDict;
public:
	void kvsRegisterSimpleCommand(const QString &szCommand,KviKvsModuleSimpleCommandExecRoutine r);
	void kvsRegisterCallbackCommand(const QString &szCommand,KviKvsModuleCallbackCommandExecRoutine r);
	void kvsRegisterFunction(const QString &szFunction,KviKvsModuleFunctionExecRoutine r);
	bool kvsRegisterAppEventHandler(unsigned int iEventIdx,KviKvsModuleEventHandlerRoutine r);
	bool kvsRegisterRawEventHandler(unsigned int iRawIdx,KviKvsModuleEventHandlerRoutine r);

	void kvsUnregisterSimpleCommand(const QString &szCommand)
		{ delete m_pModuleSimpleCommandExecRoutineDict->take(szCommand); };
	void kvsUnregisterCallbackCommand(const QString &szCommand)
		{ delete m_pModuleCallbackCommandExecRoutineDict->take(szCommand); };
	void kvsUnregisterFunction(const QString &szFunction)
		{ delete m_pModuleFunctionExecRoutineDict->take(szFunction); };
	void kvsUnregisterAppEventHandler(unsigned int iEventIdx);
	void kvsUnregisterRawEventHandler(unsigned int iRawIdx);

	void kvsUnregisterAllSimpleCommands();
	void kvsUnregisterAllCallbackCommands();
	void kvsUnregisterAllFunctions();
	void kvsUnregisterAllAppEventHandlers();
	void kvsUnregisterAllRawEventHandlers();
	void kvsUnregisterAllEventHandlers();

	KviKvsModuleSimpleCommandExecRoutine * kvsFindSimpleCommand(const QString &szCommand)
		{ return m_pModuleSimpleCommandExecRoutineDict->value(szCommand); };
	KviKvsModuleCallbackCommandExecRoutine * kvsFindCallbackCommand(const QString &szCommand)
		{ return m_pModuleCallbackCommandExecRoutineDict->value(szCommand); };
	KviKvsModuleFunctionExecRoutine * kvsFindFunction(const QString &szFunction)
		{ return m_pModuleFunctionExecRoutineDict->value(szFunction); };

	void completeCommand(const QString &cmd,KviPtrList<QString> * matches);
	void completeFunction(const QString &cmd,KviPtrList<QString> * matches);
protected:
	void registerDefaultCommands();
};



#define KVSM_REGISTER_SIMPLE_COMMAND(_pModule,_szCmd,_procname) \
	_pModule->kvsRegisterSimpleCommand(_szCmd,_procname);

#define KVSM_UNREGISTER_SIMPLE_COMMAND(_pModule,_szCmd) \
	_pModule->kvsUnregisterSimpleCommand(_szCmd);

#define KVSM_REGISTER_CALLBACK_COMMAND(_pModule,_szCmd,_procname) \
	_pModule->kvsRegisterCallbackCommand(_szCmd,_procname);

#define KVSM_UNREGISTER_CALLBACK_COMMAND(_pModule,_szCmd) \
	_pModule->kvsUnregisterCallbackCommand(_szCmd);

#define KVSM_REGISTER_FUNCTION(_pModule,_szFnc,_procname) \
	_pModule->kvsRegisterFunction(_szFnc,_procname); \

#define KVSM_UNREGISTER_FUNCTION(_pModule,_szFnc) \
	_pModule->kvsUnregisterFunction(_szFnc);

#define KVSM_UNREGISTER_ALL_SIMPLE_COMMANDS(_pModule) \
	_pModule->kvsUnregisterAllSimpleCommands();

#define KVSM_UNREGISTER_ALL_CALLBACK_COMMANDS(_pModule) \
	_pModule->kvsUnregisterAllCallbackCommands();

#define KVSM_UNREGISTER_ALL_FUNCTIONS(_pModule) \
	_pModule->kvsUnregisterAllFunctions();


#define KVSM_PARAMETER(a,b,c,d) KVS_PARAMETER(a,b,c,d)
#define KVSM_PARAMETER_IGNORED(a) KVS_PARAMETER_IGNORED(a)

#define KVSM_PARAMETERS_BEGIN(pCall) \
	KVS_PARAMETERS_BEGIN(parameter_format_list)

#define KVSM_PARAMETERS_END(pCall) \
	KVS_PARAMETERS_END \
	if(!KviKvsParameterProcessor::process(pCall->params(),pCall->context(),parameter_format_list))return false;

#define KVSM_REQUIRE_CONNECTION(pCall) \
	if(!pCall->window()->context())return c->context()->errorNoIrcContext(); \
	if(!pCall->window()->connection())return c->context()->warningNoIrcConnection();



#endif //!_KVI_KVS_MODULEINTERFACE_H_
