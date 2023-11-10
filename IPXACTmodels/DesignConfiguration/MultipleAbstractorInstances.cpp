//-----------------------------------------------------------------------------
// File: MultipleAbstractorInstances.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:abstractorInstances element.
//-----------------------------------------------------------------------------

#include "MultipleAbstractorInstances.h"

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::MultipleAbstractorInstances()
//-----------------------------------------------------------------------------
MultipleAbstractorInstances::MultipleAbstractorInstances()
{

}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::MultipleAbstractorInstancesCopy()
//-----------------------------------------------------------------------------
MultipleAbstractorInstances::MultipleAbstractorInstances(const MultipleAbstractorInstances& other):
isPresent_(other.isPresent_)
{
    Copy::copyList(other.interfaceReferences_, interfaceReferences_);

    Copy::copyList(other.abstractorInstances_, abstractorInstances_);
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::getIsPresent()
//-----------------------------------------------------------------------------
QString MultipleAbstractorInstances::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::setIsPresent()
//-----------------------------------------------------------------------------
void MultipleAbstractorInstances::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::getInterfaceReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<InterfaceRef> > > MultipleAbstractorInstances::getInterfaceReferences() const
{
    return interfaceReferences_;
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::setInterfaceReferences()
//-----------------------------------------------------------------------------
void MultipleAbstractorInstances::setInterfaceReferences(
    QSharedPointer<QList<QSharedPointer<InterfaceRef> > > newInterfaceReferences)
{
    interfaceReferences_ = newInterfaceReferences;
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::getAbstractorInstances()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > MultipleAbstractorInstances::getAbstractorInstances() const
{
    return abstractorInstances_;
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::setAbstractorInstances()
//-----------------------------------------------------------------------------
void MultipleAbstractorInstances::setAbstractorInstances(
    QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > newAbstractorInstances)
{
    abstractorInstances_ = newAbstractorInstances;
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::getDependantVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> MultipleAbstractorInstances::getDependantVLNVs() const
{
    QList<VLNV> vlnvList;

    for (QSharedPointer<AbstractorInstance> abstractor : *abstractorInstances_)
    {
        vlnvList.append(*abstractor->getAbstractorRef());
    }

    return vlnvList;
}