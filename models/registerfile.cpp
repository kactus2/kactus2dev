/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "registerfile.h"
#include "registermodel.h"
#include "generaldeclarations.h"
#include "register.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMap>
#include <QSharedPointer>

RegisterFile::RegisterFile(QDomNode& registerNode): RegisterModel(registerNode),
dim_(-1), addressOffset_(), typeIdentifier_(), range_(0), rangeAttributes_(),
registerData_() {

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:dim")) {
			dim_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();
			addressOffset_ = General::removeWhiteSpace(addressOffset_);
		}
		else if (tempNode.nodeName() == QString("spirit:typeIdentifier")) {
			typeIdentifier_ = tempNode.childNodes().at(0).nodeValue();
			typeIdentifier_ = General::removeWhiteSpace(typeIdentifier_);
		}
		else if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue().toInt();
			General::parseAttributes(tempNode, rangeAttributes_);
		}
		else if (tempNode.nodeName() == QString("spirit:register")) {
			Register* reg = new Register(tempNode);

			// if the map already contains register with same name
			if (registerData_.contains(reg->getName())) {
				throw Parse_error(QObject::tr("Multiple registers with same"
						" name in spirit:registerFile"));
			}
			// if name was available then add the register to map
			else {
				registerData_.insert(reg->getName(), QSharedPointer<Register>(
						reg));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:registerFile")) {
			RegisterFile* reg = new RegisterFile(tempNode);

			// if the map already contains register with same name
			if (registerData_.contains(reg->getName())) {
				throw Parse_error(QObject::tr("Multiple registerFiles with same"
						" name in spirit:registerFile"));
			}
			// if name was available then add the register to map
			else {
				registerData_.insert(reg->getName(),
						QSharedPointer<RegisterFile>(reg));
			}
		}
	}

	// if mandatory addressOffset is not defined
	if (addressOffset_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory addressOffset missing in "
				"spirit:registerFile"));
	}

	// if mandatory range is missing
	if (range_ == 0) {
		throw Parse_error(QObject::tr("Mandatory range missing in spirit:"
				"registerFile"));
	}
	return;
}

RegisterFile::RegisterFile( const RegisterFile& other ):
RegisterModel(other),
dim_(other.dim_),
addressOffset_(other.addressOffset_),
typeIdentifier_(other.typeIdentifier_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
registerData_() {

	for (QMap<QString, QSharedPointer<RegisterModel> >::const_iterator i = 
             other.registerData_.begin();
		i != other.registerData_.end(); ++i) {
			if (i.value()) {
				Register* reg = dynamic_cast<Register*>(i.value().data());
				RegisterFile* regFile = dynamic_cast<RegisterFile*>(i.value().data());
				QSharedPointer<RegisterModel> copy;

				if (reg) {
					copy = QSharedPointer<RegisterModel>(
						new Register(*reg));
				}
				else if (regFile) {
					copy = QSharedPointer<RegisterModel>(
						new RegisterFile(*regFile));
				}

				if (copy)
					registerData_.insert(copy->getName(), copy);
				
			}
	}
}


RegisterFile& RegisterFile::operator=( const RegisterFile& other ) {
	if (this != &other) {
		RegisterModel::operator=(other);

		dim_ = other.dim_;
		addressOffset_ = other.addressOffset_;
		typeIdentifier_ = other.typeIdentifier_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;

		registerData_.clear();
		for (QMap<QString, QSharedPointer<RegisterModel> >::const_iterator i = 
                 other.registerData_.begin();
			i != other.registerData_.end(); ++i) {
				if (i.value()) {
					Register* reg = dynamic_cast<Register*>(i.value().data());
					RegisterFile* regFile = dynamic_cast<RegisterFile*>(i.value().data());
					QSharedPointer<RegisterModel> copy;

					if (reg) {
						copy = QSharedPointer<RegisterModel>(
							new Register(*reg));
					}
					else if (regFile) {
						copy = QSharedPointer<RegisterModel>(
							new RegisterFile(*regFile));
					}

					if (copy)
						registerData_.insert(copy->getName(), copy);

				}
		}
	}
	return *this;
}


RegisterFile::~RegisterFile() {
}

void RegisterFile::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:registerFile");

	// write base class information
	RegisterModel::write(writer);

	// if optional spirit:dim is defined
	if (dim_ >= 0) {
		writer.writeTextElement("spirit:dim", QString::number(dim_));
	}

	// if mandatory addressOffset is not defined
	if (addressOffset_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory addressOffset missing in "
				"spirit:registerFile"));
	}
	else {
		writer.writeTextElement("spirit:addressOffset", addressOffset_);
	}

	// if optional typeIdentifier is defined
	if (!typeIdentifier_.isEmpty()) {
		writer.writeTextElement("spirit:typeIdentifier", typeIdentifier_);
	}

	// if mandatory range is not defined
	if (range_ <= 0) {
		throw Write_error(QObject::tr("Mandatory range missing in spirit:"
				"registerFile"));
	}
	else {
		writer.writeStartElement("spirit:range");

		General::writeAttributes(writer, rangeAttributes_);
		writer.writeCharacters(QString::number(range_));

		writer.writeEndElement(); // spirit:range
	}

	for (QMap<QString, QSharedPointer<RegisterModel> >::iterator i =
			registerData_.begin(); i != registerData_.end(); ++i) {
		i.value()->write(writer);
	}

	writer.writeEndElement(); // spirit:registerFile
}

QString RegisterFile::getAddressOffset() const {
	return addressOffset_;
}

int RegisterFile::getDim() const {
	return dim_;
}

unsigned int RegisterFile::getRange() const {
	return range_;
}

const QMap<QString,QString>& RegisterFile::getRangeAttributes() const {
	return rangeAttributes_;
}

const QMap<QString, QSharedPointer<RegisterModel> >&
RegisterFile::getRegisterData() const {
	return registerData_;
}

QString RegisterFile::getTypeIdentifier() const {
	return typeIdentifier_;
}

void RegisterFile::setAddressOffset(const QString& addressOffset) {
	this->addressOffset_ = addressOffset;
}

void RegisterFile::setDim(int dim) {
	this->dim_ = dim;
}

void RegisterFile::setRange(unsigned int range) {
	this->range_ = range;
}

void RegisterFile::setRangeAttributes(
		const QMap<QString,QString>& rangeAttributes) {
	this->rangeAttributes_ = rangeAttributes;
}

void RegisterFile::setRegisterData(
		const QMap<QString, QSharedPointer<RegisterModel> >& registerData) {
	this->registerData_ = registerData;
}

void RegisterFile::setTypeIdentifier(const QString& typeIdentifier) {
	this->typeIdentifier_ = typeIdentifier;
}
