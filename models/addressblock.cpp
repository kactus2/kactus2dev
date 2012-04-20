/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#include "addressblock.h"
#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "parameter.h"
#include "memoryblockdata.h"
#include "registerfile.h"
#include "register.h"


#include "../exceptions/write_error.h"

#include <QString>
#include <QMap>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>

AddressBlock::AddressBlock(QDomNode &memoryMapNode):
MemoryMapItem(memoryMapNode),
range_(), 
rangeAttributes_(),
width_(-1),
widthAttributes_(),
memoryBlockData_(0),
registerData_() {

	// the temporary variables to store the parsed values for memoryBlockData_
	General::Usage usage = General::RESERVED;
	General::BooleanValue volatileValue = General::BOOL_UNSPECIFIED;
	General::Access access = General::UNSPECIFIED_ACCESS;
	QList<QSharedPointer<Parameter> > tempParameters;

	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get range
		if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			// get attributes
			General::parseAttributes(tempNode, rangeAttributes_);
		}

		// get width
		else if (tempNode.nodeName() == QString("spirit:width")) {
			width_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get attributes
			General::parseAttributes(tempNode, widthAttributes_);
		}

		// get usage
		else if (tempNode.nodeName() == QString("spirit:usage")) {
			usage = General::str2Usage(tempNode.childNodes().at(0).nodeValue(),
					General::UNSPECIFIED_USAGE);
		}

		// get volatile
		else if (tempNode.nodeName() == QString("spirit:volatile")) {
			QString volatileStr = tempNode.childNodes().at(0).nodeValue();
			volatileValue = General::str2BooleanValue(volatileStr);
		}

		// get access
		else if (tempNode.nodeName() == QString("spirit:access")) {
			access = General::str2Access(tempNode.childNodes().at(0).
					nodeValue(), General::UNSPECIFIED_ACCESS);
		}

		// get parameters
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				QDomNode parameterNode = tempNode.childNodes().at(j);
				Parameter *temp = new Parameter(parameterNode);
				tempParameters.append(QSharedPointer<Parameter>(temp));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:register")) {
			registerData_.append(QSharedPointer<Register>(
					new Register(tempNode)));
		}
		else if (tempNode.nodeName() == QString("spirit:registerFile")) {
			registerData_.append(QSharedPointer<RegisterFile>(
					new RegisterFile(tempNode)));
		}
	}

	// parsing information is over so save the values to memoryBlockData
	memoryBlockData_ = QSharedPointer<MemoryBlockData>(
			new MemoryBlockData(usage, volatileValue, access, tempParameters));
	tempParameters.clear();

	// if the mandatory elements were not found
// 	if (range_.isNull() || width_ < 1) {
// 		throw Parse_error(QObject::tr(
// 				"AddressBlock: invalid AddressBlock element found"));
// 		return;
// 	}
}

AddressBlock::AddressBlock( const AddressBlock &other ):
MemoryMapItem(other),
range_(other.range_), 
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_),
memoryBlockData_(),
registerData_() {

	if (other.memoryBlockData_) {
		memoryBlockData_ = QSharedPointer<MemoryBlockData>(
			new MemoryBlockData(*other.memoryBlockData_.data()));
	}

	foreach (QSharedPointer<RegisterModel> regModel, other.registerData_) {
		if (regModel) {
			QSharedPointer<RegisterModel> copy = regModel->clone();
			registerData_.append(copy);
		}
	}
}

AddressBlock & AddressBlock::operator=( const AddressBlock &other ) {
	if (this != &other) {
		MemoryMapItem::operator=(other);
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
		width_ = other.width_;
		widthAttributes_ = other.widthAttributes_;

		if (other.memoryBlockData_) {
			memoryBlockData_ = QSharedPointer<MemoryBlockData>(
				new MemoryBlockData(*other.memoryBlockData_.data()));
		}
		else 
			memoryBlockData_ = QSharedPointer<MemoryBlockData>();

		registerData_.clear();
		foreach (QSharedPointer<RegisterModel> regModel, other.registerData_) {
			if (regModel) {
				QSharedPointer<RegisterModel> copy = regModel->clone();
				registerData_.append(copy);
			}
		}
	}
	return *this;
}

AddressBlock::~AddressBlock() {
	memoryBlockData_.clear();
}

