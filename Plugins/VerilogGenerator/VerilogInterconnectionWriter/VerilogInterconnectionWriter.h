//-----------------------------------------------------------------------------
// File: VerilogInterconnectionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 07.09.2014
//
// Description:
// Class for writing a Verilog interconnection declaration.
//-----------------------------------------------------------------------------

#ifndef VERILOGINTERCONNECTIONWRITER_H
#define VERILOGINTERCONNECTIONWRITER_H

#include <Plugins/VerilogGenerator/common/Writer.h>
#include "../veriloggeneratorplugin_global.h"
#include <Plugins/common/HDLParser/HDLParserCommon.h>

//-----------------------------------------------------------------------------
//! Class for writing a Verilog wire declaration.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogInterconnectionWriter : public Writer
{
public:

	//! The constructor.
	VerilogInterconnectionWriter(QSharedPointer<GenerationInterconnection> interconnection);

	//! The destructor.
	virtual ~VerilogInterconnectionWriter();

    /*!
     *  Writes the interconnection to given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;    

private:

	// Disable copying.
	VerilogInterconnectionWriter(VerilogInterconnectionWriter const& rhs);
	VerilogInterconnectionWriter& operator=(VerilogInterconnectionWriter const& rhs);

    /*!
     *  Gets the indentation to use.
     *
     *      @return The indentation to use.
     */
    QString indentation() const;

    /*!
     *  Gets the instance-specific parameter assignments.
     *
     *      @return The Verilog description of the instance parameters and their values.
     */
    QString parameterAssignments() const;

    /*!
     *  Gets the formatted size for the wire.
     *
     *      @return The formatted size.
     */
    QString formattedSize() const;

    //! The interconnection.
    QSharedPointer<GenerationInterconnection> interconnection_;
};

#endif // VERILOGINTERCONNECTIONWRITER_H
