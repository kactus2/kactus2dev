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
Interconnection(),
topInterfaceRef_(),
position_(),
direction_()
{

}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::HierApiInterconnection()
//-----------------------------------------------------------------------------
HierApiInterconnection::HierApiInterconnection(QString const& name, QString const& displayName,
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

    QDomElement apiInterfaceElement = connectionNode.firstChildElement("kactus2:activeApiInterface");
    QDomNamedNodeMap interfaceAttributes = apiInterfaceElement.attributes();
    QString interfaceComponentRef = interfaceAttributes.namedItem("componentRef").nodeValue();
    QString interfaceApiRef = interfaceAttributes.namedItem("apiRef").nodeValue();
    QSharedPointer<ActiveInterface> newApiInterface(new ActiveInterface(interfaceComponentRef, interfaceApiRef));
    setInterface(newApiInterface);

    QDomElement positionElement = connectionNode.firstChildElement("kactus2:position");
    QDomNamedNodeMap positionAttributers = positionElement.attributes();
    int positionX = positionAttributers.namedItem("x").nodeValue().toInt();
    int positionY = positionAttributers.namedItem("y").nodeValue().toInt();
    setPosition(QPointF(positionX, positionY));

    QDomElement directionElement = connectionNode.firstChildElement("kactus2:direction");
    QDomNamedNodeMap directionAttributes = directionElement.attributes();
    int directionX = directionAttributes.namedItem("x").nodeValue().toInt();
    int directionY = directionAttributes.namedItem("y").nodeValue().toInt();
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
// Function: HierApiDependency::HierApiDependency()
//-----------------------------------------------------------------------------
HierApiDependency::HierApiDependency(QDomNode& node) :
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
        else if (childNode.nodeName() == "kactus2:activeApiInterface")
        {
            //interface_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
            //interface_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
            interface_->setComponentReference(childNode.attributes().namedItem("kactus2:componentRef").nodeValue());
            interface_->setBusReference(childNode.attributes().namedItem("kactus2:apiRef").nodeValue());
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
QString HierApiInterconnection::type() const
{
    return QString("kactus2:hierApiDependency");
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::write()
//-----------------------------------------------------------------------------
void HierApiInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:hierApiDependency");
    writer.writeAttribute("interfaceRef", getTopInterfaceRef());

    writer.writeTextElement("ipxact:name", name());
    writer.writeTextElement("ipxact:displayName", displayName());
    writer.writeTextElement("ipxact:description", description());

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("componentRef", getInterface()->getComponentReference());
    writer.writeAttribute("apiRef", getInterface()->getBusReference());
    
    writePosition(writer);
    writeVectorDirection(writer);

    if (!getRoute().isEmpty())
    {
        writer.writeStartElement("kactus2:route");

        if (isOffPage())
        {
            writer.writeAttribute("kactus2:offPage", "true");
        }
        else
        {
            writer.writeAttribute("kactus2:offPage", "false");
        }

        foreach (QPointF const& point, getRoute())
        {
            writer.writeEmptyElement("kactus2:position");
            writer.writeAttribute("x", QString::number(int(point.x())));
            writer.writeAttribute("y", QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:hierApiDependency
}
/*
//-----------------------------------------------------------------------------
// Function: HierApiDependency::isValid()
//-----------------------------------------------------------------------------
bool HierApiDependency::isValid( QStringList& errorList, QStringList const& instanceNames, QString const& parentId ) const {
	bool valid = true;
	QString const thisId(QObject::tr("Hierarchical API dependency in %1").arg(parentId));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for API dependency in %1").arg(parentId));
		valid = false;
	}
	// instance interface:

	// if the component instance name is empty
	if (interface_->getComponentReference().isEmpty()) {
		errorList.append(QObject::tr("No component instance reference set for %1.").arg(thisId));
		valid = false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface_->getComponentReference())) {
		errorList.append(QObject::tr("%1 contains reference to component instance %2 that does not exist.").arg(
			thisId).arg(interface_->getComponentReference()));
		valid = false;
	}
	// if the API reference is empty
	if (interface_->getBusReference().isEmpty()) {
		errorList.append(QObject::tr("No API reference set for API dependency in %1").arg(thisId));
		valid = false;
	}

	// interface in top component:

	if (interfaceRef_.isEmpty()) {
		errorList.append(QObject::tr("No API reference set for top API dependency in %1.").arg(thisId));
		valid = false;
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::isValid()
//-----------------------------------------------------------------------------
bool HierApiDependency::isValid( const QStringList& instanceNames ) const {
	if (name().isEmpty()) {
		return false;
	}
	// instance interface:

	// if the component instance name is empty
	if (interface_->getComponentReference().isEmpty()) {
		return false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface_->getComponentReference())) {
		return false;
	}
	// if the API reference is empty
	if (interface_->getBusReference().isEmpty()) {
		return false;
	}

	// interface in top component:

	if (interfaceRef_.isEmpty()) {
		return false;
	}	

	// all ok
	return true;
}
*/

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::setTopInterfaceRef()
//-----------------------------------------------------------------------------
void HierApiInterconnection::setTopInterfaceRef(QString const& interfaceRef)
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
QString const& HierApiInterconnection::getTopInterfaceRef() const
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
    writer.writeEmptyElement("kactus2:position");
    writer.writeAttribute("x", QString::number(int(position_.x())));
    writer.writeAttribute("y", QString::number(int(position_.y())));
}

//-----------------------------------------------------------------------------
// Function: HierApiInterconnection::writeVectorDirection()
//-----------------------------------------------------------------------------
void HierApiInterconnection::writeVectorDirection(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement("kactus2:direction");
    writer.writeAttribute("x", QString::number(int(direction_.x())));
    writer.writeAttribute("y", QString::number(int(direction_.y())));
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