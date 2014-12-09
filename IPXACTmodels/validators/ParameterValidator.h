//-----------------------------------------------------------------------------
// File: ParameterValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Validator for the IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#ifndef PARAMETERVALIDATOR_H
#define PARAMETERVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include "IPXactValueParser.h"

#include <QList>
#include <QString>
#include <QSharedPointer>

class Choice;
class Parameter;

//-----------------------------------------------------------------------------
//! Validator for the IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ParameterValidator 
{
public:

	//! The constructor.
	ParameterValidator();

	//! The destructor.
	virtual ~ParameterValidator();

    /*!
     *  Validates the given parameter.
     *
     *      @param [in] parameter           The parameter to validate.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return True, if the parameter is valid IP-XACT, otherwise false.
     */
    virtual bool validate(Parameter const* parameter, QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

    /*!
     *  Validates the name of the given parameter.
     *
     *      @param [in] parameter   The parameter whose name to validate.
     *
     *      @return True, if the parameter name is valid, otherwise false.
     */
    virtual bool hasValidName(Parameter const* parameter) const;

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
     *  Validates the value format of the given parameter.
     *
     *      @param [in] parameter   The parameter whose format to validate.
     *
     *      @return True, if the parameter format is valid, otherwise false.
     */
    bool hasValidFormat(Parameter const* parameter) const;

    /*!
     *  Validates the value against the given format.
     *
     *      @param [in] value       The value to validate.
     *      @param [in] format      The format for the value.
     *
     *      @return True, if the value is valid for the given format, otherwise false.
     */
    virtual bool hasValidValueForFormat(QString const& value, QString const& format) const;
        
    /*!
     *  Validates the value of the parameter against its format.
     *
     *      @param [in] parameter   The parameter whose value to validate.
     *
     *      @return True, if the parameter value is valid for its format, otherwise false.
     */
    virtual bool hasValidValueForFormat(Parameter const* parameter) const;
        
    /*!
     *  Validates the bit string length of the given parameter.
     *
     *      @param [in] parameter   The parameter whose bit string length to validate.
     *
     *      @return True, if the parameter bit string length is valid, otherwise false.
     */
    bool hasValidBitStringLength(Parameter const* parameter) const;
        
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
    bool valueIsLessThanMinimum(Parameter const* parameter) const;
 
    /*!
     *  Checks if the value of the given parameter is greater than the specified maximum value.
     *
     *      @param [in] parameter   The parameter whose value to compare to maximum.
     *
     *      @return True, if the value is greater than maximum, otherwise false.
     */   
    bool valueIsGreaterThanMaximum(Parameter const* parameter) const;
     
    /*!
     *  Validates the choice reference of the given parameter.
     *
     *      @param [in] parameter           The parameter whose choice to validate.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return True, if the choice reference is valid, otherwise false.
     */  
    bool hasValidChoice(Parameter const* parameter, 
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;
 
    /*!
     *  Checks if the value of the given parameter is valid for the selected choice.
     *
     *      @param [in] parameter           The parameter whose value to check.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return True, if the value is valid, otherwise false.
     */ 
    bool hasValidValueForChoice(Parameter const* parameter, 
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

    /*!
     *  Validates the resolve type of the given parameter.
     *
     *      @param [in] parameter   The parameter whose resolve type to validate.
     *
     *      @return True, if the resolve type is valid, otherwise false.
     */      
    bool hasValidResolve(Parameter const* parameter) const;

    /*!
     *  Validates the id of the given parameter.
     *
     *      @param [in] parameter   The parameter whose id to validate.
     *
     *      @return True, if the id is valid, otherwise false.
     */   
    bool hasValidValueId(Parameter const* parameter) const;

    /*!
     *  Finds possible errors in a parameter and creates a list of the.
     *
     *      @param [in] parameter           The parameter whose errors to find.
     *      @param [in] context             Context to help locate the errors.
     *      @param [in] availableChoices    The choices available for the parameter.
     *
     *      @return List of the found errors in the parameter.
     */
    virtual QStringList findErrorsIn(Parameter const* parameter, QString const& context,
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

private:

	// Disable copying.
	ParameterValidator(ParameterValidator const& rhs);
	ParameterValidator& operator=(ParameterValidator const& rhs);
     
    /*!
     *  Checks if the parameter value and the given boundary value, minimum or maximum, should be compared.
     *
     *      @param [in] boundaryValue   The boundary value.
     *      @param [in] format          The value format of the parameter.
     *
     *      @return True, if the value and boundary should be compared, otherwise false.
     */  
    bool shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& format) const;

    /*!
     *  Evaluates the value of the given value for the given format.
     *
     *      @param [in] value   The value to evaluate.
     *      @param [in] format  The format to use for evaluation.
     *
     *      @return The evaluated value.
     */
    qreal valueOf(QString const& value, QString const& format) const;
        
    /*!
     *  Finds possible errors in a parameter name.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter name.
     */
    virtual QStringList findErrorsInName(Parameter const* parameter, QString const& context) const;
        
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
     *  Finds possible errors in a parameter value format.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter value format.
     */
    QStringList findErrorsInFormat(Parameter const* parameter, QString const& context) const;
            
    /*!
     *  Finds possible errors in a parameter bit string lenght.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter bit string length.
     */
    QStringList findErrorsInBitStringLength(Parameter const* parameter, QString const& context) const;
            
    /*!
     *  Finds possible errors in a parameter minimum value.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter minimum value.
     */
    virtual QStringList findErrorsInMinimumValue(Parameter const* parameter, QString const& context) const;
                
    /*!
     *  Finds possible errors in a parameter maximum value.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter maximum value.
     */
    virtual QStringList findErrorsInMaximumValue(Parameter const* parameter, QString const& context) const;
                     
    /*!
     *  Finds possible errors in a parameter choice reference.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the choice reference.
     */    
    virtual QStringList findErrorsInChoice(Parameter const* parameter, QString const& context,
        QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const;

    /*!
     *  Finds possible errors in a parameter resolve type.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter resolve type.
     */
    virtual QStringList findErrorsInResolve(Parameter const* parameter, QString const& context) const;
   
    /*!
     *  Finds possible errors in a parameter id.
     *
     *      @param [in] parameter   The parameter whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     *
     *      @return List of the found errors in the parameter id.
     */
    virtual QStringList findErrorsInId(Parameter const* parameter, QString const& context) const;

    /*!
     *  Finds a choice with the given name from a set of choices.
     *
     *      @param [in] choiceName   The name to search for.
     *      @param [in] choices     The set of choices to search in.
     *
     *      @return The found choice.
     */
    QSharedPointer<Choice> findChoiceByName(QString const& choiceName, 
        QSharedPointer<QList<QSharedPointer<Choice> > > choices) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Parser for resolving the parameter value, minumum and maximum.
    IPXactValueParser valueParser_;
};

#endif // PARAMETERVALIDATOR_H
