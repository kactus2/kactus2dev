//-----------------------------------------------------------------------------
// File: ChoiceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.12.2015
//
// Description:
// Validator for the ipxact:choice.
//-----------------------------------------------------------------------------

#ifndef CHOICEVALIDATOR_H
#define CHOICEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;
class Choice;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:choice.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChoiceValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    The parser to use for solving expressions.
     */
    ChoiceValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~ChoiceValidator();
    
    /*!
     *  Validates the given choice.
     *
     *      @param [in] choice  The selected choice.
     *
     *      @return True, if the choice is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Choice> choice) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] choice  The selected choice.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<Choice> choice) const;

    /*!
     *  Check if the enumerations are valid.
     *
     *      @param [in] choice  The selected choice.
     *
     *      @return True, if the enumerations are valid, otherwise false.
     */
    bool hasValidEnumerations(QSharedPointer<Choice> choice) const;

    /*!
     *  Locate errors within a choice.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] choice      The selected choice.
     *      @param [in] context     Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Choice> choice, QString const& context)
        const;

private:

	// Disable copying.
	ChoiceValidator(ChoiceValidator const& rhs);
	ChoiceValidator& operator=(ChoiceValidator const& rhs);

    /*!
     *  Find errors within name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] choice      The selected choice.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<Choice> choice, QString const& context) const;

    /*!
     *  Find errors within enumerations.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] choice      The selected choice.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInEnumerations(QVector<QString>& errors, QSharedPointer<Choice> choice, QString const& context)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // ADDRESSSPACEVALIDATOR_H
