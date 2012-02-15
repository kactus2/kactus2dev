/* 
 *  	Created on: 17.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlnamevalidator.h
 *		Project: Kactus 2
 */

#ifndef VHDLNAMEVALIDATOR_H
#define VHDLNAMEVALIDATOR_H

#include <QRegExpValidator>

/*! \brief Validator to check that input does not contain illegal vhdl characters.
 * 
 * This validator can be used for user inputs where it is needed that input does
 * not contain any characters that may cause problems for vhdl generator.
 */
class VhdlNameValidator : public QRegExpValidator {
	Q_OBJECT

public:
	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this validator.
	 *
	*/
	VhdlNameValidator(QObject *parent = 0);
	
	//! \brief The destructor
	virtual ~VhdlNameValidator();

private:
	//! \brief No copying
	VhdlNameValidator(const VhdlNameValidator& other);

	//! \brief No assignment
	VhdlNameValidator& operator=(const VhdlNameValidator& other);
	
};

#endif // VHDLNAMEVALIDATOR_H
