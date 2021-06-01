//-----------------------------------------------------------------------------
// File: FileChannel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.02.2021
//
// Description:
// Writable output channel for files e.g stdout.
//-----------------------------------------------------------------------------

#include "FileChannel.h"

//-----------------------------------------------------------------------------
// Function: FileChannel::FileChannel()
//-----------------------------------------------------------------------------
FileChannel::FileChannel(FILE* fileHandle):
    output_(fileHandle)
{

}

//-----------------------------------------------------------------------------
// Function: FileChannel::write()
//-----------------------------------------------------------------------------
void FileChannel::write(QString const& text)
{
    output_ << text;
    output_.flush();
}
