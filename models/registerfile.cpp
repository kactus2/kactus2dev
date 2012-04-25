/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "registerfile.h"
#include "registermodel.h"
#include "generaldeclarations.h"
#include "register.h"

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
			registerData_.insert(reg->getName(), QSharedPointer<Register>(
				reg));
		}
		else if (tempNode.nodeName() == QString("spirit:registerFile")) {
			RegisterFile* reg = new RegisterFile(tempNode);

			registerData_.insert(reg->getName(),
				QSharedPointer<RegisterFile>(reg));
		}
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

QSharedPointer<RegisterModel> RegisterFile::clone() {
	return QSharedPointer<RegisterModel>(new RegisterFile(*this));
}

void RegisterFile::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:registerFile");

	// write base class information
	RegisterModel::write(writer);

	// if optional spirit:dim is defined
	if (dim_ >= 0) {
		writer.writeTextElement("spirit:dim", QString::number(dim_));
	}

	writer.writeTextElement("spirit:addressOffset", addressOffset_);

	// if optional typeIdentifier is defined
	if (!typeIdentifier_.isEmpty()) {
		writer.writeTextElement("spirit:typeIdentifier", typeIdentifier_);
	}

    writer.writeStartElement("spirit:range");

    General::writeAttributes(writer, rangeAttributes_);
    writer.writeCharacters(QString::number(range_));

    writer.writeEndElement(); // spirit:range

	for (QMap<QString, QSharedPointer<RegisterModel> >::iterator i =
			registerData_.begin(); i != registerData_.end(); ++i) {
		i.value()->write(writer);
	}

	writer.writeEndElement(); // spirit:registerFile
}

bool RegisterFile::isValid( QStringList& errorList,
						   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("register file %1").arg(name_));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for register file"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (addressOffset_.isEmpty()) {
		errorList.append(QObject::tr("No address offset set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (range_ == 0) {
		errorList.append(QObject::tr("No range set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {
		if (!regModel->isValid(errorList, thisIdentifier)) {
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

bool RegisterFile::isValid() const {

	if (name_.isEmpty()) {
		return false;
	}

	if (addressOffset_.isEmpty()) {
		return false;
	}

	if (range_ == 0) {
		return false;
	}

	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {
		if (!regModel->isValid()) {
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
