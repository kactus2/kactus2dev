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
    VerilogAssignmentWriter(QString portWireName,
        QSharedPointer<MetaPortAssignment> portAssignment,
        DirectionTypes::Direction direction,
        bool isInHierPort);

	/*!
     *  The destructor.
     */
    virtual ~VerilogAssignmentWriter();

    /*!
     *  Writes the assignments to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;    
    
    /*!
    *  Creates an assignment for a port.
    *
    *      @return The Verilog description of the port connection assignment.
    */
    QString assignmentForPort() const;

private:

	// Disable copying.
	VerilogAssignmentWriter(VerilogAssignmentWriter const& rhs);
	VerilogAssignmentWriter& operator=(VerilogAssignmentWriter const& rhs);
       
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Ports
    QString portWireName_;
    QSharedPointer<MetaPortAssignment> mpa_;
    DirectionTypes::Direction direction_;
    bool isInHierPort_;
};

#endif // VERILOGASSIGNMENTWRITER_H
