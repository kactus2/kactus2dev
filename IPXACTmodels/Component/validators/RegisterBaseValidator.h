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

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/validators/MemoryArrayValidator.h>
#include <IPXACTmodels/common/validators/HierarchicalValidator.h>

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
class ModeReference;
class Mode;

//-----------------------------------------------------------------------------
//! Validator for ipxact:Register.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterBaseValidator : public HierarchicalValidator
{
public:

    /*!
     *  The constructor.
     *
     *    @param [in] expressionParser    The parser to use for solving expressions.
     *    @param [in] parameterValidator  Validator used for parameters.
     */
    RegisterBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterValidator> parameterValidator,
        Document::Revision docRevision);

    //! The destructor.
    virtual ~RegisterBaseValidator() = default;

    // Disable copying.
    RegisterBaseValidator(RegisterBaseValidator const& rhs) = delete;
    RegisterBaseValidator& operator=(RegisterBaseValidator const& rhs) = delete;

    /*!
     *  Validates the given register.
     *
     *    @param [in] selectedRegisterBase    The register to validate.
     *
     *    @return True, if the register is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid name.
     *
     *    @param [in] selectedRegisterBase    The selected register base.
     *
     *    @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid isPresent value.
     *
     *    @param [in] selectedRegisterBase    The selected register base.
     *
     *    @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid dimension value.
     *
     *    @param [in] selectedRegisterBase    The selected register.
     *
     *    @return True, if the dimension is valid, otherwise false.
     */
    bool hasValidDimensions(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains a valid address offset.
     *
     *    @param [in] selectedRegisterBase    The selected register.
     *
     *    @return True, if the address offset is valid, otherwise false.
     */
    bool hasValidAddressOffset(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *  Check if the register contains valid parameters.
     *
     *    @param [in] selectedRegisterBase    The selected register base.
     *
     *    @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<RegisterBase> selectedRegisterBase) const;

    /*!
     *	Check if the register has valid access policies (or rather the mode references of the access policies).
     *  
     *    @param [in] registerBase     The register base to check
     *	    
     * 	    @return True, if the access policies are valid, otherwise false.
     */
    bool hasValidAccessPolicies(QSharedPointer<RegisterBase> registerBase) const;

    /*!
     *	Validate the memory array of a selected register.
     *
     *    @param [in] registerBase     The register base to check.
     *
     * 	    @return True, if valid, otherwise false.
     */
    bool hasValidMemoryArray(QSharedPointer<RegisterBase> registerBase) const;

protected:
    /*!
     *  Find errors within a name.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegisterBase    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;

    /*!
     *  Find errors within is present value.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegisterBase    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;

    /*!
     *  Find errors within dimension.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegisterBase    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInDimension(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;

    /*!
     *  Find errors within address offset.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegisterBase    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInAddressOffset(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegisterBase,
        QString const& context) const;
    /*!
     *  Find errors within Parameters.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] selectedRegisterBase    The selected register.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>&errors,
        QSharedPointer<RegisterBase> selectedRegisterBase, QString const& context) const;

    /*!
     *	Find errors in the access policies of the selected register.
     *  
     *    @param [in] errors                  List of found errors.
     *    @param [in] selectedRegisterBase    The selected register.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInAccessPolicies(QStringList& errors, QSharedPointer<RegisterBase> registerBase,
        QString const& context) const;

    /*!
     *  Find errors within memory array.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] registerBase        The selected register base.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInMemoryArray(QStringList& errors, QSharedPointer<RegisterBase> registerBase,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;

    //! The IP-XACT standard revision in use.
    Document::Revision docRevision_;

    //! The memory array validator.
    MemoryArrayValidator memArrayValidator_;

    //! Available component modes.
    QSharedPointer<QList<QSharedPointer<Mode> > > componentModes_;
};

#endif // RegisterBaseValidator_H
