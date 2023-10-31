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

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::InterconnectionConfiguration()
//-----------------------------------------------------------------------------
InterconnectionConfiguration::InterconnectionConfiguration():
    Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfiguration::InterconnectionConfigurationCopy()
//-----------------------------------------------------------------------------
InterconnectionConfiguration::InterconnectionConfiguration(const InterconnectionConfiguration& other):
    Extendable(other),
isPresent_(other.isPresent_),
interconnectionRef_(other.interconnectionRef_)
{
    Copy::copyList(other.abstractorInstances_, abstractorInstances_);
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

    for (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor : *abstractorInstances_)
    {
        vlnvList.append(multipleAbstractor->getDependantVLNVs());
    }

    return vlnvList;
}
