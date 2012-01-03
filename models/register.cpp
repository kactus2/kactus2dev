/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "register.h"
#include "generaldeclarations.h"
#include "registerdefinition.h"
#include "alternateregister.h"
#include "registermodel.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QList>
#include <QSharedPointer>

Register::Register(QDomNode& registerNode): RegisterModel(registerNode),
dim_(-1), addressOffset_(), alternateRegisters_(), registerDefinition_() {

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

	registerDefinition_ = QSharedPointer<RegisterDefinition>(
			new RegisterDefinition(registerNode));

	// if mandatory addressOffset is missing
// 	if (addressOffset_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory addressOffset missing in"
// 				" spirit:register"));
// 	}
	return;
}

Register::Register( const Register& other ):
RegisterModel(other),
dim_(other.dim_),
addressOffset_(other.addressOffset_),
alternateRegisters_(),
registerDefinition_() {

	foreach (QSharedPointer<AlternateRegister> altReg, other.alternateRegisters_) {
		if (altReg) {
			QSharedPointer<AlternateRegister> copy = QSharedPointer<AlternateRegister>(
				new AlternateRegister(*altReg.data()));
			alternateRegisters_.append(copy);
		}
	}

	if (other.registerDefinition_) {
		registerDefinition_ = QSharedPointer<RegisterDefinition>(
			new RegisterDefinition(*other.registerDefinition_));
	}
}


Register& Register::operator=( const Register& other ) {
	if (this != &other) {
		RegisterModel::operator=(other);

		dim_ = other.dim_;
		addressOffset_ = other.addressOffset_;

		alternateRegisters_.clear();
		foreach (QSharedPointer<AlternateRegister> altReg, other.alternateRegisters_) {
			if (altReg) {
				QSharedPointer<AlternateRegister> copy = QSharedPointer<AlternateRegister>(
					new AlternateRegister(*altReg.data()));
				alternateRegisters_.append(copy);
			}
		}

		if (other.registerDefinition_) {
			registerDefinition_ = QSharedPointer<RegisterDefinition>(
				new RegisterDefinition(*other.registerDefinition_));
		}
		else
			registerDefinition_ = QSharedPointer<RegisterDefinition>();
	}
	return *this;
}


Register::~Register() {
	alternateRegisters_.clear();
	registerDefinition_.clear();
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

	// if mandatory addressOffset is not defined
	if (addressOffset_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory addressOffset missing in "
				"spirit:register"));
	}
	else {
		writer.writeTextElement("spirit:addressOffset", addressOffset_);
	}

	// call registerDefinition to write itself
	if (registerDefinition_) {
		registerDefinition_->write(writer);
	}

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
	const QString thisIdentifier(QObject::tr("register %1").arg(name_));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for register within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty()) {
		errorList.append(QObject::tr("No address offset set for register %1"
			" within %2").arg(name_).arg(parentIdentifier));
		valid = false;
	}

	if (registerDefinition_ && !registerDefinition_->isValid(errorList,
		thisIdentifier)) {
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

QString Register::getAddressOffset() const {
    return addressOffset_;
}

const QList<QSharedPointer<AlternateRegister> >&
Register::getAlternateRegisters() const {
    return alternateRegisters_;
}

int Register::getDim() const {
    return dim_;
}

RegisterDefinition* Register::getRegisterDefinition() const {
    return registerDefinition_.data();
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

void Register::setRegisterDefinition(RegisterDefinition* registerDefinition) {
	registerDefinition_.clear();
    this->registerDefinition_ =
    		QSharedPointer<RegisterDefinition>(registerDefinition);
}
