//-----------------------------------------------------------------------------
// File: ActiveInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.08.2015
//
// Description:
// Describes the ipxact:activeInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "ActiveInterface.h"

//-----------------------------------------------------------------------------
// Function: ActiveInterface::ActiveInterface()
//-----------------------------------------------------------------------------
ActiveInterface::ActiveInterface(std::string const& componentRef, std::string const& busRef):
HierInterface(busRef),
componentRef_(componentRef)
{

}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::ActiveInterface()
//-----------------------------------------------------------------------------
ActiveInterface::ActiveInterface(const ActiveInterface& other) :
HierInterface(other),
componentRef_(other.componentRef_)
{
    for (auto const& portName : *other.excludePorts_)
    {
        excludePorts_->append(portName);
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator=()
//-----------------------------------------------------------------------------
ActiveInterface& ActiveInterface::operator=(const ActiveInterface& other)
{
    if (this != &other)
    {
        HierInterface::operator=(other);
        componentRef_ = other.componentRef_;

        for (auto const& portName : *other.excludePorts_)
        {
            excludePorts_->append(portName);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator==()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator==(const ActiveInterface& other) const
{
    return (HierInterface::operator==(other)) &&
        (componentRef_.compare(other.componentRef_) == 0);
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator!=()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator!=(const ActiveInterface& other)
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator<()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator<(const ActiveInterface& other)
{
    int busRefeferencecomparison = HierInterface::operator<(other);
    if (busRefeferencecomparison == 0)
    {
        return componentRef_.compare(other.componentRef_) < 0;
    }
    else
    {
        return busRefeferencecomparison < 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::references()
//-----------------------------------------------------------------------------
bool ActiveInterface::references(std::string const& instanceName, std::string const& busInterfaceName)
{
    return instanceName == componentRef_ && busInterfaceName == getBusReference();
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::getComponentReference()
//-----------------------------------------------------------------------------
std::string ActiveInterface::getComponentReference() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::setComponentReference()
//-----------------------------------------------------------------------------
void ActiveInterface::setComponentReference(std::string const& newComponentReference)
{
    componentRef_ = newComponentReference;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::getExcludePorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<std::string> > ActiveInterface::getExcludePorts() const
{
    return excludePorts_;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::setExcludePorts()
//-----------------------------------------------------------------------------
void ActiveInterface::setExcludePorts(QSharedPointer<QList<std::string> > newExcludePorts)
{
    excludePorts_ = newExcludePorts;
}
