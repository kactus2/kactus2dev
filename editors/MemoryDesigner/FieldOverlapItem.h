//-----------------------------------------------------------------------------
// File: FieldOverlapItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.02.2017
//
// Description:
// Graphics item for visualizing an overlapping field in the memory designer.
//-----------------------------------------------------------------------------

#ifndef FIELDOVERLAPITEM_H
#define FIELDOVERLAPITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

class FieldGraphicsItem;

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QBrush>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing an overlapping field in the memory designer.
//-----------------------------------------------------------------------------
class FieldOverlapItem : public QGraphicsRectItem
{

public:
    //! Type of this graphics item.
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  The constructor.
     *
     *    @param [in] overlappingFieldItem    Field graphics item from which this overlap item is created from.
     *    @param [in] containingScene         Graphics scene containing the field graphics item.
     *    @param [in] overlapBrush            Brush used to draw the overlap item.
     *    @param [in] labelFont               Font used by the labels.
     *    @param [in] parentItem              The parent graphics item.
     */
    FieldOverlapItem(FieldGraphicsItem* overlappingFieldItem, QGraphicsScene* containingScene, QBrush overlapBrush,
        QFont labelFont, QGraphicsItem* parentItem = 0);

	/*!
     *  The destructor.
     */
    virtual ~FieldOverlapItem();

    /*!
     *  Get the graphics type.
     */
    virtual int type() const { return Type; }

    /*!
     *  Resize this item to match the selected rectangle.
     *
     *    @param [in] rectangle           The selected rectangle.
     *    @param [in] rectangleLineWidth  Line width of the selected rectangle.
     */
    void resizeToRectangle(QRectF rectangle, int rectangleLineWidth);

private:
    // Disable copying.
    FieldOverlapItem(FieldOverlapItem const& rhs);
    FieldOverlapItem& operator=(FieldOverlapItem const& rhs);

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions();

    /*!
     *  Fit the name label to the boundaries of the item.
     *
     *    @param [in] nameLabel   The selected name label.
     */
    void fitNameToBoundaries(QGraphicsTextItem* nameLabel);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the overlapping field.
    QString fieldName_;

    //! Label containing the item name.
    QGraphicsTextItem* nameLabel_;

    //! Label containing the combined range value.
    QGraphicsTextItem* combinedRangeLabel_;
};

//-----------------------------------------------------------------------------

#endif // FIELDOVERLAPITEM_H
