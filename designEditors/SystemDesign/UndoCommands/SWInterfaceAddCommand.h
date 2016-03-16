//-----------------------------------------------------------------------------
// File: SWInterfaceAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.02.2016
//
// Description:
// Undo add command for SW interfaces.
//-----------------------------------------------------------------------------

#ifndef SWINTERFACEADDCOMMAND_H
#define SWINTERFACEADDCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class IGraphicsItemStack;
class SWInterfaceItem;
class ApiInterface;
class ComInterface;

class Design;

// -----------------------------------------------------------------------------
// ! Undo add command for SW interfaces.
// -----------------------------------------------------------------------------
class SWInterfaceAddCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] stack               The graphics item stack to which to paste the item.
     *      @param [in] item                The SW interface item to paste.
     *      @param [in] containingDesign    The design containing the interface.
     *      @param [in] parent              The parent command.
     */
    SWInterfaceAddCommand(IGraphicsItemStack* stack, SWInterfaceItem* item,
        QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWInterfaceAddCommand();

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
    SWInterfaceAddCommand(SWInterfaceAddCommand const& rhs);
    SWInterfaceAddCommand& operator=(SWInterfaceAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The SW interface item.
    SWInterfaceItem* interfaceItem_;

    //! The API interface (if API).
    QSharedPointer<ApiInterface> apiInterface_;

    //! The COM interface (if COM).
    QSharedPointer<ComInterface> comInterface_;

    //! The item's parent component stack.
    IGraphicsItemStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;

    //! The design containing the interface.
    QSharedPointer<Design> containingDesign_;
};

//-----------------------------------------------------------------------------

#endif // SWINTERFACEADDCOMMAND_H
