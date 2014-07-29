//-----------------------------------------------------------------------------
// File: PortVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------

#ifndef PORTVERILOGWRITER_H
#define PORTVERILOGWRITER_H

#include <IPXACTmodels/port.h>

#include <QTextStream>


class PortVerilogWriter 
{
public:
    PortVerilogWriter(QSharedPointer<const Port> port);
    ~PortVerilogWriter();

    void write(QTextStream& outputStream);

private:

    bool shouldNotWrite();

    void writeIndent(QTextStream& outputStream);

    void writeDirection(QTextStream& outputStream);

    void writeType(QTextStream& outputStream);

    void writeName(QTextStream& outputStream);

    void writeDescription(QTextStream& outputStream);

    QSharedPointer<const Port> port_;
};

#endif // PORTVERILOGWRITER_H
