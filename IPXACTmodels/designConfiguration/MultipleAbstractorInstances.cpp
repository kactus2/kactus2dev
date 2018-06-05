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

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::MultipleAbstractorInstances()
//-----------------------------------------------------------------------------
MultipleAbstractorInstances::MultipleAbstractorInstances():
isPresent_(),
interfaceReferences_(new QList<QSharedPointer<InterfaceRef> >),
abstractorInstances_(new QList<QSharedPointer<AbstractorInstance> >)
{

}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::MultipleAbstractorInstancesCopy()
//-----------------------------------------------------------------------------
MultipleAbstractorInstances::MultipleAbstractorInstances(const MultipleAbstractorInstances& other):
isPresent_(other.isPresent_),
interfaceReferences_(new QList<QSharedPointer<InterfaceRef> >),
abstractorInstances_(new QList<QSharedPointer<AbstractorInstance> >)
{
    foreach (QSharedPointer<InterfaceRef> interfaceReference, *other.interfaceReferences_)
    {
        if (interfaceReference)
        {
            QSharedPointer<InterfaceRef> copy = QSharedPointer<InterfaceRef>(
                new InterfaceRef(*interfaceReference.data()));
            interfaceReferences_->append(copy);
        }
    }

    foreach (QSharedPointer<AbstractorInstance> abstractorInstance, *other.abstractorInstances_)
    {
        if (abstractorInstance)
        {
            QSharedPointer<AbstractorInstance> copy = QSharedPointer<AbstractorInstance>(
                new AbstractorInstance(*abstractorInstance.data()));
            abstractorInstances_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MultipleAbstractorInstances::~MultipleAbstractorInstances()
//-----------------------------------------------------------------------------
MultipleAbstractorInstances::~MultipleAbstractorInstances()
{

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

    foreach (QSharedPointer<AbstractorInstance> abstractor, *abstractorInstances_)
    {
        VLNV abstractorRefVLNV = *abstractor->getAbstractorRef();
        vlnvList.append(abstractorRefVLNV);
    }

    return vlnvList;
}