//-----------------------------------------------------------------------------
// File: HierConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.6.2014
//
// Description:
// Describes the spirit:hierConnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "HierConnection.h"

#include "Interface.h"
#include "VendorExtension.h"
#include "XmlUtils.h"

#include <QString>
#include <QVector2D>

//-----------------------------------------------------------------------------
// Function: HierConnection::HierConnection()
//-----------------------------------------------------------------------------
HierConnection::HierConnection(QDomNode& hierConnectionNode)
    : interfaceRef_(), interface_(QString(""), QString("")), position_(), route_(), offPage_(false),
      vendorExtensions_()
{
    QDomNamedNodeMap attributes = hierConnectionNode.attributes();

    interfaceRef_ = attributes.namedItem("spirit:interfaceRef").nodeValue();

    for (int i = 0; i < hierConnectionNode.childNodes().size(); i++) 
    {
        QDomNode node = hierConnectionNode.childNodes().at(i);

        if (node.nodeName() == "spirit:interface")
        {
            interface_ = Interface(node);
        }
        else if (node.nodeName() == "spirit:vendorExtensions")
        {
            parseVendorExtensions(node);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierConnection::HierConnection()
//-----------------------------------------------------------------------------
HierConnection::HierConnection(QString interfaceRef_,
                                       Interface interface_,
                                       QPointF const& position,
                                       QVector2D const& direction,
                                       QList<QPointF> const& route,
                                       bool offPage)
    : interfaceRef_(interfaceRef_), interface_(interface_),
      position_(position), direction_(direction), route_(route), offPage_(offPage), vendorExtensions_()
{
}

//-----------------------------------------------------------------------------
// Function: HierConnection::HierConnection()
//-----------------------------------------------------------------------------
HierConnection::HierConnection( const HierConnection& other ):
interfaceRef_(other.interfaceRef_),
interface_(other.interface_),
position_(other.position_),
direction_(other.direction_),
route_(other.route_),
offPage_(other.offPage_),
vendorExtensions_(other.vendorExtensions_)
{
}

//-----------------------------------------------------------------------------
// Function: HierConnection::~HierConnection()
//-----------------------------------------------------------------------------
HierConnection::~HierConnection()
{

}

//-----------------------------------------------------------------------------
// Function: HierConnection::operator=()
//-----------------------------------------------------------------------------
HierConnection& HierConnection::operator=( const HierConnection& other ) {
	if (this != &other) {
		interfaceRef_ = other.interfaceRef_;
		interface_ = other.interface_;
		position_ = other.position_;
		direction_ = other.direction_;
		route_ = other.route_;
        offPage_ = other.offPage_;
        vendorExtensions_ = other.vendorExtensions_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::getInterfaceRef()
//-----------------------------------------------------------------------------
QString HierConnection::getInterfaceRef() const
{
    return interfaceRef_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::getInterface()
//-----------------------------------------------------------------------------
Interface HierConnection::getInterface() const
{
    return interface_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::getPosition()
//-----------------------------------------------------------------------------
QPointF HierConnection::getPosition() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::isOffPage()
//-----------------------------------------------------------------------------
bool HierConnection::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> HierConnection::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::getDirection()
//-----------------------------------------------------------------------------
QVector2D HierConnection::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::getVendorExtensions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > HierConnection::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::setVendorExtensions()
//-----------------------------------------------------------------------------
void HierConnection::setVendorExtensions(QList<QSharedPointer<VendorExtension> > extensions)
{
    vendorExtensions_ = extensions;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::isValid()
//-----------------------------------------------------------------------------
bool HierConnection::isValid(QStringList const& instanceNames, QStringList& errorList, 
    QString const& parentIdentifier ) const 
{
	bool valid = true;

	if (interfaceRef_.isEmpty()) {
		errorList.append(QObject::tr("No interface reference set for hier connection"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!interface_.isValid(instanceNames, errorList, QObject::tr("hier connection"
		" within %1").arg(parentIdentifier))) {
			valid = false;
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: HierConnection::isValid()
//-----------------------------------------------------------------------------
bool HierConnection::isValid(QStringList const& instanceNames ) const 
{
	return !interfaceRef_.isEmpty() && interface_.isValid(instanceNames);
}

//-----------------------------------------------------------------------------
// Function: HierConnection::write()
//-----------------------------------------------------------------------------
void HierConnection::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("spirit:hierConnection");

    writer.writeAttribute("spirit:interfaceRef", interfaceRef_);

    interface_.write(writer);

    // Write custom data to vendor extensions.
    writer.writeStartElement("spirit:vendorExtensions");

    XmlUtils::writePosition(writer, position_);
    XmlUtils::writeDirection(writer, direction_);

    if (!route_.empty())
    {
        writer.writeStartElement("kactus2:route");

        if (offPage_)
        {
            writer.writeAttribute("kactus2:offPage", "true");
        }
        else
        {
            writer.writeAttribute("kactus2:offPage", "false");
        }

        foreach (QPointF const& point, route_)
        {
            XmlUtils::writePosition(writer, point);
        }

        writer.writeEndElement();
    }

    XmlUtils::writeVendorExtensions(writer, vendorExtensions_);

    writer.writeEndElement(); // spirit:vendorExtensions.

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: HierConnection::parseVendorExtensions()
//-----------------------------------------------------------------------------
void HierConnection::parseVendorExtensions(QDomNode & extensionsNode)
{
    for (int i = 0; i < extensionsNode.childNodes().size(); ++i)
    {
        QDomNode childNode = extensionsNode.childNodes().at(i);

        if (childNode.nodeName() == "kactus2:position")
        {            
            position_ = XmlUtils::parsePoint(childNode);         
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

            // Parse the route.
            for (int i = 0; i < childNode.childNodes().size(); ++i)
            {
                QDomNode posNode = childNode.childNodes().at(i);
                if (posNode.nodeName() == "kactus2:position")
                {
                    route_.append(XmlUtils::parsePoint(posNode));
                }
            }
        }
        else
        {
            QSharedPointer<VendorExtension> extension = XmlUtils::createVendorExtensionFromNode(childNode); 
            vendorExtensions_.append(extension);
        }
    }
}
