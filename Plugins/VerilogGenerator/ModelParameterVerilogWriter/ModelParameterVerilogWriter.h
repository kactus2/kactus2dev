//-----------------------------------------------------------------------------
// File: ModelParameterVerilogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------

#ifndef ModelParameterVerilogWriter_H
#define ModelParameterVerilogWriter_H

#include "../veriloggeneratorplugin_global.h"

#include <QString>
#include <QSharedPointer>
#include <QTextStream>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT ModelParameterVerilogWriter
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] modelParameter   The model parameter represented by this object.
	 */
	ModelParameterVerilogWriter(QSharedPointer<ModelParameter> modelParameter);
	
	//! The destructor
	~ModelParameterVerilogWriter();

	/*! Write the contents of the model parameter into text stream.
	 *
	 *      @param [in] output   The text stream to write the model parameter into.
	*/
	virtual void write(QTextStream& output) const;

private:
	
	//! No copying.
	ModelParameterVerilogWriter(const ModelParameterVerilogWriter& other);

	//! No assignment.
	ModelParameterVerilogWriter& operator=(const ModelParameterVerilogWriter& other);

    /*!
     *  Checks if the writer should write nothing.
     *
     *      @return True, if the writer has nothing to write, otherwise false.
     */
    bool nothingToWrite() const;

    /*!
     *  Creates a Verilog parameter declaration from the model parameter.
     *
     *      @return A Verilog parameter declaration.
     */
    QString createDeclaration() const;

    /*!
     *  Returns the formatted model parameter value.
     *     
     *      @return The formatted model parameter value.
     */
    QString formattedValue() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The model parameter to write to Verilog.
    QSharedPointer<ModelParameter> modelParameter_;
};

#endif // ModelParameterVerilogWriter_H
