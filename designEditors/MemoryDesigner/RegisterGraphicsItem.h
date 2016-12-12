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

private:
    // Disable copying.
    RegisterGraphicsItem(RegisterGraphicsItem const& rhs);
    RegisterGraphicsItem& operator=(RegisterGraphicsItem const& rhs);

    /*!
     *  Get the register end.
     *
     *      @param [in] registerItem    Memory item containing register data.
     *
     *      @return The end range of the register item.
     */
    quint64 getRegisterEnd(QSharedPointer<MemoryItem> registerItem) const;

    /*!
     *  Setup the label positions.
     */
    virtual void setLabelPositions();

    /*!
     *  Setup the field graphics items.
     *
     *      @param [in] registerItem    The selected register memory item.
     *      @param [in] registerEnd     End of the register memory item.
     */
    void setupFields(QSharedPointer<MemoryItem> registerItem, quint64 registerEnd);

    /*!
     *  Get the field memory items of the selected register memory item in the order of their offset.
     *
     *      @param [in] registerItem    The selected register memory item.
     *
     *      @return Map containing field offset and field memory item pairs.
     */
    QMap<quint64, QSharedPointer<MemoryItem> > getFieldItemsInOffsetOrder(QSharedPointer<MemoryItem> registerItem)
        const;

    /*!
     *  Create an empty field graphics item.
     *
     *      @param [in] currentOffset           Offset of the field.
     *      @param [in] lastBit                 Last bit used by the field.
     *      @param [in] oneBitWidth             Width of a one bit.
     *      @param [in] registerEnd             End of the containing register.
     *      @param [in] previousEndPosition     The end coordinate of the previous field graphics item.
     *      @param [in] widthRemainder          Current remainder of the used widths.
     *      @param [in] fieldFont               Font used in fields.
     */
    void createEmptyFieldItem(quint64 currentOffset, quint64 lastBit, qreal oneBitWidth, quint64 registerEnd,
        qreal& previousEndPosition, qreal& widthRemainder, QFont fieldFont);

    /*!
     *  Create a field graphics item.
     *
     *      @param [in] fieldName               Name of the field.
     *      @param [in] fieldOffset             Offset of the field.
     *      @param [in] fieldWidth              Width of the field.
     *      @param [in] isEmptyField            Holds whether the field is empty or not.
     *      @param [in] oneBitWidth             Width of a one bit.
     *      @param [in] registerEnd             End of the containing register.
     *      @param [in] previousEndPosition     The end coordinate of the previous field graphics item.
     *      @param [in] widthRemainder          Current remainder of the used widths.
     *      @param [in] fieldFont               Font used in fields.
     */
    void createFieldGraphicsItem(QString const& fieldName, quint64 fieldOffset, quint64 fieldWidth,
        bool isEmptyField, qreal oneBitWidth, quint64 registerEnd, qreal& previousEndPosition,
        qreal& widthRemainder, QFont fieldFont);

    /*!
     *  Get the width available for the register item.
     *
     *      @return The width available for the register item.
     */
    virtual qreal getItemWidth() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the field graphics items contained within.
    QVector<FieldGraphicsItem*> fieldItems_;

    //! Value for empty registers.
    bool isEmpty_;

    //! Width of the register.
    qreal registerWidth_;

    //! Register memory item used to construct this graphics item.
    QSharedPointer<MemoryItem> registerMemoryItem_;

    //! Value for filtering fields.
    bool filterFields_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKGRAPHICSITEM_H
