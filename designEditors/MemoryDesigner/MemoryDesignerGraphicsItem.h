//-----------------------------------------------------------------------------
// File: MemoryDesignerGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.08.2016
//
// Description:
// Parent class for memory designer graphics items.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNERGRAPHICSITEM_H
#define MEMORYDESIGNERGRAPHICSITEM_H

#include <designEditors/common/diagramgrid.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>
#include <QGraphicsTextItem>

class MemoryColumn;
class MemoryConnectionItem;
class MemoryCollisionItem;

//-----------------------------------------------------------------------------
//! Parent class for memory designer graphics items.
//-----------------------------------------------------------------------------
class MemoryDesignerGraphicsItem : public QGraphicsRectItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] itemName    Name of the memory graphics item.
     *      @param [in] parent      The parent item.
     */
    MemoryDesignerGraphicsItem(QString const& itemName, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~MemoryDesignerGraphicsItem();

    /*!
     *  Get the type of the memory item.
     */
    virtual int type() const = 0;

    /*!
     *  Get the name of the memory item.
     *
     *      @return The name of the memory item.
     */
    QString name() const;

    /*!
     *  Hide the memory range labels.
     */
    void hideMemoryRangeLabels();

    /*!
     *  Get the base address of the memory item.
     *
     *      @return The base address of the memory item.
     */
    quint64 getBaseAddress() const;

    /*!
     *  Get the end address of the memory item.
     *
     *      @return The end address of the memory item.
     */
    quint64 getLastAddress() const;

    /*!
     *  Compress this graphics item.
     *
     *      @param [in] newItemHeight   The new height of the graphics item.
     */
    void condense(qreal newItemHeight);

protected:

    /*!
     *  Setup the graphics rectangle.
     *
     *      @param [in] rectangleWidth      Width of the graphics rectangle.
     *      @param [in] rectangleHeight     Height of the graphics rectangle.
     */
    void setGraphicsRectangle(qreal rectangleWidth, qreal rectangleHeight);

    /*!
     *  Setup the tooltip and the memory range labels.
     *
     *      @param [in] identifier      Selected identifier.
     *      @param [in] memoryStart     Start address of the memory item.
     *      @param [in] memoryEnd       End address of the memory item.
     */
    void setupToolTip(QString const& identifier, quint64 memoryStart, quint64 memoryEnd);

    /*!
     *  Get the name label.
     *
     *      @return The label containing the item name.
     */
    QGraphicsTextItem* getNameLabel() const;

    /*!
     *  Get the range start label.
     *
     *      @return The label containing the item range start.
     */
    QGraphicsTextItem* getRangeStartLabel() const;

    /*!
     *  Get the range end label.
     *
     *      @return The label containing the item range end.
     */
    QGraphicsTextItem* getRangeEndLabel() const;

    /*!
     *  Format the selected value to a hexadecimal value.
     *
     *      @param [in] range   The selected value.
     *
     *      @return The value formatted to hexadecimal.
     */
    QString getValueFormattedToHexadecimal(quint64 range) const;

private:
    // Disable copying.
    MemoryDesignerGraphicsItem(MemoryDesignerGraphicsItem const& rhs);
    MemoryDesignerGraphicsItem& operator=(MemoryDesignerGraphicsItem const& rhs);

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions() = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! The range start label.
    QGraphicsTextItem* startRangeLabel_;

    //! The range end label.
    QGraphicsTextItem* endRangeLabel_;

    //! Name of the item.
    QString itemName_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERGRAPHICSITEM_H
