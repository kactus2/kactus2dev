//-----------------------------------------------------------------------------
// File: MemoryCollisionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.09.2016
//
// Description:
// Graphics item for displaying colliding memory usage in memory designer.
//-----------------------------------------------------------------------------

#ifndef MEMORYCOLLISIONITEM_H
#define MEMORYCOLLISIONITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class MemoryConnectionItem;
class MemoryDesignerGraphicsItem;

//-----------------------------------------------------------------------------
//! Graphics item for displaying colliding memory usage in memory designer.
//-----------------------------------------------------------------------------
class MemoryCollisionItem : public QObject, public QGraphicsRectItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] firstItem           The first memory connection.
     *      @param [in] secondItem          The second memory connection.
     *      @param [in] containingScene     The scene containing the memory connections.
     */
    MemoryCollisionItem(MemoryConnectionItem* firstItem, MemoryConnectionItem* secondItem,
        QGraphicsScene* containingScene);

	/*!
     *  The destructor.
     */
    virtual ~MemoryCollisionItem();

    /*!
     *  Redraw the memory connection collision item.
     */
    void reDrawCollision();

private:
    // Disable copying.
    MemoryCollisionItem(MemoryCollisionItem const& rhs);
    MemoryCollisionItem& operator=(MemoryCollisionItem const& rhs);

    /*!
     *  Setup the brush for memory collision.
     */
    void setCollisionBrush();

    /*!
     *  Set the labels.
     */
    void setLabels();

    /*!
     *  Set the collision rectangle.
     */
    void setRectangle();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first memory connection graphics item.
    MemoryConnectionItem* firstConnection_;

    //! The second memory connection graphics item.
    MemoryConnectionItem* secondConnection_;

    //! Start of the memory range of the first memory connection.
    quint64 firstRangeStart_;

    //! End of the memory range of the first memory connection.
    quint64 firstRangeEnd_;

    //! Start of the memory range of the second memory connection.
    quint64 secondRangeStart_;

    //! End of the memory range of the second memory connection.
    quint64 secondRangeEnd_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYCOLLISIONITEM_H
