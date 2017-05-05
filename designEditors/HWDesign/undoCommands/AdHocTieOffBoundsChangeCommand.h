//-----------------------------------------------------------------------------
// File: AdHocTieOffBoundsChangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2017
//
// Description:
// Undo command for changing tie off left and right bounds in the ad hoc editor.
//-----------------------------------------------------------------------------

#ifndef ADHOCTIEOFFBOUNDSCHANGECOMMAND_H
#define ADHOCTIEOFFBOUNDSCHANGECOMMAND_H

#include <designEditors/HWDesign/undoCommands/AdHocTiedValueCommand.h>

#include <designEditors/HWDesign/undoCommands/AdHocBoundsChangeCommand.h>

class AdHocItem;
class AdHocConnection;
class HWDesignDiagram;

#include <QUndoCommand>
#include <QSharedPointer>
#include <QString>

//-----------------------------------------------------------------------------
//! Undo command for changing tie off left and right bounds in the ad hoc editor.
//-----------------------------------------------------------------------------
class AdHocTieOffBoundsChangeCommand : public AdHocTiedValueCommand, public AdHocBoundsChangeCommand
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] portItem            The selected port item.
     *      @param [in] newLeftValue        New value for the left bound of the tie off.
     *      @param [in] newRightValue       New value for the right bound of the tie off.
     *      @param [in] tieOffConnection    Connection containing the tie off.
     *      @param [in] designDiagram       Diagram containing the HW design.
     *      @param [in] parent              The parent command.
     */
    AdHocTieOffBoundsChangeCommand(AdHocItem* portItem, QString const& newLeftValue, QString const& newRightValue,
        QSharedPointer<AdHocConnection> tieOffConnection, HWDesignDiagram* designDiagram,
        QUndoCommand* parent = 0);

    /*!
     *  The destructor.
     */
    ~AdHocTieOffBoundsChangeCommand();

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
    AdHocTieOffBoundsChangeCommand(AdHocTieOffBoundsChangeCommand const& rhs);
    AdHocTieOffBoundsChangeCommand& operator=(AdHocTieOffBoundsChangeCommand const& rhs);
};

#endif // ADHOCTIEOFFBOUNDSCHANGECOMMAND_H
