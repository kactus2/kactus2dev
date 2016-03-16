//-----------------------------------------------------------------------------
// File: SystemComponentAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.02.2016
//
// Description:
// Undo add command for component items in system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOMPONENTADDCOMMAND_H
#define SYSTEMCOMPONENTADDCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class ComponentItem;
class IGraphicsItemStack;
class SystemComponentItem;

class Design;

//-----------------------------------------------------------------------------
//! Undo add command for component items in system designs.
//-----------------------------------------------------------------------------
class SystemComponentAddCommand: public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] stack               The graphics item stack to which to add the item.
     *      @param [in] item                The graphics item to add.
     *      @param [in] containingDesign    The design containing the system component item.
     *      @param [in] parent              The parent command.
     */
    SystemComponentAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item,
        QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemComponentAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

	//! Signaled when the component is instantiated to the design.
	void componentInstantiated(ComponentItem* comp);

	//! Signaled when the component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem* comp);

private:
    // Disable copying.
    SystemComponentAddCommand(SystemComponentAddCommand const& rhs);
    SystemComponentAddCommand& operator=(SystemComponentAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The item's parent component stack.
    IGraphicsItemStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;

    //! The uuID of the containing HW component instance.
    QString newHwComponentId_;

    //! The design containing the system component item.
    QSharedPointer<Design> containingDesign_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCOMPONENTADDCOMMAND_H
