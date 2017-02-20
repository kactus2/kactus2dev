//-----------------------------------------------------------------------------
// File: RegisterGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Graphics item for visualizing register in the memory designer.
//-----------------------------------------------------------------------------

#ifndef REGISTERGRAPHICSITEM_H
#define REGISTERGRAPHICSITEM_H

#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>

#include <common/graphicsItems/GraphicsItemTypes.h>

class MemoryItem;
class MemoryDesignerGraphicsItem;
class FieldGraphicsItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing register in the memory designer.
//-----------------------------------------------------------------------------
class RegisterGraphicsItem : public MemoryDesignerChildGraphicsItem
{

public:
    //! Type of this graphics item.
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] registerItem        Memory item containing the register data.
     *      @param [in] isEmptyRegister     Boolean value to represent empty registers.
     *      @param [in] registerWidth       Width of the register graphics item.
     *      @param [in] filterFields        Value for filtering register fields.
     *      @param [in] containingInstance  Name of the containing component instance.
     *      @param [in] parentItem          The parent memory graphics item.
     */
    RegisterGraphicsItem(QSharedPointer<MemoryItem> registerItem, bool isEmptyRegister, qreal registerWidth,
        bool filterFields, QString const& containingInstance, MemoryDesignerGraphicsItem* parentItem);

	/*!
     *  The destructor.
     */
    virtual ~RegisterGraphicsItem();

    /*!
     *  Get the graphics type.
     */
    int type() const { return Type; }

    /*!
     *  Condense this item and the contained field graphics items.
     *
     *      @param [in] newItemHeight   The condensed height.
     */
    virtual void condense(qreal newItemHeight);

    /*!
     *  Change the width of this item and the contained field graphics items.
     *
     *      @param [in] widthChange     The change in width.
     */
    void changeWidth(qreal widthChange);

    /*!
     *  Get the maximum needed change in width to fully display the name labels of all the contained field items.
     *
     *      @return The maximum needed change in width to display the name labels of all the field items.
     */
    qreal getMaximumNeededChangeInFieldWidth() const;

    /*!
     *  Create overlapping field graphics items.
     */
    void createOverlappingFieldMarkers();

