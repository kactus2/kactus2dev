//-----------------------------------------------------------------------------
// File: SWInterfaceDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete command for SW interface in system design.
//-----------------------------------------------------------------------------

#ifndef SWINTERFACEDELETECOMMAND_H
#define SWINTERFACEDELETECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class IGraphicsItemStack;
class SWInterfaceItem;
class ApiInterface;
class ComInterface;

class Design;

//-----------------------------------------------------------------------------
//! SWInterfaceDeleteCommand class.
//-----------------------------------------------------------------------------
class SWInterfaceDeleteCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] interface           The interface to delete.
     *      @param [in] containingDesign    Design containing the SW interface.
     *      @param [in] parent              The owner of this command.
     */
    SWInterfaceDeleteCommand(SWInterfaceItem* interface, QSharedPointer<Design> containingDesign,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWInterfaceDeleteCommand();

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
    SWInterfaceDeleteCommand(SWInterfaceDeleteCommand const& rhs);
    SWInterfaceDeleteCommand& operator=(SWInterfaceDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interface item.
    SWInterfaceItem* interface_;

    //! The API interface (if API).
    QSharedPointer<ApiInterface> apiInterface_;

    //! The COM interface (if COM).
    QSharedPointer<ComInterface> comInterface_;

    //! The parent stack.
    IGraphicsItemStack* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SWINTERFACEDELETECOMMAND_H
