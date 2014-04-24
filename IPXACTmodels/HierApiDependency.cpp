//-----------------------------------------------------------------------------
// File: HierApiDependency.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.5.2012
//
// Description:
// Hierarchical API dependency.
//-----------------------------------------------------------------------------

#include "HierApiDependency.h"

#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: HierApiDependency::HierApiDependency()
//-----------------------------------------------------------------------------
HierApiDependency::HierApiDependency()
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
// Function: HierApiDependency::HierApiDependency()
//-----------------------------------------------------------------------------
HierApiDependency::HierApiDependency(QString const& name, QString const& displayName,
                                     QString const& description, QString const& interfaceRef,
                                     ApiInterfaceRef const& ref, QPointF const& position,
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
// Function: HierApiDependency::HierApiDependency()
//-----------------------------------------------------------------------------
HierApiDependency::HierApiDependency(HierApiDependency const& rhs)
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
// Function: HierApiDependency::HierApiDependency()
//-----------------------------------------------------------------------------
HierApiDependency::HierApiDependency(QDomNode& node)
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
        else if (childNode.nodeName() == "kactus2:activeApiInterface")
        {
            interface_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
            interface_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
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
// Function: HierApiDependency::~HierApiDependency()
//-----------------------------------------------------------------------------
HierApiDependency::~HierApiDependency()
{
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::write()
//-----------------------------------------------------------------------------
void HierApiDependency::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:hierApiDependency");
    writer.writeAttribute("kactus2:interfaceRef", interfaceRef_);

    writer.writeTextElement("spirit:name", name_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("kactus2:componentRef", interface_.componentRef);
    writer.writeAttribute("kactus2:apiRef", interface_.apiRef);

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

    writer.writeEndElement(); // kactus2:hierApiDependency
}

bool HierApiDependency::isValid( QStringList& errorList, QStringList const& instanceNames, QString const& parentId ) const {
	bool valid = true;
	QString const thisId(QObject::tr("Hierarchical API dependency in %1").arg(parentId));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for API dependency in %1").arg(parentId));
		valid = false;
	}
	// instance interface:

	// if the component instance name is empty
	if (interface_.componentRef.isEmpty()) {
		errorList.append(QObject::tr("No component instance reference set for %1.").arg(thisId));
		valid = false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface_.componentRef)) {
		errorList.append(QObject::tr("%1 contains reference to component instance %2 that does not exist.").arg(
			thisId).arg(interface_.componentRef));
		valid = false;
	}
	// if the API reference is empty
	if (interface_.apiRef.isEmpty()) {
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

bool HierApiDependency::isValid( const QStringList& instanceNames ) const {
	if (name_.isEmpty()) {
		return false;
	}
	// instance interface:

	// if the component instance name is empty
	if (interface_.componentRef.isEmpty()) {
		return false;
	}
	// if the component instance does not exist.
	else if (!instanceNames.contains(interface_.componentRef)) {
		return false;
	}
	// if the API reference is empty
	if (interface_.apiRef.isEmpty()) {
		return false;
	}

	// interface in top component:

	if (interfaceRef_.isEmpty()) {
		return false;
	}	

	// all ok
	return true;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::setName()
//-----------------------------------------------------------------------------
void HierApiDependency::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::setDisplayName()
//-----------------------------------------------------------------------------
void HierApiDependency::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::setDescription()
//-----------------------------------------------------------------------------
void HierApiDependency::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::setInterfaceRef()
//-----------------------------------------------------------------------------
void HierApiDependency::setInterfaceRef(QString const& interfaceRef)
{
    interfaceRef_ = interfaceRef;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::setInterface()
//-----------------------------------------------------------------------------
void HierApiDependency::setInterface(ApiInterfaceRef const& ref)
{
    interface_ = ref;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::setOffPage()
//-----------------------------------------------------------------------------
void HierApiDependency::setOffPage(bool offPage)
{
    offPage_ = offPage;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getName()
//-----------------------------------------------------------------------------
QString const& HierApiDependency::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getDisplayName()
//-----------------------------------------------------------------------------
QString const& HierApiDependency::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getDescription()
//-----------------------------------------------------------------------------
QString const& HierApiDependency::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getInterfaceRef()
//-----------------------------------------------------------------------------
QString const& HierApiDependency::getInterfaceRef() const
{
    return interfaceRef_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getInterface2()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& HierApiDependency::getInterface() const
{
    return interface_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getPosition()
//-----------------------------------------------------------------------------
QPointF const& HierApiDependency::getPosition() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getDirection()
//-----------------------------------------------------------------------------
QVector2D const& HierApiDependency::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> const& HierApiDependency::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::isOffPage()
//-----------------------------------------------------------------------------
bool HierApiDependency::isOffPage() const
{
    return offPage_;
}

//-----------------------------------------------------------------------------
// Function: HierApiDependency::operator=()
//-----------------------------------------------------------------------------
HierApiDependency& HierApiDependency::operator=(HierApiDependency const& rhs)
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
