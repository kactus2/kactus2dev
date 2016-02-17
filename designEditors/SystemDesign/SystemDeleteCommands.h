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
class ComponentItem;
class IGraphicsItemStack;
class SWPortItem;
class SWInterfaceItem;
class SystemComponentItem;
class ApiInterface;
class ComInterface;
class Design;

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The diagram column.
    SystemColumn* column_;

    //! Boolean flag for indicating if the column should be deleted in the destructor.
    bool del_;

    //! The design containing the column.
    QSharedPointer<Design> containingDesign_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDELETECOMMANDS_H
