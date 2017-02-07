//-----------------------------------------------------------------------------
// File: MessagePasser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 02.02.2017
//
// Description:
// Class for passing messages to UI.
//-----------------------------------------------------------------------------

#include "MessagePasser.h"

//-----------------------------------------------------------------------------
// Function: MessagePasser::MessagePasser()
//-----------------------------------------------------------------------------
MessagePasser::MessagePasser() : QObject(0)
{
}

//-----------------------------------------------------------------------------
// Function: MessagePasser::~MessagePasser()
//-----------------------------------------------------------------------------
MessagePasser::~MessagePasser()
{
}

//-----------------------------------------------------------------------------
// Function: MessagePasser::sendError()
//-----------------------------------------------------------------------------
void MessagePasser::sendError(QString const& message) const
{
    emit errorMessage(message);
}

//-----------------------------------------------------------------------------
// Function: MessagePasser::sendNotice()
//-----------------------------------------------------------------------------
void MessagePasser::sendNotice(QString const& message) const
{
    emit noticeMessage(message);
}