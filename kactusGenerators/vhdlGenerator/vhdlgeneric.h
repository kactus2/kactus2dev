/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERIC_H
#define VHDLGENERIC_H

#include "VhdlTypedObject.h"
#include "vhdlobject.h"

#include <QString>

class ModelParameter;

/*! \brief VhdlGeneric represents a vhdl generic in a component or entity declaration.
 *
 */
class VhdlGeneric : public VhdlTypedObject
{

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl generic.
	 * \param generic Pointer to the model parameter to create the generic from.
	 *
	*/
	VhdlGeneric(ModelParameter* generic);
	
	//! \brief The destructor
	virtual ~VhdlGeneric();

	/*! \brief Write the contents of the generic into text stream.
	 *
	 * \param stream The text stream to write the generic into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

private:
	
	//! No copying
	VhdlGeneric(const VhdlGeneric& other);

	//! No assignment
	VhdlGeneric& operator=(const VhdlGeneric& other);
};

#endif // VHDLGENERIC_H
