//-----------------------------------------------------------------------------
// File: AssertionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.01.2016
//
// Description:
// Validator for the ipxact:assertion.
//-----------------------------------------------------------------------------

#ifndef ASSERTIONVALIDATOR_H
#define ASSERTIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;
class Assertion;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:assertion.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AssertionValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    The parser to use for solving expressions.
     */
    AssertionValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
    virtual ~AssertionValidator();
    
    /*!
     *  Validates the given assertion.
     *
     *      @param [in] assertion   The selected assertion.
     *
     *      @return True, if the assertion is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Assertion> assertion) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] assertion   The selected assertion.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<Assertion> assertion) const;

    /*!
     *  Check if the assert value is valid.
     *
     *      @param [in] assertion   The selected assertion.
     *
     *      @return True, if the assert value is valid, otherwise false.
     */
    bool hasValidAssert(QSharedPointer<Assertion> assertion) const;

    /*!
     *  Locate errors within an assertion.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] assertion   The selected assertion.
     *      @param [in] context     Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Assertion> assertion,
        QString const& context) const;

private:

	// Disable copying.
	AssertionValidator(AssertionValidator const& rhs);
	AssertionValidator& operator=(AssertionValidator const& rhs);

    /*!
     *  Find errors within name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] assertion   The selected assertion.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<Assertion> assertion, QString const& context)
        const;

    /*!
     *  Find errors within assert value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] assertion   The selected assertion.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInAssert(QVector<QString>& errors, QSharedPointer<Assertion> assertion, QString const& context)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // ASSERTIONVALIDATOR_H
