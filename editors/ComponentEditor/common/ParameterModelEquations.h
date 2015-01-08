//-----------------------------------------------------------------------------
// File: ParameterModelEquations.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.12.2014
//
// Description:
// Base class for equation usage in parameter editors.
//-----------------------------------------------------------------------------

#ifndef PARAMETERMODELEQUATIONS_H
#define PARAMETERMODELEQUATIONS_H

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/component.h>

class ExpressionParser;
//-----------------------------------------------------------------------------
//! Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------
class ParameterModelEquations
{
public:

    /*!
     *  The constructor.
     */
    ParameterModelEquations();
	
    /*!
     *  The destructor.
     */
    virtual ~ParameterModelEquations();

    /*!
     *  Sets the expression parser.
     *
     *      @param [in] expressionParser    The expression parser.
     */
    void setExpressionParser(QSharedPointer <ExpressionParser> expressionParser);

protected:

    /*!
     *  Gets the symbol for functions.
     */
    QString getExpressionSymbol() const;

    /*!
     *  Gets a formatted value for a given expression.
     *
     *      @param [in] expression      The expression whose value to format.
     *
     *      @return     The formatted value for the expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Gets an italic font for an evaluated value and normal font for others.
     *
     *      @param [in] value   The value.
     *
     *      @return     Italic font for evaluated value, normal for others.
     */
    QVariant italicForEvaluatedValue(QModelIndex const& index) const;

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return     True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const = 0;

    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *      @param [in] index   The index whose expression to get.
     *
     *      @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const = 0; 

    /*!
     *  Validates the data in a parameter corresponding to a given column.
     *
     *      @param [in] index   The index whose data to validate
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const = 0;

    /*!
     *  Gets a black color for valid index and red color for invalid index.
     *
     *      @param [in] index   The index for which to get the color.
     *
     *      @return Black for valid index, red for invalid index.
     */
    QVariant blackForValidOrRedForInvalidIndex(QModelIndex const& index) const;

    /*!
     *  Checks if the value is a plain value or an expression value.
     *
     *      @param [in] value   The value.
     *
     *      @return     True if value is a plain value or an expression. False if it is neither.
     */
    bool isValuePlainOrExpression(QString const& value) const;

    /*!
     *  Parse expression to decimal number.
     *
     *      @param [in] expression  The expression to be parsed.
     *      @param [in] index       The model index of the expression.
     *
     *      @return     The decimal version of the number.
     */
    QString parseExpressionToDecimal(QString const& expression, QModelIndex const& index) const;

private:

    //! Disable copying and assignment.
    ParameterModelEquations(const ParameterModelEquations& other);
    ParameterModelEquations& operator=(const ParameterModelEquations& other);

    //! Expression parser for configurable elements.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // PARAMETERMODELEQUATIONS_H
