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
AdHocConnection::AdHocConnection(QString name, QString displayName, QString description,
    QString tiedValue, QList<QPointF> const& route,  bool offPage) :
NameGroup(name, displayName, description),
    Extendable(),
    isPresent_(),
    tiedValue_(tiedValue),
    internalPortReferences_(new QList<QSharedPointer<PortReference> > ()),
    externalPortReferences_(new QList<QSharedPointer<PortReference> > ())
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
    tiedValue_(other.tiedValue_),
    internalPortReferences_(new QList<QSharedPointer<PortReference> > ()),
    externalPortReferences_(new QList<QSharedPointer<PortReference> > ())
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:route")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!route.isEmpty())
    {
        QSharedPointer<Kactus2Group> routeGroup (new Kactus2Group("kactus2:route"));
        foreach (QPointF location, route)
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

    foreach (QSharedPointer<VendorExtension> extension, routeExtensions)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:offPage")
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnection::setOffPage()
//-----------------------------------------------------------------------------
void AdHocConnection::setOffPage(bool offPage)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:offPage")
        {
            if (!offPage)
            {
                getVendorExtensions()->removeAll(extension);
            }
            return;
        }
    }

    if (offPage)
    {
        QSharedPointer<Kactus2Placeholder> offPageExtensions (new Kactus2Placeholder("kactus2:offPage"));
        getVendorExtensions()->append(offPageExtensions);
    }
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