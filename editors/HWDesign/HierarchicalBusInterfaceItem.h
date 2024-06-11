//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALBUSINTERFACEITEM_H
#define HIERARCHICALBUSINTERFACEITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <editors/HWDesign/BusInterfaceEndPoint.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QGraphicsPolygonItem>
#include <QSharedPointer>
#include <QVector2D>

class BusInterface;
class HWComponentItem;
class HWColumn;
class Component;
class LibraryInterface;
class InterfaceGraphicsData;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! BusInterfaceItem class.
//-----------------------------------------------------------------------------
class HierarchicalBusInterfaceItem : public BusInterfaceEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERFACE };

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component containing the bus interface this item represents.
     *      @param [in] busIf       The bus interface this item represents.
     *      @param [in] dataGroup   The container for the item data.
     *      @param [in] library     Access to the library.
     *      @param [in] parent      The parent object.
     *      @param [in] isDraft     Determines if Bus Interface is draft or not.
     */
    HierarchicalBusInterfaceItem(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
        QSharedPointer<InterfaceGraphicsData> dataGroup, LibraryInterface* library, QGraphicsItem *parent = 0, bool isDraft=false);

	/*!
     *  The destructor.
     */
	~HierarchicalBusInterfaceItem() final = default;

    // Disable copying.
    HierarchicalBusInterfaceItem(HierarchicalBusInterfaceItem const& rhs) = delete;
    HierarchicalBusInterfaceItem& operator=(HierarchicalBusInterfaceItem const& rhs) = delete;

    /*!
     *  Get the graphics item type of this item.
     *
     *      @return The graphics item type of this item.
     */
    int type() const final { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns true if the port represents a hierarchical connection
     */
    bool isHierarchical() const noexcept final;

    /*!
     *  Set the direction for the bus interface item.
     *
     *      @param [in] dir     The new direction.
     */
    void setDirection(QVector2D const& dir);

	/*!
	 * Set the position of the name label.
	 */
	void setLabelPosition() final;

    /*!
     *  Gets the data extension for the bus interface.
     *
     *      @return The data vendor extension.
     */
    QSharedPointer<VendorExtension> getDataExtension() const;

protected:
    
    /*!
     *  Notifies item state changes.
     *
     *      @param [in] change  The change.
     *      @param [in] value   The new value.
     */
    QVariant itemChange(GraphicsItemChange change, QVariant const& value) final;

    /*!
     *  Event for mouse press.
     *
     *      @param [in] event   The pressed mouse button.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) final;

private:

    /*!
     *  Update the name change to the relevant contained variables.
     *
     *      @param [in] previousName    The previous name of the bus interface.
     *      @param [in] newName         The new name of the bus interface.
     */
    void updateName(QString const& previousName, QString const& newName) final;

    /*!
     *  Move by dragging with the mouse.
     */
    void moveItemByMouse() final;

    /*!
     *  Create a move command for this end point item.
     *
     *      @param [in] diagram     The containing design diagram.
     *
     *      @return The created move command.
     */
    QSharedPointer<QUndoCommand> createMouseMoveCommand(DesignDiagram* diagram) final;

    /*!
     *  Create move command for an end point that has been moved by the movement of this end point.
     *
     *      @param [in] endPoint            The selected end point.
     *      @param [in] endPointPosition    The new position of the end point.
     *      @param [in] diagram             Design diagram containing the end point.
     *      @param [in] parentCommand       Parent command.
     */
    void createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
        DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand) final;

    /*!
     *  Check if a connection can be made to the selected connection end point.
     *
     *      @param [in] otherEndPoint   The selected connection end point.
     *
     *      @return True, if the connection can be made, false otherwise.
     */
    bool canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const final;

    /*!
     *  Get the current position of the end point.
     *
     *      @return The current position of the end point.
     */
    QPointF getCurrentPosition() const final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The data extension for the item.
    QSharedPointer<InterfaceGraphicsData> dataGroup_ = nullptr;

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_ = nullptr;
};

#endif // HIERARCHICALBUSINTERFACEITEM_H
