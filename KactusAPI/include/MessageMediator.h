//-----------------------------------------------------------------------------
// File: MessageMediator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.02.2018
//
// Description:
// Interface for outputting messages to the user.
//-----------------------------------------------------------------------------

#ifndef MESSAGEMEDIATOR_H
#define MESSAGEMEDIATOR_H

#include <QString>

//-----------------------------------------------------------------------------
//! Interface for outputting messages to the user.
//-----------------------------------------------------------------------------
class MessageMediator 
{
public:

    //! The destructor.
    virtual ~MessageMediator() {};

    //! Show the given message to the user.
    virtual void showMessage(QString const& message) const = 0;

    //! Show the given error to the user.
    virtual void showError(QString const& error) const = 0;

    //! Show the given failure message to the user.
    virtual void showFailure(QString const& error) const = 0;

    //! Show the given status to the user.
    virtual void showStatusMessage(QString const& status) const = 0;
};

#endif // MESSAGEMEDIATOR_H
