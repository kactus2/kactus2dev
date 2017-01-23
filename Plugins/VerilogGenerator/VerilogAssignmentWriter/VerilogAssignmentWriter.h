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
    *  Gets an assignment for a hierarchical port.
    *
    *      @param [in] mpa          The assignment that is currently under inspection.
    *      @param [in] mPort        The port where the assignment belongs to.
    *      @param [in] isOutPort    Must be true, if port is output or inout, else must be false.
    *
    *      @return The Verilog description of the hierarchical port connection assignment.
    */
    static QString assignmentForHierPort(QSharedPointer<MetaPortAssignMent> mpa, QSharedPointer<MetaPort> mPort,
        bool isOutPort);

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
