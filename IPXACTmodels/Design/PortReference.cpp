//-----------------------------------------------------------------------------
// File: PortReference.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.08.2015
//
// Description:
// Describes either an internal or external port reference in an ad-hoc connection.
//-----------------------------------------------------------------------------

#include "PortReference.h"

//-----------------------------------------------------------------------------
// Function: PortReference::PortReference()
//-----------------------------------------------------------------------------
PortReference::PortReference(QString const& portRef, QString const& componentRef /* = QString() */) :
portRef_(portRef),
componentRef_(componentRef),
isPresent_(),
partSelect_()
{

}

//-----------------------------------------------------------------------------
// Function: PortReference::PortReference()
//-----------------------------------------------------------------------------
PortReference::PortReference(const PortReference& other):
portRef_(other.portRef_),
componentRef_(other.componentRef_),
isPresent_(other.isPresent_),
partSelect_(new PartSelect(*other.partSelect_.data()))
{

}

//-----------------------------------------------------------------------------
// Function: PortReference::~PortReference()
//-----------------------------------------------------------------------------
PortReference::~PortReference()
{

}

//-----------------------------------------------------------------------------
// Function: PortReference::operator=()
//-----------------------------------------------------------------------------
PortReference& PortReference::operator=( const PortReference& other)
{
    if (this != &other)
    {
        portRef_ = other.portRef_;
        componentRef_ = other.componentRef_;
        isPresent_ = other.isPresent_;
        partSelect_.clear();
        partSelect_ = QSharedPointer<PartSelect>(new PartSelect(*other.partSelect_.data()));
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: PortRef::getPortRef()
//-----------------------------------------------------------------------------
QString PortReference::getPortRef() const
{
    return portRef_;
}

//-----------------------------------------------------------------------------
// Function: PortRef::getComponentRef()
//-----------------------------------------------------------------------------
QString PortReference::getComponentRef() const
{
    return componentRef_;
}

/*
//-----------------------------------------------------------------------------
// Function: PortRef::isValid()
//-----------------------------------------------------------------------------
bool PortRef::isValid( bool externalRef,
    const QStringList& instanceNames, 
    QStringList& errorList, 
    const QString& parentIdentifier ) const 
{
    bool valid = true;

    // if this is internal reference then the component ref must exist
    if (!externalRef) {
        if (componentRef_.isEmpty()) {
            errorList.append(QObject::tr("No component reference set for internal"
                " port reference within %1").arg(parentIdentifier));
            valid = false;
        }
        else if (!instanceNames.contains(componentRef_)) {
            errorList.append(QObject::tr("The internal port reference in %1 "
                "contained a component reference to instance %2 that does not exist").arg(
                parentIdentifier).arg(componentRef_));
            valid = false;
        }

        if (portRef_.isEmpty()) {
            errorList.append(QObject::tr("No port reference set for internal port"
                " reference within %1").arg(parentIdentifier));
            valid = false;
        }
    }
    else {
        if (portRef_.isEmpty()) {
            errorList.append(QObject::tr("No port reference set for external port"
                " reference within %1").arg(parentIdentifier));
            valid = false;
        }
    }

    return valid;
}
*/
/*
//-----------------------------------------------------------------------------
// Function: PortRef::isValid()
//-----------------------------------------------------------------------------
bool PortRef::isValid( bool externalRef, const QStringList& instanceNames ) const 
{
    // if this is internal reference then the component ref must exist
    if (!externalRef) 
    {
        if (componentRef_.isEmpty())
        {
            return false;
        }
        else if (!instanceNames.contains(componentRef_))
        {
            return false;
        }

        if (portRef_.isEmpty())
        {
            return false;
        }
    }
    else
    {
        if (portRef_.isEmpty())
        {
            return false;
        }
    }
    return true;
}
*/

//-----------------------------------------------------------------------------
// Function: PortReference::setPortRef()
//-----------------------------------------------------------------------------
void PortReference::setPortRef(QString const& newPortRef)
{
    portRef_ = newPortRef;
}

//-----------------------------------------------------------------------------
// Function: PortReference::setComponentRef()
//-----------------------------------------------------------------------------
void PortReference::setComponentRef(QString const& newComponentRef)
{
    componentRef_ = newComponentRef;
}

//-----------------------------------------------------------------------------
// Function: PortReference::getIsPresent()
//-----------------------------------------------------------------------------
QString PortReference::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: PortReference::setIsPresent()
//-----------------------------------------------------------------------------
void PortReference::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: PortReference::getPartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> PortReference::getPartSelect() const
{
    return partSelect_;
}

//-----------------------------------------------------------------------------
// Function: PortReference::setPartSelect()
//-----------------------------------------------------------------------------
void PortReference::setPartSelect(QSharedPointer<PartSelect> newPartSelect)
{
    partSelect_ = newPartSelect;
}