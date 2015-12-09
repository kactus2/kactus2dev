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

#include <QString>
#include <QSharedPointer>
#include <QVector>

class BusDefinition;
class ExpressionParser;
class ParameterValidator2014;

//-----------------------------------------------------------------------------
//! Validator for ipxact:BusDefinition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinitionValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] expressionParser    The expression parser to use.
	 */
    BusDefinitionValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	~BusDefinitionValidator();
    
    /*!
     *  Validates the given BusDefinition.
     *
     *      @param [in] BusDefinition           The BusDefinition to validate.
     *
     *      @return True, if the BusDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<const BusDefinition> busDefinition) const;

    /*!
     *  Finds possible errors in a BusDefinition and creates a list of them.
     *
     *      @param [out] errors         List of found errors.
     *      @param [in] BusDefinition   The BusDefinition whose errors to find.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<const BusDefinition> busDefinition) const;

private:

	// Disable copying.
	BusDefinitionValidator(BusDefinitionValidator const& rhs);
	BusDefinitionValidator& operator=(BusDefinitionValidator const& rhs);

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator for parameters.
    QSharedPointer<ParameterValidator2014> parameterValidator_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
