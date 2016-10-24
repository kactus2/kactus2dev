//-----------------------------------------------------------------------------
// File: AddressSpaceGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Graphics item for visualizing an address space in the memory designer.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEGRAPHICSITEM_H
#define ADDRESSSPACEGRAPHICSITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>

class AddressSpace;
class MemoryItem;
class ConnectivityComponent;
class AddressSegmentGraphicsItem;

#include <QSharedPointer>
#include <QGraphicsPixmapItem>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing address spaces in the memory designer.
//-----------------------------------------------------------------------------
class AddressSpaceGraphicsItem : public MainMemoryGraphicsItem
{

public:
    //! The type of the address space graphics item.
    enum { Type = GFX_TYPE_ADDRESS_SPACE_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] memoryItem          Memory item containing address space data.
     *      @param [in] containingInstance  Data of the containing component instance.
     *      @param [in] parent              The parent item.
     */
    AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
        QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~AddressSpaceGraphicsItem();

    /*!
     *  Get the type of the address space graphics item.
     */
    int type() const { return Type; }

    /*!
     *  Compress the sub items contained within the address space and the space item.
     */
    virtual void condenseItemAndChildItems();

    /*!
     *  Get the minimum required height of the address space item to fit the selected memory connection.
     *
     *      @param [in] connectionBaseAddress   Base address of the selected memory connection.
     *      @param [in] connectionEndAddress    End address of the selected memory connection.
     *
     *      @return The minimum required height of the address space item.
     */
    virtual qreal getMinimumRequiredHeight(quint64 connectionBaseAddress, quint64 connectionEndAddress) const;

private:
    // Disable copying.
    AddressSpaceGraphicsItem(AddressSpaceGraphicsItem const& rhs);
    AddressSpaceGraphicsItem& operator=(AddressSpaceGraphicsItem const& rhs);

    /*!
     *  Set the positions of the labels.
     */
    virtual void setLabelPositions();

    /*!
     *  Create a new address segment graphics item.
     *
     *      @param [in] subMemoryItem   Memory item containing address segment data.
     *      @param [in] isEmpty         Boolean value for an empty address segment.
     *
     *      @return The created address segment graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem> subMemoryItem,
        bool isEmpty);

    /*!
     *  Create an empty address segment graphics item.
     *
     *      @param [in] beginAddress    Base address of the empty address segment graphics item.
     *      @param [in] rangeEnd        End address of the empty address segment graphics item.
     *
     *      @return The created address segment graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd);

    /*!
     *  Get the y-coordinate transfer for the connected memory items.
     *
     *      @param [in] currentConnectionBaseAddress    Base address of the current connection.
     *      @param [in] connectionRangeEnd              Range end of the current connection.
     *      @param [in] previousConnectionLow           Scene low point of the connection item.
     *      @param [in] connectionItem                  The connection item.
     *      @param [in] yTransfer                       Previous y-coordinate transfer.
     *
     *      @return The y-coordinate transfer.
     */
    qreal getTransferY(quint64 currentConnectionBaseAddress, quint64 connectionRangeEnd,
        quint64 previousConnectionLow, MemoryConnectionItem* connectionItem, qreal yTransfer) const;

    /*!
     *  Move the selected memory connection item.
     *
     *      @param [in] connectionItem      Memory connection item.
     *      @param [in] yTransfer           Item y transfer.
     *      @param [in] movedConnections    The already moved memory connection items.
     */
    void moveConnectionItem(MemoryConnectionItem* connectionItem, qreal yTransfer,
        QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections);

    /*!
     *  Get the height for a compressed memory sub item.
     *
     *      @param [in] mainItem                The parent item of the selected memory sub item.
     *      @param [in] subItem                 The selected memory sub item.
     *      @param [in] minimumSubItemHeight    Minimum height for the memory sub item.
     *      @param [in] yPosition               Y-coordinate for the compressed memory sub item.
     *      @param [in] movedConnections        Connections that have been moved previously.
     *
     *      @return The compressed height for the memory sub item.
     */
    virtual qreal getSubItemHeight(SubMemoryLayout* mainItem, MemoryDesignerChildGraphicsItem* subItem,
        qreal minimumSubItemHeight, quint64 yPosition,
        QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The icon for an address space with a CPU.
    QGraphicsPixmapItem* cpuIcon_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSSPACEGRAPHICSITEM_H
