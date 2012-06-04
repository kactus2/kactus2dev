//-----------------------------------------------------------------------------
// File: GraphicsColumnUndoCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2012
//
// Description:
// Undo commands for graphics columns.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCOLUMNUNDOCOMMANDS_H
#define GRAPHICSCOLUMNUNDOCOMMANDS_H

#include <QUndoCommand>
#include <QPointF>

class GraphicsColumnLayout;
class GraphicsColumn;

//-----------------------------------------------------------------------------
//! GraphicsColumnMoveCommand class.
//-----------------------------------------------------------------------------
class GraphicsColumnMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout  The column layout.
     *      @param [in] column  The column to move.
     *      @param [in] oldPos  The column's old position.
     *      @param [in] parent  The parent command.
     */
    GraphicsColumnMoveCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
                              QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~GraphicsColumnMoveCommand();

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
    GraphicsColumnMoveCommand(GraphicsColumnMoveCommand const& rhs);
    GraphicsColumnMoveCommand& operator=(GraphicsColumnMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The diagram column.
    GraphicsColumn* column_;

    //! The old position of the column.
    QPointF oldPos_;

    //! The new position of the column.
    QPointF newPos_;
};

//-----------------------------------------------------------------------------
//! GraphicsColumnAddCommand class.
//-----------------------------------------------------------------------------
class GraphicsColumnAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout  The column layout.
     *      @param [in] desc    The column to add.
     *      @param [in] parent  The parent command.
     */
    GraphicsColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
                             QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~GraphicsColumnAddCommand();

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
    GraphicsColumnAddCommand(GraphicsColumnAddCommand const& rhs);
    GraphicsColumnAddCommand& operator=(GraphicsColumnAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The column.
    GraphicsColumn* column_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCOLUMNUNDOCOMMANDS_H
