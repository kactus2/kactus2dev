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
#include <IPXACTmodels/Component/validators/RegisterBaseValidator.h>

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
class RegisterBaseValidator;
class ResetType;
class Component;
class MemoryReserve;

//-----------------------------------------------------------------------------
//! Validator for ipxact:Register.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterValidator : public RegisterBaseValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] expressionParser    The parser to use for solving expressions.
     *    @param [in] fieldValidator      Validator used for fields.
     *    @param [in] parameterValidator  Validator used for parameters.
     *    @param [in] docRevision         The IP-XACT standard revision to comply to.
	 */
    RegisterValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<FieldValidator> fieldValidator,
        QSharedPointer<ParameterValidator> parameterValidator,
        Document::Revision docRevision = Document::Revision::Std14);

	//! The destructor.
    virtual ~RegisterValidator() = default;

    // Disable copying.
    RegisterValidator(RegisterValidator const& rhs) = delete;
    RegisterValidator& operator=(RegisterValidator const& rhs) = delete;

    /*!
     *  Change the containing component.
     *
     *    @param [in] newComponent    The selected component.
     */
    void componentChange(QSharedPointer<Component> newComponent);

    /*!
     *  Get the field validator.
     *
     *    @return The field validator.
     */
    QSharedPointer<FieldValidator> getFieldValidator() const;

    /*!
     *  Validates the given register.
     *
     *    @param [in] selectedRegister    The register to validate.
     *
     *    @return True, if the register is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the register contains a valid size.
     *
     *    @param [in] selectedRegister    The selected register.
     *
     *    @return True, if the size is valid, otherwise false.
     */
    bool hasValidSize(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the register contains valid fields.
     *
     *    @param [in] selectedRegister    The selected register definition.
     *    @param [in] registerSize        Size of the containing register.
     *
     *    @return True, if the fields are valid, otherwise false.
     */
    bool hasValidFields(QSharedPointer<RegisterDefinition> selectedRegister, QString const& registerSize) const;

    /*!
     *  Check if the register contains valid alternate registers.
     *
     *    @param [in] selectedRegister    The selected register.
     *
     *    @return True, if the alternate registers are valid, otherwise false.
     */
    bool hasValidAlternateRegisters(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Check if the alternate register contains valid alternate groups.
     *
     *    @param [in] selectedRegister    The selected alternate register.
     *
     *    @return True, if the alternate groups are valid, otherwise false.
     */
    bool hasValidAlternateGroups(QSharedPointer<AlternateRegister> selectedRegister) const;

    /*!
     *	Check if a 2022 std register has a valid structure, valid combination of subelements.
     *  
     *    @param [in] selectedRegister     The register to check.
     *	    
     * 	    @return True, if the structure is valid, otherwise false.
     */
    bool hasValidStructure(QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Locate errors within a register.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegister    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<Register> selectedRegister, QString const& context)
        const;

private:

    /*!
     *  Check if the field contains a valid access value.
     *
     *    @param [in] selectedRegister    The selected register definition.
     *    @param [in] field               The selected field
     *
     *    @return True, if the access value is valid, otherwise false.
     */
    bool fieldHasValidAccess(QSharedPointer<RegisterDefinition> selectedRegsiter, QSharedPointer<Field> field)
        const;

    /*!
     *  Check if the fields contain similar definition groups.
     *
     *    @param [in] field           The selected field.
     *    @param [in] comparedField   The field being compared to
     *
     *    @return True, if the fields are similar, otherwise false.
     */
    bool fieldsHaveSimilarDefinitionGroups(QSharedPointer<Field> field, QSharedPointer<Field> comparedField) const;

    /*!
     *  Find errors within size.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegister    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInSize(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within fields.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegister    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInFields(QVector<QString>& errors, QSharedPointer<RegisterDefinition> selectedRegister,
        QString const& registerSize, QString const& context) const;

    /*!
     *  Find errors within alternate registers.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegister    The selected register.
     */
    void findErrorsInAlternateRegisters(QVector<QString>& errors, QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Find errors within alternate groups.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegister    The selected alternate register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInAlternateGroups(QVector<QString>& errors, QSharedPointer<AlternateRegister> selectedRegister,
        QString const& context) const;

    /*!
     *  Find errors within the mode references of an alternate register.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegister    The selected register.
     *    @param [in] context             Context to help locate the error.
     *    @param [in] 
     */
    void findErrorsInAlternateRegisterModeRefs(QStringList& errors,
        QSharedPointer<AlternateRegister> selectedRegister, QString const& context, 
        QStringList& checkedModeRefs, QList<unsigned int>& checkedPriorities, 
        bool* duplicateRefErrorIssued, bool* duplicatePriorityErrorIssued) const;

    /*!
     *	Validate single alternate register.
     *  
     *    @param [in] alternateRegister     The alternate register to check.
     *    @param [in] selectedRegister      The parent register.
     *	    
     * 	    @return  True, if the alternate register is valid, otherwise false.
     */
    bool alternateRegisterIsValid(QSharedPointer<AlternateRegister> alternateRegister, QSharedPointer<Register> selectedRegister) const;

    /*!
     *	Get the bit width of the field taking into account the dimensions of it if std22 is in use.
     *  
     *    @param [in] field     The field whose width to get.
     *	    
     * 	    @return The true bit width of the field.
     */
    quint64 getTrueFieldBitWidth(QSharedPointer<Field> field) const;

    /*!
     *	Create memory areas for overlap and containment check.
     *
     *    @param [in] reg                 The register to create memory areas for.
     *    @param [in] reserve             The memory reserve to add created areas to.
     */
    void setupMemoryAreas(QSharedPointer<RegisterDefinition> reg, MemoryReserve& reserve) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The validator used for fields.
    QSharedPointer<FieldValidator> fieldValidator_;

};

#endif // REGISTERVALIDATOR_H
