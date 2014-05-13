/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#include "field.h"
#include "generaldeclarations.h"
#include "parameter.h"
#include "enumeratedvalue.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QMap>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QDomNamedNodeMap>

#include <QDebug>
#include "XmlUtils.h"

Field::Field(QDomNode& fieldNode): 
id_(), 
nameGroup_(fieldNode),
bitOffset_(0), 
typeIdentifier_(),
bitWidth_(0),
bitWidthAttributes_(),
enumeratedValues_(), 
parameters_(),
volatile_(false),
access_(General::ACCESS_COUNT),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
readAction_(General::READ_ACTION_COUNT),
testable_(true),
testConstraint_(General::TEST_UNCONSTRAINED),
writeConstraint_(),
vendorExtensions_()
{

	// parse the spirit:id attribute
	QDomNamedNodeMap attributeMap = fieldNode.attributes();
	id_ = attributeMap.namedItem("spirit:id").nodeValue();
	id_ = General::removeWhiteSpace(id_);

	// go through all nodes and parse them
	for (int i = 0; i < fieldNode.childNodes().count(); ++i) {
		QDomNode tempNode = fieldNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:bitOffset")) {
			bitOffset_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:typeIdentifier")) {
			typeIdentifier_ = tempNode.childNodes().at(0).nodeValue();
			typeIdentifier_ = General::removeWhiteSpace(typeIdentifier_);
		}
		else if (tempNode.nodeName() == QString("spirit:bitWidth")) {
			bitWidth_ = tempNode.childNodes().at(0).nodeValue().toInt();
			General::parseAttributes(tempNode, bitWidthAttributes_);
		}
		else if (tempNode.nodeName() == QString("spirit:enumeratedValues")) {

			// parse each enumerated value
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode enumeratedNode = tempNode.childNodes().at(j);

				enumeratedValues_.append(QSharedPointer<EnumeratedValue>(
						new EnumeratedValue(enumeratedNode)));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// parse each parameter
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);

				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:volatile")) {
			volatile_ = General::str2Bool(tempNode.childNodes().at(0).nodeValue(), false);
		}
		else if (tempNode.nodeName() == QString("spirit:access")) {
			access_ = General::str2Access(tempNode.childNodes().at(0).nodeValue(), General::ACCESS_COUNT);
		}
		else if (tempNode.nodeName() == QString("spirit:modifiedWriteValue")) {
			modifiedWrite_ = General::str2ModifiedWrite(tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == QString("spirit:writeValueConstraint")) {
			writeConstraint_ = QSharedPointer<WriteValueConstraint>(
				new WriteValueConstraint(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:readAction")) {
			readAction_ = General::str2ReadAction(tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == QString("spirit:testable")) {
			testable_ = General::str2Bool(tempNode.childNodes().at(0).nodeValue(), true);
			QDomNamedNodeMap testAttributes = tempNode.attributes();
			QString constraint = testAttributes.namedItem("spirit:testConstraint").nodeValue();
			testConstraint_ = General::str2TestConstraint(constraint);
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                QSharedPointer<VendorExtension> extension = 
                    XmlUtils::createVendorExtensionFromNode(extensionNode); 
                vendorExtensions_.append(extension);
            }
        }
	}
}

Field::Field():
id_(), 
nameGroup_(),
bitOffset_(0), 
typeIdentifier_(),
bitWidth_(0),
bitWidthAttributes_(),
enumeratedValues_(), 
parameters_(),
volatile_(false),
access_(General::ACCESS_COUNT),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
readAction_(General::READ_ACTION_COUNT),
testable_(true),
testConstraint_(General::TEST_UNCONSTRAINED),
writeConstraint_(),
vendorExtensions_()
{

}

Field::Field( General::BooleanValue volatileValue, General::Access access ):
id_(), 
nameGroup_(),
bitOffset_(0), 
typeIdentifier_(),
bitWidth_(0),
bitWidthAttributes_(),
enumeratedValues_(), 
parameters_(),
volatile_(General::BooleanValue2Bool(volatileValue, false)),
access_(access),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
readAction_(General::READ_ACTION_COUNT),
testable_(true),
testConstraint_(General::TEST_UNCONSTRAINED),
writeConstraint_(),
vendorExtensions_()
{

}

Field::Field( const Field& other ):
id_(other.id_),
nameGroup_(other.nameGroup_),
bitOffset_(other.bitOffset_),
typeIdentifier_(other.typeIdentifier_),
bitWidth_(other.bitWidth_),
bitWidthAttributes_(other.bitWidthAttributes_),
enumeratedValues_(),
parameters_(),
volatile_(other.volatile_),
access_(other.access_),
modifiedWrite_(other.modifiedWrite_),
readAction_(other.readAction_),
testable_(other.testable_),
testConstraint_(other.testConstraint_),
writeConstraint_(),
vendorExtensions_(other.vendorExtensions_)
{

	foreach (QSharedPointer<EnumeratedValue> enumValue, other.enumeratedValues_) {
		if (enumValue) {
			QSharedPointer<EnumeratedValue> copy = QSharedPointer<EnumeratedValue>(
			new EnumeratedValue(*enumValue.data()));
			enumeratedValues_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}

	if (other.writeConstraint_) {
		writeConstraint_ = QSharedPointer<WriteValueConstraint>(new WriteValueConstraint(
			*other.writeConstraint_.data()));
	}
}

Field& Field::operator=( const Field& other ) {
	if (this != &other) {
		id_ = other.id_;
		nameGroup_ = other.nameGroup_;
		bitOffset_ = other.bitOffset_;
		typeIdentifier_ = other.typeIdentifier_;
		bitWidth_ = other.bitWidth_;
		bitWidthAttributes_ = other.bitWidthAttributes_;
		volatile_ = other.volatile_;
		access_ = other.access_;
		modifiedWrite_ = other.modifiedWrite_;
		readAction_ = other.readAction_;
		testable_ = other.testable_;
		testConstraint_ = other.testConstraint_;
        vendorExtensions_ = other.vendorExtensions_;

		enumeratedValues_.clear();
		foreach (QSharedPointer<EnumeratedValue> enumValue, other.enumeratedValues_) {
			if (enumValue) {
				QSharedPointer<EnumeratedValue> copy = QSharedPointer<EnumeratedValue>(
					new EnumeratedValue(*enumValue.data()));
				enumeratedValues_.append(copy);
			}
		}

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}

		writeConstraint_ = QSharedPointer<WriteValueConstraint>(
			new WriteValueConstraint(*other.writeConstraint_.data()));
	}
	return *this;
}

Field::~Field() {
	enumeratedValues_.clear();
	parameters_.clear();
	bitWidthAttributes_.clear();
	writeConstraint_.clear();
}

void Field::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:field");

	// if spirit:id attribute is defined
	if (!id_.isEmpty()) {
		writer.writeAttribute("spirit:id", id_);
	}

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	// if optional displayName is defined
	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	// if optional description is defined
	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	writer.writeTextElement("spirit:bitOffset", QString::number(bitOffset_));

	// if optional typeIdentifier is defined
	if (!typeIdentifier_.isEmpty()) {
		writer.writeTextElement("spirit:typeIdentifier", typeIdentifier_);
	}

    writer.writeStartElement("spirit:bitWidth");

    General::writeAttributes(writer, bitWidthAttributes_);
    writer.writeCharacters(QString::number(bitWidth_));

    writer.writeEndElement(); // spirit:bitWidth

	writer.writeTextElement("spirit:volatile", General::bool2Str(volatile_));

	if (access_ != General::ACCESS_COUNT) {
		writer.writeTextElement("spirit:access", General::access2Str(access_));
	}

	if (modifiedWrite_ != General::MODIFIED_WRITE_COUNT) {
		writer.writeTextElement("spirit:modifiedWriteValue", General::modifiedWrite2Str(modifiedWrite_));
	}

	if (writeConstraint_) {
		writeConstraint_->write(writer);
	}

	if (readAction_ != General::READ_ACTION_COUNT) {
		writer.writeTextElement("spirit:readAction", General::readAction2Str(readAction_));
	}


	// start the spirit:testable tag
	writer.writeStartElement("spirit:testable");
	
	// if the testable is true then there might a constrain attached to it
	if (testable_) {
		writer.writeAttribute("spirit:testConstraint", General::testConstraint2Str(testConstraint_));
	}
	// write the value of the element and close the tag
	writer.writeCharacters(General::bool2Str(testable_));
	writer.writeEndElement(); // spirit:testable

	// if optional enumeratedValues exist
	if (enumeratedValues_.size() != 0) {
		writer.writeStartElement("spirit:enumeratedValues");

		for (int i = 0; i < enumeratedValues_.size(); ++i) {
			enumeratedValues_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:enumeratedValues
	}

	// if optional parameters exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:field
}

bool Field::isValid(unsigned int registerSize, QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for a field within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (bitOffset_ < 0) {
		errorList.append(QObject::tr("No bit offset set for field %1 within %2").arg(
			nameGroup_.name_).arg(parentIdentifier));
		valid = false;
	}

	if (bitWidth_ <= 0) {
		errorList.append(QObject::tr("No bit width set for field %1 within %2").arg(
			nameGroup_.name_).arg(parentIdentifier));
		valid = false;
	}

	if ((bitOffset_ + bitWidth_) > registerSize) {
		errorList.append(QObject::tr("The register contains %1 bits but field's MSB bit is %2").arg(
			registerSize).arg(bitOffset_ + bitWidth_ - 1));
		valid = false;
	}

	foreach (QSharedPointer<EnumeratedValue> enumValue, enumeratedValues_) {
		if (!enumValue->isValid(errorList, QObject::tr("field %1").arg(nameGroup_.name_))) {
			valid = false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, QObject::tr("field %1").arg(nameGroup_.name_))) {
			valid = false;
		}
	}

	return valid;
}

