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

//-----------------------------------------------------------------------------
// Function: AdHocConnection::AdHocConnection()
//-----------------------------------------------------------------------------
AdHocConnection::AdHocConnection(std::string name,
    std::string tiedValue, QList<QPointF> const& route,  bool offPage) :
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
    for (auto const& interalReference : *other.internalPortReferences_)
    {
        QSharedPointer<PortReference> copy =
            QSharedPointer<PortReference>(new PortReference(*interalReference));
        internalPortReferences_->append(copy);
    }

    for (auto const& externalReference  :*other.externalPortReferences_)
    {
        QSharedPointer<PortReference> copy =
            QSharedPointer<PortReference>(new PortReference(*externalReference));
        externalPortReferences_->append(copy);
    }
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
        for (auto const& interalReference : *other.internalPortReferences_)
        {
            QSharedPointer<PortReference> copy =
                QSharedPointer<PortReference>(new PortReference(*interalReference));
            internalPortReferences_->append(copy);
        }

        externalPortReferences_->clear();
        for (auto const& externalReference : *other.externalPortReferences_)
        {
            QSharedPointer<PortReference> copy =
                QSharedPointer<PortReference>(new PortReference(*externalReference));
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
    auto extension = findVendorExtension("kactus2:route");
    getVendorExtensions()->removeAll(extension);


    if (!route.isEmpty())
    {
        QSharedPointer<Kactus2Group> routeGroup (new Kactus2Group("kactus2:route"));
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
        getGroupedExtensionsByType("kactus2:route", "kactus2:position");

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
    return findVendorExtension("kactus2:offPage") != nullptr;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setOffPage()
//-----------------------------------------------------------------------------
void AdHocConnection::setOffPage(bool offPage)
{
    setPlaceholderExtension(offPage, "kactus2:offPage");
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getIsPresent()
//-----------------------------------------------------------------------------
std::string AdHocConnection::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setIsPresent()
//-----------------------------------------------------------------------------
void AdHocConnection::setIsPresent(std::string const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::getTiedValue()
//-----------------------------------------------------------------------------
std::string AdHocConnection::getTiedValue() const
{
    return tiedValue_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setTiedValue()
//-----------------------------------------------------------------------------
void AdHocConnection::setTiedValue(std::string const& newTiedValue)
{
    tiedValue_ = newTiedValue;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::changeInterfaceComponentReferences()
//-----------------------------------------------------------------------------
void AdHocConnection::changeInterfaceComponentReferences(std::string const& oldName, std::string const& newName)
{
    for (QSharedPointer<PortReference> portReference : *internalPortReferences_)
    {
        if (portReference->getComponentRef().compare(oldName) == 0)
        {
            portReference->setComponentRef(newName);
        }
    }
}
