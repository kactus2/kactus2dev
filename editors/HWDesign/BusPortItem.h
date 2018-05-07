//-----------------------------------------------------------------------------
// File: BusPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------
#ifndef BUSPORTITEM_H
#define BUSPORTITEM_H

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
class BusPortItem : public BusInterfaceEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_PORT };

    
    /*!
     *  The constructor.
     *
     *      @param [in] busIf       The bus interface represented by the item.
     *      @param [in] library     The IP-XACT library in use.
     *      @param [in] parent      The parent object.
     */
    BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library, HWComponentItem* parent);

	/*!
     *  The destructor.
     */
	virtual ~BusPortItem();

	/*!
	 *  Get the graphics item type.
	 *
     *      @return The item type.
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
     *  Sets the endpoint temporary or not temporary. Temporary endpoints can be deleted.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    virtual void setTemporary(bool temp);

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

	/*!
	 *  Set the position of the name label.
	 */
	virtual void setLabelPosition();

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Return the correct length of the name label.
	 */
	virtual qreal getNameLength();

	/*!
	 *  Shorten the name label to better fit the component.
	 *  
	 *      @param [in] width   The width of the shortened name.
	 */
	virtual void shortenNameLabel( qreal width );

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
     *  Finds the possible opposing modes for a bus interface.
     *
     *      @param [in] busIf   The interface for which the opposing mode is searched.
     *    
     *      @return The possible opposing modes.
     */
    QList<General::InterfaceMode> getOpposingModes(QSharedPointer<BusInterface> busIf) const;

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

	//! The library interface.
    LibraryInterface* library_;
};

#endif // BUSPORTITEM_H
