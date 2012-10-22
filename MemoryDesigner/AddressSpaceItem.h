//-----------------------------------------------------------------------------
// File: AddressSpaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.9.2012
//
// Description:
// Declares the address space item class.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEITEM_H
#define ADDRESSSPACEITEM_H

#include "MemoryBaseItem.h"

#include <common/diagramgrid.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class AddressBlock;
class Component;
class AddressSpace;
class LibraryInterface;
class IGraphicsItemStack;
class AddressSectionItem;
class MemoryColumn;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing address spaces in the memory designer.
//-----------------------------------------------------------------------------
class AddressSpaceItem : public MemoryBaseItem, public IGraphicsItemStack
{
    Q_OBJECT 

public:
    enum { Type = GFX_TYPE_ADDRESS_SPACE_ITEM };

    /*!
     *  Constructor.
     *  
     *      @param [in] libInterface    The library interface.
     *      @param [in] component       The component.
     *      @param [in] addressSpace    The address space.
     *      @param [in] parent          The parent graphics item.
     */
    AddressSpaceItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
                     QSharedPointer<AddressSpace> addressSpace, QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~AddressSpaceItem();

    /*!
     *  Updates the component item to reflect the current state of the component model.
     */
    virtual void updateVisuals();

    /*!
     *  Draws address guide lines.
     *
     *      @param [in] painter  The painter for drawing.
     *      @param [in] rect     The visible rectangle area where to draw the lines.
     */
    virtual void drawGuides(QPainter* painter, QRectF const& rect) const;

    /*!
     *  Returns the actual address space.
     */
    QSharedPointer<AddressSpace> getAddressSpace();

    /*!
     *  Returns the actual address space.
     */
    QSharedPointer<AddressSpace const> getAddressSpace() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

    /*!
     *  Returns the parent graphics item stack.
     */
    IGraphicsItemStack* getParentStack();

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // IGraphicsItemStack implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Adds an item to the system column.
     *
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(QGraphicsItem* item, bool load = false);

    /*!
     *  Removes an item from the system column.
     *
     *      @param [in] item the item to remove.
     */
    void removeItem(QGraphicsItem* item);

    /*!
     *  Called when an item is moved within the column.
     *
     *      @param [in] item       The item that has been moved.
     */
    void onMoveItem(QGraphicsItem* item);

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
     */
    void onReleaseItem(QGraphicsItem* item);

    /*!
     *  Updates the item positions so that there are no violations of the stacking rule.
     */
    void updateItemPositions();

    /*!
     *  Maps the given local position to scene coordinates.
     */
    QPointF mapStackToScene(QPointF const& pos) const;

    /*!
     *  Maps the given scene position to local coordinates.
     */
    QPointF mapStackFromScene(QPointF const& pos) const;

    /*!
     *  Returns true if the stack is allowed to contain the given item.
     *
     *      @param [in] item The item to test for.
     */
    bool isItemAllowed(QGraphicsItem* item) const;

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

signals:
    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

	//! \brief Emitted right before this item is destroyed.
	void destroyed(AddressSpaceItem* comp);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Updates the name label with the given text.
     *
     *      @param [in] text The text to display in the label.
     */
    virtual void updateNameLabel(QString const& text);

    /*!
     *  Gets the height.
     *  
     *      @return The height.
     */
    qreal getHeight() const;

    /*!
     *  Updates the size.
     */
    void updateSize();

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    // Disable copying.
    AddressSpaceItem(AddressSpaceItem const& rhs);
    AddressSpaceItem& operator=(AddressSpaceItem const& rhs);

    enum
    {
        WIDTH = 280,
        NAME_COLUMN_WIDTH = 50,
        MIN_HEIGHT = 120,
        SPACING = 10,
        SECTION_X = NAME_COLUMN_WIDTH / 2
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! The component which contains the address space.
    QSharedPointer<Component> component_;

    //! The address space.
    QSharedPointer<AddressSpace> addressSpace_;

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! AUB label.
    QGraphicsTextItem* aubLabel_;

    //! The layout for the sections.
    QSharedPointer< IVGraphicsLayout<AddressSectionItem> > sectionLayout_;

    //! The address sections for the address blocks.
    QList<AddressSectionItem*> sections_;

    //! The old column from where the mouse drag event began.
    MemoryColumn* oldColumn_;
    QPointF oldPos_;
};

//-----------------------------------------------------------------------------


#endif // ADDRESSSPACEITEM_H
