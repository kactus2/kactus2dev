//-----------------------------------------------------------------------------
// File: SWPortDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete command for SW port item in system design.
//-----------------------------------------------------------------------------

#ifndef SWPORTDELETECOMMAND_H
#define SWPORTDELETECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class SWPortItem;
class SystemComponentItem;
class Design;

//-----------------------------------------------------------------------------
//! SWPortDeleteCommand class.
//-----------------------------------------------------------------------------
class SWPortDeleteCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] port                The port to delete.
     *      @param [in] containingDesign    Design containing the SW port.
     *      @param [in] parent              The owner of this command.
     */
    SWPortDeleteCommand(SWPortItem* port, QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWPortDeleteCommand();

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
    SWPortDeleteCommand(SWPortDeleteCommand const& rhs);
    SWPortDeleteCommand& operator=(SWPortDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram port.
    SWPortItem* port_;

    //! The port's parent.
    SystemComponentItem* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SWPORTDELETECOMMAND_H
