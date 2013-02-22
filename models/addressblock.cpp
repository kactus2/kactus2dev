/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#include "addressblock.h"
#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "parameter.h"
#include "registerfile.h"
#include "register.h"
#include <common/utils.h>

#include <QString>
#include <QMap>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>

AddressBlock::AddressBlock(QDomNode &memoryMapNode):
MemoryMapItem(memoryMapNode),
range_("1"), 
rangeAttributes_(),
width_(-1),
widthAttributes_(),
memoryBlockData_(),
registerData_() {

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
			General::Usage usage = General::str2Usage(tempNode.childNodes().at(0).nodeValue(),
					General::USAGE_COUNT);
			memoryBlockData_.setUsage(usage);
		}

		// get volatile
		else if (tempNode.nodeName() == QString("spirit:volatile")) {
			QString volatileStr = tempNode.childNodes().at(0).nodeValue();
			General::BooleanValue volatileValue = General::str2BooleanValue(volatileStr);
			memoryBlockData_.setVolatile(volatileValue);
		}

		// get access
		else if (tempNode.nodeName() == QString("spirit:access")) {
			General::Access access = General::str2Access(tempNode.childNodes().at(0).
					nodeValue(), General::ACCESS_COUNT);
			memoryBlockData_.setAccess(access);
		}

		// get parameters
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				QDomNode parameterNode = tempNode.childNodes().at(j);
				QSharedPointer<Parameter> temp(new Parameter(parameterNode));
				memoryBlockData_.addParameter(temp);
			}
		}
		else if (tempNode.nodeName() == QString("spirit:register")) {
			registerData_.append(QSharedPointer<Register>(
					new Register(tempNode)));
		}
// 		else if (tempNode.nodeName() == QString("spirit:registerFile")) {
// 			registerData_.append(QSharedPointer<RegisterFile>(
// 					new RegisterFile(tempNode)));
// 		}
	}
}

AddressBlock::AddressBlock():
MemoryMapItem(),
range_("1"), 
rangeAttributes_(),
width_(-1),
widthAttributes_(),
memoryBlockData_(),
registerData_() {

}

AddressBlock::AddressBlock( const AddressBlock &other ):
MemoryMapItem(other),
range_(other.range_), 
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_),
memoryBlockData_(other.memoryBlockData_),
registerData_() {

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
		memoryBlockData_ = other.memoryBlockData_;

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
}

QSharedPointer<MemoryMapItem> AddressBlock::clone() const {
	return QSharedPointer<MemoryMapItem>(new AddressBlock(*this));
}

void AddressBlock::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:addressBlock");

	// call base class write() to write the base class info
	MemoryMapItem::write(writer);

    // start the spirit:range element
    writer.writeStartElement("spirit:range");

    // write the attributes for the element
    General::writeAttributes(writer, rangeAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(range_);
    writer.writeEndElement(); // spirit:range

    // start the spirit:width tag
    writer.writeStartElement("spirit:width");

    // write the attributes for the element
    General::writeAttributes(writer, widthAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(QString::number(width_));
    writer.writeEndElement(); // spirit:width

	memoryBlockData_.write(writer);

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

	if (!memoryBlockData_.isValid(errorList, thisIdentifier)) {
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

	if (!memoryBlockData_.isValid()) {
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
	memoryBlockData_.setAccess(access);
}

General::Usage AddressBlock::getUsage() const {
	return memoryBlockData_.getUsage();
}

void AddressBlock::setUsage(General::Usage usage) {
	memoryBlockData_.setUsage(usage);
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

quint64 AddressBlock::getLastAddress() const {
	quint64 base = Utils::str2Int(baseAddress_);
	quint64 range = Utils::str2Int(range_);

	quint64 lastAddr = base + range;

	// if the base and range are undefined then return 0
	if (lastAddr == 0) {
		return 0;
	}
	
	// if they are defined then return actual last address
	return lastAddr -1;
}

QString AddressBlock::getLastAddressStr() const {
	QString str = QString::number(getLastAddress(), 16);
	str.prepend("0x");
	return str;
}

quint64 AddressBlock::getLastRegisterAddress() const {
	quint64 lastOffset = 0;
	
	// check all register models
	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {
		
		// and find registers 
		QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
		if (reg) {

			// find the largest register offset
			if (lastOffset < reg->getOffset()) {
				lastOffset = reg->getOffset();
			}
		}
	}
	return lastOffset;
}

void AddressBlock::setWidth( int width ) {
	width_ = width;
}

void AddressBlock::setRange(const QString &range) {
	range_ = range;
}

void AddressBlock::setVolatile(const General::BooleanValue volatileSetting) {
	memoryBlockData_.setVolatile(volatileSetting);
}

General::BooleanValue AddressBlock::getVolatile() const {
	return memoryBlockData_.getVolatile();
}

int AddressBlock::getWidth() const {
	return width_;
}

General::Access AddressBlock::getAccess() const {
	return memoryBlockData_.getAccess();
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

	// save the new attributes
	widthAttributes_ = widthAttributes;
}

const QList<QSharedPointer<Parameter> >& AddressBlock::getParameters() {
	return memoryBlockData_.getParameters();
}

void AddressBlock::setParameters(
		QList<QSharedPointer<Parameter> > &parameters) {

	// save the new parameters
	memoryBlockData_.setParameters(parameters);
}

QList<QSharedPointer<RegisterModel> >* AddressBlock::getRegisterPointer() {
	return &registerData_;
}

QList<QSharedPointer<RegisterModel> >& AddressBlock::getRegisterData() {
	return registerData_;
}

const QList<QSharedPointer<RegisterModel> >& AddressBlock::getRegisterData() const {
	return registerData_;
}

bool AddressBlock::uniqueRegisterNames( QStringList& regNames ) const {
	foreach (QSharedPointer<RegisterModel> reg, registerData_) {
		const QString regName = reg->getName();

		// the register name was not unique
		if (regNames.contains(regName)) {
			return false;
		}

		regNames.append(regName);
	}

	return true;
}

void AddressBlock::writeRegisters( QTextStream& stream, 
	quint64 offset, 
	bool useAddrBlockID /*= false*/, 
	const QString& idString /*= QString()*/ ) const {

	// calculate the total offset of the address block
	quint64 baseOffset = Utils::str2Int(baseAddress_);
	baseOffset += offset;

	// if id string is used
	QString id;
	if (!idString.isEmpty()) {
		id = idString;

		// if address block name is added then use separator
		if (useAddrBlockID) {
			id.append("_");
		}
	}
	if (useAddrBlockID) {
		id.append(name_.toUpper());
	}

	stream << "/*" << endl;
	stream << " * Address block: " << name_ << endl;

	if (!description_.isEmpty()) {
		stream << " * Description:" << endl;
		stream << " * " << description_ << endl;
	}

	stream << "*/" << endl;

	foreach (QSharedPointer<RegisterModel> regModel, registerData_) {

		// skip register files at this point
		QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
		if (reg) {
			reg->writeHeaderInfo(stream, baseOffset, id);
		}
	}

	stream << endl;
}
