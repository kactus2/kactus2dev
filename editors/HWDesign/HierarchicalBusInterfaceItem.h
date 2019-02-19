//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
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
     *      @param [in] parent      The parent object.
     */
    HierarchicalBusInterfaceItem(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
        QSharedPointer<InterfaceGraphicsData> dataGroup, QGraphicsItem *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~HierarchicalBusInterfaceItem();

    /*!
     *  Get the graphics item type of this item.
     *
     *      @return The graphics item type of this item.
     */
    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns true if the port represents a hierarchical connection
     */
    virtual bool isHierarchical() const;

    /*!
     *  Set the direction for the bus interface item.
     *
     *      @param [in] dir     The new direction.
     */
    void setDirection(QVector2D const& dir);

	/*!
	 * Set the position of the name label.
	 */
	virtual void setLabelPosition();

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
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);

    /*!
     *  Event for mouse press.
     *
     *      @param [in] event   The pressed mouse button.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    /*!
     *  Update the name change to the relevant contained variables.
     *
     *      @param [in] previousName    The previous name of the bus interface.
     *      @param [in] newName         The new name of the bus interface.
     */
    virtual void updateName(QString const& previousName, QString const& newName);

    /*!
     *  Move by dragging with the mouse.
     */
    virtual void moveItemByMouse();

    /*!
     *  Create a move command for this end point item.
     *
     *      @param [in] diagram     The containing design diagram.
     *
     *      @return The created move command.
     */
    virtual QSharedPointer<QUndoCommand> createMouseMoveCommand(DesignDiagram* diagram);

    /*!
     *  Create move command for an end point that has been moved by the movement of this end point.
     *
     *      @param [in] endPoint            The selected end point.
     *      @param [in] endPointPosition    The new position of the end point.
     *      @param [in] diagram             Design diagram containing the end point.
     *      @param [in] parentCommand       Parent command.
     */
    virtual void createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
        DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand);

    /*!
     *  Check if a connection can be made to the selected connection end point.
     *
     *      @param [in] otherEndPoint   The selected connection end point.
     *
     *      @return True, if the connection can be made, false otherwise.
     */
    virtual bool canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const;

    /*!
     *  Get the current position of the end point.
     *
     *      @return The current position of the end point.
     */
    virtual QPointF getCurrentPosition() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The data extension for the item.
    QSharedPointer<InterfaceGraphicsData> dataGroup_;

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_;
};

#endif // HIERARCHICALBUSINTERFACEITEM_H
