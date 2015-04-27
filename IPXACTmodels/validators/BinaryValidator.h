//-----------------------------------------------------------------------------
// File: BinaryValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.03.2015
//
// Description:
// Validator class for binary type value.
// This validator can be used to validate user inputs where the input type is specified to be bit-type.
//-----------------------------------------------------------------------------
#ifndef BINARYVALIDATOR_H
#define BINARYVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QRegularExpressionValidator>

//-----------------------------------------------------------------------------
//! NameValidator class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BinaryValidator : public QRegularExpressionValidator
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] sizeRestraint   The size restraint for the validator.
     *      @param [in] parent          Owner of this validator.
     */
    BinaryValidator(QString sizeRestraint, QObject* parent);

    /*!
     *  Destructor for the name validator.
     */
    virtual ~BinaryValidator();

    /*!
     *  Set a new regular expression for the validator.
     *
     *      @param [in] sizeRestraint   The new size restraint for the validator.
     */
    void setNewExpressionvalidator(QString const& sizeRestraint);

private:
	
	//! No copying
    BinaryValidator(const BinaryValidator& other);

	//! No assignment
    BinaryValidator& operator=(const BinaryValidator& other);
};

#endif // BINARYVALIDATOR_H
