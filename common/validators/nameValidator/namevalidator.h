/* 
 *
 *  Created on: 14.2.2011
 *      Author: Antti Kamppi
 * 		filename: namevalidator.h
 */

#ifndef NAMEVALIDATOR_H
#define NAMEVALIDATOR_H

#include <QRegExpValidator>

/*! \brief NameValidator is a validator class for IP-Xact's Name-type.
 * 
 * This validator can be used to validate user inputs where the input type 
 * is specified to be Name-type. For example this validator does not accept
 * embedded whitespaces in input.
 */
class NameValidator : public QRegExpValidator {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this validator
	 * \param startNumber If true then numbers can exist in the beginning of the name.
	 *
	*/
	NameValidator(QObject *parent, bool startNumber = false);

	//! \brief The destructor
	virtual ~NameValidator();

private:
	
	//! \brief No copying
	NameValidator(const NameValidator& other);

	//! No assignment
	NameValidator& operator=(const NameValidator& other);
};

#endif // NAMEVALIDATOR_H
