//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommanddefpopup.cpp
//   Creation date : Fri 02 Jan 2004 14:09:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_treenode_specialcommanddefpopup.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"
#include "kvi_kvs_popupmanager.h"
#include "kvi_kvs_popupmenu.h"
#include "kvi_kvs_script.h"

void KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"extpopup\" For Special Command \"defpopup\"";
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup::dump(const char * prefix)
{
	qDebug("%s SpecialCommandDefpopupLabelExtpopup",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "NAME: ";
	x += m_szName;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	qDebug("%s",x.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup::execute(KviKvsRunTimeContext *,KviKvsPopupMenu * p)
{
	p->addExtPopup(m_szItemName,m_szName,m_szText,m_szIcon,m_szCondition);
	return true;
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelItem::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"item\" For Special Command \"defpopup\"";
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelItem::dump(const char * prefix)
{
	qDebug("%s SpecialCommandDefpopupLabelItem",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "INSTRUCTION: ";
	x += m_szInstruction;
	qDebug("%s",x.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelItem::execute(KviKvsRunTimeContext *,KviKvsPopupMenu * p)
{
	p->addItem(m_szItemName,m_szInstruction,m_szText,m_szIcon,m_szCondition);
	return true;
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelLabel::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"label\" For Special Command \"defpopup\"";
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelLabel::dump(const char * prefix)
{
	qDebug("%s SpecialCommandDefpopupLabelLabel",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	qDebug("%s",x.toUtf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	qDebug("%s",x.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelLabel::execute(KviKvsRunTimeContext *,KviKvsPopupMenu * p)
{
	p->addLabel(m_szItemName,m_szText,m_szIcon,m_szCondition);
	return true;
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"separator\" For Special Command \"defpopup\"";
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator::dump(const char * prefix)
{
	debug("%s SpecialCommandDefpopupLabelSeparator",prefix);
	QString tmp = prefix;
	tmp.append("  CONDITION:");
	tmp.append(m_szCondition);
	debug("%s",tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator::execute(KviKvsRunTimeContext *,KviKvsPopupMenu * p)
{
	p->addSeparator(m_szItemName,m_szCondition);
	return true;
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"epilogue\" For Special Command \"defpopup\"";
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue::dump(const char * prefix)
{
	debug("%s SpecialCommandDefpopupLabelEpilogue",prefix);
	QString tmp = prefix;
	tmp.append("  INSTRUCTION: ");
	tmp += m_szInstruction;
	debug("%s",tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue::execute(KviKvsRunTimeContext *,KviKvsPopupMenu * p)
{
	p->addEpilogue(m_szItemName,m_szInstruction);
	return true;
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"prologue\" For Special Command \"defpopup\"";
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue::dump(const char * prefix)
{
	debug("%s SpecialCommandDefpopupLabelPrologue",prefix);
	QString tmp = prefix;
	tmp.append("  INSTRUCTION: ");
	tmp += m_szInstruction;
	debug("%s",tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue::execute(KviKvsRunTimeContext *,KviKvsPopupMenu * p)
{
	p->addPrologue(m_szItemName,m_szInstruction);
	return true;
}


KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::KviKvsTreeNodeSpecialCommandDefpopupLabelPopup(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(pLocation,QString(),QString(),QString(),QString())
{
	m_pLabels = new KviPointerList<KviKvsTreeNodeSpecialCommandDefpopupLabel>;
	m_pLabels->setAutoDelete(true);
}

KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::~KviKvsTreeNodeSpecialCommandDefpopupLabelPopup()
{
	delete m_pLabels;
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::contextDescription(QString &szBuffer)
{
	szBuffer = "Label \"popup\" For Special Command \"defpopup\"";
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::dump(const char * prefix)
{
	debug("%s SpecialCommandDefpopupLabelPopup",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	debug("%s",x.toUtf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	debug("%s",x.toUtf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	debug("%s",x.toUtf8().data());
	for(KviKvsTreeNodeSpecialCommandDefpopupLabel * l = m_pLabels->first();l;l = m_pLabels->next())
		l->dump(tmp.toUtf8().data());
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::addLabel(KviKvsTreeNodeSpecialCommandDefpopupLabel * pLabel)
{
	pLabel->setParent(this);
	m_pLabels->append(pLabel);
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
	// the name will be reset anyway
	KviKvsPopupMenu * pNew = p->addPopup(m_szItemName,m_szText,m_szIcon,m_szCondition);
	return fill(c,pNew);
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::fill(KviKvsRunTimeContext* c,KviKvsPopupMenu * p)
{
	for(KviKvsTreeNodeSpecialCommandDefpopupLabel * l = m_pLabels->first();l;l = m_pLabels->next())
	{
		if(!l->execute(c,p))return false;
	}
	return true;
}


KviKvsTreeNodeSpecialCommandDefpopup::KviKvsTreeNodeSpecialCommandDefpopup(const QChar * pLocation,KviKvsTreeNodeData * pPopupName,KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * pMainPopup)
: KviKvsTreeNodeSpecialCommand(pLocation,"defpopup")
{
	m_pPopupName = pPopupName;
	m_pPopupName->setParent(this);
	m_pMainPopup = pMainPopup;
	m_pMainPopup->setParent(this);
}

KviKvsTreeNodeSpecialCommandDefpopup::~KviKvsTreeNodeSpecialCommandDefpopup()
{
	delete m_pPopupName;
	delete m_pMainPopup;
}

void KviKvsTreeNodeSpecialCommandDefpopup::contextDescription(QString &szBuffer)
{
	szBuffer = "Special Command \"defpopup\"";
}


void KviKvsTreeNodeSpecialCommandDefpopup::dump(const char * prefix)
{
	debug("%s SpecialCommandDefpopup",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pPopupName->dump(tmp.toUtf8().data());
	m_pMainPopup->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandDefpopup::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pPopupName->evaluateReadOnly(c,&v))return false;
	QString szName;
	v.asString(szName);

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c,&swl)))return false;
	}

	KviKvsPopupMenu * pPopup = KviKvsPopupManager::instance()->lookup(szName);
	bool bWasAlreadyThere;
	if(!pPopup)
	{
		if(m_pMainPopup->isEmpty())return true; // we wanted to remove it anyway: exit silently
		// we want to create it
		pPopup = new KviKvsPopupMenu(szName);
		KviKvsPopupManager::instance()->add(szName,pPopup);
		bWasAlreadyThere = false;
	} else {
		bWasAlreadyThere = true;

		if(pPopup->isLocked())
		{
			c->error(__tr2qs_ctx("The popup '%s' is actually locked: 'self-modifications' are not allowed","kvs"),&szName);
			return false;
		}

		if(m_pMainPopup->isEmpty())
		{
			// we want to remove it
			KviKvsPopupManager::instance()->remove(szName);
			return true;
		}
		// we want to (re)create it

		if(!swl.find('m',"--merge"))pPopup->doClear();
	}


	if(!m_pMainPopup->fill(c,pPopup))
	{
		if(!bWasAlreadyThere)KviKvsPopupManager::instance()->remove(szName);
		return false;
	}
	return true;
}