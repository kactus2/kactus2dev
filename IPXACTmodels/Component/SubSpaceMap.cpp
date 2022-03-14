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
masterReference_(),
segmentRef_()
{

}

//-----------------------------------------------------------------------------
// Function: SubSpaceMap::SubSpaceMap()
//-----------------------------------------------------------------------------
SubSpaceMap::SubSpaceMap(const SubSpaceMap& other):
MemoryBlockBase(other),
masterReference_(other.masterReference_),
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
        masterReference_ = other.masterReference_;
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