bool Field::isValid(unsigned int registerSize) const {
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	if (bitOffset_ < 0) {
		return false;
	}

	if (bitWidth_ <= 0) {
		return false;
	}

	if ((bitOffset_ + bitWidth_) > registerSize) {
		return false;
	}

	foreach (QSharedPointer<EnumeratedValue> enumValue, enumeratedValues_) {
		if (!enumValue->isValid()) {
			return false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid()) {
			return false;
		}
	}

	return true;
}

int Field::getBitOffset() const {
    return bitOffset_;
}

unsigned int Field::getBitWidth() const {
    return bitWidth_;
}

int Field::getMSB() const {
	const int MSB = bitOffset_ + bitWidth_;
	
	// field is at least one bit wide
	if (MSB == 0) {
		return 0;
	}
	
	// the number of bits starts at zero
	return MSB -1;
}

const QMap<QString,QString>& Field::getBitWidthAttributes() const {
    return bitWidthAttributes_;
}

QString Field::getDescription() const {
    return nameGroup_.description_;
}

QString Field::getDisplayName() const {
    return nameGroup_.displayName_;
}

const QList<QSharedPointer<EnumeratedValue> >&
Field::getEnumeratedValues() const {
    return enumeratedValues_;
}

QList<QSharedPointer<EnumeratedValue> >& Field::getEnumeratedValues() {
	return enumeratedValues_;
}

