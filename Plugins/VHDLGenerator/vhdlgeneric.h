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
#include <QSharedPointer>

class ModuleParameter;
class ExpressionFormatter;

/*! VhdlGeneric represents a vhdl generic in a component or entity declaration.
 *
 */
class VhdlGeneric : public VhdlTypedObject
{

public:

	/*! The constructor
	 *
	 * @param [in] generic   The module parameter to create the generic from.
	 * @param [in] formatter The formatter to use for expressions in the value.
	 *
	*/
	VhdlGeneric(QSharedPointer<ModuleParameter> generic, QSharedPointer<ExpressionFormatter> formatter);

    //! No copying
    VhdlGeneric(const VhdlGeneric& other) = delete;

    //! No assignment
    VhdlGeneric& operator=(const VhdlGeneric& other) = delete;

	//! The destructor
	virtual ~VhdlGeneric() = default;

	/*! Write the contents of the generic into text stream.
	 *
	 * @param [in] stream The text stream to write the generic into.
	 *
	*/
	virtual void write(QTextStream& stream) const override final;

private:

    // Formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;
};

#endif // VHDLGENERIC_H