private:
    // Disable copying.
    RegisterGraphicsItem(RegisterGraphicsItem const& rhs);
    RegisterGraphicsItem& operator=(RegisterGraphicsItem const& rhs);

    //! Field memory items with calculated field offset and width.
    struct FieldMemoryItem
    {
        //! Offset of the field item.
        quint64 fieldOffset;

        //! Width of the field item.
        quint64 fieldWidth;

        //! Memory item containing the field item data.
        QSharedPointer<MemoryItem> fieldMemoryItem;
    };

    /*!
     *  Get the register end.
     *
     *      @param [in] addressUnitBits     Address unit bits of the register.
     *      @param [in] registerSize        Size of the register.
     *
     *      @return The end range of the register item.
     */
    quint64 getRegisterEnd(unsigned int addressUnitBits, quint64 registerSize) const;

    /*!
     *  Setup the label positions.
     */
    virtual void setLabelPositions();

    /*!
     *  Setup the field graphics items.
     *
     *      @param [in] registerItem    The selected register memory item.
     */
    void setupFields(QSharedPointer<MemoryItem> registerItem);

    /*!
     *  Get the field items in last bit order.
     *
     *      @param [in] registerItem    Memory item containing the register data.
     *
     *      @return Field items in last bit order with calculated offsets and widths.
     */
    QMap<quint64, RegisterGraphicsItem::FieldMemoryItem> getFieldItemsInLastBitOrder(
        QSharedPointer<MemoryItem> registerItem) const;

    /*!
     *  Create an empty field graphics item.
     *
     *      @param [in] currentOffset           Offset of the field.
     *      @param [in] lastBit                 Last bit used by the field.
     *      @param [in] oneBitWidth             Width of a one bit.
     *      @param [in] registerEnd             End of the containing register.
     *      @param [in] fieldsStartPosition     The start coordinate for the field graphics items.
     *      @param [in] fieldFont               Font used in fields.
     */
    void createEmptyFieldItem(quint64 currentOffset, quint64 lastBit, qreal oneBitWidth, quint64 registerEnd,
        qreal fieldsStartPosition, QFont fieldFont);

    /*!
     *  Create a field graphics item.
     *
     *      @param [in] fieldName               Name of the field.
     *      @param [in] fieldOffset             Offset of the field.
     *      @param [in] fieldWidth              Width of the field.
     *      @param [in] isEmptyField            Holds whether the field is empty or not.
     *      @param [in] oneBitWidth             Width of a one bit.
     *      @param [in] registerEnd             End of the containing register.
     *      @param [in] fieldsStartPosition     The start coordinate for the field graphics items.
     *      @param [in] fieldFont               Font used in fields.
     */
    void createFieldGraphicsItem(QString const& fieldName, quint64 fieldOffset, quint64 fieldWidth,
        bool isEmptyField, qreal oneBitWidth, quint64 registerEnd, qreal fieldsStartPosition,
        QFont fieldFont);

    /*!
     *  Get the width available for the register item.
     *
     *      @return The width available for the register item.
     */
    virtual qreal getItemWidth() const;

    /*!
     *  Create overlapping markers for the selected field.
     *
     *      @param [in] fieldIndex          Index of the selected field.
     *      @param [in] fieldCount          Number of fields contained within the register.
     *      @param [in] fieldItem           The selected field item.
     *      @param [in] fieldRectangle      Bounding rectangle of the field item.
     *      @param [in] fieldLineWidth      Line width of the field item.
     *      @param [in] overlapBrush        Brush used to draw the field item.
     *      @param [in] containingScene     Scene containing field item.
     */
    void createOverlappingMarkersForField(int fieldIndex, int fieldCount, FieldGraphicsItem* fieldItem,
        QRectF fieldRectangle, int fieldLineWidth, QBrush overlapBrush, QGraphicsScene* containingScene);

    /*!
     *  Check if a field overlaps another field.
     *
     *      @param [in] firstItemRectangle      Bounding rectangle of the first field item.
     *      @param [in] firstItemLineWidth      Line width of the first field item.
     *      @param [in] secondItemRectangle     Bounding rectangle of the second field item.
     *      @param [in] secondItemLineWidth     Line width of the first field item.
     *
     *      @return True, if the field overlaps another field, false otherwise.
     */
    bool fieldOverlapsAnotherField(QRectF firstItemRectangle, int firstItemLineWidth, QRectF secondItemRectangle,
        int secondItemLineWidth) const;

    /*!
     *  Get the x coordinate modifier for the overlapping field.
     *
     *      @param [in] firstOffset     Offset of the first field.
     *      @param [in] firstLastBit    Last bit of the first field.
     *      @param [in] secondOffset    Offset of the second field.
     *      @param [in] secondLastbit   Last bit of the second field.
     *      @param [in] changePerBit    Width change per bit.
     *
     *      @return The x coordinate modifier for the overlapping field.
     */
    qreal getOverlappingFieldPositionModifier(quint64 firstOffset, quint64 firstLastBit, quint64 secondOffset,
        quint64 secondLastbit, qreal changePerBit) const;

    /*!
     *  Get the new x coordinate for the selected field item.
     *
     *      @param [in] fieldItem               The selected field item.
     *      @param [in] fieldLastBit            Last bit of the selected field.
     *      @param [in] fieldItemWidthChange    Width change of the selected field item.
     *      @param [in] overallWidthChange      Overall width change of the contained field items.
     *      @param [in] overallOverlapModifier  Overlap modifier of the selected field item.
     *      @param [in] currentOverlapModifier  Overall overlap modifier of the contained field items.
     *      @param [in] fieldsStartPosition     Start position of the field items.
     *
     *      @return The new x coordinate.
     */
    qreal getNewFieldPositionX(FieldGraphicsItem* fieldItem, quint64 fieldLastBit, qreal fieldItemWidthChange,
        qreal overallWidthChange, qreal overallOverlapModifier, qreal currentOverlapModifier,
        qreal fieldsStartPosition) const;

    /*!
     *  Get the field width modified to fit into the register bounds.
     *
     *      @param [in] fieldLastBit    Last bit of the selected field.
     *      @param [in] fieldOffset     Offset of the selected field.
     *
     *      @return Modified width of the field item.
     */
    quint64 getModifiedFieldWidth(quint64 fieldLastBit, quint64 fieldOffset) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the field graphics items contained within.
    QVector<FieldGraphicsItem*> fieldItems_;

    //! Value for empty registers.
    bool isEmpty_;

    //! Size of the register.
    quint64 registerSize_;

    //! Address unit bits of the register.
    unsigned int addressUnitBits_;

    //! Value for filtering fields.
    bool filterFields_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKGRAPHICSITEM_H
