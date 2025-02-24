//-----------------------------------------------------------------------------
// File: GraphicsColumnUndoCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 1.6.2012
//
// Description:
// Undo commands for graphics columns.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCOLUMNUNDOCOMMANDS_H
#define GRAPHICSCOLUMNUNDOCOMMANDS_H

#include <IPXACTmodels/kactusExtensions/ColumnDesc.h>

#include <QSharedPointer>
#include <QUndoCommand>
#include <QPointF>

class GraphicsColumnLayout;
class GraphicsColumn;
class Design;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! GraphicsColumnMoveCommand class.
//-----------------------------------------------------------------------------
class GraphicsColumnMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] layout  The column layout.
     *    @param [in] column  The column to move.
     *    @param [in] oldPos  The column's old position.
     *    @param [in] parent  The parent command.
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
//! GraphicsColumnChangeCommand class.
//-----------------------------------------------------------------------------
class GraphicsColumnChangeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] column      The column.
     *    @param [in] newDesc     The column's new description.
     *    @param [in] diagram     Diagram containing the scene of the column.
     */
    GraphicsColumnChangeCommand(GraphicsColumn* column, QSharedPointer<ColumnDesc> newDesc,
        DesignDiagram* diagram, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~GraphicsColumnChangeCommand();

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
    GraphicsColumnChangeCommand(GraphicsColumnChangeCommand const& rhs);
    GraphicsColumnChangeCommand& operator=(GraphicsColumnChangeCommand const& rhs);

    void changeModifiedColumnInDesign(QSharedPointer<ColumnDesc> newColumn);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram column.
    GraphicsColumn* columnItem_;
    
    //! The column description.
    QSharedPointer<ColumnDesc> columnDesc_;

    //! The column's old description.
    QSharedPointer<ColumnDesc> oldDesc_;

    //! The column's new description.
    QSharedPointer<ColumnDesc> newDesc_;

    //! The design containing the modified column.
    QSharedPointer<Design> containingDesign_;

    //! Diagram containing the scene of the column.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------
//! GraphicsColumnResizeCommand class.
//-----------------------------------------------------------------------------
class GraphicsColumnResizeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] column      The column.
     *    @param [in] oldWidth    The column's old width.
     *    @param [in] diagram     Diagram containing the scene of the column.
     */
    GraphicsColumnResizeCommand(GraphicsColumn* column, unsigned int oldWidth, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~GraphicsColumnResizeCommand();

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
    GraphicsColumnResizeCommand(GraphicsColumnResizeCommand const& rhs);
    GraphicsColumnResizeCommand& operator=(GraphicsColumnResizeCommand const& rhs);

    /*!
     *  Change the width of the column.
     *
     *    @param [in] selectedWidth   The new width for the column.
     */
    void changeWidth(unsigned int selectedWidth);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram column.
    GraphicsColumn* column_;

    //! The old width.
    unsigned int oldWidth_;

    //! The new width.
    unsigned int newWidth_;

    //! Diagram containing the scene of the column.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCOLUMNUNDOCOMMANDS_H
