//-----------------------------------------------------------------------------
// File: ConnectionDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a connection in design diagram.
//-----------------------------------------------------------------------------

#ifndef CONNECTIONDELETECOMMAND_H
#define CONNECTIONDELETECOMMAND_H

#include <QObject>
#include <QSharedPointer>
#include <QUndoCommand>

#include <IPXACTmodels/generaldeclarations.h>

class DesignDiagram;
class HWConnection;
class PortMap;

//-----------------------------------------------------------------------------
//! Undo command for removing a connection in design diagram.
//-----------------------------------------------------------------------------
class ConnectionDeleteCommand : public QUndoCommand
{
public:

    /*!
     *  The constructor.
     *
     *    @param [in] diagram     The diagram containing the connection item.
     *    @param [in] connection  The connection to remove.
     *    @param [in] parent      The parent command.
     */
    ConnectionDeleteCommand(DesignDiagram* diagram, HWConnection* connection, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionDeleteCommand();

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
    ConnectionDeleteCommand(ConnectionDeleteCommand const& rhs);
    ConnectionDeleteCommand& operator=(ConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    HWConnection* connection_;

    //! The graphics scene.
    DesignDiagram* diagram_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // CONNECTIONDELETECOMMAND_H