QSharedPointer<MemoryMapItem> AddressBlock::clone() const {
	return QSharedPointer<MemoryMapItem>(new AddressBlock(*this));
}

void AddressBlock::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:addressBlock");

	// call base class write() to write the base class info
	MemoryMapItem::write(writer);

	if (range_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element range missing in "
				"spirit:addressBlock"));
	}
	else {
		// start the spirit:range element
		writer.writeStartElement("spirit:range");

		// write the attributes for the element
		General::writeAttributes(writer, rangeAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(range_);
		writer.writeEndElement(); // spirit:range
	}

	if (width_ == 0) {
		throw Write_error(QObject::tr("Mandatory element width missing in "
				"spirit:addressBlock"));
	}
	else {
		// start the spirit:width tag
		writer.writeStartElement("spirit:width");

		// write the attributes for the element
		General::writeAttributes(writer, widthAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(QString::number(width_));
		writer.writeEndElement(); // spirit:width
	}

	// if optional memoryBlockData instance is found
	if (memoryBlockData_) {
		memoryBlockData_->write(writer);
	}

	for (int i = 0; i < registerData_.size(); ++i) {
		registerData_.at(i)->write(writer);
	}

	writer.writeEndElement(); // spirit:addressBlock
}

bool AddressBlock::isValid( QStringList& errorList,
						   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("address block %1").arg(name_));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for address block"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (baseAddress_.isEmpty()) {
		errorList.append(QObject::tr("No base address specified for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (range_.isEmpty()) {
		errorList.append(QObject::tr("No range set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (width_ < 0) {
		errorList.append(QObject::tr("No width set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (memoryBlockData_ && !memoryBlockData_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}

	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {
		if (!regModel->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool AddressBlock::isValid() const {
	
	if (name_.isEmpty()) {
		return false;
	}

	if (baseAddress_.isEmpty()) {
		return false;
	}

	if (range_.isEmpty()) {
		return false;
	}

	if (width_ < 0) {
		return false;
	}

	if (memoryBlockData_ && !memoryBlockData_->isValid()) {
		return false;
	}

	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {
		if (!regModel->isValid()) {
			return false;
		}
	}

	return true;
}

void AddressBlock::setAccess(General::Access access) {
	memoryBlockData_->setAccess(access);
}

General::Usage AddressBlock::getUsage() const {
	return memoryBlockData_->getUsage();
}

void AddressBlock::setUsage(General::Usage usage) {
	memoryBlockData_->setUsage(usage);
}

void AddressBlock::setRangeAttributes(
		const QMap<QString, QString> &rangeAttributes) {

	// first delete old attributes
	rangeAttributes_.clear();

	// save the new attributes
	rangeAttributes_ = rangeAttributes;
}

QString AddressBlock::getRange() const {
	return range_;
}

void AddressBlock::setWidth( int width ) {
	width_ = width;
}

void AddressBlock::setRange(const QString &range) {
	range_ = range;
}

void AddressBlock::setVolatile(const General::BooleanValue volatileSetting) {
	memoryBlockData_->setVolatile(volatileSetting);
}

General::BooleanValue AddressBlock::getVolatile() const {
	return memoryBlockData_->getVolatile();
}

int AddressBlock::getWidth() const {
	return width_;
}

General::Access AddressBlock::getAccess() const {
	return memoryBlockData_->getAccess();
}

const QMap<QString, QString>& AddressBlock::getRangeAttributes(){
	return rangeAttributes_;
}

const QMap<QString, QString>& AddressBlock::getWidthAttributes(){
	return widthAttributes_;
}

void AddressBlock::setWidthAttributes(
		const QMap<QString, QString> &widthAttributes) {

	// first delete the old attributes
	widthAttributes_.clear();

	// sace the new attributes
	widthAttributes_ = widthAttributes;
}

const QList<QSharedPointer<Parameter> >& AddressBlock::getParameters() {
	return memoryBlockData_->getParameters();
}

void AddressBlock::setParameters(
		QList<QSharedPointer<Parameter> > &parameters) {

	// save the new parameters
	memoryBlockData_->setParameters(parameters);
}

QList<QSharedPointer<RegisterModel> >* AddressBlock::getRegisterPointer() {
	return &registerData_;
}
