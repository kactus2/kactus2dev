//-----------------------------------------------------------------------------
// File: ComInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.4.2012
//
// Description:
// COM interface class for defining properties of a specific COM interface
// in a SW component.
//-----------------------------------------------------------------------------

#include "ComInterface.h"

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface() :
NameGroup(),
comType_(),
transferType_(),
dir_(DirectionTypes::INOUT),
propertyValues_(),
comImplementation_(),
defaultPos_()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface(ComInterface const& rhs) :
NameGroup(rhs),
comType_(rhs.comType_),
transferType_(rhs.transferType_),
dir_(rhs.dir_),
propertyValues_(rhs.propertyValues_),
comImplementation_(rhs.comImplementation_),
defaultPos_(rhs.defaultPos_)
{

}

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface(QDomNode& node) :
NameGroup(),
comType_(),
transferType_(),
dir_(DirectionTypes::INOUT),
propertyValues_(),
comImplementation_(),
defaultPos_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == QString("ipxact:name"))
        {
            setName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QString("ipxact:displayName"))
        {
            setDisplayName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QString("ipxact:description"))
        {
            setDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == "kactus2:comType")
        {
            comType_ = VLNV::createVLNV(childNode, VLNV::COMDEFINITION);
        }
        else if (childNode.nodeName() == "kactus2:transferType")
        {
            transferType_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:comDirection")
        {
            dir_ = DirectionTypes::str2Direction(childNode.childNodes().at(0).nodeValue(), DirectionTypes::INOUT);
        }
        else if (childNode.nodeName() == "kactus2:propertyValues")
        {
            parsePropertyValues(childNode);
        }
        else if (childNode.nodeName() == "kactus2:comImplementationRef")
        {
            comImplementation_ = VLNV::createVLNV(childNode, VLNV::COMPONENT);
        }
        else if (childNode.nodeName() == "kactus2:position")
        {
            defaultPos_.setX(childNode.attributes().namedItem("x").nodeValue().toInt());
            defaultPos_.setY(childNode.attributes().namedItem("y").nodeValue().toInt());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterface::~ComInterface()
//-----------------------------------------------------------------------------
ComInterface::~ComInterface()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterface::clone()
//-----------------------------------------------------------------------------
ComInterface* ComInterface::clone() const
{
    return new ComInterface(*this);
}

//-----------------------------------------------------------------------------
// Function: ComInterface::type()
//-----------------------------------------------------------------------------
QString ComInterface::type() const
{
    return QString("kactus2:comInterface");
}

//-----------------------------------------------------------------------------
// Function: ComInterface::write()
//-----------------------------------------------------------------------------
void ComInterface::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:comInterface");

    writer.writeTextElement("ipxact:name", name());

    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }

    // Write communication type, data type and communication direction.
    writer.writeEmptyElement("kactus2:comType");
    writer.writeAttribute("vendor", comType_.getVendor());
    writer.writeAttribute("library", comType_.getLibrary());
    writer.writeAttribute("name", comType_.getName());
    writer.writeAttribute("version", comType_.getVersion());

    writer.writeTextElement("kactus2:transferType", transferType_);
    writer.writeTextElement("kactus2:comDirection", DirectionTypes::direction2Str(dir_));

    writer.writeStartElement("kactus2:propertyValues");

    // Write property values.
    QMapIterator<QString, QString> iter(propertyValues_);

    while (iter.hasNext())
    {
        iter.next();

        writer.writeEmptyElement("kactus2:propertyValue");
        writer.writeAttribute("name", iter.key());
        writer.writeAttribute("value", iter.value());
    }

    writer.writeEndElement(); // kactus2:propertyValues

	writer.writeEmptyElement("kactus2:comImplementationRef");
    writer.writeAttribute("vendor", comImplementation_.getVendor());
    writer.writeAttribute("library", comImplementation_.getLibrary());
    writer.writeAttribute("name", comImplementation_.getName());
    writer.writeAttribute("version", comImplementation_.getVersion());

    if (!defaultPos_.isNull())
    {
        writePosition(writer, defaultPos_);
    }

    writer.writeEndElement(); // kactus2:comInterface
}

