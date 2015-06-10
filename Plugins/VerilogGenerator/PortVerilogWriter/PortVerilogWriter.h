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

#include "../veriloggeneratorplugin_global.h"
#include "../common/Writer.h"

#include <IPXACTmodels/port.h>

#include <QTextStream>

class ExpressionFormatter;

//-----------------------------------------------------------------------------
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT PortVerilogWriter : public Writer
{
public:
    /*!
     *  The constructor.
     *
     *      @param [in] port                    The port to write to Verilog.
     *      @param [in] expressionFormatter     Formatter for expressions.
     */
    PortVerilogWriter(QSharedPointer<const Port> port, QSharedPointer<ExpressionFormatter> expressionFormatter);

    //! The destructor.
    ~PortVerilogWriter();

	/*! Writes the Verilog port declaration into a text stream.
	 *
	 *      @param [in] output   The text stream to write the port into.
	 */
    void write(QTextStream& outputStream) const;

private:
    
    //! No copying.
    PortVerilogWriter(const PortVerilogWriter& other);

    //! No assignment.
    PortVerilogWriter& operator=(const PortVerilogWriter& other);

    /*!
     *  Checks if the writer should write nothing.
     *
     *      @return True, if the writer has nothing to write, otherwise false.
     */
    bool nothingToWrite() const;

    /*!
     *  Creates a Verilog port declaration from the port model.     
     *
     *      @return A Verilog port declaration.
     */
    QString createDeclaration() const; 

    /*!
     *  Creates a string representation of the port direction.
     *
     *      @return The port direction.
     */
    QString direction() const;
    
    /*!
     *  Creates array bounds representation for the port.
     *
     *      @return Array bounds for the port.
     */
    QString arrayAndVectorBounds() const;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port to write to Verilog.
    QSharedPointer<const Port> port_;

    //! The formatter to use for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;
};

#endif // PORTVERILOGWRITER_H
