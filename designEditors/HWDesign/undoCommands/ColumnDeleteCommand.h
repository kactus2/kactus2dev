//-----------------------------------------------------------------------------
// File: ColumnDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a column in a design diagram.
//-----------------------------------------------------------------------------

#ifndef COLUMNDELETECOMMAND_H
#define COLUMNDELETECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QObject>

class Design;
class DesignDiagram;
class GraphicsColumn;
class GraphicsColumnLayout;

//-----------------------------------------------------------------------------
//! Undo command for removing a column in a design diagram.
//-----------------------------------------------------------------------------
class ColumnDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  The constructor.
     *
     *      @param [in] diagram     The diagram containing the column item.
     *      @param [in] layout      The column layout to use.
     *      @param [in] column      The column to remove.
     *      @param [in] parent      The parent command.
     */
    ColumnDeleteCommand(DesignDiagram* diagram, GraphicsColumnLayout* layout, GraphicsColumn* column, 
                        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ColumnDeleteCommand();

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
    ColumnDeleteCommand(ColumnDeleteCommand const& rhs);
    ColumnDeleteCommand& operator=(ColumnDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design being edited.
    QSharedPointer<Design> design_;

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The diagram column.
    GraphicsColumn* columnItem_;

    //! Boolean flag for indicating if the column should be deleted in the destructor.
    bool del_;
};

#endif // COLUMNDELETECOMMAND_H
