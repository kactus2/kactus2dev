//-----------------------------------------------------------------------------
// File: parameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.7.2010
//
// Description:
// Equals the spirit:parameter element in IP-Xact specification.
//-----------------------------------------------------------------------------

#include "parameter.h"

#include "StringPromtAtt.h"
#include "GenericVendorExtension.h"
#include "XmlUtils.h"

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QMap>
#include <QXmlStreamWriter>
#include <qmath.h>

#include <QUuid>

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter(QDomNode & parameterNode): nameGroup_(parameterNode),
value_(), attributes_(), valueAttributes_(), bitWidthVector_(), vendorExtensions_()
{
    attributes_ = XmlUtils::parseAttributes(parameterNode);

    changeOldAttributesToNewAttributes();

	for (int i = 0; i < parameterNode.childNodes().count(); ++i)
    {
		QDomNode tempNode = parameterNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:value"))
        {
			value_ = tempNode.childNodes().at(0).nodeValue();
			valueAttributes_ = XmlUtils::parseAttributes(tempNode);
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            parseVendorExtensions(tempNode);
        }
	}

    if (getValueId().isEmpty())
    {
        createUuid();
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter(): nameGroup_(), value_(), attributes_(), valueAttributes_()
{
    createUuid();
}

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter( const Parameter &other ):
nameGroup_(other.nameGroup_),
value_(other.value_),
attributes_(other.attributes_),
valueAttributes_(other.valueAttributes_)
{
    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Parameter::operator=()
//-----------------------------------------------------------------------------
Parameter & Parameter::operator=( const Parameter &other ) 
{
	if (this != &other) 
    {
		nameGroup_ = other.nameGroup_;
		value_ = other.value_;
        attributes_ = other.attributes_;
		valueAttributes_ = other.valueAttributes_;

        copyVendorExtensions(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Parameter::~Parameter()
//-----------------------------------------------------------------------------
Parameter::~Parameter()
{
    attributes_.clear();
    valueAttributes_.clear();
}

//-----------------------------------------------------------------------------
// Function: Parameter::write()
//-----------------------------------------------------------------------------
void Parameter::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement(elementIdentifier());

    XmlUtils::writeAttributes(writer, attributes_);

    nameGroup_.write(writer);

    // start the spirit:value tag
    writer.writeStartElement("spirit:value");

    // write the attributes for the element
    XmlUtils::writeAttributes(writer, valueAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(value_);
    writer.writeEndElement(); // spirit:value

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:parameter
}

//-----------------------------------------------------------------------------
// Function: Parameter::getName()
//-----------------------------------------------------------------------------
QString Parameter::getName() const
{
	return nameGroup_.name();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getDisplayName()
//-----------------------------------------------------------------------------
QString Parameter::getDisplayName() const
{
    return nameGroup_.displayName();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getDescription()
//-----------------------------------------------------------------------------
QString Parameter::getDescription() const 
{
    return nameGroup_.description();
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValue()
//-----------------------------------------------------------------------------
QString Parameter::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::setName()
//-----------------------------------------------------------------------------
void Parameter::setName(QString const& name)
{
	nameGroup_.setName(name);
}

//-----------------------------------------------------------------------------
// Function: Parameter::setDisplayName()
//-----------------------------------------------------------------------------
void Parameter::setDisplayName(const QString& displayName)
{
    nameGroup_.setDisplayName(displayName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::setDescription()
//-----------------------------------------------------------------------------
void Parameter::setDescription(const QString& description)
{
    nameGroup_.setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValue()
//-----------------------------------------------------------------------------
void Parameter::setValue(QString const& value) 
{
	value_ = value;
}

//-----------------------------------------------------------------------------
// Function: Parameter::getChoiceRef()
//-----------------------------------------------------------------------------
QString Parameter::getChoiceRef() const
{
    return valueAttributes_.value("spirit:choiceRef");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setChoiceRef()
//-----------------------------------------------------------------------------
void Parameter::setChoiceRef(QString const& choiceRef)
{ 
    setValueAttribute("spirit:choiceRef", choiceRef);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueFormat()
//-----------------------------------------------------------------------------
QString Parameter::getValueFormat() const
{
    return valueAttributes_.value("spirit:format");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueFormat()
//-----------------------------------------------------------------------------
void Parameter::setValueFormat(QString const& format)
{
    setValueAttribute("spirit:format", format);
}

//-----------------------------------------------------------------------------
// Function: parameter::getType()
//-----------------------------------------------------------------------------
QString Parameter::getType() const
{
    return attributes_.value("type");
}

//-----------------------------------------------------------------------------
// Function: parameter::setType()
//-----------------------------------------------------------------------------
void Parameter::setType(QString const& type)
{
    setAttribute("type", type);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getBitStringLength()
//-----------------------------------------------------------------------------
QString Parameter::getBitStringLength() const
{
    return valueAttributes_.value("spirit:bitStringLength");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setBitStringLength()
//-----------------------------------------------------------------------------
void Parameter::setBitStringLength(QString const& length)
{
    setValueAttribute("spirit:bitStringLength", length);
}

//-----------------------------------------------------------------------------
// Function: parameter::getBitWidthLeft()
//-----------------------------------------------------------------------------
QString Parameter::getBitWidthLeft() const
{
    if (hasBitWidth())
    {
        return bitWidthVector_->getLeft();
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::getBitWidthRight()
//-----------------------------------------------------------------------------
QString Parameter::getBitWidthRight() const
{
    if (hasBitWidth())
    {
        return bitWidthVector_->getRight();
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::setBitWidthLeft()
//-----------------------------------------------------------------------------
void Parameter::setBitWidthLeft(QString const& newBitWidthLeft)
{
    if (newBitWidthLeft.isEmpty() && getBitWidthRight().isEmpty())
    {
        vendorExtensions_.removeAll(bitWidthVector_);
        bitWidthVector_.clear();
    }
    else if (hasBitWidth())
    {
        bitWidthVector_->setLeft(newBitWidthLeft);
    }
    else
    {
        createBitWidthVector(newBitWidthLeft, QString());
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::setBitWidthRight()
//-----------------------------------------------------------------------------
void Parameter::setBitWidthRight(QString const& newBitWidthRight)
{
    if (newBitWidthRight.isEmpty() && getBitWidthLeft().isEmpty())
    {
        vendorExtensions_.removeAll(bitWidthVector_);
        bitWidthVector_.clear();
    }
    else if (hasBitWidth())
    {
        bitWidthVector_->setRight(newBitWidthRight);
    }
    else
    {
        createBitWidthVector(QString(), newBitWidthRight);
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::hasBitWidth()
//-----------------------------------------------------------------------------
bool Parameter::hasBitWidth() const
{
    if (bitWidthVector_.isNull())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::getMinimumValue()
//-----------------------------------------------------------------------------
QString Parameter::getMinimumValue() const
{
    return attributes_.value("minimum");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setMinimumValue()
//-----------------------------------------------------------------------------
void Parameter::setMinimumValue(QString const& minimum)
{
    setAttribute("minimum", minimum);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getMaximumValue()
//-----------------------------------------------------------------------------
QString Parameter::getMaximumValue() const
{
    return attributes_.value("maximum");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setMaximumValue()
//-----------------------------------------------------------------------------
void Parameter::setMaximumValue(QString const& maximum)
{
    setAttribute("maximum", maximum);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueResolve()
//-----------------------------------------------------------------------------
QString Parameter::getValueResolve() const
{
    return valueAttributes_.value("spirit:resolve");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueResolve()
//-----------------------------------------------------------------------------
void Parameter::setValueResolve(QString const& resolve)
{
    setValueAttribute("spirit:resolve", resolve);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueId()
//-----------------------------------------------------------------------------
QString Parameter::getValueId() const
{
     return valueAttributes_.value("spirit:id");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueId()
//-----------------------------------------------------------------------------
void Parameter::setValueId(QString const& id)
{
     setValueAttribute("spirit:id", id);
}

//-----------------------------------------------------------------------------
// Function: parameter::getUsageCount()
//-----------------------------------------------------------------------------
int Parameter::getUsageCount() const
{
    return attributes_.value("usageCount", "0").toInt();
}

//-----------------------------------------------------------------------------
// Function: parameter::increaseUsageCount()
//-----------------------------------------------------------------------------
void Parameter::increaseUsageCount()
{
    attributes_.insert("usageCount", QString::number(getUsageCount() + 1));
}

//-----------------------------------------------------------------------------
// Function: parameter::decreaseUsageCount()
//-----------------------------------------------------------------------------
void Parameter::decreaseUsageCount()
{
    if (getUsageCount() < 2)
    {
        attributes_.remove("usageCount");
    }

    else
    {
        attributes_.insert("usageCount", QString::number(getUsageCount() - 1));
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::hasAttribute()
//-----------------------------------------------------------------------------
bool Parameter::hasAttribute(QString const& attributeName) const
{
    return attributes_.contains(attributeName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getAttribute()
//-----------------------------------------------------------------------------
QString Parameter::getAttribute(QString const& attributeName) const
{
    return attributes_.value(attributeName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::setAttribute()
//-----------------------------------------------------------------------------
void Parameter::setAttribute(QString const& attributeName, QString const& attributeValue)
{
    if (!attributeValue.isEmpty())
    {
        attributes_.insert(attributeName, attributeValue);
    }
    else
    {
        attributes_.remove(attributeName);
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueAttribute()
//-----------------------------------------------------------------------------
QString Parameter::getValueAttribute(QString const& attributeName) const
{
    return valueAttributes_.value(attributeName);
}

//-----------------------------------------------------------------------------
// Function: Parameter::elementIdentifier()
//-----------------------------------------------------------------------------
QString Parameter::elementIdentifier() const
{
    return "spirit:parameter";
}

//-----------------------------------------------------------------------------
// Function: Parameter::elementName()
//-----------------------------------------------------------------------------
QString Parameter::elementName() const
{
    return "parameter";
}

//-----------------------------------------------------------------------------
// Function: Parameter::setValueAttribute()
//-----------------------------------------------------------------------------
void Parameter::setValueAttribute(QString const& attributeName, QString const& attributeValue)
{
    if (!attributeValue.isEmpty())
    {
        valueAttributes_.insert(attributeName, attributeValue);
    }
    else
    {
        valueAttributes_.remove(attributeName);
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::createUuid()
//-----------------------------------------------------------------------------
void Parameter::createUuid()
{
    QString formattedUuid = "uuid_" + QUuid::createUuid().toString().remove('{').remove('}').replace('-', '_');
    setValueId(formattedUuid);
}

//-----------------------------------------------------------------------------
// Function: parameter::changeOldArrayValuesToNewArrayValues()
//-----------------------------------------------------------------------------
void Parameter::changeOldAttributesToNewAttributes()
{
    if (attributes_.contains("arraySize"))
    {
        QString arraySize = attributes_.take("arraySize");
        setAttribute("kactus2:arrayLeft", arraySize);
    }

    if (attributes_.contains("arrayOffset"))
    {
        QString arrayOffset = attributes_.take("arrayOffset");
        setAttribute("kactus2:arrayRight", arrayOffset);
    }

    if (attributes_.contains("kactus2:bitWidth"))
    {
        QString bitWidth = attributes_.take("kactus2:bitWidth");
        bitWidth.append("-1");
        createBitWidthVector(bitWidth, QString::number(0));
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::parseVendorExtensions()
//-----------------------------------------------------------------------------
void Parameter::parseVendorExtensions(QDomNode const& vendorExtensionNode)
{
    int extensionCount = vendorExtensionNode.childNodes().count();
    for (int j = 0; j < extensionCount; ++j)
    {
        QDomNode extensionNode = vendorExtensionNode.childNodes().at(j);

        if (extensionNode.nodeName() == "kactus2:vector")
        {
            createBitWidthVectorFromExtensionNode(extensionNode);
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Parameter::copyVendorExtensions(Parameter const& other)
{
    foreach (QSharedPointer<VendorExtension> extension, other.vendorExtensions_)
    {
        if (extension->type() == "kactus2:vector")
        {
            vendorExtensions_.removeAll(bitWidthVector_);

            bitWidthVector_ = QSharedPointer<Kactus2Vector>(other.bitWidthVector_->clone());
            vendorExtensions_.append(bitWidthVector_);
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::createBitWidthVectorFromExtensionNode()
//-----------------------------------------------------------------------------
void Parameter::createBitWidthVectorFromExtensionNode(QDomNode const& vectorExtensionNode)
{
    QString vectorLeft;
    QString vectorRight;

    int extensionCount = vectorExtensionNode.childNodes().count();
    for (int i = 0; i < extensionCount; ++i)
    {
        QDomNode childNode = vectorExtensionNode.childNodes().at(i);

        if (childNode.nodeName() == "kactus2:left")
        {
            vectorLeft = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:right")
        {
            vectorRight = childNode.childNodes().at(0).nodeValue();
        }
    }

    if (hasBitWidth())
    {
        setBitWidthLeft(vectorLeft);
        setBitWidthRight(vectorRight);
    }
    else
    {
        createBitWidthVector(vectorLeft, vectorRight);
    }
}

//-----------------------------------------------------------------------------
// Function: parameter::createBitWidthVector()
//-----------------------------------------------------------------------------
void Parameter::createBitWidthVector(QString const& left, QString const& right)
{
    bitWidthVector_ = QSharedPointer<Kactus2Vector>(new Kactus2Vector(left, right));
    vendorExtensions_.append(bitWidthVector_);
}