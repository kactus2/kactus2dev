//-----------------------------------------------------------------------------
// File: InterconnectionConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Matches the ipxact:interconnectionConfiguration element.
//-----------------------------------------------------------------------------

#include "InterconnectionConfiguration.h"

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::InterconnectionConfiguration()
//-----------------------------------------------------------------------------
InterconnectionConfiguration::InterconnectionConfiguration():
isPresent_(),
interconnectionRef_(),
abstractorInstances_(new QList<QSharedPointer<MultipleAbstractorInstances> >)
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::InterconnectionConfigurationCopy()
//-----------------------------------------------------------------------------
InterconnectionConfiguration::InterconnectionConfiguration(const InterconnectionConfiguration& other):
isPresent_(other.isPresent_),
interconnectionRef_(other.interconnectionRef_),
abstractorInstances_(new QList<QSharedPointer<MultipleAbstractorInstances> > )
{
    foreach (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor, *other.abstractorInstances_)
    {
        if (multipleAbstractor)
        {
            QSharedPointer<MultipleAbstractorInstances> copy = QSharedPointer<MultipleAbstractorInstances>(
                new MultipleAbstractorInstances(*multipleAbstractor.data()));
            abstractorInstances_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::~InterconnectionConfiguration()
//-----------------------------------------------------------------------------
InterconnectionConfiguration::~InterconnectionConfiguration()
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::getIsPresent()
//-----------------------------------------------------------------------------
QString InterconnectionConfiguration::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::setIsPresent()
//-----------------------------------------------------------------------------
void InterconnectionConfiguration::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::getInterconnectionReference()
//-----------------------------------------------------------------------------
QString InterconnectionConfiguration::getInterconnectionReference() const
{
    return interconnectionRef_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::setInterconnectionReference()
//-----------------------------------------------------------------------------
void InterconnectionConfiguration::setInterconnectionReference(QString const& newInterconnecionReference)
{
    interconnectionRef_ = newInterconnecionReference;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::getAbstractorInstances()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > InterconnectionConfiguration::
    getAbstractorInstances() const
{
    return abstractorInstances_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::setAbstractorInstances()
//-----------------------------------------------------------------------------
void InterconnectionConfiguration::setAbstractorInstances(
    QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > newAbstractorInstances)
{
    abstractorInstances_ = newAbstractorInstances;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::getDependantVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> InterconnectionConfiguration::getDependantVLNVs() const
{
    QList<VLNV> vlnvList;

    foreach (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor, *abstractorInstances_)
    {
        vlnvList.append(multipleAbstractor->getDependantVLNVs());
    }

    return vlnvList;
}