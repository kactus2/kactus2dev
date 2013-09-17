/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymap.h"
#include "generaldeclarations.h"
#include "memorymapitem.h"
#include "subspacemap.h"
#include "addressblock.h"
#include "bank.h"

#include <common/utils.h>

#include <QString>
#include <QDomNode>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

MemoryMap::MemoryMap(QDomNode &memoryMapNode): 
nameGroup_(memoryMapNode),
id_(), 
items_(),
addressUnitBits_(0) {
	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNamedNodeMap attributeMap = memoryMapNode.attributes();
		id_ = attributeMap.namedItem("spirit:id").nodeValue();
		id_ = General::removeWhiteSpace(id_);

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get addressUnitBits
		if (tempNode.nodeName() == QString("spirit:addressUnitBits")) {
			addressUnitBits_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}

		// get SubSpaceMap
// 		else if (tempNode.nodeName() == QString("spirit:subspaceMap")) {
// 			SubspaceMap *temp = new SubspaceMap(tempNode);
// 			items_.append(QSharedPointer<SubspaceMap>(temp));
// 		}

		// get Bank
// 		else if (tempNode.nodeName() == QString("spirit:bank")) {
// 			Bank *temp = new Bank(tempNode);
// 			items_.append(QSharedPointer<Bank>(temp));
// 		}

		// get addressBlock
		else if (tempNode.nodeName() == QString("spirit:addressBlock")) {
			AddressBlock *temp = new AddressBlock(tempNode);
			items_.append(QSharedPointer<AddressBlock>(temp));
		}
	}
	return;
}

MemoryMap::MemoryMap():
nameGroup_(),
id_(), 
items_(),
addressUnitBits_(0) {
}

MemoryMap::MemoryMap( const MemoryMap &other ):
nameGroup_(other.nameGroup_),
id_(other.id_),
items_(),
addressUnitBits_(other.addressUnitBits_) {

	foreach (QSharedPointer<MemoryMapItem> item, other.items_) {
		if (item) {
			QSharedPointer<MemoryMapItem> copy = item->clone();
			items_.append(copy);
		}
	}
}

MemoryMap & MemoryMap::operator=( const MemoryMap &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		id_ = other.id_;
		addressUnitBits_ = other.addressUnitBits_;
		
		items_.clear();
		foreach (QSharedPointer<MemoryMapItem> item, other.items_) {
			if (item) {
				QSharedPointer<MemoryMapItem> copy = item->clone();
				items_.append(copy);
			}
		}
	}
	return *this;
}

MemoryMap::~MemoryMap() {
	items_.clear();
}

void MemoryMap::write(QXmlStreamWriter& writer) {
	// spirit:memoryMap can't be written here because this function
	// is used also as spirit:localMemoryMap

	// if optional attribute id is defined
	if (!id_.isEmpty()) {
		writer.writeAttribute("spirit:id", id_);
	}

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	// write all address block, bank and subspaceMap elements
	for (int i = 0; i < items_.size(); ++i) {
		items_.at(i)->write(writer);
	}

	if (addressUnitBits_ != 0) {
		writer.writeTextElement("spirit:addressUnitBits",
				QString::number(addressUnitBits_));
	}

}

