/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#include "addressblock.h"
#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "GenericVendorExtension.h"
#include "parameter.h"
#include "registerfile.h"
#include "register.h"
#include "XmlUtils.h"

#include <common/utils.h>

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

AddressBlock::AddressBlock(QDomNode &memoryMapNode):
MemoryMapItem(memoryMapNode),
range_("1"), 
rangeAttributes_(),
width_(-1),
widthAttributes_(),
memoryBlockData_(),
registerData_(),
vendorExtensions_()
{

	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get range
		if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			// get attributes
			rangeAttributes_ = XmlUtils::parseAttributes(tempNode);
		}

		// get width
		else if (tempNode.nodeName() == QString("spirit:width")) {
			width_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get attributes
			widthAttributes_ = XmlUtils::parseAttributes(tempNode);
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

AddressBlock::AddressBlock():
MemoryMapItem(),
range_("1"), 
rangeAttributes_(),
width_(-1),
widthAttributes_(),
memoryBlockData_(),
registerData_(),
vendorExtensions_()
{

}

AddressBlock::AddressBlock( const AddressBlock &other ):
MemoryMapItem(other),
range_(other.range_), 
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_),
memoryBlockData_(other.memoryBlockData_),
registerData_(),
vendorExtensions_(other.vendorExtensions_)
{

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
        vendorExtensions_ = other.vendorExtensions_;

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
    XmlUtils::writeAttributes(writer, rangeAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(range_);
    writer.writeEndElement(); // spirit:range

    // start the spirit:width tag
    writer.writeStartElement("spirit:width");

    // write the attributes for the element
    XmlUtils::writeAttributes(writer, widthAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(QString::number(width_));
    writer.writeEndElement(); // spirit:width

	memoryBlockData_.write(writer);

	for (int i = 0; i < registerData_.size(); ++i) {
		registerData_.at(i)->write(writer);
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:addressBlock
}

//-----------------------------------------------------------------------------
// Function: addressblock::isValid()
//-----------------------------------------------------------------------------
bool AddressBlock::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier ) const
{
	bool valid = true;
    const QString thisIdentifier(QObject::tr("address block %1").arg(getName()));

    if (getName().isEmpty())
    {
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

	if (!memoryBlockData_.isValid(componentChoices, errorList, thisIdentifier))
    {
		valid = false;
	}

    if (!registerData_.empty() &&
        (memoryBlockData_.getUsage() == General::RESERVED || memoryBlockData_.getUsage() == General::MEMORY))
    {
        errorList.append(QObject::tr("%1 with usage %2 must not contain registers").arg(
            thisIdentifier).arg(General::usage2Str(memoryBlockData_.getUsage())));
        valid = false;
    }

    QStringList registerNames;
	foreach (QSharedPointer<RegisterModel> regModel, registerData_)
    {
        if (registerNames.contains(regModel->getName()))
        {
            errorList.append(QObject::tr("%1 contains multiple registers with name %2").arg(
                thisIdentifier, regModel->getName()));
            valid = false;
        }
        else
        {
            registerNames.append(regModel->getName());
        }

		if (!regModel->isValid(componentChoices, errorList, thisIdentifier))
        {
			valid = false;
		}
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: addressblock::isValid()
//-----------------------------------------------------------------------------
bool AddressBlock::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
    if (getName().isEmpty())
    {
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

	if (!memoryBlockData_.isValid(componentChoices))
    {
		return false;
	}

    if (!registerData_.empty() &&
        (memoryBlockData_.getUsage() == General::RESERVED || memoryBlockData_.getUsage() == General::MEMORY))
    {
        return false;
    }

    QStringList registerNames;
	foreach (QSharedPointer<RegisterModel> regModel, registerData_)
    {
        if (registerNames.contains(regModel->getName()))
        {
            return false;
        }
        else
        {
            registerNames.append(regModel->getName());
        }

		if (!regModel->isValid(componentChoices))
        {
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
	quint64 base = General::str2Uint(baseAddress_);
	quint64 range = General::str2Uint(range_);

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

//-----------------------------------------------------------------------------
// Function: addressblock::getAllRegisterOffsets()
//-----------------------------------------------------------------------------
QStringList AddressBlock::getAllRegisterOffsets()
{
    QStringList registerOffsets;

    foreach (QSharedPointer<RegisterModel> regModel, registerData_)
    {
        QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
        if (reg)
        {
            registerOffsets.append(reg->getAddressOffset());
        }
    }

    return registerOffsets;
}

//-----------------------------------------------------------------------------
// Function: addressblock::getLastRegisterDimension()
//-----------------------------------------------------------------------------
int AddressBlock::getLastRegisterDimension(int lastRegisterIndex)
{
    QSharedPointer<RegisterModel> regModel = registerData_.at(lastRegisterIndex);
    QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
    return reg->getDim();
}

//-----------------------------------------------------------------------------
// Function: addressblock::getLastRegisterSize()
//-----------------------------------------------------------------------------
int AddressBlock::getLastRegisterSize(int lastRegisterIndex)
{
    QSharedPointer<RegisterModel> regModel = registerData_.at(lastRegisterIndex);
    QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
    return reg->getSize();
}

void AddressBlock::setWidth( int width ) {
	width_ = width;
}

//-----------------------------------------------------------------------------
// Function: addressblock::setWidthExpression()
//-----------------------------------------------------------------------------
void AddressBlock::setWidthExpression(QString const& widthExpression)
{
    widthAttributes_.insert("kactus2:widthExpression", widthExpression);
}

//-----------------------------------------------------------------------------
// Function: addressblock::getWidthExpression()
//-----------------------------------------------------------------------------
QString AddressBlock::getWidthExpression()
{
    if (hasWidthExpression())
    {
        return widthAttributes_.value("kactus2:widthExpression");
    }
    else
    {
        return QString::number(getWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: addressblock::hasWidthExpression()
//-----------------------------------------------------------------------------
bool AddressBlock::hasWidthExpression()
{
    return widthAttributes_.contains("kactus2:widthExpression");
}

//-----------------------------------------------------------------------------
// Function: addressblock::removeWidthExpression()
//-----------------------------------------------------------------------------
void AddressBlock::removeWidthExpression()
{
    widthAttributes_.remove("kactus2:widthExpression");
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

//-----------------------------------------------------------------------------
// Function: addressblock::writeRegisters()
//-----------------------------------------------------------------------------
void AddressBlock::writeRegisters( QTextStream& stream, quint64 offset, bool useAddrBlockID /*= false*/, 
    const QString& idString /*= QString()*/ ) const
{
	// calculate the total offset of the address block
	quint64 baseOffset = General::str2Uint(baseAddress_);
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
	if (useAddrBlockID)
    {
        id.append(getName().toUpper());
	}

	stream << "/*" << endl;
    stream << " * Address block: " << getName() << endl;

    if (!getDescription().isEmpty())
    {
		stream << " * Description:" << endl;
        stream << " * " << getDescription() << endl;
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
