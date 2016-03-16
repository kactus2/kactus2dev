//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDELETECOMMANDS_H
#define SYSTEMDELETECOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class GraphicsConnection;
class GraphicsColumnLayout;
class SystemColumn;
class Design;
class ColumnDesc;

//-----------------------------------------------------------------------------
//! SystemColumnDeleteCommand class.
//-----------------------------------------------------------------------------
class SystemColumnDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout              The column layout.
     *      @param [in] column              The column to delete.
     *      @param [in] containingDesign    The design containing the column.
     *      @param [in] parent              The owner of this command.
     */
    SystemColumnDeleteCommand(GraphicsColumnLayout* layout, SystemColumn* column,
        QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

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
     *      @param [in] connection  The selected connection.
     */
    void addConnectionDeleteCommand(GraphicsConnection* connection);

    /*!
     *  Get the removed column.
     *
     *      @return The removed column.
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
