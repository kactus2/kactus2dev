/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#include "modelparameter.h"

#include "GenericVendorExtension.h"

#include <QString>
#include <QDomNode>
#include <QMap>
#include <QObject>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

ModelParameter::ModelParameter(QDomNode &modelParameterNode): 
nameGroup_(modelParameterNode),
value_(QString()), 
attributes_(),
valueAttributes_(),
vendorExtensions_()
{

	// get the modelParameter attributes
	attributes_ = XmlUtils::parseAttributes(modelParameterNode);

	for (int i = 0; i < modelParameterNode.childNodes().count(); ++i) {
		QDomNode tempNode = modelParameterNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();

			// get the value attributes that define the value
			valueAttributes_ = XmlUtils::parseAttributes(tempNode);
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

ModelParameter::ModelParameter():
nameGroup_(),
value_(),
attributes_(),
valueAttributes_(),
vendorExtensions_()
{
}

// the copy constructor
ModelParameter::ModelParameter(const ModelParameter &other ):
nameGroup_(other.nameGroup_), 
value_(other.value_), 
attributes_(other.attributes_),
valueAttributes_(other.valueAttributes_),
vendorExtensions_(other.vendorExtensions_)
{
}

ModelParameter & ModelParameter::operator=( const ModelParameter &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		value_ = other.value_;
		attributes_ = other.attributes_;
		valueAttributes_ = other.valueAttributes_;
        vendorExtensions_ = other.vendorExtensions_;
	}
	return *this;
}

ModelParameter::~ModelParameter() {
	attributes_.clear();
	valueAttributes_.clear();
	return;
}

void ModelParameter::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:modelParameter");
	XmlUtils::writeAttributes(writer, attributes_);

	writer.writeTextElement("spirit:name", nameGroup_.name());

	if (!nameGroup_.displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName());
	}

	if (!nameGroup_.description().isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description());
	}

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

	writer.writeEndElement(); // spirit:modelParameter
}

bool ModelParameter::isValid() const {

	// if usage type is specified
	if (attributes_.contains(QString("spirit:usageType"))) {

		// if usage is not valid type
		QString usage = attributes_.value(QString("spirit:usageType"));
		if (usage != QString("nontyped") && usage != QString("typed"))
			return false;
	}

	// name and value must be defined
	if (nameGroup_.name().isEmpty() || value_.isEmpty())
		return false;

	return true;
}

bool ModelParameter::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (nameGroup_.name().isEmpty()) {
		valid = false;

		errorList.append(QObject::tr("No name set for model parameter within %1").arg(
			parentIdentifier));
	}

	if (attributes_.contains(QString("spirit:usageType"))) {
		// if usage is not valid type
		QString usage = attributes_.value(QString("spirit:usageType"));
		if (usage != QString("nontyped") && usage != QString("typed")) {
			valid = false;

			errorList.append(QObject::tr("Invalid usage type set for model "
				"parameter %1 within %2").arg(
				nameGroup_.name()).arg(parentIdentifier));
		}
	}

	if (value_.isEmpty()) {
		valid = false;

		errorList.append(QObject::tr("No value set for model parameter %1"
			" within %2").arg(nameGroup_.name()).arg(parentIdentifier));
	}

	return valid;
}

const QMap<QString, QString>& ModelParameter::getAttributes() {
	return attributes_;
}

void ModelParameter::setAttributes(const QMap<QString, QString> &attributes) {
	// delete the old attributes
	attributes_.clear();

	// save the new attributes
	attributes_ = attributes;
}

void ModelParameter::setValue(const QString &value) {
	value_ = value;
}

const QMap<QString, QString>& ModelParameter::getValueAttributes() {
	return valueAttributes_;
}

void ModelParameter::setValueAttributes(const
		QMap<QString, QString> &valueAttributes) {
	// delete the old value attributes
	valueAttributes_.clear();

	// save the new value attributes
	valueAttributes_ = valueAttributes;
}

QString ModelParameter::getName() const {
	return nameGroup_.name();
}

void ModelParameter::setName(const QString &name) {
	nameGroup_.setName(name);
}

QString ModelParameter::getDisplayName() const {
	return nameGroup_.displayName();
}

void ModelParameter::setDisplayName( const QString& displayName ) {
	nameGroup_.setDisplayName(displayName);
}

QString ModelParameter::getDescription() const {
	return nameGroup_.description();
}

void ModelParameter::setDescription( const QString& description ) {
	nameGroup_.setDescription(description);
}

QString ModelParameter::getValue() const {
	return value_;
}

QString ModelParameter::getDataType() const {
	return attributes_.value(QString("spirit:dataType"));
}

void ModelParameter::setDataType( const QString& dataType ) {
	attributes_.insert(QString("spirit:dataType"), dataType);
}

QString ModelParameter::getUsageType() const {
	return attributes_.value(QString("spirit:usageType"));
}

void ModelParameter::setUsageType( const QString& usageType ) {
	attributes_.insert(QString("spirit:usageType"), usageType);
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::getChoiceRef()
//-----------------------------------------------------------------------------
QString ModelParameter::getChoiceRef() const
{
    return valueAttributes_.value("spirit:choiceRef");
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::setChoiceRef()
//-----------------------------------------------------------------------------
void ModelParameter::setChoiceRef(QString const& choiceRef)
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
