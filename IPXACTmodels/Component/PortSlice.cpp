//-----------------------------------------------------------------------------
// File: PortSlice.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Describes the ipxact:portSlice element.
//-----------------------------------------------------------------------------

#include "PortSlice.h"

//-----------------------------------------------------------------------------
// Function: PortSlice::PortSlice()
//-----------------------------------------------------------------------------
PortSlice::PortSlice(QString const& name) :
NameGroup(name)
{

}

//-----------------------------------------------------------------------------
// Function: PortSlice::PortSlice()
//-----------------------------------------------------------------------------
PortSlice::PortSlice(const PortSlice &other) :
    NameGroup(other),
    portRef_(other.portRef_)
{
    if (other.partSelect_ != nullptr)
    {
        partSelect_ = QSharedPointer<PartSelect>(new PartSelect(*other.partSelect_));
    }
}

//-----------------------------------------------------------------------------
// Function: PortSlice::operator=()
//-----------------------------------------------------------------------------
PortSlice& PortSlice::operator=(const PortSlice& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        portRef_ = other.portRef_;
        
        if (other.partSelect_ != nullptr)
        {
            partSelect_ = QSharedPointer<PartSelect>(new PartSelect(*other.partSelect_));
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: PortSlice::~PortSlice()
//-----------------------------------------------------------------------------
PortSlice::~PortSlice()
{

}

//-----------------------------------------------------------------------------
// Function: PortSlice::getPortRef()
//-----------------------------------------------------------------------------
QString PortSlice::getPortRef() const
{
    return portRef_;
}

//-----------------------------------------------------------------------------
// Function: PortSlice::setPortNameRef()
//-----------------------------------------------------------------------------
void PortSlice::setPortRef(QString const& newPortRef)
{
    portRef_ = newPortRef;
}

//-----------------------------------------------------------------------------
// Function: PortSlice::getPartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> PortSlice::getPartSelect() const
{
    return partSelect_;
}

//-----------------------------------------------------------------------------
// Function: PortSlice::setPartSelect()
//-----------------------------------------------------------------------------
void PortSlice::setPartSelect(QSharedPointer<PartSelect> partSelect)
{
    partSelect_ = partSelect;
}

//-----------------------------------------------------------------------------
// Function: PortSlice::getLeftRange()
//-----------------------------------------------------------------------------
QString PortSlice::getLeftRange() const
{
    if (partSelect_)
    {
        return partSelect_->getLeftRange();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: PortSlice::getRightRange()
//-----------------------------------------------------------------------------
QString PortSlice::getRightRange() const
{
    if (partSelect_)
    {
        return partSelect_->getRightRange();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: PortSlice::setLeftRange()
//-----------------------------------------------------------------------------
void PortSlice::setLeftRange(QString const& newLeftRange)
{
    if (partSelect_.isNull())
    {
        partSelect_ = QSharedPointer<PartSelect>(new PartSelect());
    }

    return partSelect_->setLeftRange(newLeftRange);
}

//-----------------------------------------------------------------------------
// Function: PortSlice::setRightRange()
//-----------------------------------------------------------------------------
void PortSlice::setRightRange(QString const& newRightRange)
{
    if (partSelect_.isNull())
    {
        partSelect_ = QSharedPointer<PartSelect>(new PartSelect());
    }

    return partSelect_->setRightRange(newRightRange);
}
