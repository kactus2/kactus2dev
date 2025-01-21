//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDELETECOMMANDS_H
#define SYSTEMDELETECOMMANDS_H

#include <QGraphicsScene>
#include <QObject>
#include <QSharedPointer>
#include <QUndoCommand>

class GraphicsConnection;
class GraphicsColumnLayout;
class SystemColumn;
class Design;
class ColumnDesc;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! SystemColumnDeleteCommand class.
//-----------------------------------------------------------------------------
class SystemColumnDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] diagram     Diagram containing the scene of the column.
     *    @param [in] column      The column to delete.
     *    @param [in] parent      The owner of this command.
     */
    SystemColumnDeleteCommand(DesignDiagram* diagram, SystemColumn* column, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemColumnDeleteCommand();

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
    SystemColumnDeleteCommand(SystemColumnDeleteCommand const& rhs);
    SystemColumnDeleteCommand& operator=(SystemColumnDeleteCommand const& rhs);

    /*!
     *  Add delete command for the connection.
     *
     *    @param [in] connection  The selected connection.
     */
    void addConnectionDeleteCommand(GraphicsConnection* connection);

    /*!
     *  Get the removed column.
     *
     *    @return The removed column.
     */
    QSharedPointer<ColumnDesc> getDeletedColumn();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The diagram column.
    SystemColumn* graphicalColumn_;

    //! Boolean flag for indicating if the column should be deleted in the destructor.
    bool canDelete_;

    //! The design containing the column.
    QSharedPointer<Design> containingDesign_;

    //! The removed column.
    QSharedPointer<ColumnDesc> removedColumn_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDELETECOMMANDS_H
