//-----------------------------------------------------------------------------
// File: RegisterBaseValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.11.2015
//
// Description:
// Validator for ipxact:Register.
//-----------------------------------------------------------------------------

#ifndef RegisterBaseValidator_H
#define RegisterBaseValidator_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class RegisterBase;
class RegisterDefinition;
class Register;
class AlternateRegister;
class Choice;
class Field;
class FieldValidator;
class ParameterValidator;
//-----------------------------------------------------------------------------
//! Validator for ipxact:Register.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterBaseValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] parameterValidator  Validator used for parameters.
     */
    RegisterBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
                      QSharedPointer<ParameterValidator> parameterValidator);

    //! The destructor.
    ~RegisterBaseValidator();

    QSharedPointer<FieldValidator> getFieldValidator() const;

    /*!
     *  Validates the given register.
     *
     *      @param [in] selectedRegisterBase    The register to validate.
     *
     *      @return True, if the register is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid name.
     *
     *      @param [in] selectedRegisterBase    The selected register base.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid isPresent value.
     *
     *      @param [in] selectedRegisterBase    The selected register base.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid dimension value.
     *
     *      @param [in] selectedRegisterBase    The selected register.
     *
     *      @return True, if the dimension is valid, otherwise false.
     */
    bool hasValidDimension(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid address offset.
     *
     *      @param [in] selectedRegisterBase    The selected register.
     *
     *      @return True, if the address offset is valid, otherwise false.
     */
    bool hasValidAddressOffset(QSharedPointer<RegisterBase> selectedRegisterBase) const;


    /*!
     *  Check if the register contains valid parameters.
     *
     *      @param [in] selectedRegisterBase    The selected register base.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Locate errors within a register.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegisterBase    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase, QString const& context)
        const;

private:

    // Disable copying.
    RegisterBaseValidator(RegisterBaseValidator const& rhs);
    RegisterBaseValidator& operator=(RegisterBaseValidator const& rhs);

protected:
    /*!
     *  Find errors within a name.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegisterBase    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;

    /*!
     *  Find errors within is present value.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegisterBase    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;

    /*!
     *  Find errors within dimension.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegisterBase    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInDimension(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;

    /*!
     *  Find errors within address offset.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegisterBase    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInAddressOffset(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;
    /*!
     *  Find errors within Parameters.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegisterBase    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>&errors,
        QSharedPointer<RegisterBase> selectedRegisterBase, QString const& context) const;
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;
};

#endif // RegisterBaseValidator_H
