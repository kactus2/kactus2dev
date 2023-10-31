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

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection(QString name, 
    QString tiedValue, QList<QPointF> const& route,  bool offPage) :
NameGroup(name),
    Extendable(),
    tiedValue_(tiedValue)
{
    setOffPage(offPage);
    setRoute(route);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection( const AdHocConnection& other ):
NameGroup(other),
    Extendable(other),
    isPresent_(other.isPresent_),
    tiedValue_(other.tiedValue_)
{
    Copy::copyList(other.internalPortReferences_, internalPortReferences_);

    Copy::copyList(other.externalPortReferences_, externalPortReferences_);
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
        Copy::copyList(other.internalPortReferences_, internalPortReferences_);

        externalPortReferences_->clear();
        Copy::copyList(other.externalPortReferences_, externalPortReferences_);
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
void AdHocConnection::setRoute(QList<QPointF> const& route) const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:route"));
    getVendorExtensions()->removeAll(extension);

    if (!route.isEmpty())
    {
        QSharedPointer<Kactus2Group> routeGroup (new Kactus2Group(QStringLiteral("kactus2:route")));
        for (auto const& location : route)
        {
            QSharedPointer<Kactus2Position> routePoint (new Kactus2Position(location));
            routeGroup->addToGroup(routePoint);
        }

        getVendorExtensions()->append(routeGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> AdHocConnection::getRoute() const
{
    QList<QSharedPointer<VendorExtension> > routeExtensions =
        getGroupedExtensionsByType(QStringLiteral("kactus2:route"), QStringLiteral("kactus2:position"));

    QList<QPointF> route;
    for (auto const& extension : routeExtensions)
    {
        QSharedPointer<Kactus2Position> position = extension.dynamicCast<Kactus2Position>();
        route.append(position->position());
    }

    return route;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::isOffPage()
//-----------------------------------------------------------------------------
bool AdHocConnection::isOffPage() const
{
    return findVendorExtension(QStringLiteral("kactus2:offPage")) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setOffPage()
//-----------------------------------------------------------------------------
void AdHocConnection::setOffPage(bool offPage)
{
    setPlaceholderExtension(offPage, QStringLiteral("kactus2:offPage"));
}

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

//-----------------------------------------------------------------------------
// Function: AdHocConnection::changeInterfaceComponentReferences()
//-----------------------------------------------------------------------------
void AdHocConnection::changeInterfaceComponentReferences(QString const& oldName, QString const& newName) const
{
    for (QSharedPointer<PortReference> portReference : *internalPortReferences_)
    {
        if (portReference->getComponentRef().compare(oldName) == 0)
        {
            portReference->setComponentRef(newName);
        }
    }
}
