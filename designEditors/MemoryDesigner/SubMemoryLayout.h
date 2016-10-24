//-----------------------------------------------------------------------------
// File: SubMemoryLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.09.2016
//
// Description:
// Layout for sub memory items of a memory graphics item within memory designer.
//-----------------------------------------------------------------------------

#ifndef SUBMEMORYLAYOUT_H
#define SUBMEMORYLAYOUT_H

class MemoryItem;
class MemoryDesignerGraphicsItem;
class MemoryDesignerChildGraphicsItem;
class MemoryConnectionItem;
class MainMemoryGraphicsItem;

#include <QSharedPointer>
#include <QVector>
#include <QMap>

//-----------------------------------------------------------------------------
//! Layout for sub memory items of a memory graphics item within memory designer.
//-----------------------------------------------------------------------------
class SubMemoryLayout
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] memoryItem          Memory item containing main memory item data.
     *      @param [in] subItemType         Type of the sub items.
     *      @param [in] mainGraphicsItem    The main graphics item.
     */
    SubMemoryLayout(QSharedPointer<MemoryItem> memoryItem, QString const& subItemType,
        MemoryDesignerGraphicsItem* mainGraphicsItem);

	/*!
     *  The destructor.
     */
    virtual ~SubMemoryLayout();

    /*!
     *  Change the ranges of the child items.
     *
     *      @param [in] offset  The offset of the main item.
     */
    void changeChildItemRanges(quint64 offset);

    /*!
     *  Get the sub memory items.
     *
     *      @return A map containing the sub memory items with the base addresses.
     */
    QMap<quint64, MemoryDesignerChildGraphicsItem*> getSubMemoryItems() const;

    /*!
     *  Add the memory connection item to the correct memory sub items.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    void addConnectionToSubItems(MemoryConnectionItem* connectionItem);

    /*!
     *  Get the compressed height for the layout.
     *
     *      @param [in] minimumSubItemHeight    Minimum height for the memory sub items.
     *      @param [in] mainItem                The main sub memory layout.
     *
     *      @return The compressed height of the sub memory layout.
     */
    quint64 getCompressedHeight(qreal minimumSubItemHeight, SubMemoryLayout* mainItem);

protected:

    /*!
     *  Setup the sub memory items.
     *
     *      @param [in] subItemPositionX    X position of the sub memory items.
     */
    void setupSubItems(qreal subItemPositionX);

    /*!
     *  Compress the sub items.
     *
     *      @param [in] minimumSubItemHeight    The minimum height of the sub items.
     *
     *      @return The total height of the condensed sub items.
     */
    qreal condenseChildItems(qreal minimumSubItemHeight);

    /*!
     *  Compress the selected sub item.
     *
     *      @param [in] subItem                 The selected sub item.
     *      @param [in] minimumSubItemHeight    Minimum height of the sub item.
     *      @param [in] positionY               Y-coordinate for the sub item.
     */
    quint64 condenseSubItem(MemoryDesignerChildGraphicsItem* subItem, qreal minimumSubItemHeight,
        quint64 positionY);

    /*!
     *  Get the minimum required height of the sub memory layout to fit the selected memory connection.
     *
     *      @param [in] minimumSubItemHeight    Minimum height of the sub items.
     *      @param [in] connectionBaseAddress   Base address of the selected memory connection.
     *      @param [in] connectionEndAddress    End address of the selected memory connection.
     *
     *      @return The minimum required height of the sub memory layout.
     */
    qreal getMinimumRequiredHeight(qreal minimumSubItemHeight, quint64 connectionBaseAddress,
        quint64 connectionEndAddress) const;

    /*!
     *  Get the height for the memory sub item to fit in the selected memory connection item.
     *
     *      @param [in] mainItem                Main sub memory layout.
     *      @param [in] subItemBaseAddress      Base address of the selected memory sub item.
     *      @param [in] subItemLastAddress      Last address of the selected memory sub item.
     *      @param [in] subItem                 The selected memory sub item.
     *      @param [in] connectionItem          The selected memory connection item.
     *      @param [in] yPosition               Y-coordinate for the memory sub item.
     *      @param [in] newSubItemHeight        Current new height for the memory sub item.
     *      @param [in] minimumSubItemHeight    Minimum height for a memory sub item.
     *
     *      @return The height for the sub memory item to fit in the selected memory connection item.
     */
    quint64 getSubItemHeightForConnection(SubMemoryLayout* mainItem, quint64 subItemBaseAddress,
        quint64 subItemLastAddress, MemoryDesignerChildGraphicsItem* subItem, MemoryConnectionItem* connectionItem,
        quint64 yPosition, quint64 newSubItemHeight, qreal minimumSubItemHeight) const;

