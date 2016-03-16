//-----------------------------------------------------------------------------
// File: PortDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing an interface item in design diagram.
//-----------------------------------------------------------------------------

#ifndef PORTDELETECOMMAND_H
#define PORTDELETECOMMAND_H

#include <QGraphicsScene>
#include <QObject>
#include <QUndoCommand>

class DesignDiagram;
class HWComponentItem;
class HWConnectionEndpoint;

//----------------------------------------------------------------------------
//! Undo command for removing an interface item in design diagram.
//-----------------------------------------------------------------------------
class PortDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] port The port to delete.
     */
    PortDeleteCommand(DesignDiagram* diagram, HWConnectionEndpoint* port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortDeleteCommand();

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
    PortDeleteCommand(PortDeleteCommand const& rhs);
    PortDeleteCommand& operator=(PortDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram port.
    HWConnectionEndpoint* port_;

    //! The port's parent.
    HWComponentItem* componentItem_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

#endif // PORTDELETECOMMAND_H
