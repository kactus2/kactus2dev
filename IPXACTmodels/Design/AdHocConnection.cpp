//-----------------------------------------------------------------------------
// File: AdHocConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.08.2015
//
// Description:
// Describes the ipxact:adHocConnection element in an IP-XACT document
//-----------------------------------------------------------------------------

#include "AdHocConnection.h"

/*
//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection(QDomNode &adHocConnectionNode): 
    NameGroup(), 
    tiedValue_(),
    internalPortReferences_(),
    externalPortReferences_(),
    route_(), 
    offPage_(false)
{
    for (int i = 0; i < adHocConnectionNode.childNodes().size(); i++) {
        QDomNode node = adHocConnectionNode.childNodes().at(i);

        if (node.nodeName() == "spirit:name") {
            setName(node.firstChild().nodeValue());
        } else if (node.nodeName() == "spirit:displayName") {
            setDisplayName(node.firstChild().nodeValue());
        } else if (node.nodeName() == "spirit:description") {
            setDescription(node.firstChild().nodeValue());
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
}*/

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection(QString name,
    QString displayName /* = QString() */,
    QString description /* = QString() */,
    QString tiedValue /* = QString() */,
    QList<QPointF> const& route /* = QList<QPointF>() */,
    bool offPage /* = false */) :
NameGroup(name, displayName, description),
Extendable(),
isPresent_(),
tiedValue_(tiedValue),
internalPortReferences_(new QList<QSharedPointer<PortReference> > ()),
externalPortReferences_(new QList<QSharedPointer<PortReference> > ()),
route_(route),
offPage_(offPage)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection( const AdHocConnection& other ):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
tiedValue_(other.tiedValue_),
internalPortReferences_(new QList<QSharedPointer<PortReference> > ()),
externalPortReferences_(new QList<QSharedPointer<PortReference> > ()),
route_(other.route_),
offPage_(other.offPage_)
{
    foreach (QSharedPointer<PortReference> interalReference, *other.internalPortReferences_)
    {
        QSharedPointer<PortReference> copy =
            QSharedPointer<PortReference>(new PortReference(*interalReference.data()));
        internalPortReferences_->append(copy);
    }

    foreach (QSharedPointer<PortReference> externalReference, *other.externalPortReferences_)
    {
        QSharedPointer<PortReference> copy =
            QSharedPointer<PortReference>(new PortReference(*externalReference.data()));
        externalPortReferences_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::~AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::~AdHocConnection()
{
    internalPortReferences_.clear();
    externalPortReferences_.clear();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::operator=()
//-----------------------------------------------------------------------------
AdHocConnection& AdHocConnection::operator=( const AdHocConnection& other )
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        isPresent_ = other.isPresent_;
        tiedValue_ = other.tiedValue_;

        internalPortReferences_->clear();
        foreach (QSharedPointer<PortReference> interalReference, *other.internalPortReferences_)
        {
            QSharedPointer<PortReference> copy =
                QSharedPointer<PortReference>(new PortReference(*interalReference.data()));
            internalPortReferences_->append(copy);
        }

        externalPortReferences_->clear();
        foreach (QSharedPointer<PortReference> externalReference, *other.externalPortReferences_)
        {
            QSharedPointer<PortReference> copy =
                QSharedPointer<PortReference>(new PortReference(*externalReference.data()));
            externalPortReferences_->append(copy);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getInternalPortReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortReference> > > AdHocConnection::getInternalPortReferences() const
{
    return internalPortReferences_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setInternalPortReferences()
//-----------------------------------------------------------------------------
void AdHocConnection::setInternalPortReferences(QSharedPointer<QList<QSharedPointer<PortReference> > > newInternals)
{
    internalPortReferences_ = newInternals;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getExternalPortReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortReference> > > AdHocConnection::getExternalPortReferences() const
{
    return externalPortReferences_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setExternalPortReferences()
//-----------------------------------------------------------------------------
void AdHocConnection::setExternalPortReferences(QSharedPointer<QList<QSharedPointer<PortReference> > > newExternals)
{
    externalPortReferences_ = newExternals;
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

/*
//-----------------------------------------------------------------------------
// Function: AdHocConnection::isValid()
//-----------------------------------------------------------------------------
bool AdHocConnection::isValid(const QStringList& instanceNames, 
    QStringList& errorList, 
    const QString& parentIdentifier) const 
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("ad hoc connection %1").arg(name()));

    if (name().isEmpty())
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
}*/
/*
//-----------------------------------------------------------------------------
// Function: AdHocConnection::isValid()
//-----------------------------------------------------------------------------
bool AdHocConnection::isValid( const QStringList& instanceNames ) const
{
    if (name().isEmpty())
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
}*/

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getIsPresent()
//-----------------------------------------------------------------------------
QString AdHocConnection::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setIsPresent()
//-----------------------------------------------------------------------------
void AdHocConnection::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getTiedValue()
//-----------------------------------------------------------------------------
QString AdHocConnection::getTiedValue() const
{
    return tiedValue_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setTiedValue()
//-----------------------------------------------------------------------------
void AdHocConnection::setTiedValue(QString const& newTiedValue)
{
    tiedValue_ = newTiedValue;
}