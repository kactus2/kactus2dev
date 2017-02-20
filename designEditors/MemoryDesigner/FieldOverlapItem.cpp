//-----------------------------------------------------------------------------
// File: FieldOverlapItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.02.2017
//
// Description:
// Graphics item for visualizing an overlapping field in the memory designer.
//-----------------------------------------------------------------------------

#include "FieldOverlapItem.h"

#include <designEditors/MemoryDesigner/FieldGraphicsItem.h>

#include <QFontMetrics>

//-----------------------------------------------------------------------------
// Function: FieldOverlapItem::FieldOverlapItem()
//-----------------------------------------------------------------------------
FieldOverlapItem::FieldOverlapItem(FieldGraphicsItem* overlappingFieldItem, QGraphicsScene* containingScene,
    QBrush overlapBrush, QFont labelFont, QGraphicsItem* parentItem):
QGraphicsRectItem(parentItem),
fieldName_(overlappingFieldItem->name()),
nameLabel_(new QGraphicsTextItem(this)),
combinedRangeLabel_(new QGraphicsTextItem(this))
{
    nameLabel_->setPlainText(fieldName_);
    combinedRangeLabel_->setPlainText(overlappingFieldItem->getCombinedRangeText());

    nameLabel_->setFont(labelFont);
    combinedRangeLabel_->setFont(labelFont);

    int overlapLineWidth = overlappingFieldItem->pen().width();
    qreal rectangleWidth = overlappingFieldItem->boundingRect().width() - overlapLineWidth;
    qreal rectangleHeight = overlappingFieldItem->boundingRect().height() - overlapLineWidth;
    QRectF overlapRectangle(-rectangleWidth / 2, 0, rectangleWidth, rectangleHeight);

    setRect(overlapRectangle);
    setPos(overlappingFieldItem->scenePos());

    setBrush(overlapBrush);
    containingScene->addItem(this);

    QPointF namePosition = nameLabel_->scenePos();
    QPointF combinPosition = combinedRangeLabel_->scenePos();

    setLabelPositions();

    QPointF namePositionSecond = nameLabel_->scenePos();
    QPointF combinPositionSecond = combinedRangeLabel_->scenePos();

    hide();
}

//-----------------------------------------------------------------------------
// Function: FieldOverlapItem::~FieldOverlapItem()
//-----------------------------------------------------------------------------
FieldOverlapItem::~FieldOverlapItem()
{

}

//-----------------------------------------------------------------------------
// Function: FieldOverlapItem::setLabelPositions()
//-----------------------------------------------------------------------------
void FieldOverlapItem::setLabelPositions()
{
    qreal nameY = boundingRect().height() / 2 - nameLabel_->boundingRect().height() / 2 - 6;
    qreal rangeY = nameY + nameLabel_->boundingRect().height() / 2 + 1;
    qreal rangeX = - combinedRangeLabel_->boundingRect().width() / 2;

    nameLabel_->setY(nameY);
    combinedRangeLabel_->setPos(rangeX, rangeY);

    fitNameToBoundaries(nameLabel_);
}

//-----------------------------------------------------------------------------
// Function: FieldOverlapItem::fitNameToBoundaries()
//-----------------------------------------------------------------------------
void FieldOverlapItem::fitNameToBoundaries(QGraphicsTextItem* nameLabel)
{
    const int NAME_MARGIN = 14;

    QFontMetrics nameFontMetrics(nameLabel->font());
    unsigned int itemBoundingWidth = boundingRect().width() - NAME_MARGIN;

    QString elidedName = nameFontMetrics.elidedText(fieldName_, Qt::ElideRight, itemBoundingWidth);
    nameLabel->setPlainText(elidedName);
    nameLabel->setX(-nameLabel->boundingRect().width() / 2);
}

//-----------------------------------------------------------------------------
// Function: FieldOverlapItem::resizeToRectangle()
//-----------------------------------------------------------------------------
void FieldOverlapItem::resizeToRectangle(QRectF rectangle, int rectangleLineWidth)
{
    qreal newWidth = rectangle.width() - rectangleLineWidth;
    qreal newHeight = rectangle.height() - rectangleLineWidth;

    setRect(-newWidth / 2, 0, newWidth, newHeight);
}
