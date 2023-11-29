//-----------------------------------------------------------------------------
// File: NullChannel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.11.2023
//
// Description:
// Null output for user messages.
//-----------------------------------------------------------------------------

#ifndef NULL_CHANNEL_H
#define NULL_CHANNEL_H

#include "MessageMediator.h"

//-----------------------------------------------------------------------------
//! Null output for user messages.
//-----------------------------------------------------------------------------
class NullChannel: public MessageMediator
{
public:

    //! The constructor.
    explicit NullChannel() = default;

    //! The destructor.
    ~NullChannel() final = default;
        
    //! Show the given message to the user.
    void showMessage([[maybe_unused]] QString const& message) const final;
    
    //! Show the given error to the user.
    void showError([[maybe_unused]] QString const& error) const final;

    //! Show the given failure message to the user.
    void showFailure([[maybe_unused]] QString const& error) const final;

    //! Show the given status to the user.
    void showStatusMessage([[maybe_unused]] QString const& status) const final;

};

#endif // NULL_CHANNEL_H
