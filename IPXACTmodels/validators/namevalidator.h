//-----------------------------------------------------------------------------
// File: namevalidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.02.2011
//
// Description:
// Validator class for IP-Xact's Name-type.
// This validator can be used to validate user inputs where the input type 
// is specified to be Name-type. For example this validator does not accept
// embedded whitespaces in input.
//-----------------------------------------------------------------------------
#ifndef NAMEVALIDATOR_H
#define NAMEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QRegExpValidator>

//-----------------------------------------------------------------------------
//! NameValidator class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT NameValidator : public QRegExpValidator {
	Q_OBJECT

public:

    /*!
     *  Constructor for the name validator.
     *
     *      @param [in] parent   Owner of this validator.
     */
    NameValidator(QObject* parent);

	/*!
	 *  Destructor for the name validator.
	 */
	virtual ~NameValidator();

private:
	
	//! No copying
	NameValidator(const NameValidator& other);
	//! No assignment
	NameValidator& operator=(const NameValidator& other);
};

#endif // NAMEVALIDATOR_H
