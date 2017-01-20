//-----------------------------------------------------------------------------
// File: VerilogAssignmentWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 21.01.2017
//
// Description:
// Class for writing a Verilog assignments.
//-----------------------------------------------------------------------------

#ifndef VERILOGASSIGNMENTWRITER_H
#define VERILOGASSIGNMENTWRITER_H

#include <Plugins/VerilogGenerator/common/Writer.h>
#include "../veriloggeneratorplugin_global.h"

#include "Plugins/common/HDLParser/HDLParserCommon.h"

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
//!  Class for writing a Verilog assignments.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogAssignmentWriter : public Writer
{
public:

	/*!
     *  The constructor.
     */
    VerilogAssignmentWriter();

	/*!
     *  The destructor.
     */
    virtual ~VerilogAssignmentWriter();

    /*!
     *  Add a port to the writer.
     *
     *      @param [in] port          The port that needs assignment written.
     */
    void addPort(QSharedPointer<MetaPort> port);

    /*!
     *  Writes the assignments to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;    

    /*!
    *  Gets the connection assignment for a port.
    *
    *      @param [in] portName     The name of the port whose connection assignment to get.
    *
    *      @return The Verilog description of the instance port connection assignment.
    */
    static QString assignmentForPort(QSharedPointer<MetaPort> mPort, DirectionTypes::Direction inOut);

    static QString assignmentForWire(QSharedPointer<MetaPortAssignMent> mpa, QString portName);

private:

	// Disable copying.
	VerilogAssignmentWriter(VerilogAssignmentWriter const& rhs);
	VerilogAssignmentWriter& operator=(VerilogAssignmentWriter const& rhs);
       
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Ports
    QList<QSharedPointer<MetaPort> > ports_;
};

#endif // VERILOGASSIGNMENTWRITER_H