bool MemoryMap::isValid( QStringList& errorList, 
						const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("memory map %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for memory map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	QStringList memItemNames;
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		if (memItemNames.contains(memItem->getName())) {
			errorList.append(QObject::tr("%1 contains several memory map items"
				" with name %2").arg(thisIdentifier).arg(memItem->getName()));
			valid = false;
		}
		else {
			memItemNames.append(memItem->getName());
		}

		if (!memItem->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool MemoryMap::isValid() const {

	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	QStringList memItemNames;
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		if (memItemNames.contains(memItem->getName())) {
			return false;
		}
		else {
			memItemNames.append(memItem->getName());
		}

		if (!memItem->isValid()) {
			return false;
		}
	}

	return true;
}

bool MemoryMap::containsSubItems() const {
	return !items_.isEmpty();
}

void MemoryMap::setName(const QString &name) {
	nameGroup_.name_ = name;
}

void MemoryMap::setAddressUnitBits(unsigned int addressUnitBits) {
	addressUnitBits_ = addressUnitBits;
}

const QList<QSharedPointer<MemoryMapItem> >& MemoryMap::getItems() const {
	return items_;
}

QList<QSharedPointer<MemoryMapItem> >& MemoryMap::getItems() {
	return items_;
}	

QString MemoryMap::getName() const {
	return nameGroup_.name_;
}

unsigned int MemoryMap::getAddressUnitBits() const {
	return addressUnitBits_;
}

void MemoryMap::setItems(const QList<QSharedPointer<MemoryMapItem> > &items) {
	// first delete the old items
	items_.clear();

	// save the new items
	items_ = items;
}

QString MemoryMap::getDisplayName() const {
	return nameGroup_.displayName_;
}

void MemoryMap::setDisplayName( const QString& displayName ) {
	nameGroup_.displayName_ = displayName;
}

QString MemoryMap::getDescription() const {
	return nameGroup_.description_;
}

void MemoryMap::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryMap::getLastAddress() const {
    int index = -1;
    unsigned int lastBaseAddress = 0;

    for (int i = 0; i < items_.size(); ++i)
    {
        AddressBlock* block = dynamic_cast<AddressBlock*>(items_.at(i).data());

        if (block != 0)
        {
            bool ok = true;
            
            unsigned int baseAddress = Utils::str2Uint(block->getBaseAddress());

            if (ok)
            {
                if (baseAddress >= lastBaseAddress)
                {
                    lastBaseAddress = baseAddress;
                    index = i;
                }
            }
        }
    }

    if (index == -1)
    {
        return 0;
    }
    
    return lastBaseAddress + Utils::str2Uint(static_cast<AddressBlock*>(items_.at(index).data())->getRange()) - 1;
}

QString MemoryMap::getLastAddressStr() const {
	int index = -1;
	quint64 lastBaseAddress = 0;

	// check all blocks of the memory map
	for (int i = 0; i < items_.size(); ++i) {
		
		QSharedPointer<AddressBlock> block = items_.at(i).dynamicCast<AddressBlock>();
		
		if (!block) {
			continue;
		}

		// get the base of the current block
		quint64 base = Utils::str2Uint(block->getBaseAddress());

		// find the last lastBlock in the memory map
		if (base >= lastBaseAddress) {
			lastBaseAddress = base;
			index = i;
		}
	}

	if (index == -1) {
		return QString();
	}

	// calculate the last address contained in the block
	QSharedPointer<AddressBlock> lastBlock = items_.at(index).staticCast<AddressBlock>();
	Q_ASSERT(lastBlock);

	quint64 range = Utils::str2Uint(lastBlock->getRange());
	quint64 lastAddress = range + lastBaseAddress - 1;

	// if base and range were undefined then there is no last address
	if (range == 0 && lastBaseAddress == 0) {
		return QString();
	}

	QString str = QString::number(lastAddress, 16);
	str.prepend("0x");
	return str;
}

quint64 MemoryMap::getFirstAddress() const {
	quint64 firstBase = 0;
	for (int i = 0; i < items_.size(); ++i) {

		QSharedPointer<AddressBlock> block = items_.at(i).dynamicCast<AddressBlock>();
		if (!block) {
			continue;
		}
		// convert the base address to numerical format
		QString addrStr = block->getBaseAddress();
		quint64 addr = Utils::str2Uint(addrStr);

		// if this is the first block then it must be smallest so far
		if (i == 0) {	
			firstBase = addr;
		}
		// for others, check if the new base is smaller
		else if (firstBase > addr) {
			firstBase = addr;
		}
	}
	return firstBase;
}

QString MemoryMap::getFirstAddressStr() const {
	quint64 firstBase = 0;
	QString base;
	for (int i = 0; i < items_.size(); ++i) {

		QSharedPointer<AddressBlock> block = items_.at(i).dynamicCast<AddressBlock>();
		if (!block) {
			continue;
		}

		// convert the base address to numerical format
		QString addrStr = block->getBaseAddress();
		quint64 addr = Utils::str2Uint(addrStr);

		// if this is the first lastBlock then it must be smallest so far
		if (i == 0) {	
			firstBase = addr;
			base = addrStr;
		}
		// for others, check if the new base is smaller
		else if (firstBase > addr) {
			firstBase = addr;
			base = addrStr;
		}
	}
	return base;
}

bool MemoryMap::isEmpty() const {
	return items_.isEmpty() && nameGroup_.name_.isEmpty() &&
		nameGroup_.displayName_.isEmpty() && nameGroup_.description_.isEmpty();
}	

General::NameGroup& MemoryMap::getNameGroup() {
	return nameGroup_;
}

int MemoryMap::getMaxWidth() const {
	int width = 0;
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// only check the widths of address blocks
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {
			
			// find the largest width
			if (width < addrBlock->getWidth()) {
				width = addrBlock->getWidth();
			}
		}
	}
	return width;
}

bool MemoryMap::uniqueRegisterNames( QStringList& regNames ) const {
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
		
		// only address blocks contain registers
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {

			// if address blocks contain registers with same names
			if (!addrBlock->uniqueRegisterNames(regNames)) {
				return false;
			}
		}
	}
	// all address blocks contained unique names
	return true;
}

