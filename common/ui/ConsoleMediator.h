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

#include "MessageMediator.h"
//-----------------------------------------------------------------------------
//! Console output for user messages.
//-----------------------------------------------------------------------------
class ConsoleMediator: public MessageMediator
{
public:

    //! The constructor.
    explicit ConsoleMediator();

    //! The destructor.
    virtual ~ConsoleMediator();
        
    //! Show the given message to the user.
    virtual void showMessage(QString const& message) const;
    
    //! Show the given error to the user.
    virtual void showError(QString const& error) const;

    //! Show the given failure message to the user.
    virtual void showFailure(QString const& error) const;

    //! Show the given status to the user.
    virtual void showStatusMessage(QString const& status) const;

private:

    // Disable copying.
    ConsoleMediator(ConsoleMediator const& rhs);
    ConsoleMediator& operator=(ConsoleMediator const& rhs);
};

#endif // CONSOLEMEDIATOR_H
