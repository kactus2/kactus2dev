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
MonitorInterface::MonitorInterface(QString componentRef /* = QString("") */, QString busRef /* = QString("") */):
HierInterface(busRef),
componentReference_(componentRef),
path_()
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
// Function: MonitorInterface::~MonitorInterface()
//-----------------------------------------------------------------------------
MonitorInterface::~MonitorInterface()
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
bool MonitorInterface::operator==(const MonitorInterface& other)
{
    return (HierInterface::operator==(other)) &&
        (componentReference_.compare(other.componentReference_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::operator!=()
//-----------------------------------------------------------------------------
bool MonitorInterface::operator!=(const MonitorInterface& other)
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::operator<()
//-----------------------------------------------------------------------------
bool MonitorInterface::operator<(const MonitorInterface& other)
{
    int busRefeferencecomparison = HierInterface::operator<(other);
    if (busRefeferencecomparison == 0)
    {
        return componentReference_.compare(other.componentReference_, Qt::CaseInsensitive) < 0;
    }
    else
    {
        return busRefeferencecomparison < 0;
    }
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::references()
//-----------------------------------------------------------------------------
bool MonitorInterface::references(QString const& instanceName, QString const& busInterfaceName)
{
    return instanceName == componentReference_ && busInterfaceName == getBusReference();
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::getComponentReference()
//-----------------------------------------------------------------------------
QString MonitorInterface::getComponentReference() const
{
    return componentReference_;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::setComponentReference()
//-----------------------------------------------------------------------------
void MonitorInterface::setComponentReference(QString const& newComponentReference)
{
    componentReference_ = newComponentReference;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::getPath()
//-----------------------------------------------------------------------------
QString MonitorInterface::getPath() const
{
    return path_;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterface::setPath()
//-----------------------------------------------------------------------------
void MonitorInterface::setPath(QString const& newPath)
{
    path_ = newPath;
}