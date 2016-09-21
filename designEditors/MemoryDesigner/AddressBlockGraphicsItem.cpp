//-----------------------------------------------------------------------------
// File: AddressBlockGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Graphics item for visualizing an address block in the memory designer.
//-----------------------------------------------------------------------------

#include "AddressBlockGraphicsItem.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <designEditors/MemoryDesigner/RegisterGraphicsItem.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::AddressBlockGraphicsItem()
//-----------------------------------------------------------------------------
AddressBlockGraphicsItem::AddressBlockGraphicsItem(QSharedPointer<MemoryItem> blockItem, bool isEmptyBlock,
    MemoryMapGraphicsItem* memoryMapItem):
MemoryDesignerChildGraphicsItem(blockItem, "address block", blockItem->getAddress().toULongLong(),
    blockItem->getRange().toULongLong(), getBlockWidth(memoryMapItem), memoryMapItem),
registers_()
{
    setColors(KactusColors::ADDR_BLOCK_COLOR, isEmptyBlock);
    setLabelPositions();
    setupRegisters(blockItem);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::~AddressBlockGraphicsItem()
//-----------------------------------------------------------------------------
AddressBlockGraphicsItem::~AddressBlockGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::getBlockWidth()
//-----------------------------------------------------------------------------
qreal AddressBlockGraphicsItem::getBlockWidth(MemoryMapGraphicsItem* memoryMapItem) const
{
    qreal blockWidth = memoryMapItem->boundingRect().width() / 4 * 3;
    return blockWidth;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::setLabelPositions()
{
    qreal nameX = (-boundingRect().width() / 6) - getNameLabel()->boundingRect().width();
    qreal nameY = (boundingRect().height() / 2) - (getNameLabel()->boundingRect().height() / 2);

    getNameLabel()->setPos(nameX, nameY);

    qreal rangeStartX = boundingRect().left();
    qreal rangeStartY = boundingRect().top();
    qreal rangeEndY = boundingRect().bottom() - getRangeEndLabel()->boundingRect().height();

    if (!getRangeStartLabel()->isVisible())
    {
        rangeEndY += 2;
    }

    getRangeStartLabel()->setPos(rangeStartX, rangeStartY);
    getRangeEndLabel()->setPos(rangeStartX, rangeEndY);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::setupRegisters()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::setupRegisters(QSharedPointer<MemoryItem> blockItem)
{
    QString blockItemAUB = blockItem->getAUB();

    quint64 blockBaseAddress = blockItem->getAddress().toULongLong();

    if (!blockItem->getChildItems().isEmpty())
    {
        qreal registerTransferY = 0;
        quint64 registerAddress = 0;

        QMap<quint64, RegisterGraphicsItem*> registersInOrder;

        foreach (QSharedPointer<MemoryItem> registerItem, blockItem->getChildItems())
        {
            if (registerItem->getType().compare("register", Qt::CaseInsensitive) == 0)
            {
                RegisterGraphicsItem* newRegister = new RegisterGraphicsItem(registerItem, false, this);
                qreal registerXPosition = newRegister->boundingRect().width() / 4 - 1;

                registerAddress = registerItem->getAddress().toULongLong();
                registerTransferY = (registerAddress - blockBaseAddress) * GridSize * 1.5;

                newRegister->setPos(registerXPosition, registerTransferY);

                registersInOrder.insert(registerAddress, newRegister);
                registers_.append(newRegister);
            }
        }

        if (!registersInOrder.isEmpty())
        {
            if ((registersInOrder.firstKey() - blockBaseAddress) > 0)
            {
                createEmptyRegister(
                    blockBaseAddress, registersInOrder.firstKey() - 1, blockItemAUB, blockBaseAddress);
            }

            QMapIterator<quint64, RegisterGraphicsItem*> registerIterator(registersInOrder);

            quint64 currentAddress = blockBaseAddress;
            quint64 registerLastAddress = blockBaseAddress;

            while (registerIterator.hasNext())
            {
                registerIterator.next();

                currentAddress = registerIterator.key();
                if (currentAddress > registerLastAddress + 1)
                {
                    createEmptyRegister(
                        registerLastAddress + 1, currentAddress - 1, blockItemAUB, blockBaseAddress);
                }

                quint64 rangeEnd = registerIterator.value()->getLastAddress();

                registerLastAddress = qMax(registerLastAddress, rangeEnd);
            }

            if (registerLastAddress != getLastAddress())
            {
                createEmptyRegister(registerLastAddress + 1, getLastAddress(), blockItemAUB, blockBaseAddress);
            }
        }
    }
    else
    {
        createEmptyRegister(blockBaseAddress, getLastAddress(), blockItemAUB, blockBaseAddress);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::createEmptyRegister()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::createEmptyRegister(quint64 beginAddress, quint64 rangeEnd,
    QString const& addressUnitBits, quint64 blockBaseAddress)
{
    quint64 emptyRegisterOffsetInInt = beginAddress;
    quint64 emptyRegisterRangeInt = rangeEnd - emptyRegisterOffsetInInt + 1;

    QString emptyRegisterBaseAddress = QString::number(emptyRegisterOffsetInInt);
    QString emptyRegisterRange = QString::number(emptyRegisterRangeInt);

    int intAUB = addressUnitBits.toInt();
    quint64 registerSize = emptyRegisterRangeInt * intAUB;

    QSharedPointer<MemoryItem> emptyRegister (new MemoryItem("Reserved", "register"));
    emptyRegister->setAddress(emptyRegisterBaseAddress);
    emptyRegister->setSize(QString::number(registerSize));
    emptyRegister->setAUB(addressUnitBits);

    RegisterGraphicsItem* newRegister = new RegisterGraphicsItem(emptyRegister, true, this);

    quint64 registerTransferY = (emptyRegisterOffsetInInt - blockBaseAddress) * GridSize * 1.5;

    qreal registerPositionX = newRegister->boundingRect().width() / 4 - 1;

    newRegister->setPos(registerPositionX, registerTransferY);

    registers_.append(newRegister);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::changeAddressRange()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::changeAddressRange(quint64 memoryMapOffset)
{
    MemoryDesignerChildGraphicsItem::changeAddressRange(memoryMapOffset);

    foreach (RegisterGraphicsItem* registerItem, registers_)
    {
        registerItem->changeAddressRange(memoryMapOffset);
    }
}
