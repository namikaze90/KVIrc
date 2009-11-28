//=============================================================================
//
//   File : kvi_tal_groupbox.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_tal_groupbox.h"

#include <QChildEvent>
#include <QGroupBox>

KviTalGroupBox::KviTalGroupBox(QWidget * pParent, char * pcName)
: QGroupBox(pParent)
{
	setObjectName(pcName);
	m_pLayout = new QHBoxLayout(this);
	setLayout(m_pLayout);
}

KviTalGroupBox::KviTalGroupBox(QWidget * pParent)
: QGroupBox(pParent)
{
	m_pLayout = 0;
}

KviTalGroupBox::KviTalGroupBox(const QString & szTitle, QWidget * pParent)
: QGroupBox(szTitle,pParent)
{
	m_pLayout = 0;
}

KviTalGroupBox::KviTalGroupBox(Qt::Orientation orientation, QWidget * pParent)
: QGroupBox(pParent)
{
	if(orientation == Qt::Vertical)
		m_pLayout = new QHBoxLayout(this);
	else
		m_pLayout = new QVBoxLayout(this);
	setLayout(m_pLayout);
}

KviTalGroupBox::KviTalGroupBox(Qt::Orientation orientation, const QString & szTitle, QWidget * pParent)
: QGroupBox(szTitle,pParent)
{
	mOrientation = orientation;
	if(orientation == Qt::Vertical)
		m_pLayout = new QHBoxLayout(this);
	else
		m_pLayout = new QVBoxLayout(this);
	setLayout(m_pLayout);
}

KviTalGroupBox::~KviTalGroupBox()
{
}

void KviTalGroupBox::childEvent(QChildEvent * e)
{
	if(!e->child()->isWidgetType())
		return;
	if(e->child()->parent() != this)
		return;

	switch(e->type())
	{
		case QEvent::ChildAdded:
			m_pLayout->addWidget((QWidget *)(e->child()));
			break;
		case QEvent::ChildRemoved:
			m_pLayout->removeWidget((QWidget *)(e->child()));
			break;
		default:
			break;
	}
}

void KviTalGroupBox::addSpace(int iSpace)
{
	if(m_pLayout)
	{
		if(mOrientation == Qt::Vertical)
			((QHBoxLayout*)m_pLayout)->addSpacing(iSpace);
		else
			((QVBoxLayout*)m_pLayout)->addSpacing(iSpace);
	}
}

void KviTalGroupBox::setOrientation(Qt::Orientation orientation)
{
	if(m_pLayout)
		delete m_pLayout;

	mOrientation = orientation;

	if(orientation == Qt::Vertical)
		m_pLayout = new QHBoxLayout(this);
	else
		m_pLayout = new QVBoxLayout(this);
	setLayout(m_pLayout);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_groupbox.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES