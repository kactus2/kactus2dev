//-----------------------------------------------------------------------------
// File: Writer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2014
//
// Description:
// Interface for writers.
//-----------------------------------------------------------------------------

#ifndef WRITER_H
#define WRITER_H

#include <QTextStream>

//-----------------------------------------------------------------------------
//! Class for writing C/Verilog comments.
//-----------------------------------------------------------------------------
class Writer 
{
public:

	//! The destructor.
    virtual ~Writer() {}

    /*!
     *  Writes the content to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const = 0;

};

#endif // WRITER_H
