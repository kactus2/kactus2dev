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
#include <IPXACTmodels/Component/validators/MemoryReserve.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class RegisterBase;
class RegisterFile;
class RegisterValidator;
class ParameterValidator;
class Component;

//-----------------------------------------------------------------------------
//! Validator for ipxact:RegisterFile.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterFileValidator : public RegisterBaseValidator
{
public:

    /*!
     * The constructor.
     *
     *    @param [in] expressionParser     Parser for expressions.
     *    @param [in] registerValidator    Validator for registers.
     *    @param [in] parameterValidator   Validator for parameters.
     */
    RegisterFileValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterValidator> registerValidator,
        QSharedPointer<ParameterValidator> parameterValidator,
        Document::Revision docRevision);

    //! The destructor.
    virtual ~RegisterFileValidator() = default;

    // Disable copying.
    RegisterFileValidator(RegisterFileValidator const& rhs) = delete;
    RegisterFileValidator& operator=(RegisterFileValidator const& rhs) = delete;


    QSharedPointer<RegisterValidator> getRegisterValidator() const;

    void componentChange(QSharedPointer<Component> newComponent);

    /*!
    *  Validates the given register file.
    *
    *    @param [in] selectedRegisterFile    The register file to validate.
    *
    *    @return True, if the register file is valid IP-XACT, otherwise false.
    */
    bool validate(QSharedPointer<RegisterFile> selectedRegisterFile, QString const& addressUnitBits, QString const& addressBlockWidth);

    /*!
    *  Check if the register file has a valid range.
    *
    *    @param [in] selectedRegisterFile    The selected register file whose range to check.
    *
    *    @return True, if the range is valid, otherwise false.
    */
    bool hasValidRange(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    /*!
    *  Check if the register file has a valid register data.
    *
    *    @param [in] selectedRegisterFile    The selected register file whose register data to check
    *    @param [in] addressUnitBits         The number of bits per address unit.
    *    @param [in] addressBlockWidth       The width of the enclosing address block.
    *
    *    @return True, if the register data is valid, otherwise false.
    */
    bool hasValidRegisterData(QSharedPointer<RegisterFile> selectedRegisterFile, QString const& addressUnitBits, 
        QString const& addressBlockWidth);

    /*!
     *	Check if the register file has a valid structure (choice of elements).
     *  
     *    @param [in] selectedRegisterFile     The selected register file to check.
     *	    
     * 	    @return True, if the structure is valid, otherwise false.
     */
    bool hasValidStructure(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    /*!
    *  Locate errors within a register file.
    *
    *    @param [in/out] errors               List of found errors.
    *    @param [in] selectedRegisterFile     The selected register file.
    *    @param [in] context                  Context to help locate the error.
    *    @param [in] addressUnitBits          The number of bits per address unit.
    *    @param [in] addressBlockWidth        The width of the enclosing address block.
    */
    void findErrorsIn(QVector<QString>& errors,
        QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context,
        QString const& addressUnitBits,
        QString const& addressBlockWidth);

private:

    /*!
    *  Find errors within the range value.
    *
    *    @param [in/out] errors               List of found errors.
    *    @param [in] selectedRegisterFile     The selected register file.
    *    @param [in] context                  Context to help locate the error.
    */
    void findErrorsInRange(QVector<QString>& errors,
        QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context) const;

    /*!
    *  Find errors within the register data.
    *
    *    @param [in/out] errors               List of found errors.
    *    @param [in] selectedRegisterFile     The selected register file.
    *    @param [in] context                  Context to help locate the error.
    */
    void findErrorsInRegisterData(QVector<QString>& errors,
        QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context,
        QString const& addressUnitBits,
        QString const& addressBlockWidth);

    /*!
     *	Find errors within the access policies of the register file.
     *  
     *    @param [in/out] errors               List of found errors.
     *    @param [in] selectedRegisterFile     The selected register file.
     *    @param [in] context                  Context to help locate the error.
     */
    void findErrorsInAccessPolicies(QStringList& errors, QSharedPointer<RegisterFile> selectedRegisterFile,
        QString const& context) const;

    /*!
     *	Get size of the given register in address units.
     *  
     *    @param [in] targetRegister      The register whose size to get.
     *    @param [in] addressUnitBits     The number of bits per address unit in use.
     *	    
     * 	    @return The register size in address units.
     */
    quint64 getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, quint64 addressUnitBits) const;

    /*!
     *	Find errors in a given child register of the register file being checked.
     * 
     *    @param [in/out] errors                      List of found errors.
     *    @param [in] childRegister                   The child register to check.
     *    @param [in] context                         Context to help locate the error.
     *    @param [in] addressUnitBits                 The number of bits in an address unit.
     *    @param [in] parentRegisterFileRange         The range of the parent register file.
     *    @param [in] addressBlockWidth               The width of the enclosing address block.
     *    @param [in/out] reservedArea                The currently reserved memory areas.
     *    @param [in/out] registerFileNames           List of checked register names.
     *    @param [in/out] duplicateRegisterFileNames  List to keep track of duplicate registers to prevent
     *                                                  duplicate errors.
     */
    void findErrorsInChildRegister(QStringList& errors, QSharedPointer<Register> childRegister,
        QString const& context, qint64 addressUnitBits, qint64 parentRegisterFileRange, 
        qint64 addressBlockWidth, MemoryReserve& reservedArea, QStringList& registerNames, 
        QStringList& duplicateRegisterNames) const;

    /*!
     *	Find errors in a given child register file of the register file being checked.
     *
     *    @param [in/out] errors                      List of found errors.
     *    @param [in] childRegisterFile               The child register to check.
     *    @param [in] context                         Context to help locate the error.
     *    @param [in] addressUnitBits                 The number of bits in an address unit.
     *    @param [in] parentRegisterFileRange         The range of the parent register file.
     *    @param [in] addressBlockWidth               The width of the enclosing address block.
     *    @param [in/out] reservedArea                The currently reserved memory areas.
     *    @param [in/out] registerFileNames           List of checked register file names.
     *    @param [in/out] duplicateRegisterFileNames  List to keep track of duplicate register files to prevent
     *                                                  duplicate errors.
     */
    void findErrorsInChildRegisterFile(QStringList& errors, QSharedPointer<RegisterFile> childRegisterFile, 
        QString const& context, QString const& addressUnitBits, qint64 parentRegisterFileRange, 
        QString const& addressBlockWidth, MemoryReserve& reservedArea, QStringList& registerFileNames,
        QStringList& duplicateRegisterFileNames);

    /*!
     *	Get register file range (size in AUB) taking dimensions into account.
     *
     *    @param [in] targetRegisterFile     The register file to get range of.
     *
     * 	    @return The true register file range.
     */
    qint64 getTrueRegisterFileRange(QSharedPointer<RegisterFile> targetRegisterFile, quint64 singleReplicaRange) const;

    /*!
     *	Create memory areas for overlap and containment check.
     *  
     *    @param [in] regFile             The register file to create memory areas for.
     *    @param [in] reserve             The memory reserve to add created areas to.
     *    @param [in] addressUnitBits     The memory map address unit bits.
     */
    void setupMemoryAreas(QSharedPointer<RegisterFile> regFile, MemoryReserve& reserve, quint64 addressUnitBits);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    //! The validator used for fields.
    QSharedPointer<RegisterValidator> registerValidator_;
};

#endif // REGISTERFILEVALIDATOR_H
