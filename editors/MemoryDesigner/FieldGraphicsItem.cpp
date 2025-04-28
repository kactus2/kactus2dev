//-----------------------------------------------------------------------------
// File: FieldGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.11.2016
//
// Description:
// Graphics item for visualizing a field in the memory designer.
//-----------------------------------------------------------------------------

#include "FieldGraphicsItem.h"

#include <common/KactusColors.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/FieldOverlapItem.h>

#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::FieldGraphicsItem()
//-----------------------------------------------------------------------------
FieldGraphicsItem::FieldGraphicsItem(QSharedPointer<MemoryItem const> fieldItem, quint64 fieldOffset,
    quint64 fieldLastBit, qreal fieldWidth, quint64 fieldHeight, bool isEmptyField,
    QVector<QString> identifierChain, QFont labelFont,
    QSharedPointer<ConnectivityComponent const> containingInstance, bool isOutsideRegister,
    MemoryDesignerGraphicsItem* parentItem):
MemoryDesignerChildGraphicsItem(fieldItem, QStringLiteral("Field"), fieldOffset, fieldHeight, fieldWidth,
    identifierChain, containingInstance, parentItem),
combinedRangeLabel_(new QGraphicsTextItem("", this)),
fieldName_(fieldItem->getName()),
overlapIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/triangle_arrow_down.png"), this)),
fieldOffset_(fieldOffset),
fieldLastBit_(fieldLastBit),
isOutsideRegister_(isOutsideRegister),
isEmptyField_(isEmptyField)
{
    if (!fieldItem->getDisplayName().isEmpty())
    {
        fieldName_ = fieldItem->getDisplayName();
    }

    setupFieldItem(labelFont);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::FieldGraphicsItem()
//-----------------------------------------------------------------------------
FieldGraphicsItem::FieldGraphicsItem(FieldGraphicsItem const& other, MemoryDesignerGraphicsItem* parentItem):
MemoryDesignerChildGraphicsItem(other, parentItem),
combinedRangeLabel_(new QGraphicsTextItem("", this)),
fieldName_(other.fieldName_),
// overlapIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/triangle_arrow_down.png"), this))
overlapIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/triangle_arrow_down.png"), this)),
fieldOffset_(other.fieldOffset_),
fieldLastBit_(other.fieldLastBit_),
isOutsideRegister_(other.isOutsideRegister_),
isEmptyField_(other.isEmptyField_)
{
    setRect(other.rect());

    setupFieldItem(other.getNameLabel()->font());
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::setupFieldItem()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::setupFieldItem(QFont const& labelFont)
{
    qreal overlapIconPositionX = boundingRect().right() - overlapIcon_->boundingRect().width() - (GridSize / 2);
    qreal overlapIconPositionY = boundingRect().top() + (GridSize / 2);
    overlapIcon_->setPos(overlapIconPositionX, overlapIconPositionY);
    overlapIcon_->setVisible(false);

    getNameLabel()->setFont(labelFont);
    combinedRangeLabel_->setFont(labelFont);

    setupGraphicsItem(fieldOffset_, fieldLastBit_, QStringLiteral("Field"));
    setLabelPositions();

    QColor fieldColor = KactusColors::FIELD_COLOR;
    if (isOutsideRegister_)
    {
        QString toolTipAddition = QStringLiteral(
            "<br><br><b><font color=\"red\">This field is not contained within register</font></b>");
        addToToolTip(toolTipAddition);

        fieldColor = KactusColors::MISSING_COMPONENT;
    }

    setColors(fieldColor, isEmptyField_);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::~FieldGraphicsItem()
//-----------------------------------------------------------------------------
FieldGraphicsItem::~FieldGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::setEmptyItemRangeColors()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::setEmptyItemRangeColors(QColor emptyItemRangeColour)
{
    combinedRangeLabel_->setDefaultTextColor(emptyItemRangeColour);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::setLabelPositions()
{
    hideMemoryRangeLabels();

    QGraphicsTextItem* nameLabel = getNameLabel();

    if (combinedRangeLabel_->toPlainText().isEmpty())
    {
        QString rangeValue = QStringLiteral("");

        quint64 fieldBaseAddress = getBaseAddress();
        quint64 fieldLastAddress = getLastAddress();
        if (fieldBaseAddress == fieldLastAddress)
        {
            rangeValue = QString::number(fieldBaseAddress, 16).toUpper();
        }
        else
        {
            QString offset = QString::number(fieldBaseAddress, 16).toUpper();
            QString lastBit = QString::number(fieldLastAddress, 16).toUpper();

            rangeValue = lastBit + QStringLiteral("..") + offset;
        }

        combinedRangeLabel_->setPlainText(rangeValue);
    }

    qreal nameY = boundingRect().height() / 2 - nameLabel->boundingRect().height() / 2 - 6;
    qreal rangeY = nameY + nameLabel->boundingRect().height() / 2 + 1;
    qreal rangeX = - combinedRangeLabel_->boundingRect().width() / 2;

    nameLabel->setY(nameY);
    combinedRangeLabel_->setPos(rangeX, rangeY);

    fitNameToBoundaries(nameLabel);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::fitNameLabel()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::fitNameToBoundaries(QGraphicsTextItem* nameLabel)
{
    const int NAME_MARGIN = 14;

    QFontMetrics nameFontMetrics(nameLabel->font());
    unsigned int itemBoundingWidth = boundingRect().width() - NAME_MARGIN;

    QString elidedName = nameFontMetrics.elidedText(fieldName_, Qt::ElideRight, itemBoundingWidth);
    nameLabel->setPlainText(elidedName);
    nameLabel->setX(-nameLabel->boundingRect().width() / 2);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::condense()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::condense(qreal newItemHeight)
{
    QRectF itemRectangle = boundingRect();

    qreal itemXPosition = -itemRectangle.width() / 2;
    qreal itemWidth = itemRectangle.width() - 1;

    setRect(itemXPosition, 0, itemWidth, newItemHeight);
    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::changeWidth()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::changeWidth(qreal widthChange)
{
    qreal newWidth = boundingRect().width() + widthChange - 1;

    setRect(-newWidth / 2, 0, newWidth, boundingRect().height() - 1);

    QGraphicsTextItem* nameLabel = getNameLabel();

    if (widthChange > 0)
    {
        nameLabel->setPlainText(fieldName_);
    }

    qreal nameX = - nameLabel->boundingRect().width() / 2;
    nameLabel->setX(nameX);

    fitNameToBoundaries(nameLabel);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::resizeAndRepositionOverlappingItems()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::resizeAndRepositionOverlappingItems()
{
    if (!overlapFields_.isEmpty())
    {
        qreal overlapRectangleLeft = sceneBoundingRect().left();
        qreal overlapRectangleRight = sceneBoundingRect().right();
        foreach (FieldGraphicsItem::CombinedOverlappingFieldItem overlapCombination, overlapFields_)
        {
            FieldOverlapItem* overlapItem = overlapCombination.overlapItem_;
            FieldGraphicsItem* overlappedField = overlapCombination.overlappedField_;
            QRectF overlapFieldRectangle = overlappedField->sceneBoundingRect();
            int overlapFieldLineWidth = overlappedField->pen().width();
            overlapItem->resizeToRectangle(overlapFieldRectangle, overlapFieldLineWidth);

            qreal overlappedFieldCurrentPosition = overlappedField->scenePos().x();
            overlapItem->setX(overlappedFieldCurrentPosition);

            overlapRectangleLeft = qMin(overlapRectangleLeft, overlapItem->sceneBoundingRect().left());
            overlapRectangleRight = qMax(overlapRectangleRight, overlapItem->sceneBoundingRect().right());
        }

        qreal overlapWidth = overlapRectangleRight - overlapRectangleLeft;
        qreal newOverlapHeight = overlapAreaRectangle_->boundingRect().height() - overlapAreaRectangle_->pen().width();
        qreal overlapPositionX = overlapRectangleLeft + overlapWidth / 2;
        overlapAreaRectangle_->setRect(-overlapWidth / 2, 0, overlapWidth, newOverlapHeight);
        overlapAreaRectangle_->setX(overlapPositionX);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::getMaximumNeededWidthChange()
//-----------------------------------------------------------------------------
qreal FieldGraphicsItem::getNeededWithChangeToDisplayFullName() const
{
    qreal neededWithChangeToDisplayFullName = 0;

    QString currentName = getNameLabel()->toPlainText();
    if (currentName.compare(fieldName_, Qt::CaseInsensitive) != 0)
    {
        qreal currentWidth = boundingRect().width();

        getNameLabel()->setPlainText(fieldName_);
        qreal nameLabelWidth = getNameLabel()->boundingRect().width();

        neededWithChangeToDisplayFullName = nameLabelWidth - currentWidth + 12;

        quint64 fieldWidth = getLastAddress() - getBaseAddress() + 1;
        neededWithChangeToDisplayFullName = neededWithChangeToDisplayFullName / fieldWidth;
    }

    return neededWithChangeToDisplayFullName;
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::addOverlappingFieldItem()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::addOverlappingFieldItem(FieldGraphicsItem* overlappingFieldItem, QBrush overlapBrush,
    QGraphicsScene* containingScene)
{
    QRectF fieldRectangle = sceneBoundingRect();
    qreal overlapPositionY = fieldRectangle.bottom();

    if (!overlapAreaRectangle_)
    {
        createOverlapRectangle(overlapPositionY, fieldRectangle, containingScene);
    }

    qreal currentOverlapHeight = 0;
    foreach (CombinedOverlappingFieldItem overlapField, overlapFields_)
    {
        FieldOverlapItem* overlapItem = overlapField.overlapItem_;
        currentOverlapHeight += overlapItem->boundingRect().height();
    }

    setupOverlapRectangle(currentOverlapHeight, overlappingFieldItem);

    overlapPositionY += currentOverlapHeight;

    QFont labelFont = combinedRangeLabel_->font();
    FieldOverlapItem* newOverlapItem =
        new FieldOverlapItem(overlappingFieldItem, containingScene, overlapBrush, labelFont);

    newOverlapItem->setY(overlapPositionY);

    CombinedOverlappingFieldItem combinedOverlappingField;
    combinedOverlappingField.overlapItem_ = newOverlapItem;
    combinedOverlappingField.overlappedField_ = overlappingFieldItem;

    overlapFields_.append(combinedOverlappingField);

    if (!overlapIcon_->isVisible())
    {
        overlapIcon_->setVisible(true);
        setAcceptHoverEvents(true);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::createOverlapRectangle()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::createOverlapRectangle(qreal rectanglePositionY, QRectF fieldRectangle,
    QGraphicsScene* containingScene)
{
    int fieldLineWidth = pen().width();

    qreal overlapWidth = fieldRectangle.width() - fieldLineWidth;
    qreal overlapHeight = fieldRectangle.height() - fieldLineWidth;
    QRectF newOverlapRectangle(-overlapWidth / 2, 0, overlapWidth, overlapHeight);
    overlapAreaRectangle_ = new QGraphicsRectItem(newOverlapRectangle);

    overlapAreaRectangle_->setOpacity(0.9);

    QBrush overlapRectangleBrush(KactusColors::REGULAR_FIELD);
    overlapAreaRectangle_->setBrush(overlapRectangleBrush);

    overlapAreaRectangle_->setPos(scenePos().x(), rectanglePositionY);
    containingScene->addItem(overlapAreaRectangle_);

    overlapAreaRectangle_->hide();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::setupOverlapRectangle()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::setupOverlapRectangle(qreal currentOverlapHeight, FieldGraphicsItem* newOverlapItem)
{
    qreal newOverlapItemLineWidth = newOverlapItem->pen().width();

    QRectF overlappingItemRectangle = newOverlapItem->sceneBoundingRect();
    QRectF oldOverlapRectangle = overlapAreaRectangle_->sceneBoundingRect();

    qreal newOverlapRectangleLeft =
        qMin(oldOverlapRectangle.left(), overlappingItemRectangle.left() + newOverlapItemLineWidth);
    qreal newOverlapRectangleRight =
        qMax(oldOverlapRectangle.right(), overlappingItemRectangle.right() - newOverlapItemLineWidth);

    qreal overlapWidth = newOverlapRectangleRight - newOverlapRectangleLeft;
    qreal overlapHeight = currentOverlapHeight + overlappingItemRectangle.height();

    QRectF newOverlapRectangle(-overlapWidth / 2, 0, overlapWidth, overlapHeight);
    overlapAreaRectangle_->setRect(newOverlapRectangle);

    qreal newPositionX = newOverlapRectangleLeft + (overlapWidth / 2);
    overlapAreaRectangle_->setX(newPositionX);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::mousePressEvent()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !overlapFields_.isEmpty())
    {
        foreach (CombinedOverlappingFieldItem combinedOverlapField, overlapFields_)
        {
            FieldOverlapItem* overlapField = combinedOverlapField.overlapItem_;

            overlapField->show();
            overlapAreaRectangle_->show();

            combinedOverlapField.overlappedField_->hide();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void FieldGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    if (!overlapFields_.isEmpty())
    {
        foreach (CombinedOverlappingFieldItem combindedOverlapField, overlapFields_)
        {
            combindedOverlapField.overlapItem_->hide();
            overlapAreaRectangle_->hide();
            combindedOverlapField.overlappedField_->show();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldGraphicsItem::getCombinedRangeText()
//-----------------------------------------------------------------------------
QString FieldGraphicsItem::getCombinedRangeText() const
{
    return combinedRangeLabel_->toPlainText();
}
