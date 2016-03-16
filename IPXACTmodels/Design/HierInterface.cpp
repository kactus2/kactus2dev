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

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <QPointF>

//-----------------------------------------------------------------------------
// Function: HierInterface::HierInterface()
//-----------------------------------------------------------------------------
HierInterface::HierInterface(QString busRef /* = QString("") */) :
Extendable(),
busRef_(busRef),
isPresent_(),
description_()
{

}

//-----------------------------------------------------------------------------
// Function: HierInterface::HierInterface()
//-----------------------------------------------------------------------------
HierInterface::HierInterface(const HierInterface& other) :
Extendable(other),
busRef_(other.busRef_),
isPresent_(other.isPresent_),
description_(other.description_)
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
bool HierInterface::operator==(const HierInterface& other)
{
    return (busRef_.compare(other.busRef_, Qt::CaseInsensitive) == 0);
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator!=()
//-----------------------------------------------------------------------------
bool HierInterface::operator!=(const HierInterface& other)
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: HierInterface::operator<()
//-----------------------------------------------------------------------------
bool HierInterface::operator<(const HierInterface& other)
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:route")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newRoute.isEmpty())
    {
        QSharedPointer<Kactus2Group> routeGroup (new Kactus2Group("kactus2:route"));

        foreach (QPointF position, newRoute)
        {
            QSharedPointer<Kactus2Placeholder> newPosition (new Kactus2Placeholder("kactus2:position"));
            newPosition->setAttribute("x", QString::number(int(position.x())));
            newPosition->setAttribute("y", QString::number(int(position.y())));

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
    QList<QSharedPointer<VendorExtension> > routeExtension = getGroupedExtensionsByType("kactus2:route", "kactus2:position");

    QList<QPointF> route;

    if (!routeExtension.isEmpty())
    {
        foreach (QSharedPointer<VendorExtension> extension, routeExtension)
        {
            QSharedPointer<Kactus2Placeholder> position = extension.dynamicCast<Kactus2Placeholder>();
            int positionX = position->getAttributeValue("x").toInt();
            int positionY = position->getAttributeValue("y").toInt();

            route.append(QPointF(positionX, positionY));
        }
    }

    return route;
}
