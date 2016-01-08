//-----------------------------------------------------------------------------
// File: ComponentGeneratorValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.01.2016
//
// Description:
// Validator for the component generator.
//-----------------------------------------------------------------------------

#ifndef COMPONENTGENERATORVALIDATOR_H
#define COMPONENTGENERATORVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class ComponentGenerator;

class ParameterValidator2014;
//-----------------------------------------------------------------------------
//! Validator for the base ipxact:componentGenerator.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentGeneratorValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    The parser used for solving expressions.
     *      @param [in] parameterValidator  Validator used for parameters.
     */
    ComponentGeneratorValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterValidator2014> parameterValidator);

	//! The destructor.
	~ComponentGeneratorValidator();
    
    /*!
     *  Validates the given component generator.
     *
     *      @param [in] generator   The component generator to validate.
     *
     *      @return True, if the component generator is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<ComponentGenerator> generator) const;

    /*!
     *  Check if the component generator contains a valid name.
     *
     *      @param [in] generator   The selected component generator.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<ComponentGenerator> generator) const;

    /*!
     *  Check if the component generator contains a phase.
     *
     *      @param [in] generator   The selected component generator.
     *
     *      @return True, if the phase is valid, otherwise false.
     */
    bool hasValidPhase(QSharedPointer<ComponentGenerator> generator) const;

    /*!
     *  Check if the component generator contains a valid generator exe.
     *
     *      @param [in] generator   The selected component generator.
     *
     *      @return True, if the generator exe is valid, otherwise false.
     */
    bool hasValidGeneratorExe(QSharedPointer<ComponentGenerator> generator) const;

    /*!
     *  Locate errors within a component generator.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] generator   The selected component generator.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<ComponentGenerator> generator,
        QString const& context) const;

private:

	// Disable copying.
	ComponentGeneratorValidator(ComponentGeneratorValidator const& rhs);
	ComponentGeneratorValidator& operator=(ComponentGeneratorValidator const& rhs);

    /*!
     *  Find errors in name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] generator   The selected component generator.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<ComponentGenerator> generator,
        QString const& context) const;

    /*!
     *  Find errors in phase.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] generator   The selected component generator.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInPhase(QVector<QString>& errors, QSharedPointer<ComponentGenerator> generator,
        QString const& context) const;

    /*!
     *  Find errors in generator exe.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] generator   The selected component generator.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInGeneratorExe(QVector<QString>& errors, QSharedPointer<ComponentGenerator> generator,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used parameter validator.
    QSharedPointer<ParameterValidator2014> parameterValidator_;
};

#endif // COMPONENTGENERATORVALIDATOR_H
