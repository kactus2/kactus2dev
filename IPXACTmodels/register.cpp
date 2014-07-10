/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "register.h"
#include "generaldeclarations.h"
#include "alternateregister.h"
#include "registermodel.h"
#include "GenericVendorExtension.h"

#include <common/utils.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include "XmlUtils.h"

Register::Register(QDomNode& registerNode):
RegisterModel(registerNode),
dim_(-1),
addressOffset_(),
alternateRegisters_(), 
registerDefinition_(registerNode),
vendorExtensions_()
{

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:dim")) {
			dim_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();
			addressOffset_ = XmlUtils::removeWhiteSpace(addressOffset_);
		}
		else if (tempNode.nodeName() == QString("spirit:alternateRegisters")) {

			// parse each alternate register
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode alternateRegNode = tempNode.childNodes().at(j);

				alternateRegisters_.append(QSharedPointer<AlternateRegister>(
						new AlternateRegister(alternateRegNode)));
			}
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

Register::Register():
RegisterModel(),
dim_(-1),
addressOffset_("0x0"),
alternateRegisters_(), 
registerDefinition_(),
vendorExtensions_()
{
}


Register::Register( General::BooleanValue volatileValue, General::Access access ):
RegisterModel(),
dim_(-1),
addressOffset_(),
alternateRegisters_(), 
registerDefinition_(),
vendorExtensions_()
{
	registerDefinition_.setVolatile(volatileValue);
	registerDefinition_.setAccess(access);
}

Register::Register( const Register& other ):
RegisterModel(other),
dim_(other.dim_),
addressOffset_(other.addressOffset_),
alternateRegisters_(),
registerDefinition_(other.registerDefinition_),
vendorExtensions_(other.vendorExtensions_)
{

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
        vendorExtensions_ = other.vendorExtensions_;

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

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:register
}

bool Register::isValid( QStringList& errorList,
					   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("register %1").arg(nameGroup_.name()));

	if (nameGroup_.name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for register within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty()) {
		errorList.append(QObject::tr("No address offset set for register %1"
			" within %2").arg(nameGroup_.name()).arg(parentIdentifier));
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

	if (nameGroup_.name().isEmpty()) {
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
	return Utils::str2Uint(addressOffset_);
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

void Register::clearReset() {
	registerDefinition_.clearReset();
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

void Register::writeHeaderInfo( QTextStream& stream, quint64 offset, const QString& idString /*= QString()*/ ) const {

	// calculate the total address of the register
	quint64 regOffset = Utils::str2Uint(addressOffset_);
	regOffset += offset;

	QString offsetStr = QString::number(regOffset, 16);
	offsetStr.prepend("0x");

	stream << "/*" << endl;
	stream << " * Register name: " << nameGroup_.name() << endl;
	
	if (!nameGroup_.description().isEmpty()) {
		stream << " * Description:" << endl;
		stream << " * " << nameGroup_.description() << endl;
	}

	stream << "*/" << endl;
	stream << "#define ";

	if (idString.isEmpty()) {
		stream << nameGroup_.name().toUpper() << " " << offsetStr << endl; 
	}
	else {
		stream << idString.toUpper() << "_" << nameGroup_.name().toUpper() << " " << offsetStr << endl;
	}
}
