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
     *
     *      @param [in] firstRangeStart     The base address of the first connection.
     *      @param [in] firstRangeEnd       The end address of the first connection.
     *      @param [in] secondRangeStart    The base address of the second connection.
     *      @param [in] secondRangeEnd      The end address of the second connection.
     */
    void setLabels(QString firstRangeStart, QString firstRangeEnd, QString secondRangeStart,
        QString secondRangeEnd);
};

//-----------------------------------------------------------------------------

#endif // MEMORYCOLLISIONITEM_H
