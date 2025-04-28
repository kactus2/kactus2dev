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

#include <editors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>

class MemoryItem;
class MemoryDesignerChildGraphicsItem;
class FieldOverlapItem;

#include <QSharedPointer>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

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
     *    @param [in] fieldName           Name of the field.
     *    @param [in] displayName         Display name of the field.
     *    @param [in] fieldOffset         Offset of the field.
     *    @param [in] fieldLastBit        Last bit used by the field.
     *    @param [in] fieldWidth          Width of the field.
     *    @param [in] fieldHeight         Height of the field.
     *    @param [in] isEmptyField        Value for empty fields.
     *    @param [in] identifierChain     Chain of strings identifying this item.
     *    @param [in] labelFont           Font for the field labels.
     *    @param [in] containingInstance  Name of the containing component instance.
     *    @param [in] isOutsideRegister   Holds whether the field is out of bounds of the register or not.
     *    @param [in] parentItem          The parent graphics item.
     */
    FieldGraphicsItem(QSharedPointer<MemoryItem const> fieldItem, quint64 fieldOffset, quint64 fieldLastBit,
        qreal fieldWidth, quint64 fieldHeight, bool isEmptyField, QVector<QString> identifierChain,
        QFont labelFont, QSharedPointer<ConnectivityComponent const> containingInstance, bool isOutsideRegister,
        MemoryDesignerGraphicsItem* parentItem);

    /*!
     *  The copy constructor.
     *
     *    @param [in] other     The original field item.
     *
     *    @return Copy of the original field item.
     */
    FieldGraphicsItem(FieldGraphicsItem const& other, MemoryDesignerGraphicsItem* parentItem);

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
     *    @param [in] newItemHeight   The new height.
     */
    virtual void condense(qreal newItemHeight);

    /*!
     *  Change the width.
     *
     *    @param [in] widthChange     Change in width..
     */
    void changeWidth(qreal widthChange);

    /*!
     *  Get the needed change in width to fully display the name label within this item.
     *
     *    @return The needed change in width to fully display the name label within this item.
     */
    qreal getNeededWithChangeToDisplayFullName() const;

    /*!
     *  Add an overlap field item from the overlapping field graphics item.
     *
     *    @param [in] overlappingFieldItem    The overlapping field graphics item.
     *    @param [in] overlapBrush            Brush used to draw the overlap item.
     *    @param [in] containingScene         Scene containing the overlapping field item.
     */
    void addOverlappingFieldItem(FieldGraphicsItem* overlappingFieldItem, QBrush overlapBrush,
        QGraphicsScene* containingScene);

    /*!
     *  Get the text from the combined range label.
     *
     *    @return The text contained within the combined range label.
     */
    QString getCombinedRangeText() const;

    /*!
     *  Resize and reposition the overlapping items when the fields are extended.
     */
    void resizeAndRepositionOverlappingItems();

protected:

    /*!
     *  Handler for the mouse press event.
     *
     *    @param [in] event   The mouse press event.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Handler for the hover leave event.
     *
     *    @param [in] event   The hover leave event.
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    // Disable copying.
    FieldGraphicsItem& operator=(FieldGraphicsItem const& rhs);

    /*!
     *  Setup the field item.
     *
     *    @param [in] labelFont     Font used for the labels.
     */
    void setupFieldItem(QFont const& labelFont);

    /*!
     *  Set colors for ranges in an empty field graphics item.
     *
     *    @param [in] emptyItemRangeColour    The color for the empty field item ranges.
     */
    virtual void setEmptyItemRangeColors(QColor emptyItemRangeColour);

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

    /*!
     *  Create the rectangle for containing the overlapping field items.
     *
     *    @param [in] rectanglePositionY  Y coordinate of the rectangle.
     *    @param [in] fieldRectangle      The field bounding rectangle.
     *    @param [in] containingScene     Scene containing the field item.
     */
    void createOverlapRectangle(qreal rectanglePositionY, QRectF fieldRectangle, QGraphicsScene* containingScene);

    /*!
     *  Setup the rectangle containing the overlapping field items.
     *
     *    @param [in] currentOverlapHeight    Current height of the overlapping rectangle.
     *    @param [in] newOverlapItem          The new overlapping field item.
     */
    void setupOverlapRectangle(qreal currentOverlapHeight, FieldGraphicsItem* newOverlapItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Overlapping item containing both the overlapping field graphics item and the overlap field item
    //! constructed from it.
    struct CombinedOverlappingFieldItem
    {
        //! The overlap field item.
        FieldOverlapItem* overlapItem_;

        //! The overlapped field graphics item.
        FieldGraphicsItem* overlappedField_;
    };

    //! Label for the combined range value.
    QGraphicsTextItem* combinedRangeLabel_;

    //! The original name of the field.
    QString fieldName_;

    //! List of all the overlapped field items.
    QVector<CombinedOverlappingFieldItem> overlapFields_;

    //! Icon to display the overlapped field items.
    QGraphicsPixmapItem* overlapIcon_;

    //! Rectangle for containing all the overlapped field graphics items.
    QGraphicsRectItem* overlapAreaRectangle_ = nullptr;

    //! Offset of the field.
    quint64 fieldOffset_ = 0;

    //! Last bit used by the field.
    quint64 fieldLastBit_ = 0;

    //! Flag for fields outside the containing register.
    bool isOutsideRegister_ = false;

    //! Flag for an empty field.
    bool isEmptyField_ = false;
};

//-----------------------------------------------------------------------------

#endif // FIELDGRAPHICSITEM_H
