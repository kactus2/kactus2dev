//-----------------------------------------------------------------------------
// File: MemoryItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.9.2012
//
// Description:
// Declares the memory map item class.
//-----------------------------------------------------------------------------

#ifndef MEMORYITEM_H
#define MEMORYITEM_H

#include "MemoryBaseItem.h"

#include <common/diagramgrid.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class AddressBlock;
class Component;
class MemoryMap;
class LibraryInterface;
class IGraphicsItemStack;
class AddressSectionItem;
class MemoryColumn;

//-----------------------------------------------------------------------------
//! MemoryItem class.
//-----------------------------------------------------------------------------
class MemoryItem : public MemoryBaseItem, public IGraphicsItemStack
{
    Q_OBJECT 

public:
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  Constructor.
     *  
     *      @param [in] libInterface    The library interface.
     *      @param [in] instanceName    The name of the component instance containing the memory map.
     *      @param [in] component       The component.
     *      @param [in] memoryMap       The memory map.
     *      @param [in] parent          The parent graphics item.
     */
    MemoryItem(LibraryInterface* libInterface, QString const& instanceName,
               QSharedPointer<Component> component,
               QSharedPointer<MemoryMap> memoryMap, QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~MemoryItem();

    /*!
     *  Updates the component item to reflect the current state of the component model.
     */
    virtual void updateVisuals();

    /*!
     *  Converts the given address to this address space.
     *
     *      @param [in] address The original address to convert.
     *      @param [in] source  The source for the original address.
     */
    virtual unsigned int convertAddress(unsigned int address, MemoryBaseItem* source) const;

    /*!
     *  Returns the name of the component instance.
     */
    QString const& getInstanceName() const;

    /*!
     *  Returns the parent component.
     */
    QSharedPointer<Component const> getComponent() const;

    /*!
     *  Returns the actual memory map.
     */
    QSharedPointer<MemoryMap> getMemoryMap();

    /*!
     *  Returns the actual memory map.
     */
    QSharedPointer<MemoryMap const> getMemoryMap() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

    /*!
     *  Returns the parent graphics item stack.
     */
    IGraphicsItemStack* getParentStack();

    /*!
     *  Returns the list of address block sections.
     */
    virtual QList<AddressSectionItem*> const& getSections() const;

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

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

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

private:
    // Disable copying.
    MemoryItem(MemoryItem const& rhs);
    MemoryItem& operator=(MemoryItem const& rhs);

    enum
    {
        WIDTH = 160,
        NAME_COLUMN_WIDTH = 40,
        MIN_HEIGHT = 120,
        SPACING = 10,
        SECTION_X = NAME_COLUMN_WIDTH / 2
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! The name of the component instance containing the memory map.
    QString instanceName_;

    //! The component which contains the memory map.
    QSharedPointer<Component> component_;

    //! The memory map.
    QSharedPointer<MemoryMap> memoryMap_;

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

#endif // MEMORYITEM_H
