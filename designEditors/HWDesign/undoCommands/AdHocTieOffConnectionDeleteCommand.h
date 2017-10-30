//-----------------------------------------------------------------------------
// File: AdHocTieOffConnectionDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.10.2017
//
// Description:
// Undo command for deleting ad hoc tie off connection.
//-----------------------------------------------------------------------------

#ifndef ADHOCTIEOFFCONNECTIONDELETECOMMAND_H
#define ADHOCTIEOFFCONNECTIONDELETECOMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>

class AdHocConnection;
class Design;

//-----------------------------------------------------------------------------
//! Undo command for deleting ad hoc tie off connection.
//-----------------------------------------------------------------------------
class AdHocTieOffConnectionDeleteCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] containingDesign    The design containing the ad hoc tie off connection.
     *      @param [in] connection          The deleted ad hoc connection containing a tie off value.
     *      @param [in] parent              The parent undo command.
     */
    AdHocTieOffConnectionDeleteCommand(QSharedPointer<Design> containingDesign,
        QSharedPointer<AdHocConnection> connection, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AdHocTieOffConnectionDeleteCommand();

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
    AdHocTieOffConnectionDeleteCommand(AdHocTieOffConnectionDeleteCommand const& rhs);
    AdHocTieOffConnectionDeleteCommand& operator=(AdHocTieOffConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design containing the ad hoc tie off connection.
    QSharedPointer<Design> containingDesign_;

    //! The deleted ad hoc connection containing a tie off value.
    QSharedPointer<AdHocConnection> tieOffConnection_;
};

#endif // ADHOCTIEOFFCONNECTIONDELETECOMMAND_H
