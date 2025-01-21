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
class ParameterValidator;
class ChoiceValidator;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Validator for ipxact:BusDefinition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinitionValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] library             The library interface.
     *    @param [in] expressionParser    The expression parser to use.
	 */
    BusDefinitionValidator(LibraryInterface* library, QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~BusDefinitionValidator();
    
    /*!
     *  Validates the given BusDefinition.
     *
     *    @param [in] BusDefinition           The BusDefinition to validate.
     *
     *    @return True, if the BusDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<const BusDefinition> busDefinition) const;

    /*!
     *  Finds possible errors in a BusDefinition and creates a list of them.
     *
     *    @param [out] errors         List of found errors.
     *    @param [in] BusDefinition   The BusDefinition whose errors to find.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<const BusDefinition> busDefinition) const;

private:

	// Disable copying.
	BusDefinitionValidator(BusDefinitionValidator const& rhs);
	BusDefinitionValidator& operator=(BusDefinitionValidator const& rhs);

    /*!
     *  Check if the extend value of the selected bus definition is valid.
     *
     *    @param [in] busDefinition   The selected bus definition.
     *
     *    @return True, if the extend value is valid, false otherwise.
     */
    bool extendIsValid(QSharedPointer<const BusDefinition> busDefinition) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator for choices.
    QSharedPointer<ChoiceValidator> choiceValidator_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
