//-----------------------------------------------------------------------------
// File: SubSpaceMap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.01.2022
//
// Description:
// Contains elements for sub space maps.
//-----------------------------------------------------------------------------

#include "SubSpaceMap.h"

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::SubSpaceMap()
//-----------------------------------------------------------------------------
SubSpaceMap::SubSpaceMap(QString const& name) :
NameGroup(name),
Extendable(),
masterReference_(),
segmentRef_(),
isPresent_(),
baseAddress_(),
parameters_(new QList<QSharedPointer<Parameter> >())
{

}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::SubSpaceMap()
//-----------------------------------------------------------------------------
SubSpaceMap::SubSpaceMap(const SubSpaceMap& other):
NameGroup(other),
Extendable(other),
masterReference_(),
segmentRef_(),
isPresent_(),
baseAddress_(),
parameters_(new QList<QSharedPointer<Parameter> >())
{
    copyParameters(other);
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::copyParameters()
//-----------------------------------------------------------------------------
void SubSpaceMap::copyParameters(SubSpaceMap const& other)
{
    parameters_->reserve(other.parameters_->count());
    foreach(QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
        parameters_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::operator=()
//-----------------------------------------------------------------------------
SubSpaceMap& SubSpaceMap::operator=( const SubSpaceMap& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        masterReference_ = other.masterReference_;
        segmentRef_ = other.segmentRef_;
        isPresent_ = other.isPresent_;
        baseAddress_ = other.baseAddress_;

        parameters_->clear();
        copyParameters(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::clone()
//-----------------------------------------------------------------------------
QSharedPointer<SubSpaceMap> SubSpaceMap::clone() const
{
    return QSharedPointer<SubSpaceMap>(new SubSpaceMap(*this));
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::getMasterReference()
//-----------------------------------------------------------------------------
QString SubSpaceMap::getMasterReference() const
{
    return masterReference_;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::setMasterReference()
//-----------------------------------------------------------------------------
void SubSpaceMap::setMasterReference(QString const& newMasterReference)
{
    masterReference_ = newMasterReference;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::getSegmentReference()
//-----------------------------------------------------------------------------
QString SubSpaceMap::getSegmentReference() const
{
    return segmentRef_;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::setSegmentReference()
//-----------------------------------------------------------------------------
void SubSpaceMap::setSegmentReference(QString const& newSegmentReference)
{
    segmentRef_ = newSegmentReference;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::getIsPresent()
//-----------------------------------------------------------------------------
QString SubSpaceMap::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::setIsPresent()
//-----------------------------------------------------------------------------
void SubSpaceMap::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::getBaseAddress()
//-----------------------------------------------------------------------------
QString SubSpaceMap::getBaseAddress() const
{
    return baseAddress_;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::setBaseAddress()
//-----------------------------------------------------------------------------
void SubSpaceMap::setBaseAddress(QString const& newBaseAddress)
{
    baseAddress_ = newBaseAddress;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::getParametesrs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > SubSpaceMap::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::setParameters()
//-----------------------------------------------------------------------------
void SubSpaceMap::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}
