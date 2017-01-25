//-----------------------------------------------------------------------------
// File: VerilogTopDefaultWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 24.01.2017
//
// Description:
// Class for writing default values for ports of the top component.
//-----------------------------------------------------------------------------

#ifndef VERILOGTOPDEFAULTWRITER_H
#define VERILOGTOPDEFAULTWRITER_H

#include <Plugins/VerilogGenerator/common/Writer.h>
#include "../veriloggeneratorplugin_global.h"

#include "Plugins/common/HDLParser/HDLParserCommon.h"

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Class for writing default values for ports of the top component.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogTopDefaultWriter : public Writer
{
public:

	/*!
     *  The constructor.
     *
     *      @param [in] mPort           The port, which default value assignment will be written, if applicable.
     */
    VerilogTopDefaultWriter(QSharedPointer<MetaPort> mPort);

	/*!
     *  The destructor.
     */
    virtual ~VerilogTopDefaultWriter();

    /*!
     *  Writes the assignments to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;

private:

	// Disable copying.
	VerilogTopDefaultWriter(VerilogTopDefaultWriter const& rhs);
	VerilogTopDefaultWriter& operator=(VerilogTopDefaultWriter const& rhs);
       
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port.
    QSharedPointer<MetaPort> mPort_;
};

#endif // VERILOGTOPDEFAULTWRITER_H