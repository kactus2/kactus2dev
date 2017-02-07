//-----------------------------------------------------------------------------
// File: VerilogWireWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.08.2014
//
// Description:
// Class for writing a Verilog wire declaration.
//-----------------------------------------------------------------------------

#ifndef VERILOGWIREWRITER_H
#define VERILOGWIREWRITER_H

#include <Plugins/VerilogGenerator/common/Writer.h>
#include "../veriloggeneratorplugin_global.h"
#include <Plugins/common/HDLParser/HDLParserCommon.h>

//-----------------------------------------------------------------------------
//! Class for writing a Verilog wire declaration.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogWireWriter : public Writer
{
public:

    //! The constructor.
    VerilogWireWriter(QString name, QPair<QString, QString> bounds);

	//! The destructor.
	virtual ~VerilogWireWriter();

    /*!
     *  Writes the wire to given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;    

private:

	// Disable copying.
	VerilogWireWriter(VerilogWireWriter const& rhs);
	VerilogWireWriter& operator=(VerilogWireWriter const& rhs);
       
    /*!
     *  Creates a Verilog wire declaration.     
     *
     *      @return The Verilog wire declaration for this wire.
     */
    QString createDeclaration() const;

    /*!
     *  Gets the formatted size for the wire.
     *
     *      @return The formatted size.
     */
    QString formattedSize() const;

    //! The wire.
    QString name_;
    QPair<QString, QString> bounds_;
};

#endif // VERILOGWIREWRITER_H
