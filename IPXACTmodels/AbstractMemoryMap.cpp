//-----------------------------------------------------------------------------
// File: RemapStatesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Abstract memory-map contains common memory-map elements.
//-----------------------------------------------------------------------------

#include "AbstractMemoryMap.h"

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
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::AbstractMemoryMap()
//-----------------------------------------------------------------------------
AbstractMemoryMap::AbstractMemoryMap(QDomNode &abstractMemoryMapNode):
nameGroup_(abstractMemoryMapNode),
id_(),
items_()
{
    for (int i = 0; i < abstractMemoryMapNode.childNodes().count(); ++i)
    {
        QDomNamedNodeMap attributeMap = abstractMemoryMapNode.attributes();
        id_ = attributeMap.namedItem("spirit:id").nodeValue();
        id_ = XmlUtils::removeWhiteSpace(id_);

        QDomNode tempNode = abstractMemoryMapNode.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:addressBlock"))
        {
            AddressBlock* temp = new AddressBlock(tempNode);
            items_.append(QSharedPointer<AddressBlock>(temp));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::AbstractMemoryMap()
//-----------------------------------------------------------------------------
AbstractMemoryMap::AbstractMemoryMap():
nameGroup_(),
id_(),
items_()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::AbstractMemoryMap()
//-----------------------------------------------------------------------------
AbstractMemoryMap::AbstractMemoryMap(const AbstractMemoryMap &other):
nameGroup_(other.nameGroup_),
id_(other.id_),
items_()
{
    foreach (QSharedPointer<MemoryMapItem> item, other.items_)
    {
        if (item)
        {
            QSharedPointer<MemoryMapItem> copy = item->clone();
            items_.append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::operator=()
//-----------------------------------------------------------------------------
AbstractMemoryMap & AbstractMemoryMap::operator=( const AbstractMemoryMap& other)
{
    if (this != &other)
    {
        nameGroup_ = other.nameGroup_;
        id_ = other.id_;

        items_.clear();
        foreach (QSharedPointer<MemoryMapItem> item, other.items_)
        {
            if (item)
            {
                QSharedPointer<MemoryMapItem> copy = item->clone();
                items_.append(copy);
            }
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::~AbstractMemoryMap()
//-----------------------------------------------------------------------------
AbstractMemoryMap::~AbstractMemoryMap()
{
    items_.clear();
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::write()
//-----------------------------------------------------------------------------
void AbstractMemoryMap::write(QXmlStreamWriter& writer)
{
    if (!id_.isEmpty())
    {
        writer.writeAttribute("spirit:id", id_);
    }

    writer.writeTextElement("spirit:name", nameGroup_.name());

    if (!nameGroup_.displayName().isEmpty())
    {
        writer.writeTextElement("spirit:displayName", nameGroup_.displayName());
    }

    if (!nameGroup_.description().isEmpty())
    {
        writer.writeTextElement("spirit:description", nameGroup_.description());
    }

    foreach (QSharedPointer<MemoryMapItem> item, items_)
    {
        item->write(writer);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::isValid()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("memory map %1").arg(nameGroup_.name()));

    if (nameGroup_.name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for memory map within %1").arg(parentIdentifier));
        valid = false;
    }

    QStringList memItemNames;
    foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
        if (memItemNames.contains(memItem->getName()))
        {
            errorList.append(QObject::tr("%1 contains several memory map items with name %2").arg(
                memItem->getName()));
            valid = false;
        }
        else
        {
            memItemNames.append(memItem->getName());
        }

        if (!memItem->isValid(componentChoices, errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::isValid()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
    if (nameGroup_.name().isEmpty())
    {
        return false;
    }

    QStringList memItemNames;
    foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
        if (memItemNames.contains(memItem->getName()))
        {
            return false;
        }
        else
        {
            memItemNames.append(memItem->getName());
        }

        if (!memItem->isValid(componentChoices))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::containsSubItems()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::containsSubItems() const
{
    return !items_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::setName()
//-----------------------------------------------------------------------------
void AbstractMemoryMap::setName(const QString &name)
{
    nameGroup_.setName(name);
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getItems()
//-----------------------------------------------------------------------------
const QList<QSharedPointer<MemoryMapItem> >& AbstractMemoryMap::getItems() const
{
    return items_;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getItems()
//-----------------------------------------------------------------------------
QList<QSharedPointer<MemoryMapItem> >& AbstractMemoryMap::getItems()
{
    return items_;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getName()
//-----------------------------------------------------------------------------
QString AbstractMemoryMap::getName() const
{
    return nameGroup_.name();
}

void AbstractMemoryMap::setItems(const QList<QSharedPointer<MemoryMapItem> > &newItems)
{
    items_.clear();

    items_ = newItems;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getDisplayName()
//-----------------------------------------------------------------------------
QString AbstractMemoryMap::getDisplayName() const
{
    return nameGroup_.displayName();
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::setDisplayName()
//-----------------------------------------------------------------------------
void AbstractMemoryMap::setDisplayName(const QString& displayName)
{
    nameGroup_.setDisplayName(displayName);
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getDescription()
//-----------------------------------------------------------------------------
QString AbstractMemoryMap::getDescription() const
{
    return nameGroup_.description();
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::setDescription()
//-----------------------------------------------------------------------------
void AbstractMemoryMap::setDescription(const QString& description)
{
    nameGroup_.setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getLastAddress()
//-----------------------------------------------------------------------------
quint64 AbstractMemoryMap::getLastAddress() const
{
    int index = -1;
    unsigned int lastBaseAddress = 0;

    for (int i = 0; i < items_.size(); ++i)
    {
        AddressBlock* block = dynamic_cast<AddressBlock*>(items_.at(i).data());
        if (block != 0)
        {
            bool ok = true;
            unsigned int baseAddress = General::str2Uint(block->getBaseAddress());

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

    return lastBaseAddress + General::str2Uint(static_cast<AddressBlock*>(items_.at(index).data())->getRange()) - 1;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getLastAddressStr()
//-----------------------------------------------------------------------------
QString AbstractMemoryMap::getLastAddressStr() const
{
	int index = -1;
	quint64 lastBaseAddress = 0;

	for (int i = 0; i < items_.size(); ++i)
    {
		QSharedPointer<AddressBlock> block = items_.at(i).dynamicCast<AddressBlock>();
		
		if (!block)
        {
			continue;
		}

		// get the base of the current block
		quint64 base = General::str2Uint(block->getBaseAddress());

		// find the last lastBlock in the memory map
		if (base >= lastBaseAddress)
        {
			lastBaseAddress = base;
			index = i;
		}
	}

	if (index == -1)
    {
		return QString();
	}

	// calculate the last address contained in the block
	QSharedPointer<AddressBlock> lastBlock = items_.at(index).staticCast<AddressBlock>();
	Q_ASSERT(lastBlock);

	quint64 range = General::str2Uint(lastBlock->getRange());
	quint64 lastAddress = range + lastBaseAddress - 1;

	// if base and range were undefined then there is no last address
	if (range == 0 && lastBaseAddress == 0)
    {
		return QString();
	}

	QString str = QString::number(lastAddress, 16);
	str.prepend("0x");
	return str;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getFirstAddressStr()
//-----------------------------------------------------------------------------
QString AbstractMemoryMap::getFirstAddressStr() const
{
	quint64 firstBase = 0;
	QString base;
	for (int i = 0; i < items_.size(); ++i)
    {
		QSharedPointer<AddressBlock> block = items_.at(i).dynamicCast<AddressBlock>();
		if (!block)
        {
			continue;
		}

		// convert the base address to numerical format
		QString addrStr = block->getBaseAddress();
		quint64 addr = General::str2Uint(addrStr);

		// if this is the first lastBlock then it must be smallest so far
		if (i == 0)
        {
			firstBase = addr;
			base = addrStr;
		}
		// for others, check if the new base is smaller
		else if (firstBase > addr)
        {
			firstBase = addr;
			base = addrStr;
		}
	}
	return base;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::isEmpty()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::isEmpty() const
{
	return items_.isEmpty() && nameGroup_.name().isEmpty() && nameGroup_.displayName().isEmpty() &&
        nameGroup_.description().isEmpty();
}	

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getNameGroup()
//-----------------------------------------------------------------------------
NameGroup& AbstractMemoryMap::getNameGroup()
{
	return nameGroup_;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::getMaxWidth()
//-----------------------------------------------------------------------------
int AbstractMemoryMap::getMaxWidth() const
{
	int width = 0;
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		// only check the widths of address blocks
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock)
        {
			// find the largest width
			if (width < addrBlock->getWidth())
            {
				width = addrBlock->getWidth();
			}
		}
	}
	return width;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::uniqueRegisterNames()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::uniqueRegisterNames(QStringList& regNames) const
{
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		// only address blocks contain registers
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock)
        {
			// if address blocks contain registers with same names
			if (!addrBlock->uniqueRegisterNames(regNames))
            {
				return false;
			}
		}
	}
	// all address blocks contained unique names
	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::writeRegisters()
//-----------------------------------------------------------------------------
void AbstractMemoryMap::writeRegisters(QTextStream& stream, quint64 offset, bool useAddrBlockID /* = false */,
    const QString& idString /* = QString() */) const
{
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		// only address blocks contain registers
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock && addrBlock->getUsage() == General::REGISTER)
        {
			// tell address block to write its registers
			addrBlock->writeRegisters(stream, offset, useAddrBlockID, idString);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::uniqueMemoryNames()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::uniqueMemoryNames(QStringList& memNames) const
{
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		// only address blocks can be memory blocks
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		
		// if address block's type is memory
		if (addrBlock && addrBlock->getUsage() == General::MEMORY)
        {
			const QString memName = addrBlock->getName();

			// if memory name is not unique
			if (memNames.contains(memName))
            {
				return false;
			}

			memNames.append(memName);
		}
	}

	// all were unique
	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::writeMemoryAddresses()
//-----------------------------------------------------------------------------
void AbstractMemoryMap::writeMemoryAddresses(QTextStream& stream, quint64 offset,
    const QString& idString /* = QString() */) const
{
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		// only address blocks can be memory blocks, also write reserved blocks
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock && (addrBlock->getUsage() == General::MEMORY ||addrBlock->getUsage() == General::RESERVED))
        {
			// calculate the total offset of the memory
			quint64 memoryOffset = General::str2Uint(addrBlock->getBaseAddress());
			quint64 totalOffset = offset + memoryOffset;
			
			// the start address of the block
			QString offsetStr = QString::number(totalOffset, 16);
			offsetStr.prepend("0x");

			// the last address contained in the block
			quint64 endAddr = General::str2Uint(addrBlock->getLastAddressStr());
			
			// the last address is the total offset incremented with the last address without the base
			endAddr += offset;
			QString endAddrStr = QString::number(endAddr, 16);
			endAddrStr.prepend("0x");

			stream << "/*" << endl;

			// if the block is memory
			if (addrBlock->getUsage() == General::MEMORY)
            {
				stream << " * Memory block name: " << addrBlock->getName() << endl;
			}
			// if the block is reserved type
			else
            {
				stream << " * Reserved block name: " << addrBlock->getName() << endl;
			}

			stream << " * Width: " << addrBlock->getWidth() << endl;
			stream << " * Range: " << addrBlock->getRange() << endl;
			QString accessStr = General::access2Str(addrBlock->getAccess());
			if (!accessStr.isEmpty())
            {
				stream << " * Access: " << accessStr << endl;
			}
			stream << "*/" << endl;
			
			// write the define for the memory
			stream << "#define ";
			if (!idString.isEmpty())
            {
				stream << idString.toUpper() << "_" << addrBlock->getName().toUpper() << "_START " <<
                    offsetStr << endl;
				stream << "#define " << idString.toUpper() << "_" << addrBlock->getName().toUpper() << "_END " <<
                    endAddrStr << endl;
			}
			else
            {
				stream << addrBlock->getName().toUpper() << "_START " << offsetStr << endl;
				stream << "#define " << addrBlock->getName().toUpper() << "_END " << endAddrStr << endl;
			}

			stream << endl;
		}
		
	}
}
