//-----------------------------------------------------------------------------
// File: VerilogTiedValueWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.05.2016
//
// Description:
// Class for writing a Verilog port tied value declaration.
//-----------------------------------------------------------------------------

#include "VerilogTiedValueWriter.h"

//-----------------------------------------------------------------------------
// Function: VerilogTiedValueWriter::VerilogTiedValueWriter()
//-----------------------------------------------------------------------------
VerilogTiedValueWriter::VerilogTiedValueWriter():
portTiedValues_()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogTiedValueWriter::~VerilogTiedValueWriter()
//-----------------------------------------------------------------------------
VerilogTiedValueWriter::~VerilogTiedValueWriter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogTiedValueWriter::addPortTiedValue()
//-----------------------------------------------------------------------------
void VerilogTiedValueWriter::addPortTiedValue(QString const& portName, QString const& tiedValue)
{
    QString formattedTiedValue = tiedValue;

    portTiedValues_.insert(portName, formattedTiedValue);
}

//-----------------------------------------------------------------------------
// Function: VerilogTiedValueWriter::write()
//-----------------------------------------------------------------------------
void VerilogTiedValueWriter::write(QTextStream& output) const
{
    if (!portTiedValues_.isEmpty())
    {
        QString informationLine = "    // Tie off values for the ports of the encompassing component";

        output << informationLine << endl;

        QMapIterator<QString, QString> tieInterceptor(portTiedValues_);
        while (tieInterceptor.hasNext())
        {
            tieInterceptor.next();

			if (!tieInterceptor.value().isEmpty())
			{
				QString outputLine = "    assign " + tieInterceptor.key() + " = " + tieInterceptor.value() + ";";

				output << outputLine << endl;
			}
        }

        output << endl;
    }
}
