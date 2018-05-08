//-----------------------------------------------------------------------------
// File: SystemAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.10.2011
//
// Description:
// Undo add commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMADDCOMMANDS_H
#define SYSTEMADDCOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class ComponentItem;
class IGraphicsItemStack;
class SystemComponentItem;
class SWPortItem;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! SWPortAddCommand class.
//-----------------------------------------------------------------------------
class SWPortAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component   The component to which to add a port.
     *      @param [in] pos         The position where to add the port.
     *      @param [in] diagram     Diagram containing the scene of the component.
     *      @param [in] parent      The parent command.
     */
    SWPortAddCommand(SystemComponentItem* component, QPointF const& pos, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

    /*!
     *  Constructor.
     *
     *      @param [in] component   The component to which to add a port.
     *      @param [in] port        The port to add.
     *      @param [in] diagram     Diagram containing the scene of the component.
     *      @param [in] parent      The parent command.
     */
    SWPortAddCommand(SystemComponentItem* component, SWPortItem* port, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWPortAddCommand();

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
    SWPortAddCommand(SWPortAddCommand const& rhs);
    SWPortAddCommand& operator=(SWPortAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent component.
    SystemComponentItem* component_;

    //! The port position.
    QPointF pos_;

    //! The diagram port.
    SWPortItem* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;

    //! Diagram containing the scene of the component.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMADDCOMMANDS_H
