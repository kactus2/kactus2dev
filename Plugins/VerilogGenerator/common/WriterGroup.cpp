//-----------------------------------------------------------------------------
// File: WriterGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.08.2014
//
// Description:
// Composite for writers.
//-----------------------------------------------------------------------------

#include "WriterGroup.h"

//-----------------------------------------------------------------------------
// Function: WriterGroup::WriterGroup()
//-----------------------------------------------------------------------------
WriterGroup::WriterGroup(): childWriters_()
{

}

//-----------------------------------------------------------------------------
// Function: WriterGroup::~WriterGroup()
//-----------------------------------------------------------------------------
WriterGroup::~WriterGroup()
{

}

//-----------------------------------------------------------------------------
// Function: WriterGroup::write()
//-----------------------------------------------------------------------------
void WriterGroup::write(QTextStream& output) const
{
    foreach(QSharedPointer<Writer> writer, childWriters_)
    {
        writer->write(output);
    }

    if (!childWriters_.isEmpty())
    {
        output << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: WriterGroup::add()
//-----------------------------------------------------------------------------
void WriterGroup::add(QSharedPointer<Writer> writer)
{
    childWriters_.append(writer);
}

//-----------------------------------------------------------------------------
// Function: WriterGroup::hasWriters()
//-----------------------------------------------------------------------------
bool WriterGroup::hasWriters() const
{
    return !childWriters_.isEmpty();
}
