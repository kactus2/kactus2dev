//-----------------------------------------------------------------------------
// File: ChannelRelay.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ChannelRelay.h"


//-----------------------------------------------------------------------------
// Function: ChannelRelay::ChannelRelay()
//-----------------------------------------------------------------------------
ChannelRelay::ChannelRelay(QObject* parent):
    QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ChannelRelay::write()
//-----------------------------------------------------------------------------
void ChannelRelay::write(QString const& text)
{
    emit data(text);
}
