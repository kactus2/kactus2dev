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

protected:

    /*!
     *  Setup the sub memory items.
     *
     *      @param [in] subItemPositionX    X position of the sub memory items.
     */
    void setupSubItems(qreal subItemPositionX);

    /*!
     *  Get the sub memory items.
     *
     *      @return A map containing the sub memory items with the base addresses.
     */
    QMap<quint64, MemoryDesignerChildGraphicsItem*> getSubMemoryItems() const;

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
     *  Get the connections within the range of the selected sub item.
     *
     *      @param [in] subItem             The selected sub item.
     *      @param [in] parentConnections   Connections of the parent item.
     *
     *      @return A map containing the memory connection items with the base addresses.
     */
    QMap<quint64, MemoryConnectionItem*> getSubItemConnections(MemoryDesignerChildGraphicsItem* subItem,
        QVector<MemoryConnectionItem*> parentConnections);

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
