//-----------------------------------------------------------------------------
// File: RegisterFileValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Dan Chianucci
// Date: 04.06.2018
//
// Description:
// Validator for ipxact:RegisterFile.
//-----------------------------------------------------------------------------

#ifndef REGISTERFILEVALIDATOR_H
#define REGISTERFILEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/Component/validators/RegisterBaseValidator.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class RegisterBase;
class RegisterFile;
class RegisterValidator;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Validator for ipxact:RegisterFile.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterFileValidator : public RegisterBaseValidator
{
public:

    /*!
     * The constructor.
     *
     *     @param [in] expressionParser     Parser for expressions.
     *     @param [in] registerValidator    Validator for registers.
     *     @param [in] parameterValidator   Validator for parameters.
     */
    RegisterFileValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterValidator> registerValidator,
        QSharedPointer<ParameterValidator> parameterValidator,
        Document::Revision docRevision = Document::Revision::Std14);

    //! The destructor.
    virtual ~RegisterFileValidator() = default;

    // Disable copying.
    RegisterFileValidator(RegisterFileValidator const& rhs) = delete;
    RegisterFileValidator& operator=(RegisterFileValidator const& rhs) = delete;


    QSharedPointer<RegisterValidator> getRegisterValidator() const;

    /*!
    *  Validates the given register file.
    *
    *      @param [in] selectedRegisterFile    The register file to validate.
    *
    *      @return True, if the register file is valid IP-XACT, otherwise false.
    */
    bool validate(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    /*!
    *  Check if the register file has a valid range.
    *
    *      @param [in] selectedRegisterFile    The selected register file whose range to check.
    *
    *      @return True, if the range is valid, otherwise false.
    */
    bool hasValidRange(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    /*!
    *  Check if the register file has a valid register data.
    *
    *      @param [in] selectedRegisterFile    The selected register file whose register data to check.
    *
    *      @return True, if the register data is valid, otherwise false.
    */
    bool hasValidRegisterData(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    /*!
     *	Check if the register file has a valid memory array.
     *  
     *      @param [in] selectedRegisterfile     The selected register file to check.
     *	    
     * 	    @return True, if the memory array is valid, otherwise false.
     */
    bool hasValidMemoryArray(QSharedPointer<RegisterFile> selectedRegisterfile) const;

    /*!
     *	Check if the register file has a valid structure (choice of elements).
     *  
     *      @param [in] selectedRegisterFile     The selected register file to check.
     *	    
     * 	    @return True, if the structure is valid, otherwise false.
     */
    bool hasValidStructure(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    /*!
    *  Locate errors within a register file.
    *
    *      @param [in/out] errors               List of found errors.
    *      @param [in] selectedRegisterFile     The selected register file.
    *      @param [in] context                  Context to help locate the error.
    */
    void findErrorsIn(QVector<QString>& errors,
        QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context) const;

private:

    /*!
    *  Find errors within the range value.
    *
    *      @param [in/out] errors               List of found errors.
    *      @param [in] selectedRegisterFile     The selected register file.
    *      @param [in] context                  Context to help locate the error.
    */
    void findErrorsInRange(QVector<QString>& errors,
        QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context) const;

    /*!
    *  Find errors within the register data.
    *
    *      @param [in/out] errors               List of found errors.
    *      @param [in] selectedRegisterFile     The selected register file.
    *      @param [in] context                  Context to help locate the error.
    */
    void findErrorsInRegisterData(QVector<QString>& errors,
        QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context) const;

    /*!
     *	Find errors within the memory array of the register file.
     *  
     *      @param [in/out] errors               List of found errors.
     *      @param [in] selectedRegisterFile     The selected register file.
     *      @param [in] context                  Context to help locate the error.
     */
    void findErrorsInMemoryArray(QStringList& errors, QSharedPointer<RegisterFile> selectedRegisterFile, 
        QString const& context) const;

    /*!
     *	Find errors within the access policies of the register file.
     *  
     *      @param [in/out] errors               List of found errors.
     *      @param [in] selectedRegisterFile     The selected register file.
     *      @param [in] context                  Context to help locate the error.
     */
    void findErrorsInAccessPolicies(QStringList& errors, QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    //! The validator used for fields.
    QSharedPointer<RegisterValidator> registerValidator_;
};

#endif // REGISTERFILEVALIDATOR_H
