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
HierInterface::HierInterface(std::string busRef) :
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
    return busRef_.compare(other.busRef_) == 0;
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
    int busResult = busRef_.compare(other.busRef_);

    return busResult < 0;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getBusReference()
//-----------------------------------------------------------------------------
std::string HierInterface::getBusReference() const
{
    return busRef_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setBusReference()
//-----------------------------------------------------------------------------
void HierInterface::setBusReference(std::string const& newBusReference)
{
    busRef_ = newBusReference;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getIsPresent()
//-----------------------------------------------------------------------------
std::string HierInterface::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setIsPresent()
//-----------------------------------------------------------------------------
void HierInterface::setIsPresent(std::string const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::getDescription()
//-----------------------------------------------------------------------------
std::string HierInterface::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setDescription()
//-----------------------------------------------------------------------------
void HierInterface::setDescription(std::string const& newDescription)
{
    description_ = newDescription;
}

//-----------------------------------------------------------------------------
// Function: HierInterface::setRoute()
//-----------------------------------------------------------------------------
void HierInterface::setRoute(QList<QPointF> newRoute)
{
    auto extension = findVendorExtension("kactus2:route");
    getVendorExtensions()->removeAll(extension);

    if (!newRoute.isEmpty())
    {
        QSharedPointer<Kactus2Group> routeGroup (new Kactus2Group("kactus2:route"));

        for (QPointF const& position : newRoute)
        {
            QSharedPointer<Kactus2Placeholder> newPosition(new Kactus2Placeholder("kactus2:position"));
            newPosition->setAttribute(std::string("x"), std::to_string(int(position.x())));
            newPosition->setAttribute(std::string("y"), std::to_string(int(position.y())));

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
        getGroupedExtensionsByType("kactus2:route", "kactus2:position");

    QList<QPointF> route;

    if (!routeExtension.isEmpty())
    {
        for (QSharedPointer<VendorExtension> extension : routeExtension)
        {
            QSharedPointer<Kactus2Placeholder> position = extension.dynamicCast<Kactus2Placeholder>();
            int positionX = std::stoi(position->getAttributeValue(QStringLiteral("x")));
            int positionY = std::stoi(position->getAttributeValue(QStringLiteral("y")));

            route.append(QPointF(positionX, positionY));
        }
    }

    return route;
}
