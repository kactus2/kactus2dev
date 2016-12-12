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
     *      @param [in] itemName        Name of the memory graphics item.
     *      @param [in] instanceName    Name of the containing component instance.
     *      @param [in] parent          The parent item.
     */
    MemoryDesignerGraphicsItem(QString const& itemName, QString const& instanceName, QGraphicsItem* parent = 0);

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
     *  Hide the start memory range label.
     */
    void hideStartRangeLabel();

    /*!
     *  Hide the end memory range label.
     */
    void hideEndRangeLabel();

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
     *  Set a new base address.
     *
     *      @param [in] newBaseAddress  The new base address.
     */
    void setBaseAddress(quint64 newBaseAddress);

    /*!
     *  Set a new last address.
     *
     *      @param [in] newLastAddress  The new last address.
     */
    void setLastAddress(quint64 newLastAddress);

    /*!
     *  Compress this graphics item.
     *
     *      @param [in] newItemHeight   The new height of the graphics item.
     */
    virtual void condense(qreal newItemHeight);

    /*!
     *  Add a memory connection graphics item.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    virtual void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Get the connected memory connection items.
     *
     *      @return A map containing the memory connection items in the order of their base addresses.
     */
    QMap<quint64, MemoryConnectionItem*> getMemoryConnections() const;

    /*!
     *  Get the connected memory connection items in vector formate.
     *
     *      @return The connected memory connection items in vector format.
     */
    QVector<MemoryConnectionItem*> getConnectionsInVector() const;

    /*!
     *  Fit the selected label to this item.
     *
     *      @param [in] label   The selected label.
     */
    virtual void fitLabel(QGraphicsTextItem* label);

    /*!
     *  Get the name of the containing component instance.
     *
     *      @return Name of the containing component instance.
     */
    QString getContainingInstance() const;

protected:

    /*!
     *  Setup the graphics rectangle.
     *
     *      @param [in] rectangleWidth      Width of the graphics rectangle.
     *      @param [in] rectangleHeight     Height of the graphics rectangle.
     */
    void setGraphicsRectangle(qreal rectangleWidth, qreal rectangleHeight);

    /*!
     *  Setup range labels.
     *
     *      @param [in] memoryStart     The range start.
     *      @param [in] memoryEnd       The range end.
     */
    void setupLabels(quint64 memoryStart, quint64 memoryEnd);

    /*!
     *  Setup the tooltip and the memory range labels.
     *
     *      @param [in] identifier      Selected identifier.
     */
    void setupToolTip(QString const& identifier);

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

    /*!
     *  Check if the selected label collides with range labels.
     *
     *      @param [in] label   The selected label.
     *
     *      @return True, if the selected label collides with range labels, otherwise false.
     */
    virtual bool labelCollidesWithRangeLabels(QGraphicsTextItem* label) const;

private:
    // Disable copying.
    MemoryDesignerGraphicsItem(MemoryDesignerGraphicsItem const& rhs);
    MemoryDesignerGraphicsItem& operator=(MemoryDesignerGraphicsItem const& rhs);

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions() = 0;

    /*!
     *  Get the amount numbers used to display memory ranges.
     *
     *      @param [in] memoryStart     Range start.
     *      @param [in] memoryEnd       Range end.
     *
     *      @return The amount of numbers used to display memory ranges.
     */
    int getAmountOfLabelNumbers(quint64 memoryStart, quint64 memoryEnd) const;

    /*!
     *  Get the available width of this item.
     *
     *      @return The available width of this item.
     */
    virtual qreal getItemWidth() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! The range start label.
    QGraphicsTextItem* startRangeLabel_;

    //! The range end label.
    QGraphicsTextItem* endRangeLabel_;

    //! The base address.
    quint64 baseAddress_;

    //! The last address.
    quint64 lastAddress_;

    //! Name of the item.
    QString itemName_;

    //! The amount of numbers used to display memory ranges.
    int amountOfLabelNumbers_;

    //! Name of the containing component instance.
    QString instanceName_;

    //! Map containing memory connection items and their base addresses.
    QMap<quint64, MemoryConnectionItem*> memoryConnections_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERGRAPHICSITEM_H
