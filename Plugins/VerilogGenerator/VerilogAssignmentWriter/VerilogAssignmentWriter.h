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
        QSharedPointer<MetaPort> mPort,
        bool isInHierPort);

	/*!
     *  The destructor.
     */
    virtual ~VerilogAssignmentWriter() = default;

    // Disable copying.
    VerilogAssignmentWriter(VerilogAssignmentWriter const& rhs) = delete;
    VerilogAssignmentWriter& operator=(VerilogAssignmentWriter const& rhs) = delete;

    /*!
     *  Writes the assignments to the given output.
     *
     *    @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const override;    
    
private:

    /*!
    *  Creates an assignment for a port.
    *
    *    @return The Verilog description of the port connection assignment.
    */
    QString assignmentForPort() const;

    /*!
    *  Creates the logical side of the port assignment.
    *
    *    @return The logical side for the port connection.
    */
    QString createLogicalAssignment(bool assignToPort) const;

    /*!
    *  Creates the physical side of the port assignment.
    *
    *    @return The physical side for the port connection.
    */
    QString createPhysicalAssignment() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Ports
    QString portWireName_;

    QSharedPointer<MetaPortAssignment> portAssignment_;

    QSharedPointer<MetaPort> mPort_;

    bool isHierarchicalPort_;
};

#endif // VERILOGASSIGNMENTWRITER_H
