//-----------------------------------------------------------------------------
// File: BusDefinitionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 17.11.2015
//
// Description:
// Validator for ipxact:BusDefinition.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONVALIDATOR_H
#define BUSDEFINITIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:BusDefinition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinitionValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    BusDefinitionValidator();

	//! The destructor.
	~BusDefinitionValidator();
    
    /*!
     *  Validates the given BusDefinition.
     *
     *      @param [in] BusDefinition           The BusDefinition to validate.
     *
     *      @return True, if the BusDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(BusDefinition const* busDefinition) const;

    /*!
     *  Finds possible errors in a BusDefinition and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] BusDefinition   The BusDefinition whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<BusDefinition> busDefinition,
		QString const& context) const;

private:

	// Disable copying.
	BusDefinitionValidator(BusDefinitionValidator const& rhs);
	BusDefinitionValidator& operator=(BusDefinitionValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
