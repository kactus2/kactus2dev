//-----------------------------------------------------------------------------
// File: ComDefinitionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Validator for kactus2:ComDefinition.
//-----------------------------------------------------------------------------

#ifndef COMDEFINITIONVALIDATOR_H
#define COMDEFINITIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QSharedPointer>
#include <QVector>

class ComDefinition;

//-----------------------------------------------------------------------------
//! Validator for ipxact:ComDefinition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComDefinitionValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] expressionParser    The expression parser to use.
	 */
    ComDefinitionValidator();

	//! The destructor.
	~ComDefinitionValidator();
    
    /*!
     *  Validates the given ComDefinition.
     *
     *      @param [in] ComDefinition           The ComDefinition to validate.
     *
     *      @return True, if the ComDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<const ComDefinition> comDefinition) const;

    /*!
     *  Finds possible errors in a ComDefinition and creates a list of them.
     *
     *      @param [out] errors         List of found errors.
     *      @param [in] ComDefinition   The ComDefinition whose errors to find.
     */
    virtual void findErrorsIn(QVector<QString>& errorList, QSharedPointer<const ComDefinition> comDefinition) const;

private:

	// Disable copying.
	ComDefinitionValidator(ComDefinitionValidator const& rhs);
	ComDefinitionValidator& operator=(ComDefinitionValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
