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
class Component;

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
     *      @param [in] component           The component containing the design.
     *      @param [in] parent              The owner of this command.
     */
    SWInterfaceDeleteCommand(SWInterfaceItem* interface, QSharedPointer<Design> containingDesign,
        QSharedPointer<Component> component, QUndoCommand* parent = 0);

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
    SWInterfaceItem* interfaceItem_;

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

    //! The design containing the interface.
    QSharedPointer<Design> containingDesign_;

    //! The component containing the SW interface.
    QSharedPointer<Component> containingComponent_;
};

//-----------------------------------------------------------------------------

#endif // SWINTERFACEDELETECOMMAND_H
