//-----------------------------------------------------------------------------
// File: NullChannel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.11.2023
//
// Description:
// Null output for user messages.
//-----------------------------------------------------------------------------

#include <KactusAPI/include/NullChannel.h>

//-----------------------------------------------------------------------------
// Function: NullChannel::showMessage()
//-----------------------------------------------------------------------------
void NullChannel::showMessage(QString const& /*message*/) const
{
    // Do nothing.
}

//-----------------------------------------------------------------------------
// Function: NullChannel::showError()
//-----------------------------------------------------------------------------
void NullChannel::showError(QString const& /*error*/) const
{
    // Do nothing.
}

//-----------------------------------------------------------------------------
// Function: NullChannel::showFailure()
//-----------------------------------------------------------------------------
void NullChannel::showFailure(QString const& /*error*/) const
{
    // Do nothing.
}

//-----------------------------------------------------------------------------
// Function: NullChannel::showStatusMessage()
//-----------------------------------------------------------------------------
void NullChannel::showStatusMessage(QString const& /*status*/) const
{
    // Do nothing.
}
