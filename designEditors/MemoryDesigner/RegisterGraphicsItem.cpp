//-----------------------------------------------------------------------------
// File: RegisterGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Graphics item for visualizing register in the memory designer.
//-----------------------------------------------------------------------------

#include "RegisterGraphicsItem.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/AddressBlockGraphicsItem.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::RegisterGraphicsItem()
//-----------------------------------------------------------------------------
RegisterGraphicsItem::RegisterGraphicsItem(QSharedPointer<MemoryItem> registerItem, bool isEmptyRegister,
    AddressBlockGraphicsItem* blockItem):
MemoryDesignerChildGraphicsItem(registerItem->getName(), "register", registerItem->getAddress().toULongLong(),
    getRegisterEnd(registerItem), getRegisterWidth(blockItem), blockItem)
{
    setColors(KactusColors::REGISTER_COLOR, isEmptyRegister);
    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::~RegisterGraphicsItem()
//-----------------------------------------------------------------------------
RegisterGraphicsItem::~RegisterGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getRegisterEnd()
//-----------------------------------------------------------------------------
quint64 RegisterGraphicsItem::getRegisterEnd(QSharedPointer<MemoryItem> registerItem) const
{
    int addressUnitBits = registerItem->getAUB().toInt();

    quint64 registerSize = registerItem->getSize().toULongLong();

    quint64 registerEnd = registerSize / addressUnitBits;
    return registerEnd;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::setLabelPositions()
{
    qreal nameX = boundingRect().right() - getNameLabel()->boundingRect().width();
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
// Function: RegisterGraphicsItem::getRegisterWidth()
//-----------------------------------------------------------------------------
qreal RegisterGraphicsItem::getRegisterWidth(AddressBlockGraphicsItem* blockItem) const
{
    int registerWidth = blockItem->boundingRect().width() / 3 * 2 + 1;
    return registerWidth;
}
