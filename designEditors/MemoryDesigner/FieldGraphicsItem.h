//-----------------------------------------------------------------------------
// File: FieldGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.11.2016
//
// Description:
// Graphics item for visualizing a field in the memory designer.
//-----------------------------------------------------------------------------

#ifndef FIELDGRAPHICSITEM_H
#define FIELDGRAPHICSITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>

class MemoryItem;
class MemoryDesignerChildGraphicsItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing a field in the memory designer.
//-----------------------------------------------------------------------------
class FieldGraphicsItem : public MemoryDesignerChildGraphicsItem
{

public:
    //! Type of this graphics item.
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] fieldName           Name of the field.
     *      @param [in] fieldOffset         Offset of the field.
     *      @param [in] fieldLastBit        Last bit used by the field.
     *      @param [in] fieldWidth          Width of the field.
     *      @param [in] fieldHeight         Height of the field.
     *      @param [in] isEmptyField        Value for empty fields.
     *      @param [in] parentItem          The parent graphics item.
     */
    FieldGraphicsItem(QString const& fieldName, quint64 fieldOffset, quint64 fieldLastBit, qreal fieldWidth,
        quint64 fieldHeight, bool isEmptyField, MemoryDesignerGraphicsItem* parentItem);

	/*!
     *  The destructor.
     */
    virtual ~FieldGraphicsItem();

    /*!
     *  Get the graphics type.
     */
    virtual int type() const { return Type; }

    /*!
     *  Condense this graphics item.
     *
     *      @param [in] newItemHeight   The new height.
     */
    virtual void condense(qreal newItemHeight);

    /*!
     *  Change the width.
     *
     *      @param [in] widthChange     Change in width..
     */
    void changeWidth(qreal widthChange);

private:
    // Disable copying.
    FieldGraphicsItem(FieldGraphicsItem const& rhs);
    FieldGraphicsItem& operator=(FieldGraphicsItem const& rhs);

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions();

    /*!
     *  Get the last address of the field item.
     *
     *      @param [in] fieldItem   The selected field memory item.
     *
     *      @return The field last address.
     */
    quint64 getFieldLastAddress(QSharedPointer<MemoryItem> fieldItem) const;

    /*!
     *  Remove zeros from the front of a given value.
     *
     *      @param [in] rangeValue  The selected value.
     *
     *      @return The range value in string format with zeros removed from the front.
     */
    QString removeZerosFromRangeValue(quint64 rangeValue) const;

    /*!
     *  Fit the name label to the boundaries of the item.
     *
     *      @param [in] nameLabel   The selected name label.
     */
    void fitNameToBoundaries(QGraphicsTextItem* nameLabel);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Value for empty fields.
    bool isEmpty_;

    //! Label for the combined range value.
    QGraphicsTextItem* combinedRangeLabel_;

    //! Height of the field.
    qreal fieldHeight_;

    //! The original name of the field.
    QString fieldName_;
};

//-----------------------------------------------------------------------------

#endif // FIELDGRAPHICSITEM_H
