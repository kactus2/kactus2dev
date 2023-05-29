//-----------------------------------------------------------------------------
// File: HierApiInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Hierarchical API connection.
//-----------------------------------------------------------------------------

#include "HierApiInterconnection.h"

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::HierApiInterconnection ()
//-----------------------------------------------------------------------------
HierApiInterconnection::HierApiInterconnection() :
Interconnection()
{

}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::HierApiInterconnection()
//-----------------------------------------------------------------------------
HierApiInterconnection::HierApiInterconnection(std::string const& name, std::string const& displayName,
    std::string const& description, std::string const& interfaceRef, QSharedPointer<ActiveInterface> ref,
    QPointF const& position, QVector2D const& direction, QList<QPointF> const& route) :
Interconnection(name, ref, displayName, description),
topInterfaceRef_(interfaceRef),
position_(position),
direction_(direction)
{
    setRoute(route);
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::HierApiInterconnection()
//-----------------------------------------------------------------------------
HierApiInterconnection::HierApiInterconnection(HierApiInterconnection const& rhs) :
Interconnection(rhs),
topInterfaceRef_(rhs.topInterfaceRef_),
position_(rhs.position_),
direction_(rhs.position_)
{

}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::HierApiInterconnection()
//-----------------------------------------------------------------------------
HierApiInterconnection::HierApiInterconnection(QDomNode& connectionNode) :
Interconnection()
{
    setName(connectionNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue().toStdString());
    setDisplayName(connectionNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
    setDescription(connectionNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());

    QDomNamedNodeMap connectionAttributes = connectionNode.attributes();
    setTopInterfaceRef(connectionAttributes.namedItem(QStringLiteral("interfaceRef")).nodeValue().toStdString());

    QDomElement apiInterfaceElement = connectionNode.firstChildElement(QStringLiteral("kactus2:activeApiInterface"));
    QDomNamedNodeMap interfaceAttributes = apiInterfaceElement.attributes();
    auto interfaceComponentRef = interfaceAttributes.namedItem(QStringLiteral("componentRef")).nodeValue().toStdString();
    auto interfaceApiRef = interfaceAttributes.namedItem(QStringLiteral("apiRef")).nodeValue().toStdString();
    QSharedPointer<ActiveInterface> newApiInterface(new ActiveInterface(interfaceComponentRef, interfaceApiRef));
    setInterface(newApiInterface);

    QDomElement positionElement = connectionNode.firstChildElement(QStringLiteral("kactus2:position"));
    QDomNamedNodeMap positionAttributers = positionElement.attributes();
    int positionX = positionAttributers.namedItem(QStringLiteral("x")).nodeValue().toInt();
    int positionY = positionAttributers.namedItem(QStringLiteral("y")).nodeValue().toInt();
    setPosition(QPointF(positionX, positionY));

    QDomElement directionElement = connectionNode.firstChildElement(QStringLiteral("kactus2:direction"));
    QDomNamedNodeMap directionAttributes = directionElement.attributes();
    int directionX = directionAttributes.namedItem(QStringLiteral("x")).nodeValue().toInt();
    int directionY = directionAttributes.namedItem(QStringLiteral("y")).nodeValue().toInt();
    setDirection(QVector2D(directionX, directionY));

    QDomElement routeElement = connectionNode.firstChildElement(QStringLiteral("kactus2:route"));
    if (!routeElement.isNull())
    {
        setOffPage(routeElement.attribute(QStringLiteral("kactus2:offPage")) == QLatin1String("true"));

        QList<QPointF> newRoute;

        QDomNodeList positionList = routeElement.childNodes();
        for (int i = 0; i < positionList.count(); ++i)
        {
            QDomNode routePositionNode = positionList.at(i);
            QDomNamedNodeMap routePositionAttributes = routePositionNode.attributes();
            int routePositionX = routePositionAttributes.namedItem(QStringLiteral("x")).nodeValue().toInt();
            int routePositionY = routePositionAttributes.namedItem(QStringLiteral("y")).nodeValue().toInt();
            newRoute.append(QPointF(routePositionX, routePositionY));
        }

        setRoute(newRoute);
    }
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::HierApiInterconnection()
//-----------------------------------------------------------------------------
HierApiInterconnection::~HierApiInterconnection()
{

}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::clone()
//-----------------------------------------------------------------------------
HierApiInterconnection* HierApiInterconnection::clone() const
{
    return new HierApiInterconnection(*this);
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::type()
//-----------------------------------------------------------------------------
std::string HierApiInterconnection::type() const
{
    return "kactus2:hierApiDependency";
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::write()
//-----------------------------------------------------------------------------
void HierApiInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:hierApiDependency"));
    writer.writeAttribute(QStringLiteral("interfaceRef"), QString::fromStdString(getTopInterfaceRef()));

    writer.writeTextElement(QStringLiteral("ipxact:name"), name());
    writer.writeTextElement(QStringLiteral("ipxact:displayName"), displayName());
    writer.writeTextElement(QStringLiteral("ipxact:description"), description());

    writer.writeEmptyElement(QStringLiteral("kactus2:activeApiInterface"));
    writer.writeAttribute(QStringLiteral("componentRef"), 
        QString::fromStdString(getInterface()->getComponentReference()));
    writer.writeAttribute(QStringLiteral("apiRef"), QString::fromStdString(getInterface()->getBusReference()));
    
    writePosition(writer);
    writeVectorDirection(writer);

    if (!getRoute().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("kactus2:route"));

        if (isOffPage())
        {
            writer.writeAttribute(QStringLiteral("kactus2:offPage"), QStringLiteral("true"));
        }
        else
        {
            writer.writeAttribute(QStringLiteral("kactus2:offPage"), QStringLiteral("false"));
        }

        for (QPointF const& point : getRoute())
        {
            writer.writeEmptyElement(QStringLiteral("kactus2:position"));
            writer.writeAttribute(QStringLiteral("x"), QString::number(int(point.x())));
            writer.writeAttribute(QStringLiteral("y"), QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:hierApiDependency
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::setTopInterfaceRef()
//-----------------------------------------------------------------------------
void HierApiInterconnection::setTopInterfaceRef(std::string const& interfaceRef)
{
    topInterfaceRef_ = interfaceRef;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::setInterface()
//-----------------------------------------------------------------------------
void HierApiInterconnection::setInterface(QSharedPointer<ActiveInterface> ref)
{
    setStartInterface(ref);
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::getTopInterfaceRef()
//-----------------------------------------------------------------------------
std::string const& HierApiInterconnection::getTopInterfaceRef() const
{
    return topInterfaceRef_;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> HierApiInterconnection::getInterface() const
{
    return getStartInterface();
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::getPosition()
//-----------------------------------------------------------------------------
QPointF const& HierApiInterconnection::getPosition() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::getDirection()
//-----------------------------------------------------------------------------
QVector2D const& HierApiInterconnection::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::operator=()
//-----------------------------------------------------------------------------
HierApiInterconnection& HierApiInterconnection::operator=(HierApiInterconnection const& other)
{
    if (&other != this)
    {
        Interconnection::operator=(other);
        topInterfaceRef_ = other.topInterfaceRef_;
        position_ = other.position_;
        direction_ = other.direction_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::writePosition()
//-----------------------------------------------------------------------------
void HierApiInterconnection::writePosition(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(QStringLiteral("kactus2:position"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(position_.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(position_.y())));
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::writeVectorDirection()
//-----------------------------------------------------------------------------
void HierApiInterconnection::writeVectorDirection(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(QStringLiteral("kactus2:direction"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(direction_.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(direction_.y())));
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::setPosition()
//-----------------------------------------------------------------------------
void HierApiInterconnection::setPosition(QPointF const& newPosition)
{
    position_ = newPosition;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::setDirection()
//-----------------------------------------------------------------------------
void HierApiInterconnection::setDirection(QVector2D const& newDirection)
{
    direction_ = newDirection;
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::setRoute()
//-----------------------------------------------------------------------------
void HierApiInterconnection::setRoute(QList<QPointF> newRoute)
{
    getInterface()->setRoute(newRoute);
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> HierApiInterconnection::getRoute() const
{
    return getInterface()->getRoute();
}