//-----------------------------------------------------------------------------
// File: MonitorInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.08.2015
//
// Description:
// Describes the ipxact:MonitorInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "MonitorInterface.h"

//-----------------------------------------------------------------------------
// Function: MonitorInterface::MonitorInterface()
//-----------------------------------------------------------------------------
MonitorInterface::MonitorInterface(std::string componentRef, std::string busRef):
HierInterface(busRef),
componentReference_(componentRef)
{

}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::MonitorInterface()
//-----------------------------------------------------------------------------
MonitorInterface::MonitorInterface(const MonitorInterface& other) :
HierInterface(other),
componentReference_(other.componentReference_),
path_(other.path_)
{
   
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::operator=()
//-----------------------------------------------------------------------------
MonitorInterface& MonitorInterface::operator=(const MonitorInterface& other)
{
    if (this != &other)
    {
        HierInterface::operator=(other);
        componentReference_ = other.componentReference_;
        path_ = other.path_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::operator==()
//-----------------------------------------------------------------------------
bool MonitorInterface::operator==(const MonitorInterface& other) const
{
    return (HierInterface::operator==(other)) &&
        (componentReference_.compare(other.componentReference_) == 0);
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::operator!=()
//-----------------------------------------------------------------------------
bool MonitorInterface::operator!=(const MonitorInterface& other) const
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::operator<()
//-----------------------------------------------------------------------------
bool MonitorInterface::operator<(const MonitorInterface& other) const
{
    int busRefeferencecomparison = HierInterface::operator<(other);
    if (busRefeferencecomparison == 0)
    {
        return componentReference_.compare(other.componentReference_) < 0;
    }
    else
    {
        return busRefeferencecomparison < 0;
    }
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::references()
//-----------------------------------------------------------------------------
bool MonitorInterface::references(std::string const& instanceName, std::string const& busInterfaceName) const
{
    return instanceName == componentReference_ && busInterfaceName == getBusReference();
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::getComponentReference()
//-----------------------------------------------------------------------------
std::string MonitorInterface::getComponentReference() const
{
    return componentReference_;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::setComponentReference()
//-----------------------------------------------------------------------------
void MonitorInterface::setComponentReference(std::string const& newComponentReference)
{
    componentReference_ = newComponentReference;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::getPath()
//-----------------------------------------------------------------------------
std::string MonitorInterface::getPath() const
{
    return path_;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::setPath()
//-----------------------------------------------------------------------------
void MonitorInterface::setPath(std::string const& newPath)
{
    path_ = newPath;
}