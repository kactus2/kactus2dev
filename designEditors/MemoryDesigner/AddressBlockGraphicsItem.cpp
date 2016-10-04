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
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::AddressBlockGraphicsItem()
//-----------------------------------------------------------------------------
AddressBlockGraphicsItem::AddressBlockGraphicsItem(QSharedPointer<MemoryItem> blockItem, bool isEmptyBlock,
    MemoryMapGraphicsItem* memoryMapItem):
MemoryDesignerChildGraphicsItem(blockItem->getName(), "address block", blockItem->getAddress().toULongLong(),
    blockItem->getRange().toULongLong(), getBlockWidth(memoryMapItem), memoryMapItem),
SubMemoryLayout(blockItem, MemoryDesignerConstants::REGISTER_TYPE, this),
addressUnitBits_(blockItem->getAUB())
{
    setColors(KactusColors::ADDR_BLOCK_COLOR, isEmptyBlock);
    setLabelPositions();

    qreal xPosition = boundingRect().width() / 6 - 1;
    setupSubItems(xPosition);
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
    int blockWidth = memoryMapItem->boundingRect().width() / 4 * 3 + 1;
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
// Function: AddressBlockGraphicsItem::createNewSubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressBlockGraphicsItem::createNewSubItem(
    QSharedPointer<MemoryItem> subMemoryItem, bool isEmpty)
{
    return new RegisterGraphicsItem(subMemoryItem, isEmpty, this);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::createEmptySubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressBlockGraphicsItem::createEmptySubItem(quint64 beginAddress,
    quint64 rangeEnd)
{
    quint64 emptyRegisterRangeInt = rangeEnd - beginAddress + 1;

    QString emptyRegisterBaseAddress = QString::number(beginAddress);
    QString emptyRegisterRange = QString::number(emptyRegisterRangeInt);

    int intAUB = addressUnitBits_.toInt();
    quint64 registerSize = emptyRegisterRangeInt * intAUB;

    QSharedPointer<MemoryItem> emptyRegister (new MemoryItem("Reserved", MemoryDesignerConstants::REGISTER_TYPE));
    emptyRegister->setAddress(emptyRegisterBaseAddress);
    emptyRegister->setSize(QString::number(registerSize));
    emptyRegister->setAUB(addressUnitBits_);

    return createNewSubItem(emptyRegister, true);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::changeAddressRange()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::changeAddressRange(quint64 memoryMapOffset)
{
    MemoryDesignerChildGraphicsItem::changeAddressRange(memoryMapOffset);

    SubMemoryLayout::changeChildItemRanges(memoryMapOffset);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::condenseRegistersToConnection()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphicsItem::condenseRegistersToConnection(MemoryConnectionItem* connectionItem,
    quint64 connectionBaseAdddress, quint64 connectionLastAddress, qreal minimumRegisterHeight)
{
    quint64 newBlockHeight = 0;

    QVector<MemoryDesignerChildGraphicsItem*> registersInConnection;
    QVector<MemoryDesignerChildGraphicsItem*> registersBelowConnection;

    foreach (MemoryDesignerChildGraphicsItem* subItem, getSubMemoryItems())
    {
        quint64 subItemBaseAddress = subItem->getBaseAddress();

        if (subItemBaseAddress >= connectionBaseAdddress)
        {
            if (subItemBaseAddress >= connectionLastAddress)
            {
                registersBelowConnection.append(subItem);
            }
            else
            {
                registersInConnection.append(subItem);
            }
        }
    }

    if (!registersInConnection.isEmpty())
    {
        MemoryDesignerChildGraphicsItem* firstItem = registersInConnection.first();
        qreal firstItemTop = firstItem->sceneBoundingRect().top();
        newBlockHeight = firstItemTop - sceneBoundingRect().top();

        qreal registerHeight = getCondensedRegisterHeightForConnection(
            connectionItem, registersInConnection, firstItemTop, minimumRegisterHeight);

        foreach (MemoryDesignerChildGraphicsItem* registerItem, registersInConnection)
        {
            newBlockHeight = condenseSubItem(registerItem, registerHeight, newBlockHeight);
        }
    }

    foreach (MemoryDesignerChildGraphicsItem* registerItem, registersBelowConnection)
    {
        newBlockHeight = condenseSubItem(registerItem, minimumRegisterHeight, newBlockHeight);
    }

    if (newBlockHeight > 0)
    {
        condense(newBlockHeight);
    }

    return newBlockHeight;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::getCondensedRegisterHeightForConnection()
//-----------------------------------------------------------------------------
qreal AddressBlockGraphicsItem::getCondensedRegisterHeightForConnection(MemoryConnectionItem* connectionItem,
    QVector<MemoryDesignerChildGraphicsItem*> registersInConnection, qreal registerStartPositionY,
    qreal minimumHeight) const
{
    qreal availableArea = connectionItem->sceneBoundingRect().bottom() - registerStartPositionY;

    int amountOfRegistersToBeCondensed = 0;

    foreach (MemoryDesignerChildGraphicsItem* registerItem, registersInConnection)
    {
        quint64 registerBaseAddress = registerItem->getBaseAddress();
        quint64 registerLastAddress = registerItem->getLastAddress();
        if (registerLastAddress - registerBaseAddress > 1)
        {
            ++amountOfRegistersToBeCondensed;
        }
        else
        {
            availableArea = availableArea - registerItem->boundingRect().height();
        }
    }

    qreal registerHeight = minimumHeight;
    if (amountOfRegistersToBeCondensed > 0)
    {
        registerHeight = availableArea / amountOfRegistersToBeCondensed - 1;
        if (registerHeight < minimumHeight)
        {
            registerHeight = minimumHeight;
        }
    }

    return registerHeight;
}
