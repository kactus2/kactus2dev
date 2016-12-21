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
    qreal registerWidth, bool filterFields, QString const& containingInstance,
    MemoryDesignerGraphicsItem* parentItem):
MemoryDesignerChildGraphicsItem(registerItem->getName(), QStringLiteral("Register"),
    registerItem->getAddress().toULongLong(),
    getRegisterEnd(registerItem->getAUB().toUInt(), registerItem->getSize().toULongLong()), registerWidth,
    containingInstance, parentItem),
fieldItems_(),
isEmpty_(isEmptyRegister),
registerSize_(registerItem->getSize().toULongLong()),
addressUnitBits_(registerItem->getAUB().toUInt()),
filterFields_(filterFields)
{
    setColors(KactusColors::REGISTER_COLOR, isEmptyRegister);
    setLabelPositions();

    if (!isEmptyRegister && !filterFields)
    {
        setupFields(registerItem);
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
quint64 RegisterGraphicsItem::getRegisterEnd(unsigned int addressUnitBits, quint64 registerSize) const
{
    quint64 registerEnd = registerSize / addressUnitBits;
    return registerEnd;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::setLabelPositions()
{
    qreal nameX = 0;
    QRectF registerRectangle = boundingRect();
    QRectF nameRectangle = getNameLabel()->boundingRect();

    if (filterFields_)
    {
        nameX = registerRectangle.right() - nameRectangle.width();
    }
    else
    {
        nameX = registerRectangle.left() + MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 - nameRectangle.width();
    }

    qreal nameY = (registerRectangle.height() / 2) - (nameRectangle.height() / 2);

    getNameLabel()->setPos(nameX, nameY);

    qreal rangeStartX = registerRectangle.left();
    qreal rangeStartY = registerRectangle.top();
    qreal rangeEndY = registerRectangle.bottom() - getRangeEndLabel()->boundingRect().height();

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
void RegisterGraphicsItem::setupFields(QSharedPointer<MemoryItem> registerItem)
{
    quint64 registerEnd = getRegisterEnd(addressUnitBits_, registerSize_);
    QString registerName = getNameLabel()->toPlainText();

    qint64 firstUnusedBit = registerSize_;
    qreal registerWidth = boundingRect().width();
    qreal fieldsStartPosition = MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2;
    qreal oneBitWidth = (registerWidth - fieldsStartPosition) / registerSize_;
    qreal previousEndPosition = boundingRect().left() + fieldsStartPosition;
    qreal widthRemainder = 0;

    QFont fieldFont = getNameLabel()->font();
    fieldFont.setPointSizeF(fieldFont.pointSizeF() - 1.6);

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
                previousEndPosition, widthRemainder, fieldFont);
        }

        createFieldGraphicsItem(fieldItem->getName(), fieldOffset, fieldWidth, false, oneBitWidth, registerEnd,
            previousEndPosition, widthRemainder, fieldFont);

        firstUnusedBit = fieldOffset;
    }

    if (firstUnusedBit > 0)
    {
        createEmptyFieldItem(
            0, firstUnusedBit - 1, oneBitWidth, registerEnd, previousEndPosition, widthRemainder, fieldFont);
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

    foreach (QSharedPointer<MemoryItem> fieldItem, registerItem->getChildItems())
    {
        if (fieldItem->getType().compare(MemoryDesignerConstants::FIELD_TYPE, Qt::CaseInsensitive) == 0)
        {
            quint64 fieldAddress = fieldItem->getAddress().toULongLong();
            quint64 fieldRegisteredOffset = fieldItem->getOffset().toULongLong();

            quint64 fieldOffset = (fieldAddress - registerOffset) * addressUnitBits_ + fieldRegisteredOffset;
            fieldMap.insertMulti(fieldOffset, fieldItem);
        }
    }

    return fieldMap;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createEmptyFieldItem()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createEmptyFieldItem(quint64 currentOffset, quint64 lastBit, qreal oneBitWidth,
    quint64 registerEnd, qreal& previousEndPosition, qreal& widthRemainder, QFont fieldFont)
{
    quint64 fieldWidth = lastBit - currentOffset + 1;

    createFieldGraphicsItem(MemoryDesignerConstants::RESERVED_NAME, currentOffset, fieldWidth, true, oneBitWidth,
        registerEnd, previousEndPosition, widthRemainder, fieldFont);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createFieldGraphicsItem()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createFieldGraphicsItem(QString const& fieldName, quint64 fieldOffset,
    quint64 fieldWidth, bool isEmptyField, qreal oneBitWidth, quint64 registerEnd, qreal& previousEndPosition,
    qreal& widthRemainder, QFont fieldFont)
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
        registerEnd, isEmptyField, fieldFont, getContainingInstance(), this);
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
        qreal changePerBit = widthChange / registerSize_;
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

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getMaximumNeededChangeInFieldWidth()
//-----------------------------------------------------------------------------
qreal RegisterGraphicsItem::getMaximumNeededChangeInFieldWidth() const
{
    qreal maximumWidthChange = 0;

    foreach (FieldGraphicsItem* fieldItem, fieldItems_)
    {
        maximumWidthChange = qMax(maximumWidthChange, fieldItem->getNeededWithChangeToDisplayFullName());
    }

    maximumWidthChange = registerSize_ * maximumWidthChange;

    return maximumWidthChange;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getItemWidth()
//-----------------------------------------------------------------------------
qreal RegisterGraphicsItem::getItemWidth() const
{
    qreal itemWidth = boundingRect().width();
    if (!fieldItems_.isEmpty())
    {
        itemWidth = MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2;
    }

    return itemWidth;
}
