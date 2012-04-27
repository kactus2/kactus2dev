/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppia
 */

#include "registerdefinition.h"
#include "generaldeclarations.h"
#include "reset.h"
#include "field.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QList>
#include <QDomNode>
#include <QObject>
#include <QSharedPointer>
#include <QMap>

#include <QDebug>

RegisterDefinition::RegisterDefinition(QDomNode& registerDefNode ):
typeIdentifier_(), size_(0),
sizeAttributes_(), volatile_(General::BOOL_UNSPECIFIED),
access_(General::UNSPECIFIED_ACCESS), reset_(), fields_() {

	for (int i = 0; i < registerDefNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerDefNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:typeIdentifier")) {
			typeIdentifier_ = tempNode.childNodes().at(0).nodeValue();
			typeIdentifier_ = General::removeWhiteSpace(typeIdentifier_);
		}
		else if (tempNode.nodeName() == QString("spirit:size")) {
			size_ = tempNode.childNodes().at(0).nodeValue().toInt();
			General::parseAttributes(tempNode, sizeAttributes_);
		}
		else if (tempNode.nodeName() == QString("spirit:volatile")) {
			volatile_ = General::str2BooleanValue(
					tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == QString("spirit:access")) {
			access_ = General::str2Access(
					tempNode.childNodes().at(0).nodeValue(),
					General::UNSPECIFIED_ACCESS);
		}
		else if (tempNode.nodeName() == QString("spirit:reset")) {
			reset_ = QSharedPointer<Reset>(new Reset(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:field")) {

			fields_.append(QSharedPointer<Field>(new Field(tempNode)));
		}
	}

	// if size was not defined
// 	if (size_ == 0) {
// 		throw Parse_error(QObject::tr("Mandatory size not defined in"
// 				" spirit:register"));
// 	}
}

RegisterDefinition::RegisterDefinition( const RegisterDefinition& other ):
typeIdentifier_(other.typeIdentifier_),
size_(other.size_),
sizeAttributes_(other.sizeAttributes_),
volatile_(other.volatile_),
access_(other.access_),
reset_(),
fields_() {

	if (other.reset_) {
		reset_ = QSharedPointer<Reset>(new Reset(*other.reset_.data()));
	}

	foreach (QSharedPointer<Field> field, other.fields_) {
		if (field) {
			QSharedPointer<Field> copy = QSharedPointer<Field>(
				new Field(*field.data()));
			fields_.append(copy);
		}
	}
}

RegisterDefinition& RegisterDefinition::operator=( const RegisterDefinition& other ) {
	if (this != &other) {
		typeIdentifier_ = other.typeIdentifier_;
		size_ = other.size_;
		sizeAttributes_ = other.sizeAttributes_;
		volatile_ = other.volatile_;
		access_ = other.access_;

		if (other.reset_) {
			reset_ = QSharedPointer<Reset>(new Reset(*other.reset_.data()));
		}
		else {
			reset_ = QSharedPointer<Reset>();
		}
		
		fields_.clear();
		foreach (QSharedPointer<Field> field, other.fields_) {
			if (field) {
				QSharedPointer<Field> copy = QSharedPointer<Field>(
					new Field(*field.data()));
				fields_.append(copy);
			}
		}
	}
	return *this;
}

RegisterDefinition::~RegisterDefinition() {
}

void RegisterDefinition::write(QXmlStreamWriter& writer) {
	// if optional typeIdentifier is specified
	if (!typeIdentifier_.isEmpty()) {
		writer.writeTextElement("spirit:typeIdentifier", typeIdentifier_);
	}

    // open the spirit:size tag
    writer.writeStartElement("spirit:size");

    General::writeAttributes(writer, sizeAttributes_);
    writer.writeCharacters(QString::number(size_));

    writer.writeEndElement(); // spirit:size

	// if optional volatile is defined
	if (volatile_ != General::BOOL_UNSPECIFIED) {
		writer.writeTextElement("spirit:volatile",
				General::booleanValue2Str(volatile_));
	}

	// if optional access is specified
	if (access_ != General::UNSPECIFIED_ACCESS) {
		writer.writeTextElement("spirit:access",
				General::access2Str(access_));
	}

	// if optional reset exists
	if (reset_) {
		reset_->write(writer);
	}

	// write all fields
	for (int i = 0; i < fields_.size(); ++i) {
		fields_.at(i)->write(writer);
	}
	return;
}

bool RegisterDefinition::isValid( QStringList& errorList, 
								 const QString& parentIdentifier ) const {

	bool valid = true;

	if (size_ == 0) {
		errorList.append(QObject::tr("No size specified for %1").arg(parentIdentifier));
		valid = false;
	}

	if (reset_ && !reset_->isValid(errorList, parentIdentifier)) {
		valid = false;
	}

	foreach (QSharedPointer<Field> field, fields_) {
		if (!field->isValid(errorList, parentIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool RegisterDefinition::isValid() const {

	if (size_ == 0) {
		return false;
	}

	if (reset_ && !reset_->isValid()) {
		return false;
	}

	foreach (QSharedPointer<Field> field, fields_) {
		if (!field->isValid()) {
			return false;
		}
	}

	return true;
}

General::Access RegisterDefinition::getAccess() const {
	return access_;
}

const QList<QSharedPointer<Field> >& RegisterDefinition::getFields() {
	return fields_;
}

Reset* RegisterDefinition::getReset() const {
	return reset_.data();
}

unsigned int RegisterDefinition::getSize() const {
	return size_;
}

const QMap<QString,QString>& RegisterDefinition::getSizeAttributes() const {
	return sizeAttributes_;
}

QString RegisterDefinition::getTypeIdentifier() const {
	return typeIdentifier_;
}

General::BooleanValue RegisterDefinition::getVolatile() const {
	return volatile_;
}

void RegisterDefinition::setAccess(General::Access access) {
	this->access_ = access;
}

void RegisterDefinition::setFields(
		const QList<QSharedPointer<Field> >& fields) {
	this->fields_ = fields;
}

void RegisterDefinition::setReset(Reset* reset) {
	if (reset_) {
		reset_.clear();
	}
	this->reset_ = QSharedPointer<Reset>(reset);
}

void RegisterDefinition::setSize(unsigned int size) {
	this->size_ = size;
}

void RegisterDefinition::setSizeAttributes(
		const QMap<QString,QString>& sizeAttributes) {
	sizeAttributes_.clear();
	this->sizeAttributes_ = sizeAttributes;
}

void RegisterDefinition::setTypeIdentifier(const QString& typeIdentifier) {
	this->typeIdentifier_ = typeIdentifier;
}

void RegisterDefinition::setVolatile(General::BooleanValue volatileValue) {
	this->volatile_ = volatileValue;
}
