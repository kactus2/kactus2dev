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
     *  Check if the graphics item is connected to the selected connection item.
     *
     *      @param [in] comparisonConnectionItem    The selected connection item.
     *
     *      @return True, if the graphics item is connected to the selected connection item, false otherwise.
     */
    bool hasConnection(MemoryConnectionItem* comparisonConnectionItem) const;

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

    /*!
     *  Change the address range of the memory item.
     *
     *      @param [in] offset  The offset of the parent item.
     */
    virtual void changeAddressRange(quint64 offset);

    /*!
     *  Compress this item to contain the selected addresses.
     *
     *      @param [in] uncutAddresses  The addresses that remain after the compression.
     *      @param [in] CUTMODIFIER     Modifier for the cut areas.
     */
    virtual void compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER);

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
     *  Add text to the tooltip.
     *
     *      @param [in] toolTipAddition     Text to be added to the tooltip.
     */
    void addToToolTip(QString const& toolTipAddition);

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
     *  Check if the selected label collides with range labels.
     *
     *      @param [in] label       The selected label.
     *      @param [in] fontHeight  Height of the text used in the selected label.
     *
     *      @return True, if the selected label collides with range labels, otherwise false.
     */
    virtual bool labelCollidesWithRangeLabels(QGraphicsTextItem* label, qreal fontHeight) const;

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions() = 0;

private:
    // Disable copying.
    MemoryDesignerGraphicsItem(MemoryDesignerGraphicsItem const& rhs);
    MemoryDesignerGraphicsItem& operator=(MemoryDesignerGraphicsItem const& rhs);

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

    //! Name of the containing component instance.
    QString instanceName_;

    //! Map containing memory connection items and their base addresses.
    QMap<quint64, MemoryConnectionItem*> memoryConnections_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERGRAPHICSITEM_H