//-----------------------------------------------------------------------------
// Function: ComInterface::isValid()
//-----------------------------------------------------------------------------
bool ComInterface::isValid(QStringList& errorList, QString const& parentId) const
{
    QString const thisId = QObject::tr("COM interface '%1'").arg(name());
    bool valid = true;

    if (name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for a COM interface in %1").arg(parentId));
        valid = false;
    }

    if (!comType_.isEmpty() && !comType_.isValid())
    {
        errorList.append(QObject::tr("Invalid COM type set in %1").arg(thisId));
        valid = false;
    }

    // Check for property values having no name.
    QMapIterator<QString, QString> iter(propertyValues_);

    while (iter.hasNext())
    {
        iter.next();

        if (iter.key().isEmpty())
        {
            errorList.append(QObject::tr("No name specified for a property value in %1").arg(thisId));
            valid = false;
        }
    }

	 if (!comImplementation_.isEmpty() && !comImplementation_.isValid()) {
		 errorList.append(QObject::tr("Invalid COM implementation reference in %1").arg(thisId));
		 valid = false;
	 }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::isValid()
//-----------------------------------------------------------------------------
bool ComInterface::isValid() const
{
    if (name().isEmpty())
    {
        return false;
    }

    if (!comType_.isEmpty() && !comType_.isValid())
    {
        return false;
    }

    // Check for property values having no name.
    QMapIterator<QString, QString> iter(propertyValues_);

    while (iter.hasNext())
    {
        iter.next();

        if (iter.key().isEmpty())
        {
            return false;
        }
    }

	 if (!comImplementation_.isEmpty() && !comImplementation_.isValid()) {
		 return false;
	 }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setComType()
//-----------------------------------------------------------------------------
void ComInterface::setComType(VLNV const& vlnv, QList< QSharedPointer<ComProperty> > const* properties)
{
	comType_ = vlnv;

	if ( properties != NULL )
	{
		foreach (QSharedPointer<ComProperty const> prop, *properties)
		{
			propertyValues_[prop->name()] = prop->getDefaultValue();
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setTransferType()
//-----------------------------------------------------------------------------
void ComInterface::setTransferType(QString const& transferType)
{
    transferType_ = transferType;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setDirection()
//-----------------------------------------------------------------------------
void ComInterface::setDirection(DirectionTypes::Direction dir)
{
    dir_ = dir;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setPropertyValues()
//-----------------------------------------------------------------------------
void ComInterface::setPropertyValues(QMap<QString, QString> const& values)
{
    propertyValues_ = values;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getComType()
//-----------------------------------------------------------------------------
VLNV const& ComInterface::getComType() const
{
    return comType_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getTransferType()
//-----------------------------------------------------------------------------
QString const& ComInterface::getTransferType() const
{
    return transferType_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction ComInterface::getDirection() const
{
    return dir_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& ComInterface::getPropertyValues() const
{
    return propertyValues_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::operator=()
//-----------------------------------------------------------------------------
ComInterface& ComInterface::operator=(ComInterface const& rhs)
{
    if (&rhs != this)
    {
        NameGroup::operator=(rhs);
        comType_ = rhs.comType_;
        dir_ = rhs.dir_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComInterface::parsePropertyValues(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("name").nodeValue();
            QString value = propNode.attributes().namedItem("value").nodeValue();

            propertyValues_.insert(name, value);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getComImplementation()
//-----------------------------------------------------------------------------
const VLNV& ComInterface::getComImplementation() const
{
    return comImplementation_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setComImplementation()
//-----------------------------------------------------------------------------
void ComInterface::setComImplementation( const VLNV& implementationVLNV )
{
	comImplementation_ = implementationVLNV;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setDefaultPos()
//-----------------------------------------------------------------------------
void ComInterface::setDefaultPos(QPointF const& pos)
{
    defaultPos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF const& ComInterface::getDefaultPos() const
{
    return defaultPos_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::writePosition()
//-----------------------------------------------------------------------------
void ComInterface::writePosition(QXmlStreamWriter& writer, QPointF const& pos) const
{
    writer.writeEmptyElement("kactus2:position");
    writer.writeAttribute("x", QString::number(int(pos.x())));
    writer.writeAttribute("y", QString::number(int(pos.y())));
}