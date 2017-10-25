//-----------------------------------------------------------------------------
// File: GraphicsColumnAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.10.2017
//
// Description:
// Undo command for adding graphics columns.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCOLUMNADDCOMMAND_H
#define GRAPHICSCOLUMNADDCOMMAND_H

#include <QSharedPointer>
#include <QUndoCommand>

class GraphicsColumnLayout;
class GraphicsColumn;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! Undo command for adding graphics columns.
//-----------------------------------------------------------------------------
class GraphicsColumnAddCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] layout      The column layout.
     *      @param [in] column      The column to add.
     *      @param [in] diagram     Design diagram containing the new column.
     *      @param [in] parent      The parent command.
     */
    GraphicsColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column, DesignDiagram* diagram,
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

protected:

    /*!
     *  Get the added graphics column.
     *
     *      @return The new graphics column.
     */
    GraphicsColumn* getColumn() const;

    /*!
     *  Get the column layout.
     *
     *      @return The column layout.
     */
    GraphicsColumnLayout* getLayout() const;

    /*!
     *  Get the design diagram.
     *
     *      @return The design diagram.
     */
    DesignDiagram* getDiagram() const;

    /*!
     *  Check if the column should be deleted.
     *
     *      @return True, if the column should be deleted, false otherwise.
     */
    bool columnShouldBeDeleted() const;

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

    //! Diagram containing the columns.
    DesignDiagram* diagram_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCOLUMNADDCOMMAND_H
