//-----------------------------------------------------------------------------
// File: ConsoleMediator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.02.2018
//
// Description:
// Console output for user messages.
//-----------------------------------------------------------------------------

#ifndef CONSOLEMEDIATOR_H
#define CONSOLEMEDIATOR_H

#include <KactusAPI/KactusAPIGlobal.h>

#include "MessageMediator.h"
//-----------------------------------------------------------------------------
//! Console output for user messages.
//-----------------------------------------------------------------------------
class KACTUS2_API ConsoleMediator: public MessageMediator
{
public:

    //! The constructor.
    explicit ConsoleMediator() = default;

    //! The destructor.
    ~ConsoleMediator() final = default;
        
    //! Show the given message to the user.
    void showMessage(QString const& message) const final;
    
    //! Show the given error to the user.
    void showError(QString const& error) const final;

    //! Show the given failure message to the user.
    void showFailure(QString const& error) const final;

    //! Show the given status to the user.
    void showStatusMessage(QString const& status) const final;

};

#endif // CONSOLEMEDIATOR_H
