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
	
    if (value_.isEmpty())
	{
        return false;
    }

    if (formatSet())
    {    
        if (!isValidFormat())
        {
            return false;
        }

        if (getValueFormat() == "bitString" && getBitStringLength().isEmpty())
        {
            return false;
        }

        if (!isValidValueForFormat())
        {
            return false;
        }
        
        if (!isValidBoundaryForFormat(getMinimumValue()))
        { 
            return false;
        }

        if (!isValidBoundaryForFormat(getMaximumValue()))
        { 
            return false;
        }

        if (shouldCompareValueToMinimum() && valueIsLessThanMinimum())
        {
            return false;
        }

        if (shouldCompareValueToMaximum() && valueIsGreaterThanMaximum())
        {
            return false;
        }
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

    if (formatSet())
    {
        if (!isValidFormat())
        {
            errorList.append(QObject::tr("Invalid format %1 specified for %2 %3 within %4").arg(
                getValueFormat(), elementName(), getName(), parentIdentifier));
            valid = false;
        }

        if (getValueFormat() == "bitString" && getBitStringLength().isEmpty())
        {      
            errorList.append(QObject::tr("No bit string length specified for %2 %3 within %4").arg(
                getValueFormat(), elementName(), getName(), parentIdentifier));
            valid = false;
        }

        if (!isValidValueForFormat())
        {
            errorList.append(QObject::tr("Value %1 is not valid for format %2 in %3 %4 within %5"
                ).arg(value_, getValueFormat(), elementName(), getName(), parentIdentifier));
            valid = false;
        }

        if (!isValidBoundaryForFormat(getMinimumValue()))
        {
            errorList.append(QObject::tr("Minimum value %1 is not valid for format %2 in %3 %4 within %5"
                ).arg(getMinimumValue(), getValueFormat(), elementName(), getName(), parentIdentifier));
            valid = false;
        }

        if (!isValidBoundaryForFormat(getMaximumValue()))
        {
            errorList.append(QObject::tr("Maximum value %1 is not valid for format %2 in %3 %4 within %5"
                ).arg(getMaximumValue(), getValueFormat(), elementName(), getName(), parentIdentifier));
            valid = false;
        }

        if (shouldCompareValueToMinimum() && valueIsLessThanMinimum())
        {
            errorList.append(QObject::tr("Value %1 violates minimum value %2 in %3 %4 within %5"
                ).arg(value_, getMinimumValue(), elementName(), getName(), parentIdentifier));
            valid = false;
        }

        if (shouldCompareValueToMaximum() && valueIsGreaterThanMaximum())
        {
            errorList.append(QObject::tr("Value %1 violates maximum value %2 in %3 %4 within %5"
                ).arg(value_, getMaximumValue(), elementName(), getName(), parentIdentifier));
            valid = false;
        }
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
// Function: Parameter::formatSet()
//-----------------------------------------------------------------------------
bool Parameter::formatSet() const
{
    return !getValueFormat().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Parameter::isValidFormat()
//-----------------------------------------------------------------------------
bool Parameter::isValidFormat() const
{
    return getValueFormat().isEmpty() || getValueFormat() == "bool" || getValueFormat() == "bitString" ||
        getValueFormat() == "long" || getValueFormat() == "float" || getValueFormat() == "string";
}

//-----------------------------------------------------------------------------
// Function: Parameter::isValidBoundaryForFormat()
//-----------------------------------------------------------------------------
bool Parameter::isValidBoundaryForFormat(QString const& boundary) const
{
    QString format = getValueFormat();
    if (boundary.isEmpty() || format.isEmpty() || format == "bool" || format == "string")
    {
        return true;
    }

    QRegExp validatingExp;
    if (format == "bitString")
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

    return validatingExp.exactMatch(boundary);
}


//-----------------------------------------------------------------------------
// Function: Parameter::shouldCompareValueToMinimum()
//-----------------------------------------------------------------------------
bool Parameter::shouldCompareValueToMinimum() const
{
    return !getMinimumValue().isEmpty() && 
        (getValueFormat() == "long" || getValueFormat() == "float");
}

//-----------------------------------------------------------------------------
// Function: Parameter::shouldCompareValueToMaximum()
//-----------------------------------------------------------------------------
bool Parameter::shouldCompareValueToMaximum() const
{
    return !getMaximumValue().isEmpty() && 
        (getValueFormat() == "long" || getValueFormat() == "float");
}

//-----------------------------------------------------------------------------
// Function: Parameter::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool Parameter::valueIsLessThanMinimum() const
{
    return valueOf(value_) < valueOf(getMinimumValue());
}

//-----------------------------------------------------------------------------
// Function: Parameter::valueIsGreaterThanMaximum()
//-----------------------------------------------------------------------------
bool Parameter::valueIsGreaterThanMaximum() const
{
    return valueOf(value_) > valueOf(getMaximumValue());
}

//-----------------------------------------------------------------------------
// Function: Parameter::toLong()
//-----------------------------------------------------------------------------
qreal Parameter::valueOf(QString const& value) const
{
    if (getValueFormat() == "long")
    {
        return longValueOf(value);
    }
    else if (getValueFormat() == "float")
    {
        return floatValueOf(value);
    }
    else
    {
        return value.toDouble();
    }	
}

//-----------------------------------------------------------------------------
// Function: Parameter::longValueOf()
//-----------------------------------------------------------------------------
qreal Parameter::longValueOf(QString const& value) const
{
    if (value.startsWith("0x", Qt::CaseInsensitive) || value.startsWith('#'))
    {
        QString hexValue = value;
        return hexValue.remove('#').toLong(0, 16);
    }
    else if (value.endsWith('k', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024;
    }
    else if (value.endsWith('M', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024;
    }
    else if (value.endsWith('G', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024*1024;
    }
    else if (value.endsWith('T', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024*1024*1024;
    }
    else
    {
        return value.toLong();
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::floatValueOf()
//-----------------------------------------------------------------------------
qreal Parameter::floatValueOf(QString const& value) const
{
    if (value.contains('e'))
    {
        qreal coefficient = value.left(value.indexOf('e')).toFloat();
        qreal exponent = value.mid(value.indexOf('e') + 1).toFloat();
        qreal result = coefficient * qPow(10, exponent);
        return result;
    }
    else
    {
        return value.toFloat();
    }
}
