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

class AddressSectionItem;

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
     *  Converts the given address to this address space.
     *
     *      @param [in] address The original address to convert.
     *      @param [in] source  The source for the original address.
     */
    virtual quint64 convertAddress(quint64 address, MemoryBaseItem* source) const;

    /*!
     *  Draws address guide lines.
     *
     *      @param [in] painter  The painter for drawing.
     *      @param [in] rect     The visible rectangle area where to draw the lines.
     */
    virtual void drawGuides(QPainter* painter, QRectF const& rect) const;

    /*!
     *  Draws a divider for start address.
     *
     *      @param [in] painter The painter.
     *      @param [in] rect    The rectangle for the visible drawing area.
     *      @param [in] y       The divider y coordinate in scene coordinates.
     *      @param [in] address The reference memory address.
     */
    virtual void drawStartAddressDivider(QPainter* painter, QRectF const& rect, int y, quint64 address) const;

    /*!
     *  Draws a divider for end address.
     *
     *      @param [in] painter The painter.
     *      @param [in] rect    The rectangle for the visible drawing area.
     *      @param [in] y       The divider y coordinate in scene coordinates.
     *      @param [in] address The reference memory address.
     */
    virtual void drawEndAddressDivider(QPainter* painter, QRectF const& rect, int y, quint64 address) const;

    /*!
     *  Returns the sections.
     */
    virtual QList<AddressSectionItem*> const& getSections() const;

signals:
    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

    //! \brief Emitted right before this item is destroyed.
    void destroyed(MemoryBaseItem* item);
};

#endif // MEMORYBASEITEM_H
