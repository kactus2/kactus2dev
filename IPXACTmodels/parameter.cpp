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

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter(QDomNode & parameterNode): nameGroup_(parameterNode), attributes_(),
value_(), valueAttributes_()
{
    attributes_ = XmlUtils::parseAttributes(parameterNode);

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
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j)
            {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

//-----------------------------------------------------------------------------
// Function: Parameter::Parameter()
//-----------------------------------------------------------------------------
Parameter::Parameter(): nameGroup_(), value_(), attributes_(), valueAttributes_()
{

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
// Function: Parameter::getMinimumValue()
//-----------------------------------------------------------------------------
QString Parameter::getMinimumValue() const
{
    return valueAttributes_.value("spirit:minimum");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setMinimumValue()
//-----------------------------------------------------------------------------
void Parameter::setMinimumValue(QString const& minimum)
{
    setValueAttribute("spirit:minimum", minimum);
}

//-----------------------------------------------------------------------------
// Function: Parameter::getMaximumValue()
//-----------------------------------------------------------------------------
QString Parameter::getMaximumValue() const
{
    return valueAttributes_.value("spirit:maximum");
}

//-----------------------------------------------------------------------------
// Function: Parameter::setMinimumValue()
//-----------------------------------------------------------------------------
void Parameter::setMaximumValue(QString const& maximum)
{
    setValueAttribute("spirit:maximum", maximum);
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
