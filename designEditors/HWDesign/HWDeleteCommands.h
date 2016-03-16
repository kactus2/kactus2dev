//-----------------------------------------------------------------------------
// File: HWDeleteCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.8.2011
//
// Description:
// Undo delete commands for the design widget.
//-----------------------------------------------------------------------------

#ifndef HWDELETECOMMANDS_H
#define HWDELETECOMMANDS_H

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class BusInterface;
class Port;
class HWConnection;
class HWConnectionEndpoint;
class BusPortItem;
class HWComponentItem;
class BusInterfaceItem;
class GraphicsColumn;
class GraphicsColumnLayout;
class ComponentItem;
class Component;

//-----------------------------------------------------------------------------
//! DeletePhysicalPortCommand class.
//-----------------------------------------------------------------------------
class DeletePhysicalPortCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component from which to delete a physical port.
	 *      @param [in] port       The port to delete.
     *      @param [in] parent     The parent command.
     */
   DeletePhysicalPortCommand(QSharedPointer<Component>, QSharedPointer<Port> port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~DeletePhysicalPortCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
   DeletePhysicalPortCommand(DeletePhysicalPortCommand const& rhs);
   DeletePhysicalPortCommand& operator=(DeletePhysicalPortCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target component model.
    QSharedPointer<Component> component_;

    //! The port to delete.
	QSharedPointer<Port> port_;
};

//-----------------------------------------------------------------------------

#endif // HWDELETECOMMANDS_H
