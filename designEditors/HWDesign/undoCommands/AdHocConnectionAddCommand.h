//-----------------------------------------------------------------------------
// File: AhHocConnectionAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.10.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef ADHOCCONNECTION_ADD_COMMAND_H
#define ADHOCCONNECTION_ADD_COMMAND_H

#include <QSharedPointer>
#include <QUndoCommand>

class AdHocConnection;
class AdHocConnectionItem;
class Design;
class DesignDiagram;
//-----------------------------------------------------------------------------
//! AhHocConnectionAddCommand class.
//-----------------------------------------------------------------------------
class AdHocConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] diagram     The diagram containing the item.
     *      @param [in] conn        The adhoc connection to add.
     *      @param [in] design      The design containing the adhoc connection.
     *      @param [in] parent      The parent command.
     */
    AdHocConnectionAddCommand(DesignDiagram* diagram, AdHocConnectionItem* conn, QSharedPointer<Design> design,
        QUndoCommand* parent = 0);

    //! The destructor.
    ~AdHocConnectionAddCommand();

    //! Undoes the command.
    virtual void undo();

    //! Redoes the command.
    virtual void redo();

private:
    // Disable copying.
    AdHocConnectionAddCommand(AdHocConnectionAddCommand const& rhs);
    AdHocConnectionAddCommand& operator=(AdHocConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    AdHocConnectionItem* connection_;

    //! The diagram containing the added item.
    DesignDiagram* scene_;

    //! The design containing the ad hoc connection represented by the item.
    QSharedPointer<Design> design_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;

    //! If true, port copy operation is a part of this undo command.
    bool portsCopied_;
};

#endif // ADHOCCONNECTION_ADD_COMMAND_H
