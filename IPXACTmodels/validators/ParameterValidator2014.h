//-----------------------------------------------------------------------------
// File: SystemVerilogValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.12.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGVALIDATOR_H
#define SYSTEMVERILOGVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QString>

class IPXACTMODELS_EXPORT ParameterValidator2014 : public ParameterValidator 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser   The parser to use for solving expressions.
	 */
	ParameterValidator2014(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	~ParameterValidator2014();
    
    /*!
     *  Validates the given parameter.
     *
     *      @param [in] parameter           The parameter to validate.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return True, if the parameter is valid IP-XACT, otherwise false.
     */
    virtual bool validate(Parameter const* parameter, 
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

    /*!
     *  Validates the value of the given parameter.
     *
     *      @param [in] parameter           The parameter whose value to validate.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return True, if the parameter value is valid, otherwise false.
     */
     virtual bool hasValidValue(Parameter const* parameter, 
         QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;
         
    /*!
     *  Validates the value type of the given parameter.
     *
     *      @param [in] parameter   The parameter whose type to validate.
     *
     *      @return True, if the parameter type is valid, otherwise false.
     */
    virtual bool hasValidType(Parameter const* parameter) const;

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
    virtual bool hasValidValueForType(Parameter const* parameter) const;

    /*!
     *  Validates the minimum value of the given parameter.
     *
     *      @param [in] parameter   The parameter whose minimum value to validate.
     *
     *      @return True, if the minimum value is valid, otherwise false.
     */
    virtual bool hasValidMinimumValue(Parameter const* parameter) const;
            
    /*!
     *  Validates the maximum value of the given parameter.
     *
     *      @param [in] parameter   The parameter whose maximum value to validate.
     *
     *      @return True, if the maximum value is valid, otherwise false.
     */
    virtual bool hasValidMaximumValue(Parameter const* parameter) const;
         
    /*!
     *  Checks if the value of the given parameter is less than the specified minimum value.
     *
     *      @param [in] parameter   The parameter whose value to compare to minimum.
     *
     *      @return True, if the value is less than minimum, otherwise false.
     */
    virtual bool valueIsLessThanMinimum(Parameter const* parameter) const;

    /*!
     *  Checks if the value of the given parameter is greater than the specified maximum value.
     *
     *      @param [in] parameter   The parameter whose value to compare to maximum.
     *
     *      @return True, if the value is greater than maximum, otherwise false.
     */   
    virtual bool valueIsGreaterThanMaximum(Parameter const* parameter) const;

    /*!
     *  Finds possible errors in a parameter and creates a list of them.
     *
     *      @param [in] parameter           The parameter whose errors to find.
     *      @param [in] context             Context to help locate the errors.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return List of the found errors in the parameter.
     */
    virtual QStringList findErrorsIn(Parameter const* parameter, QString const& context, 
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

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
     *  Finds possible errors in a parameter value.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter value.
     */
    virtual QStringList findErrorsInValue(Parameter const* parameter, QString const& context, 
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;
          
    /*!
     *  Finds possible errors in a parameter value type.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter value format.
     */
    virtual QStringList findErrorsInType(Parameter const* parameter, QString const& context) const;

    /*!
     *  Check if the values in the array are valid for the given type.
     *
     *      @param [in] arrayExpression     The array.
     *      @param [in] type                The type of the parameter.
     *
     *      @return True, if the contents of the array are valid for type, otherwise false.
     */
    bool isArrayValidForType(QString const& arrayExpression, QString const& type) const;

private:

	// Disable copying.
	ParameterValidator2014(ParameterValidator2014 const& rhs);
	ParameterValidator2014& operator=(ParameterValidator2014 const& rhs);

    //! The expression parser to use for solving minimum, maximum and value.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
