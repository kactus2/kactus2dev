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
ActiveInterface::ActiveInterface(QString const& componentRef, QString const& busRef):
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
    for (QString const& portName : *other.excludePorts_)
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

        excludePorts_->clear();
        for (QString const& portName : *other.excludePorts_)
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
        (componentRef_.compare(other.componentRef_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator!=()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator!=(const ActiveInterface& other) const
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::operator<()
//-----------------------------------------------------------------------------
bool ActiveInterface::operator<(const ActiveInterface& other) const
{
    int busRefeferencecomparison = HierInterface::operator<(other);
    if (busRefeferencecomparison == 0)
    {
        return componentRef_.compare(other.componentRef_, Qt::CaseInsensitive) < 0;
    }
    else
    {
        return busRefeferencecomparison < 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::references()
//-----------------------------------------------------------------------------
bool ActiveInterface::references(QString const& instanceName, QString const& busInterfaceName) const
{
    return instanceName == componentRef_ && busInterfaceName == getBusReference();
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::getComponentReference()
//-----------------------------------------------------------------------------
QString ActiveInterface::getComponentReference() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::setComponentReference()
//-----------------------------------------------------------------------------
void ActiveInterface::setComponentReference(QString const& newComponentReference)
{
    componentRef_ = newComponentReference;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::getExcludePorts()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> ActiveInterface::getExcludePorts() const
{
    return excludePorts_;
}

//-----------------------------------------------------------------------------
// Function: ActiveInterface::setExcludePorts()
//-----------------------------------------------------------------------------
void ActiveInterface::setExcludePorts(QSharedPointer<QStringList> newExcludePorts)
{
    excludePorts_ = newExcludePorts;
}
