//-----------------------------------------------------------------------------
// File: VerilogTiedValueWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 13.05.2016
//
// Description:
// Class for writing a Verilog port tied value declaration.
//-----------------------------------------------------------------------------

#ifndef VERILOGTIEDVALUEWRITER_H
#define VERILOGTIEDVALUEWRITER_H

#include <Plugins/VerilogGenerator/common/Writer.h>
#include "../veriloggeneratorplugin_global.h"

#include <QSharedPointer>

class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Class for writing a Verilog port tied value declaration.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogTiedValueWriter : public Writer
{
public:

	/*!
     *  The constructor.
     */
    VerilogTiedValueWriter(QSharedPointer<ExpressionFormatter> formatter);

	/*!
     *  The destructor.
     */
    virtual ~VerilogTiedValueWriter();

    /*!
     *  Add a port tied value to the writer.
     *
     *      @param [in] portName    The name of the port containing the tied value.
     *      @param [in] tiedValue   The new tied value.
     */
    void addPortTiedValue(QString const& portName, QString const& tiedValue);

    /*!
     *  Writes the tied values to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;    

private:

	// Disable copying.
	VerilogTiedValueWriter(VerilogTiedValueWriter const& rhs);
	VerilogTiedValueWriter& operator=(VerilogTiedValueWriter const& rhs);
       
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Port name, tied value pairs.
    QMap<QString, QString> portTiedValues_;

    //! The used expression formatter.
    QSharedPointer<ExpressionFormatter> formatter_;
};

#endif // VERILOGTIEDVALUEWRITER_H
