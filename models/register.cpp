/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "register.h"
#include "generaldeclarations.h"
#include "alternateregister.h"
#include "registermodel.h"
#include <common/utils.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QList>
#include <QSharedPointer>

Register::Register(QDomNode& registerNode):
RegisterModel(registerNode),
dim_(-1),
addressOffset_(),
alternateRegisters_(), 
registerDefinition_(registerNode) {

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:dim")) {
			dim_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();
			addressOffset_ = General::removeWhiteSpace(addressOffset_);
		}
		else if (tempNode.nodeName() == QString("spirit:alternateRegisters")) {

			// parse each alternate register
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode alternateRegNode = tempNode.childNodes().at(j);

				alternateRegisters_.append(QSharedPointer<AlternateRegister>(
						new AlternateRegister(alternateRegNode)));
			}
		}
	}
}

Register::Register():
RegisterModel(),
dim_(-1),
addressOffset_("0x0"),
alternateRegisters_(), 
registerDefinition_() {
}


Register::Register( General::BooleanValue volatileValue, General::Access access ):
RegisterModel(),
dim_(-1),
addressOffset_(),
alternateRegisters_(), 
registerDefinition_() {
	registerDefinition_.setVolatile(volatileValue);
	registerDefinition_.setAccess(access);
}

Register::Register( const Register& other ):
RegisterModel(other),
dim_(other.dim_),
addressOffset_(other.addressOffset_),
alternateRegisters_(),
registerDefinition_(other.registerDefinition_) {

	foreach (QSharedPointer<AlternateRegister> altReg, other.alternateRegisters_) {
		if (altReg) {
			QSharedPointer<AlternateRegister> copy = QSharedPointer<AlternateRegister>(
				new AlternateRegister(*altReg.data()));
			alternateRegisters_.append(copy);
		}
	}
}

Register& Register::operator=( const Register& other ) {
	if (this != &other) {
		RegisterModel::operator=(other);

		dim_ = other.dim_;
		addressOffset_ = other.addressOffset_;
		registerDefinition_ = other.registerDefinition_;

		alternateRegisters_.clear();
		foreach (QSharedPointer<AlternateRegister> altReg, other.alternateRegisters_) {
			if (altReg) {
				QSharedPointer<AlternateRegister> copy = QSharedPointer<AlternateRegister>(
					new AlternateRegister(*altReg.data()));
				alternateRegisters_.append(copy);
			}
		}
	}
	return *this;
}


Register::~Register() {
	alternateRegisters_.clear();
}

QSharedPointer<RegisterModel> Register::clone() {
	return QSharedPointer<RegisterModel>(new Register(*this));
}

void Register::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:register");

	// call base class to write base class info
	RegisterModel::write(writer);

	// if optional spirit:dim is defined
	if (dim_ >= 0) {
		writer.writeTextElement("spirit:dim", QString::number(dim_));
	}

	writer.writeTextElement("spirit:addressOffset", addressOffset_);

	// call registerDefinition to write itself
	registerDefinition_.write(writer);

	// if any alternate registers exist
	if (alternateRegisters_.size() != 0) {
		writer.writeStartElement("spirit:alternateRegisters");

		// write each alternate register
		for (int i = 0; i < alternateRegisters_.size(); ++i) {
			alternateRegisters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:alternateRegisters
	}
	writer.writeEndElement(); // spirit:register
}

bool Register::isValid( QStringList& errorList,
					   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("register %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for register within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty()) {
		errorList.append(QObject::tr("No address offset set for register %1"
			" within %2").arg(nameGroup_.name_).arg(parentIdentifier));
		valid = false;
	}

	if (!registerDefinition_.isValid(errorList, thisIdentifier)) {
			valid = false;
	}

	foreach (QSharedPointer<AlternateRegister> alterRegister, alternateRegisters_) {
		if (!alterRegister->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool Register::isValid() const {

	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	if (addressOffset_.isEmpty()) {
		return false;
	}

	if (!registerDefinition_.isValid()) {
		return false;
	}

	foreach (QSharedPointer<AlternateRegister> alterRegister, alternateRegisters_) {
		if (!alterRegister->isValid()) {
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

QString Register::getAddressOffset() const {
    return addressOffset_;
}

quint64 Register::getOffset() const {
	return Utils::str2Int(addressOffset_);
}

const QList<QSharedPointer<AlternateRegister> >&
Register::getAlternateRegisters() const {
    return alternateRegisters_;
}

int Register::getDim() const {
    return dim_;
}

void Register::setAddressOffset(const QString& addressOffset) {
    this->addressOffset_ = addressOffset;
}

void Register::setAlternateRegisters(
		const QList<QSharedPointer<AlternateRegister> >& alternateRegisters) {
	alternateRegisters_.clear();
    this->alternateRegisters_ = alternateRegisters;
}

void Register::setDim(int dim) {
    this->dim_ = dim;
}

const QList<QSharedPointer<Field> >& Register::getFields() const {
	return registerDefinition_.getFields();
}

QList<QSharedPointer<Field> >& Register::getFields() {
	return registerDefinition_.getFields();
}

unsigned int Register::getWidth() const {
	return registerDefinition_.getSize();
}

QString Register::getTypeIdentifier() const {
	return registerDefinition_.getTypeIdentifier();
}

void Register::setTypeIdentifier( const QString& typeIdentifier ) {
	registerDefinition_.setTypeIdentifier(typeIdentifier);
}

unsigned int Register::getSize() const {
	return registerDefinition_.getSize();
}

void Register::setSize( unsigned int size ) {
	registerDefinition_.setSize(size);
}

General::BooleanValue Register::getVolatile() const {
	return registerDefinition_.getVolatile();
}

void Register::setVolatile( General::BooleanValue volatileValue ) {
	registerDefinition_.setVolatile(volatileValue);
}

General::Access Register::getAccess() const {
	return registerDefinition_.getAccess();
}

void Register::setAccess( General::Access access ) {
	registerDefinition_.setAccess(access);
}

QString Register::getRegisterValue() const {
	return registerDefinition_.getRegisterValue();
}

void Register::setRegisterValue( const QString& registerValue ) {
	registerDefinition_.setRegisterValue(registerValue);
}

QString Register::getRegisterMask() const {
	return registerDefinition_.getRegisterMask();
}

void Register::setRegisterMask( const QString& registerMask ) {
	registerDefinition_.setRegisterMask(registerMask);
}

unsigned int Register::getMSB() const {
	return registerDefinition_.getMSB();
}
