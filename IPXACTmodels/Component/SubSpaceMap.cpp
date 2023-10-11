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
SubSpaceMap::SubSpaceMap(QString const& name, QString const& baseAddress):
MemoryBlockBase(name, baseAddress),
initiatorReference_(),
segmentRef_()
{

}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::SubSpaceMap()
//-----------------------------------------------------------------------------
SubSpaceMap::SubSpaceMap(const SubSpaceMap& other):
MemoryBlockBase(other),
initiatorReference_(other.initiatorReference_),
segmentRef_(other.segmentRef_)
{

}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::operator=()
//-----------------------------------------------------------------------------
SubSpaceMap& SubSpaceMap::operator=( const SubSpaceMap& other)
{
    if (this != &other)
    {
        MemoryBlockBase::operator=(other);
        initiatorReference_ = other.initiatorReference_;
        segmentRef_ = other.segmentRef_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::clone()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockBase> SubSpaceMap::clone() const
{
    return QSharedPointer<SubSpaceMap>(new SubSpaceMap(*this));
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::getMasterReference()
//-----------------------------------------------------------------------------
QString SubSpaceMap::getInitiatorReference() const
{
    return initiatorReference_;
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::setMasterReference()
//-----------------------------------------------------------------------------
void SubSpaceMap::setInitiatorReference(QString const& newInitiatorReference)
{
    initiatorReference_ = newInitiatorReference;
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
