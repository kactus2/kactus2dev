//-----------------------------------------------------------------------------
// File: NMTokenValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.12.2014
//
// Description:
// Validator for IP-Xact's NMtoken-type
//-----------------------------------------------------------------------------

#ifndef NMTOKENVALIDATOR_H
#define NMTOKENVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QRegExpValidator>

//-----------------------------------------------------------------------------
//! NMTokenValidator class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT NMTokenValidator : public QRegExpValidator {
	Q_OBJECT

public:

    /*!
     *  Constructor for the validator.
     *
     *      @param [in] parent   Parent of the validator.
     */
    NMTokenValidator(QObject* parent);

    /*!
     *  Destructor of the validator.
     */
    virtual ~NMTokenValidator();

private:
	
	//! No copying
    NMTokenValidator(const NMTokenValidator& other);
	//! No assignment
    NMTokenValidator& operator=(const NMTokenValidator& other);
};

#endif // NMTOKENVALIDATOR_H
