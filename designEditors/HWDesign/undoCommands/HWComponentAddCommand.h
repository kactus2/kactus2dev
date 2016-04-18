//-----------------------------------------------------------------------------
// File: HWComponentAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.04.2016
//
// Description:
// Undo add command for component items in HW design.
//-----------------------------------------------------------------------------

#ifndef HWCOMPONENTADDCOMMAND_H
#define HWCOMPONENTADDCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class IGraphicsItemStack;
class ComponentItem;
class Design;

//-----------------------------------------------------------------------------
//! Undo add command for component items in HW design.
//-----------------------------------------------------------------------------
class HWComponentAddCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in] design  The design containing the hw component.
     *      @param [in] stack   The stack to which to add the item.
     *      @param [in] item    The item to add.
     *      @param [in] parent  The parent command.
     */
    HWComponentAddCommand(QSharedPointer<Design> design, IGraphicsItemStack* stack, ComponentItem* item,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~HWComponentAddCommand();

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
    HWComponentAddCommand(HWComponentAddCommand const& rhs);
    HWComponentAddCommand& operator=(HWComponentAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design containing the hw component item.
    QSharedPointer<Design> containingDesign_;

    //! The graphics item.
    ComponentItem* item_;

    //! The item's parent column.
    IGraphicsItemStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // HWCOMPONENTADDCOMMAND_H