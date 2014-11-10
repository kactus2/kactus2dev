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
    writer.writeStartElement(elementName());

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
// Function: Parameter::setName()
//-----------------------------------------------------------------------------
void Parameter::setName(QString const& name)
{
	nameGroup_.setName(name);
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
// Function: Parameter::getValue()
//-----------------------------------------------------------------------------
QString Parameter::getValue() const
{
	return value_;
}

//-----------------------------------------------------------------------------
// Function: Parameter::getDescription()
//-----------------------------------------------------------------------------
QString Parameter::getDescription() const 
{
	return nameGroup_.description();
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
		return false;

	else if (value_.isEmpty())
		return false;

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
		errorList.append(QObject::tr("No name specified for parameter within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (value_.isEmpty())
    {
		errorList.append(QObject::tr("No value specified for parameter %1 within %2").arg(
			getName()).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: Parameter::elementName()
//-----------------------------------------------------------------------------
QString Parameter::elementName() const
{
    return "spirit:parameter";
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
