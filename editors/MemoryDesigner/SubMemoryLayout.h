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
     *      @param [in] filterSubItems      Value for filtering memory sub items.
     *      @param [in] mainGraphicsItem    The main graphics item.
     */
    SubMemoryLayout(QSharedPointer<MemoryItem const> memoryItem, QString const& subItemType, bool filterSubItems,
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
    QMultiMap<quint64, MemoryDesignerChildGraphicsItem*> getSubMemoryItems() const;

    /*!
     *  Add the memory connection item to the correct memory sub items.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    void addConnectionToSubItems(MemoryConnectionItem* connectionItem);

    /*!
     *  Check whether the sub items are filtered or not.
     *
     *      @return True, if the sub items are filtered, otherwise false.
     */
    bool subItemsAreFiltered() const;

    /*!
     *  Resize the name labels for the sub memory items.
     */
    void resizeSubItemNameLabels();

    /*!
     *  Create overlapping markings for sub memory items.
     */
    void createOverlappingSubItemMarkings();

    /*!
     *  Get the height addition of the out of bounds sub memory items.
     *
     *      @return The height addition of the out of bounds sub memory items.
     */
    quint64 getSubItemHeightAddition() const;

    /*!
     *  Get the addresses that are retained after item compression.
     *
     *      @return Addresses that are retained after compressing the item.
     */
    QVector<quint64> getUnCutAddresses() const;

    /*!
     *  Get the coordinates that are retained after item compression.
     *
     *       @return Coordinates that are retained after compressing the item.
     */
    QVector<qreal> getUnCutCoordinates() const;

    /*!
     *  Compress the contained sub memory items to contain the retained coordinates.
     *
     *      @param [in] unCutCoordinates            The retained coordinates.
     *      @param [in] CUTMODIFIER                 Modifier for the cut area.
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     */
    void compressSubItemsToUnCutCoordinates(QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER,
        bool memoryItemsAreCompressed);

    /*!
     *  Get layout height modified by the height of the out of bounds sub memory items.
     *
     *      @return True height of the layout.
     */
    virtual quint64 getHeightWithSubItems() const;

    /*!
     *  Get the scene bounding rectangle of the layout modified by the height of the sub memory items.
     *
     *      @return True scene bounding rectangle of the layout.
     */
    virtual QRectF getSceneRectangleWithSubItems() const;

protected:

    /*!
     *  Set a new value for filtering memory sub items.
     *
     *      @param [in] filterValue     New filtering value for the memory sub items.
     */
    void setFilterSubItems(bool filterValue);

    /*!
     *  Get the type of the memory sub items.
     *
     *      @return The type of the memory sub items.
     */
    QString getSubItemType() const;

    /*!
     *  Setup the sub memory items.
     *
     *      @param [in] subItemPositionX    X position of the sub memory items.
     *      @param [in] subItemType         Type of the memory sub items.
     *      @param [in] memoryItem          The memory item containing the sub items.
     */
    void setupSubItems(qreal subItemPositionX, QString const& subItemType,
        QSharedPointer<MemoryItem const> memoryItem);

    /*!
     *  Compress the sub items.
     *
     *      @param [in] itemBaseAddress             Base address of the containing item.
     *      @param [in] itemLastAddress             Last address of the containing item.
     *      @param [in] minimumSubItemHeight        The minimum height of the sub items.
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     *
     *      @return The total height of the condensed sub items.
     */
    qreal compressChildItems(quint64 itemBaseAddress, quint64 itemLastAddress, qreal minimumSubItemHeight,
        bool memoryItemsAreCondensed);

    /*!
     *  Compress the selected sub item.
     *
     *      @param [in] subItem                     The selected sub item.
     *      @param [in] minimumSubItemHeight        Minimum height of the sub item.
     *      @param [in] positionY                   Y-coordinate for the sub item.
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     */
    quint64 compressSubItem(MemoryDesignerChildGraphicsItem* subItem, qreal minimumSubItemHeight,
        quint64 positionY, bool memoryItemsAreCondensed);

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
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem const> subMemoryItem,
        bool isEmpty) = 0;

    /*!
     *  Create an empty sub memory graphics item and position it.
     *
     *      @param [in] subItemBaseAddress      Base address of the empty sub memory item.
     *      @param [in] subItemLastAddress      Last address of the empty sub memory item.
     *      @param [in] subItemPositionX        X coordinate of the empty sub memory item.
     *      @param [in] mainItemBaseAddress     Base address of the main memory item.
     */
    void createAndPositionNewEmptySubItem(quint64 subItemBaseAddress, quint64 subItemLastAddress,
        qreal subItemPositionX, quint64 mainItemBaseAddress);

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
     *      @param [in] subItemOffset           Offset of the sub memory item.
     *      @param [in] newSubItem              The selected sub memory item.
     */
    void positionNewSubItem(qreal subItemXPosition, quint64 mainItemBaseAddress, quint64 subItemOffset,
        MemoryDesignerChildGraphicsItem* newSubItem);

    /*!
     *  Set the color for a faulty sub memory item.
     *
     *      @param [in] subItem     The selected sub memory item.
     */
    void setFaultySubItemColor(MemoryDesignerChildGraphicsItem* subItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! A map containing the sub memory items and their base addresses.
    QMultiMap<quint64, MemoryDesignerChildGraphicsItem*> subMemoryItems_;

    //! Type of the contained memory item.
    QString itemType_;

    //! The main graphics memory item.
    MemoryDesignerGraphicsItem* mainGraphicsItem_;

    //! Type of the sub memory items.
    QString subItemType_;

    //! Value for filtering sub memory items.
    bool filterSubItems_;
};

//-----------------------------------------------------------------------------

#endif // SUBMEMORYLAYOUT_H
