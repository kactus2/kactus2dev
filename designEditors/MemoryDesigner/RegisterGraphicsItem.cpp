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
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/FieldGraphicsItem.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::RegisterGraphicsItem()
//-----------------------------------------------------------------------------
RegisterGraphicsItem::RegisterGraphicsItem(QSharedPointer<MemoryItem> registerItem, bool isEmptyRegister,
    qreal registerWidth, MemoryDesignerGraphicsItem* parentItem):
MemoryDesignerChildGraphicsItem(registerItem->getName(), "register", registerItem->getAddress().toULongLong(),
    getRegisterEnd(registerItem), registerWidth, parentItem),
fieldItems_(),
isEmpty_(isEmptyRegister),
registerWidth_(registerWidth),
registerMemoryItem_(registerItem)
{
    setColors(KactusColors::REGISTER_COLOR, isEmptyRegister);
    setLabelPositions();

    if (!isEmptyRegister)
    {
        setupFields(registerItem, getRegisterEnd(registerItem));
    }
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
    qreal nameX = boundingRect().left() + MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 -
        getNameLabel()->boundingRect().width();

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
// Function: RegisterGraphicsItem::condense()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::condense(qreal newItemHeight)
{
    foreach (FieldGraphicsItem* fieldItem, fieldItems_)
    {
        fieldItem->condense(newItemHeight);
    }

    MemoryDesignerGraphicsItem::condense(newItemHeight);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::setupFields()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::setupFields(QSharedPointer<MemoryItem> registerItem, quint64 registerEnd)
{
    quint64 registerSize = registerItem->getSize().toULongLong();
    qint64 firstUnusedBit = registerSize;

    qreal registerWidth = boundingRect().width();
    qreal fieldsStartPosition = MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2;
    qreal oneBitWidth = (registerWidth - fieldsStartPosition) / registerSize;
    qreal previousEndPosition = boundingRect().left() + fieldsStartPosition;
    qreal widthRemainder = 0;

    if (isEmpty_)
    {
        oneBitWidth = 55;
        createEmptyFieldItem(
            firstUnusedBit, firstUnusedBit, oneBitWidth, registerEnd, previousEndPosition, widthRemainder);
    }
    else
    {
        QMapIterator<quint64, QSharedPointer<MemoryItem> > subItemIterator(getFieldItemsInOffsetOrder(registerItem));
        subItemIterator.toBack();
        while (subItemIterator.hasPrevious())
        {
            subItemIterator.previous();
            QSharedPointer<MemoryItem> fieldItem = subItemIterator.value();
            quint64 fieldOffset = subItemIterator.key();
            quint64 fieldWidth = fieldItem->getWidth().toULongLong();
            qint64 lastBit = fieldOffset + fieldWidth - 1;
            if (lastBit < firstUnusedBit - 1)
            {
                createEmptyFieldItem(lastBit + 1, firstUnusedBit - 1, oneBitWidth, registerEnd,
                    previousEndPosition, widthRemainder);
            }

            createFieldGraphicsItem(fieldItem->getName(), fieldOffset, fieldWidth, false, oneBitWidth, registerEnd,
                previousEndPosition, widthRemainder);

            firstUnusedBit = fieldOffset;
        }

        if (firstUnusedBit > 0)
        {
            createEmptyFieldItem(
                0, firstUnusedBit - 1, oneBitWidth, registerEnd, previousEndPosition, widthRemainder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getFieldItemsInOffsetOrder()
//-----------------------------------------------------------------------------
QMap<quint64, QSharedPointer<MemoryItem> > RegisterGraphicsItem::getFieldItemsInOffsetOrder(
    QSharedPointer<MemoryItem> registerItem) const
{
    QMap<quint64, QSharedPointer<MemoryItem> > fieldMap;

    quint64 registerOffset = registerItem->getAddress().toULongLong();
    unsigned int mapAUB = registerItem->getAUB().toUInt();

    foreach (QSharedPointer<MemoryItem> fieldItem, registerItem->getChildItems())
    {
        if (fieldItem->getType().compare(MemoryDesignerConstants::FIELD_TYPE, Qt::CaseInsensitive) == 0)
        {
            quint64 fieldAddress = fieldItem->getAddress().toULongLong();
            quint64 fieldRegisteredOffset = fieldItem->getOffset().toULongLong();

            quint64 fieldOffset = (fieldAddress - registerOffset) * mapAUB + fieldRegisteredOffset;
            fieldMap.insertMulti(fieldOffset, fieldItem);
        }
    }

    return fieldMap;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createEmptyFieldItem()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createEmptyFieldItem(quint64 currentOffset, quint64 lastBit, qreal oneBitWidth,
    quint64 registerEnd, qreal& previousEndPosition, qreal& widthRemainder)
{
    quint64 fieldWidth = lastBit - currentOffset + 1;

    createFieldGraphicsItem(MemoryDesignerConstants::RESERVED_NAME, currentOffset, fieldWidth, true, oneBitWidth,
        registerEnd, previousEndPosition, widthRemainder);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createFieldGraphicsItem()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createFieldGraphicsItem(QString const& fieldName, quint64 fieldOffset,
    quint64 fieldWidth, bool isEmptyField, qreal oneBitWidth, quint64 registerEnd, qreal& previousEndPosition,
    qreal& widthRemainder)
{
    quint64 fieldLastBit = fieldOffset + fieldWidth - 1;
    qreal fieldItemWidth = fieldWidth * oneBitWidth;
    qreal fieldWidthRemainder = fieldItemWidth - int(fieldItemWidth);
    widthRemainder += fieldWidthRemainder;
    bool fieldOverlap = widthRemainder >= 0.95;
    if (fieldOverlap)
    {
        fieldItemWidth++;
        widthRemainder = widthRemainder - int(widthRemainder);
    }

    FieldGraphicsItem* newField = new FieldGraphicsItem(fieldName, fieldOffset, fieldLastBit, fieldItemWidth,
        registerEnd, isEmptyField, isEmpty_, this);
    fieldItems_.append(newField);

    int overlapModifier = 0;
    if (fieldOverlap)
    {
        overlapModifier = 1;
    }

    qreal fieldItemPositionX = previousEndPosition + fieldItemWidth / 2 - overlapModifier;
    newField->setPos(fieldItemPositionX, 0);

    previousEndPosition += newField->boundingRect().width() - 1 - overlapModifier;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::changeWidth()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::changeWidth(qreal widthChange)
{
    if (!isEmpty_)
    {
        quint64 registerSize = registerMemoryItem_->getSize().toULongLong();
        qreal changePerBit = widthChange / registerSize;
        qreal previousEndPosition = boundingRect().left() + MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2;

        foreach (FieldGraphicsItem* fieldItem, fieldItems_)
        {
            qreal fieldItemWidthChange =
                (fieldItem->getLastAddress() - fieldItem->getBaseAddress() + 1) * changePerBit;

            fieldItem->changeWidth(fieldItemWidthChange);
            qreal fieldItemWidth = fieldItem->boundingRect().width() - 1;
            fieldItem->setPos(previousEndPosition + fieldItemWidth / 2, fieldItem->pos().y());

            previousEndPosition += fieldItem->boundingRect().width() - 1;
        }
    }
    else
    {
        qreal newWidth = boundingRect().width() + widthChange - 1;
        qreal xMovement = widthChange / 2;
        setPos(pos().x() + xMovement, pos().y());
        setRect(-newWidth / 2, 0, newWidth, boundingRect().height() - 1);
        setLabelPositions();
    }
}
