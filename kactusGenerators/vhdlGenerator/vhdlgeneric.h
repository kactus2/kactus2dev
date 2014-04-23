/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgeneric.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERIC_H
#define VHDLGENERIC_H

#include "vhdlobject.h"

#include <QString>

class VhdlComponentDeclaration;
class VhdlGenerator2;
class ModelParameter;

/*! \brief VhdlGeneric represents a vhdl generic in a component or entity declaration.
 *
 */
class VhdlGeneric : public VhdlObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl generic.
	 * \param name Name for this vhdl object.
	 * \param type Type for this vhdl object.
	 * \param defaultValue The default value for the generic.
	 * \param description The description for this vhdl object.
	 *
	*/
	VhdlGeneric(VhdlGenerator2* parent, 
		const QString& name = QString(),
		const QString& type = QString(),
		const QString& defaultValue = QString(),
		const QString& description = QString());

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl generic.
	 * \param generic Pointer to the model parameter to create the generic from.
	 *
	*/
	VhdlGeneric(VhdlGenerator2* parent, ModelParameter* generic);

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl generic.
	 * \param name Name for this vhdl object.
	 * \param type Type for this vhdl object.
	 * \param defaultValue The default value for the generic.
	 * \param description The description for this vhdl object.
	 *
	*/
	VhdlGeneric(VhdlComponentDeclaration* parent,
		const QString& name = QString(),
		const QString& type = QString(),
		const QString& defaultValue = QString(),
		const QString& description = QString());

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this vhdl generic.
	 * \param generic Pointer to the model parameter to create the generic from.
	 *
	*/
	VhdlGeneric(VhdlComponentDeclaration* parent, ModelParameter* generic);
	
	//! \brief The destructor
	virtual ~VhdlGeneric();

	/*! \brief Write the contents of the generic into text stream.
	 *
	 * \param stream The text stream to write the generic into.
	 *
	*/
	virtual void write(QTextStream& stream) const;

private:
	
	//! \brief No copying
	VhdlGeneric(const VhdlGeneric& other);

	//! \brief No assignment
	VhdlGeneric& operator=(const VhdlGeneric& other);
};

#endif // VHDLGENERIC_H
