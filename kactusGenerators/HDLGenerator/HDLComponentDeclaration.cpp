//-----------------------------------------------------------------------------
// File: HDLComponentDeclaration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for HDl component declarations.
//-----------------------------------------------------------------------------

#include "HDLComponentDeclaration.h"

#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: HDLComponentDeclaration::HDLComponentDeclaration()
//-----------------------------------------------------------------------------
HDLComponentDeclaration::HDLComponentDeclaration(QSharedPointer<Component> component, QObject* parent):
HDLObject(parent, component->getVlnv()->getName(), component->getVlnv()->getName(), QString(), component->getDescription()),
component_(component),
instantations_() 
{
	Q_ASSERT(component);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentDeclaration::~HDLComponentDeclaration()
//-----------------------------------------------------------------------------
HDLComponentDeclaration::~HDLComponentDeclaration()
{

}

//-----------------------------------------------------------------------------
// Function: HDLComponentDeclaration::componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> HDLComponentDeclaration::componentModel() const
{
	return component_;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentDeclaration::addInstantiation()
//-----------------------------------------------------------------------------
void HDLComponentDeclaration::addInstantiation(QSharedPointer<HDLComponentInstance> instance )
{
	Q_ASSERT(!instantations_.contains(instance));
	instantations_.append(instance);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentDeclaration::instances()
//-----------------------------------------------------------------------------
QList<QSharedPointer< HDLComponentInstance> > HDLComponentDeclaration::instances() const
{
    return instantations_;
}