private:
    // Disable copying.
    SubMemoryLayout(SubMemoryLayout const& rhs);
    SubMemoryLayout& operator=(SubMemoryLayout const& rhs);

    /*!
     *  Create a new sub memory graphics item.
     *
     *      @param [in] subMemoryItem   Memory item containing sub memory item data.
     *      @param [in] isEmpty         Boolean value for an empty sub memory item.
     *
     *      @return The created sub memory graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem> subMemoryItem,
        bool isEmpty) = 0;

    /*!
     *  Create an empty sub memory graphics item.
     *
     *      @param [in] beginAddress    Base address of the empty sub memory graphics item.
     *      @param [in] rangeEnd        End address of the empty sub memory graphics item.
     *
     *      @return The created sub memory graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd) = 0;

    /*!
     *  Place a sub memory graphics item.
     *
     *      @param [in] subItemXPosition        X position of the sub memory item.
     *      @param [in] mainItemBaseAddress     Base address of the main memory item.
     *      @param [in] newSubItem              The selected sub memory item.
     */
    void positionNewSubItem(qreal subItemXPosition, quint64 mainItemBaseAddress,
        MemoryDesignerChildGraphicsItem* newSubItem);

    /*!
     *  Get the height for the selected memory sub item.
     *
     *      @param [in] mainItem                The main memory layout.
     *      @param [in] subItem                 The selected memory sub item.
     *      @param [in] minimumSubItemHeight    Minimum height for a memory sub item.
     *      @param [in] yPosition               Y-coordinate for the memory sub item.
     *      @param [in] movedConnections        Connections that have been moved.
     *
     *      @return The height for the memory sub item.
     */
    virtual qreal getSubItemHeight(SubMemoryLayout* mainItem, MemoryDesignerChildGraphicsItem* subItem,
        qreal minimumSubItemHeight, quint64 yPosition,
        QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections);

    /*!
     *  Get the memory sub items connected to the selected memory connection item.
     *
     *      @param [in] memoryItem      The main sub memory layout.
     *      @param [in] connectionItem  The selected memory connection item.
     *
     *      @return Memory sub items connected to the selected memory connection item.
     */
    QVector<MemoryDesignerChildGraphicsItem*> getSubItemsInConnection(SubMemoryLayout* memoryItem,
        MemoryConnectionItem* connectionItem) const;

    /*!
     *  Get the height of the available area for the memory sub item.
     *
     *      @param [in] mainItem                The main sub memory layout.
     *      @param [in] yPosition               Y-coordinate of the memory sub item.
     *      @param [in] subItemsInConnection    The memory sub items connected to the selected memory connection.
     *      @param [in] newSubItemHeight        The current height of the memory sub item.
     *      @param [in] connectionItem          The selected memory connection item.
     *
     *      @return The available area for the memory sub item.
     */
    qreal getAvailableArea(SubMemoryLayout* mainItem, quint64 yPosition,
        QVector<MemoryDesignerChildGraphicsItem*> subItemsInConnection, quint64 newSubItemHeight,
        MemoryConnectionItem* connectionItem) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! A map containing the sub memory items and their base addresses.
    QMap<quint64, MemoryDesignerChildGraphicsItem*> subMemoryItems_;

    //! Memory item containing main memory item data.
    QSharedPointer<MemoryItem> memoryItem_;

    //! The main graphics memory item.
    MemoryDesignerGraphicsItem* mainGraphicsItem_;

    //! Type of the sub memory items.
    QString subItemType_;
};

//-----------------------------------------------------------------------------

#endif // SUBMEMORYLAYOUT_H
