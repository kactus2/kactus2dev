//-----------------------------------------------------------------------------
// File: HDLModelParameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for all HDL representations of model parameters.
//-----------------------------------------------------------------------------

#ifndef HDLMODELPARAMETER_H
#define HDLMODELPARAMETER_H

#include "HDLObject.h"

#include <QTextStream>
#include <QObject>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Base class for all HDL representations of model parameters.
//-----------------------------------------------------------------------------
class HDLModelParameter : public HDLObject
{
	Q_OBJECT

public:

	/*!
	 *  <Description>.
	 *
	 *      @param [in] modelParameter      The represented model parameter.
	 *      @param [in] parent              The parent object.
	 */
	HDLModelParameter(ModelParameter* modelParameter, QObject* parent = 0);
	
	//! The destructor
	virtual ~HDLModelParameter();

	/*! Write the contents of the model parameter into text stream.
	 *
	 *      @param [in]  stream The text stream to write the model parameter into.
	 *
	*/
	virtual void write(QTextStream& stream) const = 0;

private:
	
	//! \brief No copying
	HDLModelParameter(const HDLModelParameter& other);

	//! \brief No assignment
	HDLModelParameter& operator=(const HDLModelParameter& other);
};

#endif // HDLMODELPARAMETER_H
