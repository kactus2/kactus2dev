//-----------------------------------------------------------------------------
// File: MemoryBaseItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.10.2012
//
// Description:
// Memory base item class.
//-----------------------------------------------------------------------------

#ifndef MEMORYBASEITEM_H
#define MEMORYBASEITEM_H

#include <QObject>
#include <QGraphicsRectItem>

//-----------------------------------------------------------------------------
//! Memory base item class.
//-----------------------------------------------------------------------------
class MemoryBaseItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent item.
     */
    MemoryBaseItem(QGraphicsItem* parent);

    /*!
     *  Destructor.
     */
    virtual ~MemoryBaseItem();

    /*!
     *  Draws address guide lines.
     *
     *      @param [in] painter  The painter for drawing.
     *      @param [in] rect     The visible rectangle area where to draw the lines.
     */
    virtual void drawGuides(QPainter* painter, QRectF const& rect) const;

signals:
    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

    //! \brief Emitted right before this item is destroyed.
    void destroyed(MemoryBaseItem* item);
};

#endif // MEMORYBASEITEM_H
