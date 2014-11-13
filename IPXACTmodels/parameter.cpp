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
    if (!choiceRef.isEmpty())
    {    
        valueAttributes_.insert("spirit:choiceRef", choiceRef);
    }
    else
    {
        valueAttributes_.remove("spirit:choiceRef");
    }
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
    if (!format.isEmpty())
    {    
        valueAttributes_.insert("spirit:format", format);
    }
    else
    {
        valueAttributes_.remove("spirit:format");
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
// Function: Parameter::getAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& Parameter::getAttributes() const
{
    return attributes_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::getValueAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& Parameter::getValueAttributes()  const
{
    return valueAttributes_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::isValid()
//-----------------------------------------------------------------------------
bool Parameter::isValid() const 
{
	if (getName().isEmpty())
    {
		return false;
    }
	else if (value_.isEmpty())
	{
        return false;
    }
    else if (!isValidFormat())
    {
        return false;
    }
    else if (!isValidValueForFormat())
    {
        return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: Parameter::isValid()
//-----------------------------------------------------------------------------
bool Parameter::isValid( QStringList& errorList, const QString& parentIdentifier) const
{
	bool valid = true;

	if (getName().isEmpty()) 
    {
		errorList.append(QObject::tr("No name specified for %1 within %2").arg(elementName(), parentIdentifier));
		valid = false;
	}

	if (value_.isEmpty())
    {
		errorList.append(QObject::tr("No value specified for %1 %2 within %3").arg(
			elementName(), getName(), parentIdentifier));
		valid = false;
	}

    if (!isValidFormat())
    {
        errorList.append(QObject::tr("Invalid format %1 specified for %2 %3 within %4").arg(
            getValueFormat(), elementName(), getName(), parentIdentifier));
        valid = false;
    }

    if (!isValidValueForFormat())
    {
        errorList.append(QObject::tr("Value %1 is not valid for format %2 in %3 %4 within %5"
            ).arg(value_, getValueFormat(), elementName(), getName(), parentIdentifier));
        valid = false;
    }

	return valid;
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
// Function: Parameter::isValidValueForFormat()
//-----------------------------------------------------------------------------
bool Parameter::isValidValueForFormat() const
{
    QString format = getValueFormat();
    if (format.isEmpty() || format == "string")
    {
        return true;
    }

    QRegExp validatingExp;
    if (format == "bool")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_BOOL_VALUE);
    }
    else if (format == "bitString")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_BITSTRING_VALUE);
    }
    else if (format == "long")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_LONG_VALUE);
    }
    else if (format == "float")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_FLOAT_VALUE);
    }

    return validatingExp.exactMatch(value_);
}

//-----------------------------------------------------------------------------
// Function: Parameter::isValidFormat()
//-----------------------------------------------------------------------------
bool Parameter::isValidFormat() const
{
    return getValueFormat().isEmpty() || getValueFormat() == "bool" || getValueFormat() == "bitString" ||
        getValueFormat() == "long" || getValueFormat() == "float" || getValueFormat() == "string";
}
