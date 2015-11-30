//-----------------------------------------------------------------------------
// File: ComponentDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a component item in a design diagram.
//-----------------------------------------------------------------------------

#ifndef COMPONENTDELETECOMMAND_H
#define COMPONENTDELETECOMMAND_H

#include <QUndoCommand>
#include <QObject>

class ComponentItem;
class DesignDiagram;
class GraphicsColumn;
class HWComponentItem;

//-----------------------------------------------------------------------------
//! Undo command for removing a component item in a design diagram.
//-----------------------------------------------------------------------------
class ComponentDeleteCommand : public QObject, public QUndoCommand
{
	Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] diagram     The diagram containing the component item.
     *      @param [in] column      The column containing the component item.
     *      @param [in] component   The component item to remove.
     *      @param [in] parent      The parent command.
     */
    ComponentDeleteCommand(DesignDiagram* diagram, GraphicsColumn* column, HWComponentItem* component,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentDeleteCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

	//! Emitted when a new component is instantiated to the design.
	void componentInstantiated(ComponentItem*);

	//! Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem*);

private:
    // Disable copying.
    ComponentDeleteCommand(ComponentDeleteCommand const& rhs);
    ComponentDeleteCommand& operator=(ComponentDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram component.
    HWComponentItem* componentItem_;

    //! The component's parent column.
    GraphicsColumn* column_;

    //! The graphics diagram.
    DesignDiagram* diagram_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

#endif // COMPONENTDELETECOMMAND_H
