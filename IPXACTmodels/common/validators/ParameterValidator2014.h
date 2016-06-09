//-----------------------------------------------------------------------------
// File: ParameterValidator2014.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.12.2014
//
// Description:
// Validator for ipxact:parameter.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGVALIDATOR_H
#define SYSTEMVERILOGVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QRegularExpressionValidator>
#include <QString>
#include <QSharedPointer>

class Parameter;
class Choice;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Validator for ipxact:parameter.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ParameterValidator2014 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] availavleChocies    The currently available choices.
	 */
    ParameterValidator2014(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices);

	//! The destructor.
	virtual ~ParameterValidator2014();
    
    /*!
     *  Change the available choices.
     *
     *      @param [in] newChoices  The new choices.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices);

    /*!
     *  Validates the given parameter.
     *
     *      @param [in] parameter           The parameter to validate.
     *
     *      @return True, if the parameter is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Check if the parameter has a valid name.
     *
     *      @param [in] parameter   The parameter being checked.
     */
    bool hasValidName(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Validates the value of the given parameter.
     *
     *      @param [in] parameter           The parameter whose value to validate.
     *
     *      @return True, if the parameter value is valid, otherwise false.
     */
     virtual bool hasValidValue(QSharedPointer<const Parameter> parameter) const;
         
    /*!
     *  Validates the value type of the given parameter.
     *
     *      @param [in] parameter   The parameter whose type to validate.
     *
     *      @return True, if the parameter type is valid, otherwise false.
     */
    virtual bool hasValidType(QSharedPointer<const Parameter> parameter) const;

    /*!
     *   Checks if the given value is valid for the given type.
     *
     *      @param [in] value   The value to validate.
     *      @param [in] type    The type to which resolve the value.
     *
     *      @return True, if the value is valid for the given type, otherwise false.
     */
    virtual bool hasValidValueForType(QString const& value, QString const& type) const;

    /*!
     *   Checks if the value of the given parameter is valid for the given type.
     *
     *      @param [in] parameter   The parameter whose value to validate.
     *
     *      @return True, if the value is valid for the given type, otherwise false.
     */
    virtual bool hasValidValueForType(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Validates the minimum value of the given parameter.
     *
     *      @param [in] parameter   The parameter whose minimum value to validate.
     *
     *      @return True, if the minimum value is valid, otherwise false.
     */
    virtual bool hasValidMinimumValue(QSharedPointer<const Parameter> parameter) const;
            
    /*!
     *  Validates the maximum value of the given parameter.
     *
     *      @param [in] parameter   The parameter whose maximum value to validate.
     *
     *      @return True, if the maximum value is valid, otherwise false.
     */
    virtual bool hasValidMaximumValue(QSharedPointer<const Parameter> parameter) const;
         
    /*!
     *  Check if the parameter has a valid choice.
     *
     *      @param [in] parameter           The selected parameter.
     *
     *      @return True, if the choice is valid, otherwise false.
     */
    bool hasValidChoice(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Check if the parameter has a valid value for a choice.
     *
     *      @param [in] parameter           The selected parameter.
     *
     *      @return True if the value is valid for a choice, otherwise false.
     */
    bool hasValidValueForChoice(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Check if the parameter has a valid resolve value.
     *
     *      @param [in] parameter   The selected parameter.
     *
     *      @return True, if the parameter has a valid resolve, otherwise false.
     */
    bool hasValidResolve(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Check if the parameter has a valid ID.
     *
     *      @param [in] parameter   The selected parameter.
     *
     *      @return True, if the ID is valid, otherwise false.
     */
    bool hasValidValueId(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Checks if the value of the given parameter is less than the specified minimum value.
     *
     *      @param [in] parameter   The parameter whose value to compare to minimum.
     *
     *      @return True, if the value is less than minimum, otherwise false.
     */
    virtual bool valueIsLessThanMinimum(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Checks if the value of the given parameter is greater than the specified maximum value.
     *
     *      @param [in] parameter   The parameter whose value to compare to maximum.
     *
     *      @return True, if the value is greater than maximum, otherwise false.
     */   
    virtual bool valueIsGreaterThanMaximum(QSharedPointer<const Parameter> parameter) const;

    /*!
     *  Finds possible errors in a parameter and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Parameter> parameter, QString const& context)
        const;

    /*!
     *  Check if the given array is valid.
     *
     *      @param [in] arrayLeft   The left value of the array.
     *      @param [in] arrayRight  The right value of the array.
     *
     *      @return True, if the array has valid values, false otherwise.
     */
    bool validateArrayValues(QString const& arrayLeft, QString const& arrayRight) const;

    /*!
     *  Check if the given parameter vector is valid.
     *
     *      @param [in] parameter   The parameter being examined.
     *
     *      @return True, if the vector is valid, false otherwise.
     */
    virtual bool hasValidVector(QSharedPointer<const Parameter> parameter) const;

protected:
             
    /*!
     *  Checks if the parameter value and the given boundary value, minimum or maximum, should be compared.
     *
     *      @param [in] boundaryValue   The boundary value.
     *      @param [in] type            The type format of the parameter.
     *
     *      @return True, if the value and boundary should be compared, otherwise false.
     */  
    virtual bool shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& type) const;
         
    /*!
     *  Evaluates the value of the given value for the given format.
     *
     *      @param [in] value   The value to evaluate.
     *      @param [in] type    The type to use for evaluation. Obsolete.
     *
     *      @return The evaluated value.
     */    
    virtual qreal valueOf(QString const& value, QString const& type) const;
            
    /*!
     *  Finds possible errors in a parameter name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<Parameter> parameter, QString const& context)
        const;

    /*!
     *  Finds possible errors in a parameter value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsInValue(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
        QString const& context) const;
          
    /*!
     *  Finds the errors in minimum value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInMinimumValue(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
        QString const& context) const;

    /*!
     *  Finds the errors in maximum value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInMaximumValue(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
        QString const& context) const;
   
    /*!
     *  Check if the value is valid for format.
     *
     *      @param [in] value   The value being examined.
     *
     *      @return True, if the value is in a valid format, false otherwise.
     */
    bool hasValidValueForFormat(QString const& value) const;
   
    /*!
     *  Finds the errors in choice value.
     *
     *      @param [in] errors      List of found errors
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInChoice(QVector<QString>& errors, QSharedPointer<Parameter> parameter, QString const& context)
        const;
    
    /*!
     *  Finds the errors in resolve value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInResolve(QVector<QString>& errors, QSharedPointer<Parameter> parameter, QString const& context)
        const;

    /*!
     *  Finds possible errors in a parameter value type.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsInType(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
        QString const& context) const;

    /*!
     *  Check if the values in the array are valid for the given type.
     *
     *      @param [in] arrayExpression     The array.
     *      @param [in] type                The type of the parameter.
     *
     *      @return True, if the contents of the array are valid for type, otherwise false.
     */
    bool isArrayValidForType(QString const& arrayExpression, QString const& type) const;

    /*!
     *  Finds possible errors in a bit vector.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInVector(QVector<QString>& errors, QSharedPointer<Parameter> parameter, QString const& context)
        const;

private:

	// Disable copying.
	ParameterValidator2014(ParameterValidator2014 const& rhs);
	ParameterValidator2014& operator=(ParameterValidator2014 const& rhs);

    /*!
     *  Checks if the value of the given parameter is less than the specified minimum value.
     *
     *      @param [in] parameter       The parameter whose value to compare to minimum.
     *      @param [in] solvedValue     The resolved parameter value.
     *      @param [in] type            The parameter type.
     *
     *      @return True, if the value is less than minimum, otherwise false.
     */
    bool valueIsLessThanMinimum(QSharedPointer<const Parameter> parameter, QString const& solvedValue, 
        QString const& type) const;
    
    /*!
     *  Checks if the value of the given parameter is greater than the specified maximum value.
     *
     *      @param [in] parameter   The parameter whose value to compare to maximum.
     *      @param [in] solvedValue     The resolved parameter value.
     *      @param [in] type            The parameter type.
     *
     *      @return True, if the value is greater than maximum, otherwise false.
     */   
    bool valueIsGreaterThanMaximum(QSharedPointer<const Parameter> parameter, QString const& solvedValue,
        QString const& type) const;

    /*!
     *  Split the given array to a list.
     *
     *      @param [in] arrayValue  The given array.
     *
     *      @return The list of values in the array.
     */
    QStringList splitArrayToList(QString const& arrayValue) const;

    /*!
     *  Check if all the values in the array are of the same size.
     *
     *      @param [in] bitArray    The array to check.
     *      @param [in] type        The given type of the array.
     *
     *      @return True, if the array values are of the same size, otherwise false.
     */
    bool arrayValuesAreSameSize(QStringList const& bitArray, QString const& type) const;

    /*!
     *  Finds the choice.
     *
     *      @param [in] choiceName  The name of the choice to be found.
     */
    QSharedPointer<Choice> findChoiceByName(QString const& choiceName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use for solving minimum, maximum and value.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The available choices for a parameter.
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices_;

    QRegularExpressionValidator typeValidator_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
