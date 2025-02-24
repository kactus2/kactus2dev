//-----------------------------------------------------------------------------
// File: ActiveBusInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------
#ifndef ACTIVEBUSINTERFACEITEM_H
#define ACTIVEBUSINTERFACEITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <editors/HWDesign/BusInterfaceEndPoint.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QSharedPointer>
#include <QPolygonF>

class BusInterface;
class HWComponentItem;
class LibraryInterface;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! HWConnection represents graphically an IP-XACT bus interface in a component instance
//-----------------------------------------------------------------------------
class ActiveBusInterfaceItem : public BusInterfaceEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_PORT };

    
    /*!
     *  The constructor.
     *
     *    @param [in] busIf       The bus interface represented by the item.
     *    @param [in] library     The IP-XACT library in use.
     *    @param [in] parent      The parent object.
     */
    ActiveBusInterfaceItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library, HWComponentItem* parent);

	/*!
     *  The destructor.
     */
	~ActiveBusInterfaceItem() final = default;

    // Disable copying.
    ActiveBusInterfaceItem(ActiveBusInterfaceItem const& rhs) = delete;
    ActiveBusInterfaceItem& operator=(ActiveBusInterfaceItem const& rhs) = delete;

	/*!
	 *  Get the graphics item type.
	 *
     *    @return The item type.
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
     *  Sets the endpoint temporary or not temporary. Temporary endpoints can be deleted.
     *
     *    @param [in] temp True if temporary; false if not temporary.
     */
    void setTemporary(bool temp) final;

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    bool isDirectionFixed() const noexcept final;

	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition() final;

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Return the correct length of the name label.
	 */
	qreal getNameLength() final;

	/*!
	 *  Shorten the name label to better fit the component.
	 *  
	 *    @param [in] width   The width of the shortened name.
	 */
	void shortenNameLabel( qreal width ) final;

protected:

    /*!
     *  Notifies item state changes.
     *
     *    @param [in] change  The change.
     *    @param [in] value   The new value.
     */
    QVariant itemChange(GraphicsItemChange change, QVariant const& value) final;

    /*!
     *  Event for mouse press.
     *
     *    @param [in] event   The pressed mouse button.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) final;

private:
    
    /*!
     *  Update the name change to the relevant contained variables.
     *
     *    @param [in] previousName    The previous name of the bus interface.
     *    @param [in] newName         The new name of the bus interface.
     */
    void updateName(QString const& previousName, QString const& newName) final;

    /*!
     *  Finds the possible opposing modes for a bus interface.
     *
     *    @param [in] busIf   The interface for which the opposing mode is searched.
     *    
     *    @return The possible opposing modes.
     */
    QList<General::InterfaceMode> getOpposingModes(QSharedPointer<BusInterface> busIf) const;

    /*!
     *  Move by dragging with the mouse.
     */
    void moveItemByMouse() final;

    /*!
     *  Create a move command for this end point item.
     *
     *    @param [in] diagram     The containing design diagram.
     *
     *    @return The created move command.
     */
    QSharedPointer<QUndoCommand> createMouseMoveCommand(DesignDiagram* diagram) final;

    /*!
     *  Create move command for an end point that has been moved by the movement of this end point.
     *
     *    @param [in] endPoint            The selected end point.
     *    @param [in] endPointPosition    The new position of the end point.
     *    @param [in] diagram             Design diagram containing the end point.
     *    @param [in] parentCommand       Parent command.
     */
    void createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
        DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand) final;

    /*!
     *  Check if a connection can be made to the selected connection end point.
     *
     *    @param [in] otherEndPoint   The selected connection end point.
     *
     *    @return True, if the connection can be made, false otherwise.
     */
    bool canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const final;

    /*!
     *  Get the current position of the end point.
     *
     *    @return The current position of the end point.
     */
    QPointF getCurrentPosition() const final;
};

#endif // ACTIVEBUSINTERFACEITEM_H
