//-----------------------------------------------------------------------------
// File: HierInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.08.2015
//
// Description:
// Describes the ipxact:hierInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "HierInterface.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <QPointF>

//-----------------------------------------------------------------------------
// Function: HierInterface::HierInterface()
//-----------------------------------------------------------------------------
HierInterface::HierInterface(QString const& busRef) :
Extendable(),
busRef_(busRef)
{

}

//-----------------------------------------------------------------------------
// Function: HierInterface::~HierInterface()
//-----------------------------------------------------------------------------
HierInterface::~HierInterface()
{

}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator=()
//-----------------------------------------------------------------------------
HierInterface& HierInterface::operator=(const HierInterface& other)
{
    if (this != &other)
    {
        Extendable::operator =(other);
        busRef_ = other.busRef_;
        isPresent_ = other.isPresent_;
        description_ = other.description_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator==()
//-----------------------------------------------------------------------------
bool HierInterface::operator==(const HierInterface& other) const
{
    return (busRef_.compare(other.busRef_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator!=()
//-----------------------------------------------------------------------------
bool HierInterface::operator!=(const HierInterface& other) const
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator<()
//-----------------------------------------------------------------------------
bool HierInterface::operator<(const HierInterface& other) const
{
    int busResult = busRef_.compare(other.busRef_, Qt::CaseInsensitive);

    return busResult < 0;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getBusReference()
//-----------------------------------------------------------------------------
QString HierInterface::getBusReference() const
{
    return busRef_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setBusReference()
//-----------------------------------------------------------------------------
void HierInterface::setBusReference(QString const& newBusReference)
{
    busRef_ = newBusReference;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getIsPresent()
//-----------------------------------------------------------------------------
QString HierInterface::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setIsPresent()
//-----------------------------------------------------------------------------
void HierInterface::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getDescription()
//-----------------------------------------------------------------------------
QString HierInterface::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setDescription()
//-----------------------------------------------------------------------------
void HierInterface::setDescription(QString const& newDescription)
{
    description_ = newDescription;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setRoute()
//-----------------------------------------------------------------------------
void HierInterface::setRoute(QList<QPointF> newRoute)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:route"));
    getVendorExtensions()->removeAll(extension);

    if (!newRoute.isEmpty())
    {
        QSharedPointer<Kactus2Group> routeGroup (new Kactus2Group(QStringLiteral("kactus2:route")));

        for (QPointF const& position : newRoute)
        {
            QSharedPointer<Kactus2Position> newPosition(new Kactus2Position(position));
            routeGroup->addToGroup(newPosition);
        }

        getVendorExtensions()->append(routeGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> HierInterface::getRoute() const
{
    QList<QSharedPointer<VendorExtension> > routeExtension = 
        getGroupedExtensionsByType(QStringLiteral("kactus2:route"), QStringLiteral("kactus2:position"));

    QList<QPointF> route;

    if (!routeExtension.isEmpty())
    {
        for (QSharedPointer<VendorExtension> extension : routeExtension)
        {
            QSharedPointer<Kactus2Position> position = extension.dynamicCast<Kactus2Position>();
            route.append(position->position());
        }
    }

    return route;
}
