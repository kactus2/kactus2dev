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

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/FieldGraphicsItem.h>

#include <QBrush>
#include <QFont>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::RegisterGraphicsItem()
//-----------------------------------------------------------------------------
RegisterGraphicsItem::RegisterGraphicsItem(QSharedPointer<MemoryItem const> registerItem, bool isEmptyRegister,
    qreal registerWidth, QVector<QString> identifierChain, bool filterFields,
    QSharedPointer<ConnectivityComponent const> containingInstance, MemoryDesignerGraphicsItem* parentItem):
MemoryDesignerChildGraphicsItem(registerItem, QStringLiteral("Register"), registerItem->getAddress().toULongLong(),
    getRegisterEnd(registerItem->getAUB().toUInt(), registerItem->getSize().toULongLong()), registerWidth,
    identifierChain, containingInstance, parentItem),
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
    for (FieldGraphicsItem* fieldItem : fieldItems_)
    {
        fieldItem->condense(newItemHeight);
    }

    MemoryDesignerGraphicsItem::condense(newItemHeight);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::setupFields()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::setupFields(QSharedPointer<MemoryItem const> registerItem)
{
    quint64 registerEnd = getRegisterEnd(addressUnitBits_, registerSize_);
    QString registerName = getNameLabel()->toPlainText();

    quint64 firstUnusedBit = registerSize_;
    qreal registerWidth = boundingRect().width();
    qreal subItemPosition = MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2;
    qreal oneBitWidth = (registerWidth - subItemPosition) / registerSize_;
    qreal fieldsStartPosition = boundingRect().left() + subItemPosition;

    QFont fieldFont = getNameLabel()->font();
    if (registerEnd == 1)
    {
        fieldFont.setPointSizeF(fieldFont.pointSizeF() - 1.6);
    }
    else
    {
        fieldFont.setPointSizeF(fieldFont.pointSizeF() - 1);
    }

    QMultiMap<quint64, RegisterGraphicsItem::FieldMemoryItem> subItems = getFieldItemsInLastBitOrder(registerItem);
    QMultiMapIterator<quint64, RegisterGraphicsItem::FieldMemoryItem> subItemIterator(subItems);
    subItemIterator.toBack();
    while (subItemIterator.hasPrevious())
    {
        subItemIterator.previous();

        RegisterGraphicsItem::FieldMemoryItem fieldMemory = subItemIterator.value();
        QSharedPointer<MemoryItem const> fieldItem = fieldMemory.fieldMemoryItem;
        quint64 fieldOffset = fieldMemory.fieldOffset;
        quint64 fieldWidth = fieldMemory.fieldWidth;
        quint64 lastBit = subItemIterator.key();

        if (firstUnusedBit > 0 && lastBit < firstUnusedBit - 1)
        {
            createEmptyFieldItem(lastBit + 1, firstUnusedBit - 1, oneBitWidth, registerEnd,
                fieldsStartPosition, fieldFont);
        }

        createFieldGraphicsItem(fieldItem, fieldOffset, fieldWidth, false, oneBitWidth, registerEnd,
            fieldsStartPosition, fieldFont);

        if (fieldOffset < firstUnusedBit)
        {
            firstUnusedBit = fieldOffset;
        }
    }

    if (firstUnusedBit > 0)
    {
        createEmptyFieldItem(0, firstUnusedBit - 1, oneBitWidth, registerEnd, fieldsStartPosition, fieldFont);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getFieldItemsInLastBitOrder()
//-----------------------------------------------------------------------------
QMultiMap<quint64, RegisterGraphicsItem::FieldMemoryItem> RegisterGraphicsItem::getFieldItemsInLastBitOrder(
    QSharedPointer<MemoryItem const> registerItem) const
{
    QMultiMap<quint64, RegisterGraphicsItem::FieldMemoryItem> fieldMap;

    quint64 registerOffset = registerItem->getAddress().toULongLong();

    for (QSharedPointer<MemoryItem const> fieldItem : registerItem->getChildItems())
    {
        if (fieldItem->getType().compare(MemoryDesignerConstants::FIELD_TYPE, Qt::CaseInsensitive) == 0)
        {
            quint64 fieldAddress = fieldItem->getAddress().toULongLong();
            quint64 fieldRegisteredOffset = fieldItem->getOffset().toULongLong();

            quint64 fieldOffset = (fieldAddress - registerOffset) * addressUnitBits_ + fieldRegisteredOffset;

            quint64 fieldWidth = fieldItem->getWidth().toULongLong();
            if (fieldWidth > 0)
            {
                quint64 lastBit = fieldOffset + fieldWidth - 1;

                FieldMemoryItem newFieldMemoryItem;
                newFieldMemoryItem.fieldOffset = fieldOffset;
                newFieldMemoryItem.fieldWidth = fieldWidth;
                newFieldMemoryItem.fieldMemoryItem = fieldItem;

                fieldMap.insert(lastBit, newFieldMemoryItem);
            }
        }
    }

    return fieldMap;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createEmptyFieldItem()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createEmptyFieldItem(quint64 currentOffset, quint64 lastBit, qreal oneBitWidth,
    quint64 registerEnd, qreal fieldsStartPosition, QFont fieldFont)
{
    quint64 fieldWidth = lastBit - currentOffset + 1;

    QSharedPointer<MemoryItem const> newEmptyFieldItem
        (new MemoryItem(MemoryDesignerConstants::RESERVED_NAME, MemoryDesignerConstants::FIELD_TYPE));

    createFieldGraphicsItem(newEmptyFieldItem, currentOffset, fieldWidth, true, oneBitWidth, registerEnd,
        fieldsStartPosition, fieldFont);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createFieldGraphicsItem()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createFieldGraphicsItem(QSharedPointer<MemoryItem const> fieldItem, quint64 fieldOffset,
    quint64 fieldWidth, bool isEmptyField, qreal oneBitWidth, quint64 registerEnd, qreal fieldsStartPosition,
    QFont fieldFont)
{
    quint64 fieldLastBit = fieldOffset + fieldWidth - 1;

    quint64 modifiedFieldWidth = getModifiedFieldWidth(fieldLastBit, fieldOffset);
    bool fieldIsOutsideRegister = modifiedFieldWidth < fieldWidth;

    qreal fieldItemWidth = modifiedFieldWidth * oneBitWidth;

    FieldGraphicsItem* newField =
        new FieldGraphicsItem(fieldItem, fieldOffset, fieldLastBit, fieldItemWidth, registerEnd, isEmptyField,
        getIdentifierChain(), fieldFont, getContainingInstance(), fieldIsOutsideRegister, this);

    connect(newField, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);

    fieldItems_.append(newField);

    qreal fieldItemPositionX = fieldsStartPosition + fieldItemWidth / 2;
    if (fieldLastBit < registerSize_ - 1)
    {
        fieldItemPositionX += (((registerSize_ - fieldLastBit) - 1) * oneBitWidth);
    }

    newField->setPos(fieldItemPositionX, 0);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getModifiedFieldWidth()
//-----------------------------------------------------------------------------
quint64 RegisterGraphicsItem::getModifiedFieldWidth(quint64 fieldLastBit, quint64 fieldOffset) const
{
    quint64 modifiedFieldWidth = fieldLastBit - fieldOffset + 1;

    if (fieldLastBit >= registerSize_)
    {
        if (fieldOffset > registerSize_ - 2)
        {
            modifiedFieldWidth = 2;
        }
        else
        {
            modifiedFieldWidth = modifiedFieldWidth - (fieldLastBit - registerSize_ + 1);
        }
    }

    return modifiedFieldWidth;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createOverlappingFieldMarkers()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createOverlappingFieldMarkers()
{
    int fieldCount = fieldItems_.size();

    if (fieldCount > 1)
    {
        QBrush overlapBrush(KactusColors::MISSING_COMPONENT);

        QGraphicsScene* containingScene = scene();

        for (int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
        {
            FieldGraphicsItem* currentField = fieldItems_.at(fieldIndex);

            QRectF fieldRectangle = currentField->sceneBoundingRect();
            int fieldLineWidth = currentField->pen().width();

            createOverlappingMarkersForField(fieldIndex, fieldCount, currentField, fieldRectangle, fieldLineWidth,
                overlapBrush, containingScene);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::createOverlappingMarkersForField()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::createOverlappingMarkersForField(int fieldIndex, int fieldCount,
    FieldGraphicsItem* fieldItem, QRectF fieldRectangle, int fieldLineWidth, QBrush overlapBrush,
    QGraphicsScene* containingScene)
{
    for (int comparisonIndex = fieldIndex + 1; comparisonIndex < fieldCount; ++comparisonIndex)
    {
        FieldGraphicsItem* comparisonField = fieldItems_.at(comparisonIndex);

        QRectF comparisonRectangle = comparisonField->sceneBoundingRect();
        int comparisonLineWidth = comparisonField->pen().width();

        if (fieldOverlapsAnotherField(fieldRectangle, fieldLineWidth, comparisonRectangle, comparisonLineWidth))
        {
            fieldItem->setBrush(overlapBrush);
            comparisonField->setBrush(overlapBrush);

            fieldItem->addOverlappingFieldItem(comparisonField, overlapBrush, containingScene);
            comparisonField->addOverlappingFieldItem(fieldItem, overlapBrush, containingScene);
        }
        else
        {
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::fieldOverlapsAnotherField()
//-----------------------------------------------------------------------------
bool RegisterGraphicsItem::fieldOverlapsAnotherField(QRectF firstItemRectangle, int firstItemLineWidth,
    QRectF secondItemRectangle, int secondItemLineWidth) const
{
    qreal firstItemLeft = firstItemRectangle.left() + firstItemLineWidth;
    qreal firstItemRight = firstItemRectangle.right() - firstItemLineWidth;

    qreal secondItemLeft = secondItemRectangle.left() + secondItemLineWidth;
    qreal secondItemRight = secondItemRectangle.right() - secondItemLineWidth;

    return secondItemLeft <= firstItemRight && secondItemRight >= firstItemLeft;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::changeWidth()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::changeWidth(qreal widthChange)
{
    if (!isEmpty_)
    {
        qreal changePerBit = widthChange / registerSize_;
        qreal overallWidthChange = 0;
        qreal overallOverlapModifier = 0;

        quint64 previousOffset = registerSize_;
        quint64 previousLastBit = registerSize_;

        qreal subItemPosition = MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2;
        qreal fieldsStartPosition = boundingRect().left() + subItemPosition;

        for (FieldGraphicsItem* fieldItem : fieldItems_)
        {
            quint64 fieldOffset = fieldItem->getBaseAddress();
            quint64 fieldLastBit = fieldItem->getLastAddress();

            quint64 fieldItemWidth = getModifiedFieldWidth(fieldLastBit, fieldOffset);
            qreal fieldItemWidthChange = fieldItemWidth * changePerBit;

            fieldItem->changeWidth(fieldItemWidthChange);

            qreal overlapModifier = getOverlappingFieldPositionModifier(
                previousOffset, previousLastBit, fieldOffset, fieldLastBit, changePerBit);

            qreal newFieldPositionX = getNewFieldPositionX(fieldItem, fieldLastBit, fieldItemWidthChange,
                overallWidthChange, overallOverlapModifier, overlapModifier, fieldsStartPosition);

            fieldItem->setX(newFieldPositionX);

            if (fieldOffset < previousOffset)
            {
                previousOffset = fieldOffset;
                previousLastBit = fieldLastBit;
                overallWidthChange += fieldItemWidthChange;
                overallOverlapModifier += overlapModifier;
            }
        }

        for (FieldGraphicsItem* fieldItem : fieldItems_)
        {
            fieldItem->resizeAndRepositionOverlappingItems();
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
// Function: RegisterGraphicsItem::getOverlappingFieldPositionModifier()
//-----------------------------------------------------------------------------
qreal RegisterGraphicsItem::getOverlappingFieldPositionModifier(quint64 firstOffset, quint64 firstLastBit,
    quint64 secondOffset, quint64 secondLastbit, qreal changePerBit) const
{
    qreal overlapModifier = 0;

    if (secondLastbit < registerSize_ && secondLastbit >= firstOffset)
    {
        quint64 bitDifference = secondLastbit - firstOffset + 1;

        overlapModifier = bitDifference * changePerBit;
    }
    else if (firstLastBit > registerSize_ && secondLastbit >= registerSize_)
    {
        if (firstOffset >= registerSize_ - 2 || secondOffset >= registerSize_ - 2)
        {
            overlapModifier = 2 * changePerBit;
        }
        else
        {
            quint64 offsetMax = qMax(firstOffset, secondOffset);

            quint64 bitDifference = registerSize_ - offsetMax + 1;
            overlapModifier = bitDifference * changePerBit;
        }
    }

    return overlapModifier;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::getNewFieldPositionX()
//-----------------------------------------------------------------------------
qreal RegisterGraphicsItem::getNewFieldPositionX(FieldGraphicsItem* fieldItem, quint64 fieldLastBit,
    qreal fieldItemWidthChange, qreal overallWidthChange, qreal overallOverlapModifier,
    qreal currentOverlapModifier, qreal fieldsStartPosition) const
{
    qreal newFieldPositionX = 0;

    if (fieldLastBit >= registerSize_)
    {
        qreal fieldWidth = fieldItem->boundingRect().width();
        newFieldPositionX = fieldsStartPosition + fieldWidth / 2;
    }
    else
    {
        newFieldPositionX = fieldItem->pos().x() + fieldItemWidthChange / 2 + overallWidthChange -
            (overallOverlapModifier + currentOverlapModifier);
    }

    return newFieldPositionX;
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

//-----------------------------------------------------------------------------
// Function: RegisterGraphicsItem::setNewIdentifierChain()
//-----------------------------------------------------------------------------
void RegisterGraphicsItem::setNewIdentifierChain(QVector<QString> newIdentifiers)
{
    MemoryDesignerGraphicsItem::setNewIdentifierChain(newIdentifiers);

    for (FieldGraphicsItem* fieldItem : fieldItems_)
    {
        QVector<QString> newFieldIdentifierChain = newIdentifiers;
        newFieldIdentifierChain.append(fieldItem->name());

        fieldItem->setNewIdentifierChain(newFieldIdentifierChain);
    }
}
