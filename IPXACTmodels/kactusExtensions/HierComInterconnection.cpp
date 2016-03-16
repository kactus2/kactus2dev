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
    setName(connectionNode.firstChildElement("ipxact:name").firstChild().nodeValue());
    setDisplayName(connectionNode.firstChildElement("ipxact:displayName").firstChild().nodeValue());
    setDescription(connectionNode.firstChildElement("ipxact:description").firstChild().nodeValue());

    QDomNamedNodeMap connectionAttributes = connectionNode.attributes();
    setTopInterfaceRef(connectionAttributes.namedItem("interfaceRef").nodeValue());

    QDomElement comInterfaceElement = connectionNode.firstChildElement("kactus2:activeComInterface");
    QDomNamedNodeMap interfaceAttributes = comInterfaceElement.attributes();
    QString interfaceComponentRef = interfaceAttributes.namedItem("componentRef").nodeValue();
    QString interfaceApiRef = interfaceAttributes.namedItem("comRef").nodeValue();
    QSharedPointer<ActiveInterface> newApiInterface(new ActiveInterface(interfaceComponentRef, interfaceApiRef));
    setInterface(newApiInterface);

    QDomElement positionElement = connectionNode.firstChildElement("kactus2:position");
    int positionX = positionElement.attribute("x").toInt();
    int positionY = positionElement.attribute("y").toInt();
    setPosition(QPointF(positionX, positionY));

    QDomElement directionElement = connectionNode.firstChildElement("kactus2:direction");
    int directionX = directionElement.attribute("x").toInt();
    int directionY = directionElement.attribute("y").toInt();
    setDirection(QVector2D(directionX, directionY));

    QDomElement routeElement = connectionNode.firstChildElement("kactus2:route");
    if (!routeElement.isNull())
    {
        QList<QPointF> newRoute;
        if (routeElement.attribute("kactus2:offPage") == "true")
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
            int routePositionX = routePositionAttributes.namedItem("x").nodeValue().toInt();
            int routePositionY = routePositionAttributes.namedItem("y").nodeValue().toInt();
            newRoute.append(QPointF(routePositionX, routePositionY));
        }

        setRoute(newRoute);
    }
}

/*
//-----------------------------------------------------------------------------
// Function: HierComConnection::HierComConnection()
//-----------------------------------------------------------------------------
HierComConnection::HierComConnection(QDomNode& node) :
NameGroup(),
//name_(), displayName_(), desc_(),
interfaceRef_(),
interface_(new ActiveInterface()),
position_(),
direction_(1.0, 0.0),
route_(),
offPage_(false)
{
    interfaceRef_ = node.attributes().namedItem("kactus2:interfaceRef").nodeValue();

    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "ipxact:name")
        {
            setName(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "ipxact:displayName")
        {
            setDisplayName(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "ipxact:description")
        {
            setDescription(childNode.childNodes().at(0).nodeValue());
        }
        else if (childNode.nodeName() == "kactus2:activeComInterface")
        {
            interface_->setComponentReference(childNode.attributes().namedItem("kactus2:componentRef").nodeValue());
            interface_->setBusReference(childNode.attributes().namedItem("kactus2:comRef").nodeValue());
        }
        else if (childNode.nodeName() == "kactus2:position")
        {
            QDomNamedNodeMap attributeMap = childNode.attributes();
            position_.setX(attributeMap.namedItem("x").nodeValue().toInt());
            position_.setY(attributeMap.namedItem("y").nodeValue().toInt());
        }
        else if (childNode.nodeName() == "kactus2:direction")
        {
            QDomNamedNodeMap attributeMap = childNode.attributes();
            direction_.setX(attributeMap.namedItem("x").nodeValue().toInt());
            direction_.setY(attributeMap.namedItem("y").nodeValue().toInt());
        }
        else if (childNode.nodeName() == "kactus2:route")
        {
            offPage_ = childNode.attributes().namedItem("kactus2:offPage").nodeValue() == "true";

            for (int i = 0; i < childNode.childNodes().size(); ++i)
            {
                QDomNode posNode = childNode.childNodes().at(i);
                QPointF pos;

                if (posNode.nodeName() == "kactus2:position")
                {
                    pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
                    pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
                    route_.append(pos);
                }
            }
        }
    }
}*/

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
    return QString("kactus2:hierComConnection");
}

//-----------------------------------------------------------------------------
// Function: HierComInterconnection::write()
//-----------------------------------------------------------------------------
void HierComInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:hierComConnection");
    writer.writeAttribute("interfaceRef", topInterfaceRef_);

    writer.writeTextElement("ipxact:name", name());
    writer.writeTextElement("ipxact:displayName", displayName());
    writer.writeTextElement("ipxact:description", description());

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("componentRef", getInterface()->getComponentReference());
    writer.writeAttribute("comRef", getInterface()->getBusReference());

    writePosition(writer);
    writeVectorDirection(writer);

    if (!getRoute().isEmpty())
    {
        writer.writeStartElement("kactus2:route");

        if (isOffPage())
        {
            writer.writeAttribute("offPage", "true");
        }
        else
        {
            writer.writeAttribute("offPage", "false");
        }

        foreach (QPointF const& point, getRoute())
        {
            writer.writeEmptyElement("kactus2:position");
            writer.writeAttribute("x", QString::number(int(point.x())));
            writer.writeAttribute("y", QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:hierComConnection
}

/*
bool HierComConnection::isValid( QStringList& errorList, QStringList const& instanceNames, QString const& parentId ) const {
	bool valid = true;
	QString const thisId(QObject::tr("Hierarchical COM connection in %1").arg(parentId));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for Hierarchical COM connection in %1").arg(parentId));
		valid = false;
	}

	// Validate the interface references.
	if (interface_->getComponentReference().isEmpty()) {
		errorList.append(QObject::tr("No component reference set for hierarchical COM interface in %1").arg(thisId));
		valid = false;
	}
	else if (!instanceNames.contains(interface_->getComponentReference())) {
		errorList.append(QObject::tr("Hierarchical COM interface in %1 contains a reference "
			"to component instance '%2' that does not exist.").arg(
			thisId).arg(interface_->getComponentReference()));
		valid = false;
	}

	if (interface_->getBusReference().isEmpty()) {
		errorList.append(QObject::tr("No COM reference set for hierarchical COM interface in %1").arg(thisId));
		valid = false;
	}

	if (interfaceRef_.isEmpty()) {
		errorList.append(QObject::tr("No COM reference set for top COM connection in %1.").arg(thisId));
		valid = false;
	}

	return valid;
}
*//*
bool HierComConnection::isValid( const QStringList& instanceNames ) const {
	if (name().isEmpty()) {
		return false;
	}

	// Validate the interface references.
	if (interface_->getComponentReference().isEmpty()) {
		return false;
	}
	else if (!instanceNames.contains(interface_->getComponentReference())) {
		return false;
	}

	if (interface_->getBusReference().isEmpty()) {
		return false;
	}

	if (interfaceRef_.isEmpty()) {
		return false;
	}
	return true;
}*/

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
    writer.writeEmptyElement("kactus2:position");
    writer.writeAttribute("x", QString::number(int(position_.x())));
    writer.writeAttribute("y", QString::number(int(position_.y())));
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::writeVectorDirection()
//-----------------------------------------------------------------------------
void HierComInterconnection::writeVectorDirection(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement("kactus2:direction");
    writer.writeAttribute("x", QString::number(int(direction_.x())));
    writer.writeAttribute("y", QString::number(int(direction_.y())));
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