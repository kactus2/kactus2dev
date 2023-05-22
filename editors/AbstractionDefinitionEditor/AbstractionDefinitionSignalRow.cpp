//-----------------------------------------------------------------------------
// File: AbstractionDefinitionSignalRow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.01.2020
//
// Description:
// Represents a row in the abstraction ports table by grouping the Port, Wire and Transactional elements.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionSignalRow.h"

#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::()
//-----------------------------------------------------------------------------
AbstractionDefinitionSignalRow::AbstractionDefinitionSignalRow():
abstraction_(QSharedPointer<PortAbstraction>(new PortAbstraction())),
mode_(General::INTERFACE_MODE_COUNT),
wire_(),
transactional_(),
lockExtendData_(false),
lockPortData_(false)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::operator==()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionSignalRow::operator==(AbstractionDefinitionSignalRow const& other) const
{
    return  abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
        mode_ == other.mode_ && comparedSignalIsSameType(other) &&
        lockExtendData_ == other.lockExtendData_ && lockPortData_ == other.lockPortData_ &&
        (mode_ != General::SYSTEM || getSystemGroup() == other.getSystemGroup());
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::getSystemGroup()
//-----------------------------------------------------------------------------
QString AbstractionDefinitionSignalRow::getSystemGroup() const
{
    if (mode_ == General::SYSTEM)
    {
        if (wire_)
        {
            return wire_->getSystemGroup();
        }
        else if (transactional_)
        {
            return transactional_->getSystemGroup();
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::comparedSignalIsSameType()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionSignalRow::comparedSignalIsSameType(
    AbstractionDefinitionSignalRow const& comparisonSignal) const
{
    if ((wire_ && comparisonSignal.wire_) || (transactional_ && comparisonSignal.transactional_))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::operator!=()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionSignalRow::operator!=(AbstractionDefinitionSignalRow const& other) const
{
    return  abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
        mode_ != other.mode_ ||
        !comparedSignalIsSameType(other) ||
        getSystemGroup() != other.getSystemGroup() ||
        lockExtendData_ != other.lockExtendData_ || lockPortData_ != other.lockPortData_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionSignalRow::operator<( const AbstractionDefinitionSignalRow& other ) const
{
	// Order by name, mode and system group.
	if (abstraction_->getLogicalName() == other.abstraction_->getLogicalName()) 
    {
        if (mode_ == other.mode_)
        {
            return getSystemGroup() < other.getSystemGroup();
        }

		return mode_ < other.mode_;
	}
	else
    {
		return abstraction_->getLogicalName() < other.abstraction_->getLogicalName();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::getMode()
//-----------------------------------------------------------------------------
General::InterfaceMode AbstractionDefinitionSignalRow::getMode() const
{
    return mode_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::setMode()
//-----------------------------------------------------------------------------
void AbstractionDefinitionSignalRow::setMode(General::InterfaceMode const& newMode)
{
    mode_ = newMode;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::getPortAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinitionSignalRow::getPortAbstraction() const
{
    return abstraction_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::setPortAbstraction()
//-----------------------------------------------------------------------------
void AbstractionDefinitionSignalRow::setPortAbstraction(QSharedPointer<PortAbstraction> newAbstraction)
{
    abstraction_ = newAbstraction;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::getWire()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> AbstractionDefinitionSignalRow::getWire() const
{
    return wire_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::setWire()
//-----------------------------------------------------------------------------
void AbstractionDefinitionSignalRow::setWire(QSharedPointer<WirePort> newWire)
{
    wire_ = newWire;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::getTransactional()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> AbstractionDefinitionSignalRow::getTransactional() const
{
    return transactional_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::setTransactional()
//-----------------------------------------------------------------------------
void AbstractionDefinitionSignalRow::setTransactional(QSharedPointer<TransactionalPort> newTransactional)
{
    transactional_ = newTransactional;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::isExtendDataLocked()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionSignalRow::isExtendDataLocked() const
{
    return lockExtendData_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::lockExtendData()
//-----------------------------------------------------------------------------
void AbstractionDefinitionSignalRow::lockExtendData(bool newLockStatus)
{
    lockExtendData_ = newLockStatus;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::isPortDataLocked()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionSignalRow::isPortDataLocked() const
{
    return lockPortData_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionSignalRow::lockPortData()
//-----------------------------------------------------------------------------
void AbstractionDefinitionSignalRow::lockPortData(bool newLockStatus)
{
    lockPortData_ = newLockStatus;
}
