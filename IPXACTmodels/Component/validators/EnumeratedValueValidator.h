//-----------------------------------------------------------------------------
// File: EnumeratedValueValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.11.2015
//
// Description:
// Validator for ipxact:EnumeratedValue.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUEVALIDATOR_H
#define ENUMERATEDVALUEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class EnumeratedValue;

//-----------------------------------------------------------------------------
//! Validator for ipxact:EnumeratedValue.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT EnumeratedValueValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
	 */
    EnumeratedValueValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	~EnumeratedValueValidator();
    
    /*!
     *  Validates the given enumerated value.
     *
     *      @param [in] enumeratedValue     The enumerated value to validate.
     *
     *      @return True, if the enumerated value is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<EnumeratedValue> enumeratedValue) const;

    /*!
     *  Check if the enumerated value contains a valid name.
     *
     *      @param [in] enumeratedValue     The selected enumerated value.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<EnumeratedValue> enumeratedValue) const;

    /*!
     *  Check if the enumerated value contains a valid value.
     *
     *      @param [in] enumeratedValue     The selected enumerated value.
     *
     *      @return True, if the value is valid, otherwise false.
     */
    bool hasValidValue(QSharedPointer<EnumeratedValue> enumeratedValue) const;

    /*!
     *  Locate errors within an enumerated value.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] enumeratedValue     The selected enumerated value.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<EnumeratedValue> enumeratedValue,
        QString const& context) const;

private:

	// Disable copying.
	EnumeratedValueValidator(EnumeratedValueValidator const& rhs);
	EnumeratedValueValidator& operator=(EnumeratedValueValidator const& rhs);

    /*!
     *  Find errors within a name.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] enumeratedValue     The selected enumerated value.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<EnumeratedValue> enumeratedValue,
        QString const& context) const;

    /*!
     *  Find errors within a value.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] enumeratedValue     The selected enumerated value.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInValue(QVector<QString>& errors, QSharedPointer<EnumeratedValue> enumeratedValue,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // ENUMERATEDVALUEVALIDATOR_H
