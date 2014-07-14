/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERIC_H
#define VHDLGENERIC_H

#include <kactusGenerators/HDLGenerator/HDLModelParameter.h>

#include <QString>

class ModelParameter;

/*! VhdlGeneric represents a vhdl generic in a component or entity declaration.
 *
 */
class VhdlGeneric : public HDLModelParameter {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * @param [in] parent   Pointer to the owner of this vhdl generic.
	 * @param [in] generic  Pointer to the model parameter to create the generic from.
	 *
	*/
	VhdlGeneric(ModelParameter* generic, QObject* parent = 0);
	
	//! The destructor
	virtual ~VhdlGeneric();

	/*! Write the contents of the generic into text stream.
	 *
	 * @param [in] stream   The text stream to write the generic into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

private:
	
	//! No copying.
	VhdlGeneric(const VhdlGeneric& other);

	//! No assignment.
	VhdlGeneric& operator=(const VhdlGeneric& other);
};

#endif // VHDLGENERIC_H