void MemoryMap::writeRegisters( QTextStream& stream,
	quint64 offset, 
	bool useAddrBlockID /*= false*/,
	const QString& idString /*= QString()*/ ) const {
		
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// only address blocks contain registers
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock && addrBlock->getUsage() == General::REGISTER) {

			// tell address block to write its registers
			addrBlock->writeRegisters(stream, offset, useAddrBlockID, idString);
		}
	}
}

bool MemoryMap::uniqueMemoryNames( QStringList& memNames ) const {
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// only address blocks can be memory blocks
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		
		// if address block's type is memory
		if (addrBlock && addrBlock->getUsage() == General::MEMORY) {
			const QString memName = addrBlock->getName();

			// if memory name is not unique
			if (memNames.contains(memName)) {
				return false;
			}

			memNames.append(memName);
		}
	}

	// all were unique
	return true;
}

void MemoryMap::writeMemoryAddresses( QTextStream& stream,
	quint64 offset,
	const QString& idString /*= QString()*/ ) const {

	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// only address blocks can be memory blocks, also write reserved blocks
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock && (addrBlock->getUsage() == General::MEMORY ||
			addrBlock->getUsage() == General::RESERVED)) {

			// calculate the total offset of the memory
			quint64 memoryOffset = Utils::str2Uint(addrBlock->getBaseAddress());
			quint64 totalOffset = offset + memoryOffset;
			
			// the start address of the block
			QString offsetStr = QString::number(totalOffset, 16);
			offsetStr.prepend("0x");

			// the last address contained in the block
			quint64 endAddr = Utils::str2Uint(addrBlock->getLastAddressStr());
			
			// the last address is the total offset incremented with the last address without the base
			endAddr += offset;
			QString endAddrStr = QString::number(endAddr, 16);
			endAddrStr.prepend("0x");

			stream << "/*" << endl;

			// if the block is memory
			if (addrBlock->getUsage() == General::MEMORY) {
				stream << " * Memory block name: " << addrBlock->getName() << endl;
			}
			// if the block is reserved type
			else {
				stream << " * Reserved block name: " << addrBlock->getName() << endl;
			}

			stream << " * Width: " << addrBlock->getWidth() << endl;
			stream << " * Range: " << addrBlock->getRange() << endl;
			QString accessStr = General::access2Str(addrBlock->getAccess());
			if (!accessStr.isEmpty()) {
				stream << " * Access: " << accessStr << endl;
			}
			stream << "*/" << endl;
			
			// write the define for the memory
			stream << "#define ";
			if (!idString.isEmpty()) {
				stream << idString.toUpper() << "_" << addrBlock->getName().toUpper() <<
					"_START " << offsetStr << endl;
				stream << "#define " << idString.toUpper() << "_" << addrBlock->getName().toUpper() << "_END " <<
					endAddrStr << endl;
			}
			else {
				stream << addrBlock->getName().toUpper() << "_START " << offsetStr << endl;
				stream << "#define " << addrBlock->getName().toUpper() << "_END " << endAddrStr << endl;
			}

			stream << endl;
		}
		
	}
}
