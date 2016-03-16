//-----------------------------------------------------------------------------
// File: ComponentInstancePasteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2016
//
// Description:
// Undo command for pasting a component instance in a design.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEPASTECOMMAND_H
#define COMPONENTINSTANCEPASTECOMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>

class Component;
class ComponentInstance;
class ComponentItem;
class Design;
class DesignDiagram;
class GraphicsColumn;

//-----------------------------------------------------------------------------
//! Undo command for pasting a component instance in a design.
//-----------------------------------------------------------------------------
class ComponentInstancePasteCommand : public QUndoCommand
{
public:

	//! The constructor.
    ComponentInstancePasteCommand(QSharedPointer<Component> component,
        QSharedPointer<ComponentInstance> componentInstance, QPointF const& position, 
        GraphicsColumn* column, DesignDiagram* diagram, QUndoCommand* parent);

	//! The destructor.
	~ComponentInstancePasteCommand();

    virtual void undo();

    virtual void redo();


private:

	// Disable copying.
	ComponentInstancePasteCommand(ComponentInstancePasteCommand const& rhs);
	ComponentInstancePasteCommand& operator=(ComponentInstancePasteCommand const& rhs);

    //! The component instance to add in the design.
    QSharedPointer<ComponentInstance> componentInstance_;

    //! The target design.
    QSharedPointer<Design> design_;
};

#endif // COMPONENTINSTANCEPASTECOMMAND_H