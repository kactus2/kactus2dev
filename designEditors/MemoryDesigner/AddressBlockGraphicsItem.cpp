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
    bool filterRegisters, qreal addressBlockWidth, MemoryMapGraphicsItem* memoryMapItem):
MemoryDesignerChildGraphicsItem(blockItem->getName(), "address block", blockItem->getAddress().toULongLong(),
    blockItem->getRange().toULongLong(), addressBlockWidth, memoryMapItem),
SubMemoryLayout(blockItem, MemoryDesignerConstants::REGISTER_TYPE, filterRegisters, this),
addressUnitBits_(blockItem->getAUB())
{
    setColors(KactusColors::ADDR_BLOCK_COLOR, isEmptyBlock);
    setLabelPositions();

    qreal xPosition = MemoryDesignerConstants::MAPSUBITEMPOSITIONX - 1;
    setupSubItems(xPosition, getSubItemType(), blockItem);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::~AddressBlockGraphicsItem()
//-----------------------------------------------------------------------------
AddressBlockGraphicsItem::~AddressBlockGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::setLabelPositions()
{
    qreal nameX = 0;
    if (subItemsAreFiltered())
    {
        nameX = boundingRect().right() - getNameLabel()->boundingRect().width();
    }
    else
    {
        nameX = boundingRect().left() + MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 -
            getNameLabel()->boundingRect().width();
    }
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
    return new RegisterGraphicsItem(subMemoryItem, isEmpty, getRegisterWidth(), this);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::getRegisterWidth()
//-----------------------------------------------------------------------------
qreal AddressBlockGraphicsItem::getRegisterWidth() const
{
    int registerWidth = boundingRect().width() - MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 + 1;
    return registerWidth;
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

    QSharedPointer<MemoryItem> emptyRegister (
        new MemoryItem(MemoryDesignerConstants::RESERVED_NAME, MemoryDesignerConstants::REGISTER_TYPE));
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
// Function: AddressBlockGraphicsItem::addMemoryConnection()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::addMemoryConnection(MemoryConnectionItem* connectionItem)
{
    MemoryDesignerGraphicsItem::addMemoryConnection(connectionItem);

    addConnectionToSubItems(connectionItem);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::getItemWidth()
//-----------------------------------------------------------------------------
qreal AddressBlockGraphicsItem::getItemWidth() const
{
    qreal registerWidth = 0;

    if (!subItemsAreFiltered())
    {
        registerWidth = getSubMemoryItems().first()->boundingRect().width();
    }
    qreal itemBoundingWidth = boundingRect().width() - registerWidth;
    return itemBoundingWidth;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphicsItem::changeWidth()
//-----------------------------------------------------------------------------
void AddressBlockGraphicsItem::changeWidth(qreal widthChange)
{
    if (!subItemsAreFiltered())
    {
        QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
        while (subItemIterator.hasNext())
        {
            subItemIterator.next();
            MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
            RegisterGraphicsItem* registerItem = dynamic_cast<RegisterGraphicsItem*>(subItem);
            if (registerItem)
            {
                registerItem->changeWidth(widthChange);
            }
        }
    }
}
