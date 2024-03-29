//-----------------------------------------------------------------------------
// File: ComInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 5.4.2012
//
// Description:
// COM interface class for defining properties of a specific COM interface
// in a SW component.
//-----------------------------------------------------------------------------

#include "ComInterface.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface() :
NameGroup()
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
defaultPos_(rhs.getDefaultPos())
{

}

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface(QDomNode& node) :
NameGroup()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == QLatin1String("ipxact:name"))
        {
            setName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:displayName"))
        {
            setDisplayName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:shortDescription"))
        {
            setShortDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:description"))
        {
            setDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:comType"))
        {
            comType_ = CommonItemsReader::parseVLNVAttributes(childNode, VLNV::COMDEFINITION);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:transferType"))
        {
            transferType_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:comDirection"))
        {
            dir_ = DirectionTypes::str2Direction(childNode.childNodes().at(0).nodeValue(), DirectionTypes::INOUT);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:propertyValues"))
        {
            parsePropertyValues(childNode);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:comImplementationRef"))
        {
            comImplementation_ = CommonItemsReader::parseVLNVAttributes(childNode, VLNV::COMPONENT);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:position"))
        {
            defaultPos_.parsePosition(childNode);
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
    return QStringLiteral("kactus2:comInterface");
}

//-----------------------------------------------------------------------------
// Function: ComInterface::write()
//-----------------------------------------------------------------------------
void ComInterface::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:comInterface"));

    writer.writeTextElement(QStringLiteral("ipxact:name"), name());

    CommonItemsWriter::writeDisplayName(writer, displayName());

    CommonItemsWriter::writeDescription(writer, description());

    // Write communication type, data type and communication direction.
    writer.writeEmptyElement(QStringLiteral("kactus2:comType"));
    CommonItemsWriter::writeVLNVAttributes(writer, comType_);

    writer.writeTextElement(QStringLiteral("kactus2:transferType"), transferType_);
    writer.writeTextElement(QStringLiteral("kactus2:comDirection"), DirectionTypes::direction2Str(dir_));

    writer.writeStartElement(QStringLiteral("kactus2:propertyValues"));

    // Write property values.
    QMapIterator<QString, QString> iter(propertyValues_);

    while (iter.hasNext())
    {
        iter.next();

        writer.writeEmptyElement(QStringLiteral("kactus2:propertyValue"));
        writer.writeAttribute(QStringLiteral("name"), iter.key());
        writer.writeAttribute(QStringLiteral("value"), iter.value());
    }

    writer.writeEndElement(); // kactus2:propertyValues

	writer.writeEmptyElement(QStringLiteral("kactus2:comImplementationRef"));
    CommonItemsWriter::writeVLNVAttributes(writer, comImplementation_);

    if (getDefaultPos().isNull() == false)
    {
        defaultPos_.write(writer);
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

	if ( properties != nullptr )
	{
		for (QSharedPointer<ComProperty const> prop : *properties)
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

        if (propNode.nodeName() == QLatin1String("kactus2:propertyValue"))
        {
            QString name = propNode.attributes().namedItem(QStringLiteral("name")).nodeValue();
            QString value = propNode.attributes().namedItem(QStringLiteral("value")).nodeValue();

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
    defaultPos_.setPosition(pos);
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF ComInterface::getDefaultPos() const
{
    return defaultPos_.position();
}
