//-----------------------------------------------------------------------------
// File: AdHocConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes the spirit:adHocConnection element in an IP-XACT document
//-----------------------------------------------------------------------------

#include "AdHocConnection.h"

#include "generaldeclarations.h"
#include "PortRef.h"

#include <QList>

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection(QDomNode &adHocConnectionNode): 
    nameGroup_(), 
    tiedValue_(),
    internalPortReferences_(),
    externalPortReferences_(),
    route_(), 
    offPage_(false)
{
    for (int i = 0; i < adHocConnectionNode.childNodes().size(); i++) {
        QDomNode node = adHocConnectionNode.childNodes().at(i);

        if (node.nodeName() == "spirit:name") {
            nameGroup_.name_ = node.firstChild().nodeValue();
        } else if (node.nodeName() == "spirit:displayName") {
            nameGroup_.displayName_ = node.firstChild().nodeValue();
        } else if (node.nodeName() == "spirit:description") {
            nameGroup_.description_ = node.firstChild().nodeValue();
        } else if (node.nodeName() == "spirit:internalPortReference") {
            internalPortReferences_.append(PortRef(node));
        } else if (node.nodeName() == "spirit:externalPortReference") {
            externalPortReferences_.append(PortRef(node));
        }
    }

    QDomNamedNodeMap attributes = adHocConnectionNode.attributes();

    if (attributes.contains("spirit:tiedValue")) {
        tiedValue_ = attributes.namedItem("spirit:tiedValue").nodeValue();
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection(QString name,
    QString displayName,
    QString description,
    QString tiedValue,
    QList<PortRef> internalPortReference,
    QList<PortRef> externalPortReference,
    QList<QPointF> const& route, bool offPage):
    nameGroup_(name),
    tiedValue_(tiedValue), 
    internalPortReferences_(internalPortReference),
    externalPortReferences_(externalPortReference),
    route_(route), offPage_(offPage)
{
    nameGroup_.displayName_ = displayName; 
    nameGroup_.description_ = description;

}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection( const AdHocConnection& other ):
nameGroup_(other.nameGroup_),
    tiedValue_(other.tiedValue_),
    internalPortReferences_(other.internalPortReferences_),
    externalPortReferences_(other.externalPortReferences_),
    route_(other.route_), offPage_(other.offPage_) 
{
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::~AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::~AdHocConnection()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::operator=()
//-----------------------------------------------------------------------------
AdHocConnection& AdHocConnection::operator=( const AdHocConnection& other ) {
    if (this != &other) {
        nameGroup_ = other.nameGroup_;
        tiedValue_ = other.tiedValue_;
        internalPortReferences_ = other.internalPortReferences_;
        externalPortReferences_ = other.externalPortReferences_;
        route_ = other.route_;
        offPage_ = other.offPage_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::name()
//-----------------------------------------------------------------------------
QString AdHocConnection::name() const
{
    return nameGroup_.name_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::displayName()
//-----------------------------------------------------------------------------
QString AdHocConnection::displayName() const
{
    return nameGroup_.displayName_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::description()
//-----------------------------------------------------------------------------
QString AdHocConnection::description() const
{
    return nameGroup_.description_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::internalPortReferences()
//-----------------------------------------------------------------------------
QList<PortRef> AdHocConnection::internalPortReferences() const
{
    return internalPortReferences_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::externalPortReferences()
//-----------------------------------------------------------------------------
QList<PortRef> AdHocConnection::externalPortReferences() const
{
    return externalPortReferences_;
}
//-----------------------------------------------------------------------------
// Function: AdHocConnection::setRoute()
//-----------------------------------------------------------------------------
void AdHocConnection::setRoute(QList<QPointF> const& route)
{
    route_ = route;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> AdHocConnection::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::isOffPage()
//-----------------------------------------------------------------------------
bool AdHocConnection::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setOffPage()
//-----------------------------------------------------------------------------
void AdHocConnection::setOffPage(bool offPage)
{
    offPage_ = offPage;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::isValid()
//-----------------------------------------------------------------------------
bool AdHocConnection::isValid(const QStringList& instanceNames, 
    QStringList& errorList, 
    const QString& parentIdentifier) const 
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("ad hoc connection %1").arg(nameGroup_.name_));

    if (nameGroup_.name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for ad hoc connection"
            " within %1").arg(parentIdentifier));
        valid = false;
    }

    if (internalPortReferences_.isEmpty())
    {
        errorList.append(QObject::tr("At least one internal port reference must be"
            " listed in %1 within %2").arg(thisIdentifier).arg(parentIdentifier));
        valid = false;
    }
    else
    {
        foreach (PortRef portRef, internalPortReferences_) 
        {
            if (!portRef.isValid(false, instanceNames, errorList, thisIdentifier)) 
            {
                valid = false;
            }
        }
    }

    foreach (PortRef portRef, externalPortReferences_)
    {
        if (!portRef.isValid(true, instanceNames, errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::isValid()
//-----------------------------------------------------------------------------
bool AdHocConnection::isValid( const QStringList& instanceNames ) const
{
    if (nameGroup_.name_.isEmpty())
    {
        return false;
    }

    if (internalPortReferences_.isEmpty())
    {
        return false;
    }
    else 
    {
        foreach (PortRef portRef, internalPortReferences_) 
        {
            if (!portRef.isValid(false, instanceNames)) 
            {
                return false;
            }
        }
    }

    foreach (PortRef portRef, externalPortReferences_) 
    {
        if (!portRef.isValid(true, instanceNames)) 
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::write()
//-----------------------------------------------------------------------------
void AdHocConnection::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("spirit:adHocConnection");

    nameGroup_.write(writer);

    foreach (PortRef internalRef, internalPortReferences_)
    {
       internalRef.write(writer);
    }

    foreach (PortRef externalRef, externalPortReferences_)
    {
        externalRef.write(writer);
    }

    writer.writeEndElement();
}
