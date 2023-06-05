//-----------------------------------------------------------------------------
// File: HierComInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.8.2015
//
// Description:
// Hierarchical COM connection.
//-----------------------------------------------------------------------------

#include "HierComInterconnection.h"

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::HierComInterconnection()
//-----------------------------------------------------------------------------
HierComInterconnection::HierComInterconnection() :
Interconnection(),
topInterfaceRef_(),
position_(),
direction_()
{

}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::HierComInterconnection()
//-----------------------------------------------------------------------------
HierComInterconnection::HierComInterconnection(QString const& name, QString const& displayName,
    QString const& description, QString const& interfaceRef, QSharedPointer<ActiveInterface> ref,
    QPointF const& position, QVector2D const& direction, QList<QPointF> const& route) :
Interconnection(name, ref, displayName, description),
topInterfaceRef_(interfaceRef),
position_(position),
direction_(direction)
{
    setRoute(route);
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::HierComInterconnection()
//-----------------------------------------------------------------------------
HierComInterconnection::HierComInterconnection(HierComInterconnection const& rhs) :
Interconnection(rhs),
topInterfaceRef_(rhs.topInterfaceRef_),
position_(rhs.position_),
direction_(rhs.direction_)
{

}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::HierComInterconnection()
//-----------------------------------------------------------------------------
HierComInterconnection::HierComInterconnection(QDomNode& connectionNode) :
Interconnection(),
topInterfaceRef_(),
position_(),
direction_()
{
    setName(connectionNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());
    setDisplayName(connectionNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
    setDescription(connectionNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());

    QDomNamedNodeMap connectionAttributes = connectionNode.attributes();
    setTopInterfaceRef(connectionAttributes.namedItem(QStringLiteral("interfaceRef")).nodeValue());

    QDomElement comInterfaceElement = connectionNode.firstChildElement(QStringLiteral("kactus2:activeComInterface"));
    QDomNamedNodeMap interfaceAttributes = comInterfaceElement.attributes();
    QString interfaceComponentRef = interfaceAttributes.namedItem(QStringLiteral("componentRef")).nodeValue();
    QString interfaceApiRef = interfaceAttributes.namedItem(QStringLiteral("comRef")).nodeValue();
    QSharedPointer<ActiveInterface> newApiInterface(new ActiveInterface(interfaceComponentRef, interfaceApiRef));
    setInterface(newApiInterface);

    QDomElement positionElement = connectionNode.firstChildElement(QStringLiteral("kactus2:position"));
    int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
    int positionY = positionElement.attribute(QStringLiteral("y")).toInt();
    setPosition(QPointF(positionX, positionY));

    QDomElement directionElement = connectionNode.firstChildElement(QStringLiteral("kactus2:direction"));
    int directionX = directionElement.attribute(QStringLiteral("x")).toInt();
    int directionY = directionElement.attribute(QStringLiteral("y")).toInt();
    setDirection(QVector2D(directionX, directionY));

    QDomElement routeElement = connectionNode.firstChildElement(QStringLiteral("kactus2:route"));
    if (!routeElement.isNull())
    {
        QList<QPointF> newRoute;
        if (routeElement.attribute(QStringLiteral("kactus2:offPage")) == QLatin1String("true"))
        {
            setOffPage(true);
        }
        else
        {
            setOffPage(false);
        }

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
// Function: HierComInterconnection::~HierComInterconnection()
//-----------------------------------------------------------------------------
HierComInterconnection::~HierComInterconnection()
{

}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::clone()
//-----------------------------------------------------------------------------
HierComInterconnection* HierComInterconnection::clone() const
{
    return new HierComInterconnection(*this);
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::type()
//-----------------------------------------------------------------------------
QString HierComInterconnection::type() const
{
    return QStringLiteral("kactus2:hierComConnection");
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::write()
//-----------------------------------------------------------------------------
void HierComInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:hierComConnection"));
    writer.writeAttribute(QStringLiteral("interfaceRef"), topInterfaceRef_);

    writer.writeTextElement(QStringLiteral("ipxact:name"), name());
    writer.writeTextElement(QStringLiteral("ipxact:displayName"), displayName());
    writer.writeTextElement(QStringLiteral("ipxact:description"), description());

    writer.writeEmptyElement(QStringLiteral("kactus2:activeComInterface"));
    writer.writeAttribute(QStringLiteral("componentRef"), getInterface()->getComponentReference());
    writer.writeAttribute(QStringLiteral("comRef"), getInterface()->getBusReference());

    writePosition(writer);
    writeVectorDirection(writer);

    if (!getRoute().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("kactus2:route"));

        if (isOffPage())
        {
            writer.writeAttribute(QStringLiteral("offPage"), QStringLiteral("true"));
        }
        else
        {
            writer.writeAttribute(QStringLiteral("offPage"), QStringLiteral("false"));
        }

        foreach (QPointF const& point, getRoute())
        {
            writer.writeEmptyElement(QStringLiteral("kactus2:position"));
            writer.writeAttribute(QStringLiteral("x"), QString::number(int(point.x())));
            writer.writeAttribute(QStringLiteral("y"), QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:hierComConnection
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::setInterface()
//-----------------------------------------------------------------------------
void HierComInterconnection::setInterface(QSharedPointer<ActiveInterface> ref)
{
    setStartInterface(ref);
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::setTopInterfaceRef()
//-----------------------------------------------------------------------------
void HierComInterconnection::setTopInterfaceRef(QString const& interfaceRef)
{
    topInterfaceRef_ = interfaceRef;
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::getTopInterfaceRef()
//-----------------------------------------------------------------------------
QString const& HierComInterconnection::getTopInterfaceRef() const
{
    return topInterfaceRef_;
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ActiveInterface> HierComInterconnection::getInterface() const
{
    return getStartInterface();
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::getPosition()
//-----------------------------------------------------------------------------
QPointF const& HierComInterconnection::getPosition() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::getDirection()
//-----------------------------------------------------------------------------
QVector2D const& HierComInterconnection::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::operator=()
//-----------------------------------------------------------------------------
HierComInterconnection& HierComInterconnection::operator=(HierComInterconnection const& other)
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
// Function: HierApiDependency::writePosition()
//-----------------------------------------------------------------------------
void HierComInterconnection::writePosition(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(QStringLiteral("kactus2:position"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(position_.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(position_.y())));
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::writeVectorDirection()
//-----------------------------------------------------------------------------
void HierComInterconnection::writeVectorDirection(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(QStringLiteral("kactus2:direction"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(direction_.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(direction_.y())));
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::setPosition()
//-----------------------------------------------------------------------------
void HierComInterconnection::setPosition(QPointF const& newPosition)
{
    position_ = newPosition;
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::setDirection()
//-----------------------------------------------------------------------------
void HierComInterconnection::setDirection(QVector2D const& newDirection)
{
    direction_ = newDirection;
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::setRoute()
//-----------------------------------------------------------------------------
void HierComInterconnection::setRoute(QList<QPointF> newRoute)
{
    getInterface()->setRoute(newRoute);
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> HierComInterconnection::getRoute() const
{
    return getInterface()->getRoute();
}