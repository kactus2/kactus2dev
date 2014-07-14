//-----------------------------------------------------------------------------
// File: VerilogParameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// VerilogParameter represents a Verilog parameter in a component or entity declaration.
//-----------------------------------------------------------------------------

#ifndef VERILOGPARAMETER_H
#define VERILOGPARAMETER_H

#include <kactusGenerators/HDLGenerator/HDLModelParameter.h>

#include <QString>

class ModelParameter;

//-----------------------------------------------------------------------------
//! VerilogParameter represents a Verilog parameter in a component or entity declaration.
//-----------------------------------------------------------------------------
class VerilogParameter : public HDLModelParameter
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] modelParameter   The model parameter represented by this object.
	 *      @param [in] parent           The parent object.
	 */
	VerilogParameter(ModelParameter* modelParameter, QObject* parent = 0);
	
	//! The destructor
	virtual ~VerilogParameter();

	/*! Write the contents of the parameter into text stream.
	 *
	 *      @param [in] stream   The text stream to write the parameter into.
	*/
	virtual void write(QTextStream& stream) const;

private:
	
	//! No copying.
	VerilogParameter(const VerilogParameter& other);

	//! No assignment.
	VerilogParameter& operator=(const VerilogParameter& other);
    
    /*!
     *  Returns the formatted model parameter value.
     *     
     *      @return The formatted model parameter value.
     */
    QString formattedValue() const;
};

#endif // VERILOGPARAMETER_H
