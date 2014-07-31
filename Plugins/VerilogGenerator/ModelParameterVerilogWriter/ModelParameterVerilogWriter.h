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

#include <kactusGenerators/HDLGenerator/HDLModelParameter.h>

#include <QString>
#include <QSharedPointer>

class ModelParameter;

//-----------------------------------------------------------------------------
//! ModelParameterVerilogWriter represents a Verilog parameter in a component or entity declaration.
//-----------------------------------------------------------------------------
class ModelParameterVerilogWriter
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] modelParameter   The model parameter represented by this object.
	 *      @param [in] parent           The parent object.
	 */
	ModelParameterVerilogWriter(QSharedPointer<ModelParameter> modelParameter);
	
	//! The destructor
	virtual ~ModelParameterVerilogWriter();

	/*! Write the contents of the parameter into text stream.
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
     *  Returns the formatted model parameter value.
     *     
     *      @return The formatted model parameter value.
     */
    QString formattedValue() const;

    QSharedPointer<ModelParameter> modelParameter_;
};

#endif // ModelParameterVerilogWriter_H
