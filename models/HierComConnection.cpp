//-----------------------------------------------------------------------------
// File: HierComConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.5.2012
//
// Description:
// Hierarchical API dependency.
//-----------------------------------------------------------------------------

#include "HierComConnection.h"

#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: HierComConnection::HierComConnection()
//-----------------------------------------------------------------------------
HierComConnection::HierComConnection()
    : name_(),
      displayName_(),
      desc_(),
      interfaceRef_(),
      interface_(),
      route_(),
      offPage_(false)
{
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::HierComConnection()
//-----------------------------------------------------------------------------
HierComConnection::HierComConnection(QString const& name, QString const& displayName,
                                     QString const& description, QString const& interfaceRef,
                                     ComInterfaceRef const& ref, QPointF const& position,
                                     QVector2D const& direction, QList<QPointF> const& route)
    : name_(name),
      displayName_(displayName),
      desc_(description),
      interfaceRef_(interfaceRef),
      interface_(ref),
      position_(position),
      direction_(direction),
      route_(route),
      offPage_(false)
{

}

//-----------------------------------------------------------------------------
// Function: HierComConnection::HierComConnection()
//-----------------------------------------------------------------------------
HierComConnection::HierComConnection(HierComConnection const& rhs)
    : name_(rhs.name_),
      displayName_(rhs.displayName_),
      desc_(rhs.desc_),
      interfaceRef_(rhs.interfaceRef_),
      interface_(rhs.interface_),
      position_(rhs.position_),
      direction_(rhs.direction_),
      route_(rhs.route_),
      offPage_(rhs.offPage_)
{
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::HierComConnection()
//-----------------------------------------------------------------------------
HierComConnection::HierComConnection(QDomNode& node)
    : name_(),
      displayName_(),
      desc_(),
      interfaceRef_(),
      interface_(),
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

        if (childNode.nodeName() == "spirit:name")
        {
            name_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            displayName_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            desc_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:activeComInterface")
        {
            interface_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
            interface_.comRef = childNode.attributes().namedItem("kactus2:comRef").nodeValue();
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
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::~HierComConnection()
//-----------------------------------------------------------------------------
HierComConnection::~HierComConnection()
{
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::write()
//-----------------------------------------------------------------------------
void HierComConnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:hierComConnection");
    writer.writeAttribute("kactus2:interfaceRef", interfaceRef_);

    writer.writeTextElement("spirit:name", name_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("kactus2:componentRef", interface_.componentRef);
    writer.writeAttribute("kactus2:comRef", interface_.comRef);

    XmlUtils::writePosition(writer, position_);
    XmlUtils::writeDirection(writer, direction_);

    if (!route_.isEmpty())
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
            writer.writeEmptyElement("kactus2:position");
            writer.writeAttribute("x", QString::number(int(point.x())));
            writer.writeAttribute("y", QString::number(int(point.y())));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement(); // kactus2:hierComConnection
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::setName()
//-----------------------------------------------------------------------------
void HierComConnection::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::setDisplayName()
//-----------------------------------------------------------------------------
void HierComConnection::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::setDescription()
//-----------------------------------------------------------------------------
void HierComConnection::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::setInterfaceRef()
//-----------------------------------------------------------------------------
void HierComConnection::setInterfaceRef(QString const& interfaceRef)
{
    interfaceRef_ = interfaceRef;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::setInterface()
//-----------------------------------------------------------------------------
void HierComConnection::setInterface(ComInterfaceRef const& ref)
{
    interface_ = ref;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::setOffPage()
//-----------------------------------------------------------------------------
void HierComConnection::setOffPage(bool offPage)
{
    offPage_ = offPage;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getName()
//-----------------------------------------------------------------------------
QString const& HierComConnection::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getDisplayName()
//-----------------------------------------------------------------------------
QString const& HierComConnection::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getDescription()
//-----------------------------------------------------------------------------
QString const& HierComConnection::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getInterfaceRef()
//-----------------------------------------------------------------------------
QString const& HierComConnection::getInterfaceRef() const
{
    return interfaceRef_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getInterface2()
//-----------------------------------------------------------------------------
ComInterfaceRef const& HierComConnection::getInterface() const
{
    return interface_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getPosition()
//-----------------------------------------------------------------------------
QPointF const& HierComConnection::getPosition() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getDirection()
//-----------------------------------------------------------------------------
QVector2D const& HierComConnection::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> const& HierComConnection::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::isOffPage()
//-----------------------------------------------------------------------------
bool HierComConnection::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: HierComConnection::operator=()
//-----------------------------------------------------------------------------
HierComConnection& HierComConnection::operator=(HierComConnection const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        interfaceRef_ = rhs.interfaceRef_;
        interface_ = rhs.interface_;
        position_ = rhs.position_;
        direction_ = rhs.direction_;
        route_ = rhs.route_;
        offPage_ = rhs.offPage_;
    }

    return *this;
}

