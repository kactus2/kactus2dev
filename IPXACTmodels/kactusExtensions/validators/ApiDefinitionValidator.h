//-----------------------------------------------------------------------------
// File: ApiDefinitionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Validator for kactus2:ApiDefinition.
//-----------------------------------------------------------------------------

#ifndef APIDEFINITIONVALIDATOR_H
#define APIDEFINITIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QSharedPointer>
#include <QVector>

class ApiDefinition;

//-----------------------------------------------------------------------------
//! Validator for ipxact:ApiDefinition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiDefinitionValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] expressionParser    The expression parser to use.
	 */
    ApiDefinitionValidator();

	//! The destructor.
	~ApiDefinitionValidator();
    
    /*!
     *  Validates the given ApiDefinition.
     *
     *      @param [in] ApiDefinition           The ApiDefinition to validate.
     *
     *      @return True, if the ApiDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<const ApiDefinition> apiDefinition) const;

    /*!
     *  Finds possible errors in a ApiDefinition and creates a list of them.
     *
     *      @param [out] errors         List of found errors.
     *      @param [in] ApiDefinition   The ApiDefinition whose errors to find.
     */
    virtual void findErrorsIn(QVector<QString>& errorList, QSharedPointer<const ApiDefinition> apiDefinition) const;

private:

	// Disable copying.
	ApiDefinitionValidator(ApiDefinitionValidator const& rhs);
	ApiDefinitionValidator& operator=(ApiDefinitionValidator const& rhs);
};

#endif