QString Field::getName() const {
    return nameGroup_.name_;
}

const QList<QSharedPointer<Parameter> >& Field::getParameters() const {
    return parameters_;
}

QString Field::getTypeIdentifier() const {
    return typeIdentifier_;
}

void Field::setBitOffset(int bitOffset) {
    bitOffset_ = bitOffset;
}

void Field::setBitWidth(unsigned int bitWidth) {
    bitWidth_ = bitWidth;
}

void Field::setBitWidthAttributes(
		const QMap<QString,QString>& bitWidthAttributes) {
    bitWidthAttributes_ = bitWidthAttributes;
}

void Field::setDescription(const QString& description) {
    nameGroup_.description_ = description;
}

void Field::setDisplayName(const QString& displayName) {
    nameGroup_.displayName_ = displayName;
}

void Field::setEnumeratedValues(
		const QList<QSharedPointer<EnumeratedValue> >& enumeratedValues) {
    enumeratedValues_ = enumeratedValues;
}

void Field::setName(const QString& name) {
    nameGroup_.name_ = name;
}

void Field::setParameters(const QList<QSharedPointer<Parameter> >& parameters) {
	parameters_.clear();
    parameters_ = parameters;
}

void Field::setTypeIdentifier(const QString& typeIdentifier) {
    typeIdentifier_ = typeIdentifier;
}

QString Field::getId() const {
    return id_;
}

void Field::setId(const QString& id) {
    id_ = id;
}

bool Field::getVolatile() const {
	return volatile_;
}

void Field::setVolatile( bool volatileValue ) {
	volatile_ = volatileValue;
}

General::Access Field::getAccess() const {
	return access_;
}

void Field::setAccess( General::Access access ) {
	access_ = access;
}

General::ModifiedWrite Field::getModifiedWrite() const {
	return modifiedWrite_;
}

void Field::setModifiedWrite( const General::ModifiedWrite modWriteValue ) {
	modifiedWrite_ = modWriteValue;
}

General::ReadAction Field::getReadAction() const {
	return readAction_;
}

void Field::setReadAction( const General::ReadAction readAction ) {
	readAction_ = readAction;
}

bool Field::getTestable() const {
	return testable_;
}

void Field::setTestable( bool testable ) {
	testable_ = testable;
}

General::TestConstraint Field::getTestConstraint() const {
	return testConstraint_;
}

void Field::setTestConstraint( const General::TestConstraint testContraint ) {
	testConstraint_ = testContraint;
}

const QSharedPointer<WriteValueConstraint> Field::getWriteConstraint() const {
	return writeConstraint_;
}

QSharedPointer<WriteValueConstraint> Field::getWriteConstraint() {
	if (!writeConstraint_) {
		writeConstraint_ = QSharedPointer<WriteValueConstraint>(new WriteValueConstraint());
	}
	return writeConstraint_;
}
