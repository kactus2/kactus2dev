//-----------------------------------------------------------------------------
// File: WriteChannel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef WRITECHANNEL_H
#define WRITECHANNEL_H

#include <QString>

class WriteChannel
{
public:
    
    //! The destructor.
    virtual ~WriteChannel() = default;

    virtual void write(QString const& text) = 0;

protected:

    //! The constructor.
    WriteChannel() = default;
};

#endif // WRITECHANNEL_H
