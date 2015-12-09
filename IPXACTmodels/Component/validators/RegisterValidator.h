//-----------------------------------------------------------------------------
// File: RegisterValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.11.2015
//
// Description:
// Validator for ipxact:Register.
//-----------------------------------------------------------------------------

#ifndef REGISTERVALIDATOR_H
#define REGISTERVALIDATOR_H

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
class ParameterValidator2014;
//-----------------------------------------------------------------------------
//! Validator for ipxact:Register.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] fieldValidator      Validator used for fields.
     *      @param [in] parameterValidator  Validator used for parameters.
	 */
    RegisterValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<FieldValidator> fieldValidator,
        QSharedPointer<ParameterValidator2014> parameterValidator);

	//! The destructor.
	~RegisterValidator();
    
    QSharedPointer<FieldValidator> getFieldValidator() const;

    /*!
     *  Validates the given register.
     *
     *      @param [in] selectedRegister    The register to validate.
     *
     *      @return True, if the register is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the register contains a valid name.
     *
     *      @param [in] selectedRegister    The selected register base.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<RegisterBase> selectedRegister) const;

    /*!
     *  Check if the register contains a valid isPresent value.
     *
     *      @param [in] selectedRegister    The selected register base.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<RegisterBase> selectedRegister) const;

    /*!
     *  Check if the register contains a valid dimension value.
     *
     *      @param [in] selectedRegister    The selected register.
     *
     *      @return True, if the dimension is valid, otherwise false.
     */
    bool hasValidDimension(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the register contains a valid address offset.
     *
     *      @param [in] selectedRegister    The selected register.
     *
     *      @return True, if the address offset is valid, otherwise false.
     */
    bool hasValidAddressOffset(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the register contains a valid size.
     *
     *      @param [in] selectedRegister    The selected register.
     *
     *      @return True, if the size is valid, otherwise false.
     */
    bool hasValidSize(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the register contains valid fields.
     *
     *      @param [in] selectedRegister    The selected register definition.
     *      @param [in] registerSize        Size of the containing register.
     *
     *      @return True, if the fields are valid, otherwise false.
     */
    bool hasValidFields(QSharedPointer<RegisterDefinition> selectedRegister, QString const& registerSize) const;

    /*!
     *  Check if the register contains valid alternate registers.
     *
     *      @param [in] selectedRegister    The selected register.
     *
     *      @return True, if the alternate registers are valid, otherwise false.
     */
    bool hasValidAlternateRegisters(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the alternate register contains valid alternate groups.
     *
     *      @param [in] selectedRegister    The selected alternate register.
     *
     *      @return True, if the alternate groups are valid, otherwise false.
     */
    bool hasValidAlternateGroups(QSharedPointer<AlternateRegister> selectedRegister) const;

    /*!
     *  Check if the register contains valid parameters.
     *
     *      @param [in] selectedRegister    The selected register base.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<RegisterBase> selectedRegister) const;

    /*!
     *  Locate errors within a register.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<Register> selectedRegister, QString const& context)
        const;

private:

	// Disable copying.
	RegisterValidator(RegisterValidator const& rhs);
	RegisterValidator& operator=(RegisterValidator const& rhs);

    /*!
     *  Check if the field contains a valid access value.
     *
     *      @param [in] selectedRegister    The selected register definition.
     *      @param [in] field               The selected field
     *
     *      @return True, if the access value is valid, otherwise false.
     */
    bool fieldHasValidAccess(QSharedPointer<RegisterDefinition> selectedRegsiter, QSharedPointer<Field> field)
        const;

    /*!
     *  Check if the fields contain similar definition groups.
     *
     *      @param [in] field           The selected field.
     *      @param [in] comparedField   The field being compared to
     *
     *      @return True, if the fields are similar, otherwise false.
     */
    bool fieldsHaveSimilarDefinitionGroups(QSharedPointer<Field> field, QSharedPointer<Field> comparedField) const;

    /*!
     *  Find errors within a name.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within is present value.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within dimension.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInDimension(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within address offset.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInAddressOffset(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within size.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInSize(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within fields.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInFields(QVector<QString>& errors, QSharedPointer<RegisterDefinition> selectedRegister,
        QString const& registerSize, QString const& context) const;

    /*!
     *  Find errors within alternate registers.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     */
    void findErrorsInAlternateRegisters(QVector<QString>& errors, QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Find errors within alternate groups.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected alternate register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInAlternateGroups(QVector<QString>& errors, QSharedPointer<AlternateRegister> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within register parameters.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] selectedRegister    The selected register.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>&errors, QSharedPointer<RegisterBase> selectedRegister,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for fields.
    QSharedPointer<FieldValidator> fieldValidator_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator2014> parameterValidator_;
};

#endif // REGISTERVALIDATOR_H
