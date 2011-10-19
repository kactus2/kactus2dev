//-----------------------------------------------------------------------------
// File: SystemChangeCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.10.2011
//
// Description:
// Undo change commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCHANGECOMMANDS_H
#define SYSTEMCHANGECOMMANDS_H

#include <LibraryManager/vlnv.h>

#include <QUndoCommand>

class SWComponentItem;

//-----------------------------------------------------------------------------
//! SWComponentPacketizeCommand class.
//-----------------------------------------------------------------------------
class SWComponentPacketizeCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component item.
     *      @param [in] vlnv       The VLNV of the packetized component.
     *      @param [in] parent     The parent command.
     */
    SWComponentPacketizeCommand(SWComponentItem* component, VLNV const& vlnv, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWComponentPacketizeCommand();

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
    SWComponentPacketizeCommand(SWComponentPacketizeCommand const& rhs);
    SWComponentPacketizeCommand& operator=(SWComponentPacketizeCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The software component.
    SWComponentItem* component_;

    //! The component VLNV.
    VLNV vlnv_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCHANGECOMMANDS_H
