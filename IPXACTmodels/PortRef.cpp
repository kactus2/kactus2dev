//-----------------------------------------------------------------------------
// File: PortRef.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes a port reference in an ad-hoc connection.
//-----------------------------------------------------------------------------

#include "PortRef.h"

#include <QString>
#include <QStringList>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: PortRef::PortRef()
//-----------------------------------------------------------------------------
PortRef::PortRef(QDomNode& portReferenceNode)
    : portRef_(), componentRef_(), left_(-1), right_(-1)
{
    QDomNamedNodeMap attributes = portReferenceNode.attributes();

    portRef_ = attributes.namedItem("spirit:portRef").nodeValue();

    if (portReferenceNode.nodeName() == "spirit:internalPortReference")
    {
        componentRef_ = attributes.namedItem("spirit:componentRef").nodeValue();
    }

    if (attributes.contains("spirit:left"))
    {
        left_ = attributes.namedItem("spirit:left").nodeValue().toInt();
    }

    if (attributes.contains("spirit:right"))
    {
        right_ = attributes.namedItem("spirit:right").nodeValue().toInt();
    }
}

//-----------------------------------------------------------------------------
// Function: PortRef::PortRef()
//-----------------------------------------------------------------------------
PortRef::PortRef(QString portRef, QString componentRef, int left, int right): 
portRef_(portRef),
    componentRef_(componentRef),
    left_(left),
    right_(right)
{

}

//-----------------------------------------------------------------------------
// Function: PortRef::PortRef()
//-----------------------------------------------------------------------------
PortRef::PortRef( const PortRef& other ):
portRef_(other.portRef_),
    componentRef_(other.componentRef_),
    left_(other.left_),
    right_(other.right_) 
{

}

//-----------------------------------------------------------------------------
// Function: PortRef::~PortRef()
//-----------------------------------------------------------------------------
PortRef::~PortRef()
{

}

//-----------------------------------------------------------------------------
// Function: PortRef::operator=()
//-----------------------------------------------------------------------------
PortRef& PortRef::operator=( const PortRef& other ) 
{
    if (this != &other) {
        portRef_	= other.portRef_;
        componentRef_ = other.componentRef_;
        left_ = other.left_;
        right_ = other.right_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: PortRef::getPortRef()
//-----------------------------------------------------------------------------
QString PortRef::getPortRef() const
{
    return portRef_;
}

//-----------------------------------------------------------------------------
// Function: PortRef::getComponentRef()
//-----------------------------------------------------------------------------
QString PortRef::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: PortRef::getLeft()
//-----------------------------------------------------------------------------
int PortRef::getLeft() const
{
    return left_;
}

//-----------------------------------------------------------------------------
// Function: PortRef::getLeft()
//-----------------------------------------------------------------------------
int PortRef::getRight() const
{
    return right_;
}

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

//-----------------------------------------------------------------------------
// Function: PortRef::write()
//-----------------------------------------------------------------------------
void PortRef::write(QXmlStreamWriter& writer) const
{
    if (componentRef_.isEmpty())
    {
        writer.writeEmptyElement("spirit:externalPortReference");
    }
    else
    {
        writer.writeEmptyElement("spirit:internalPortReference");
        writer.writeAttribute("spirit:componentRef", componentRef_);
    }

    writer.writeAttribute("spirit:portRef", portRef_);

    if (left_ >= 0)
    {
        writer.writeAttribute("spirit:left", QString::number(left_));
    }

    if (right_ >= 0)
    {
        writer.writeAttribute("spirit:right", QString::number(right_));
    }
}
