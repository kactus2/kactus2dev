//-----------------------------------------------------------------------------
// File: AddressSectionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.9.2012
//
// Description:
// Declares the address block section class.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSECTIONITEM_H
#define ADDRESSSECTIONITEM_H

#include "MemoryBaseItem.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class AddressBlock;
class MemoryItem;
class AddressSubsection;

//-----------------------------------------------------------------------------
//! Address block section class.
//-----------------------------------------------------------------------------
class AddressSectionItem : public MemoryBaseItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_ADDRESS_SECTION_ITEM };

    //-----------------------------------------------------------------------------
    //! Usage type enumeration.
    //-----------------------------------------------------------------------------
    enum UsageType
    {
        USAGE_UNSPECIFIED = 0,
        USAGE_READ_ONLY,
        USAGE_READ_WRITE,
        USAGE_READ_WRITE_ONCE,
        USAGE_REGISTERS
    };

    /*!
     *  Constructor.
     *  
     *      @param [in] name            The name of the section.
     *      @param [in] startAddress    The start address of the section.
     *      @param [in] range           The address range.
     *      @param [in] parent          The parent.
     */
    AddressSectionItem(QString const& name, quint64 startAddress,
                       quint64 range, QGraphicsItem* parent);

    /*!
     *  Destructor.
     */
    virtual ~AddressSectionItem();

    /*!
     *  Sets the usage type.
     *
     *      @param [in] usageType The usage type.
     */
    void setUsageType(UsageType usageType);

    /*!
     *  Sets the color of the section.
     *
     *      @param [in] color The color to set.
     */
    virtual void setColor(QColor const& color);

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
     *  Returns the start address.
     */
    quint64 getStartAddress() const;

    /*!
     *  Returns the end address.
     */
    quint64 getEndAddress() const;

    int type() const { return Type; }

    enum
    {
        WIDTH = 120,
        ADDR_COLUMN_WIDTH = 90,
        MIN_SECTION_HEIGHT = 120,
        SPACING = 10,
    };

protected:
    //! Called when the mouse hover enters the column header.
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the mouse hover moves inside the column header.
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the mouse hover leaves the column header.
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the user presses the mouse over the column.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user double-clicks the section.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //! Draws the memory item.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /*!
     *  Sets the height of the section item.
     *  
     *      @param [in] height  The height to set.
     */
    virtual void setHeight(int height);

private:
    // Disable copying.
    AddressSectionItem(AddressSectionItem const& rhs);
    AddressSectionItem& operator=(AddressSectionItem const& rhs);

    /*!
     *  Updates the mouse cursor based on the hover position.
     *
     *      @param [in] event The hover event information.
     */
    void updateCursor(QGraphicsSceneHoverEvent* event);

    /*!
     *  Converts the address to a hexadecimal string.
     *  
     *      @param [in] address The address.
     *  
     *      @return Address as a string.
     */
    static QString toHexString(quint64 address);

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! The name of the section.
    QString name_;

    //! The start address.
    quint64 startAddress_;

    //! the address range.
    quint64 range_;

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! The usage type.
    UsageType usageType_;

    //! Usage type label.
    QGraphicsTextItem* typeLabel_;

    //! If true, the mouse hovers near the resize area.
    bool mouseNearResizeArea_;

    //! The label for start address.
    QGraphicsTextItem* startAddressLabel_;

    //! The label for end address.
    QGraphicsTextItem* endAddressLabel_;
};

#endif // ADDRESSSECTIONITEM_H
