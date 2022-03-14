//-----------------------------------------------------------------------------
// File: AddressBlockValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.11.2015
//
// Description:
// Validator for ipxact:addressBlock.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKVALIDATOR_H
#define ADDRESSBLOCKVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/validators/MemoryBlockValidator.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class AddressBlock;
class Register;
class ResetType;
class Component;
class RegisterValidator;
class RegisterFileValidator;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Validator for ipxact:addressBlock.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressBlockValidator : public MemoryBlockValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] registerValidator   Validator used for registers.
     *      @param [in] parameterValidator  Validator used for parameters.
	 */
    AddressBlockValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterValidator> registerValidator,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
        QSharedPointer<ParameterValidator> parameterValidator);

	//! The destructor.
	~AddressBlockValidator() = default;
    
    /*!
     *  Change the containing component.
     *
     *      @param [in] newComponent    The selected component.
     */
    void componentChange(QSharedPointer<Component> newComponent);

    /*!
     *  Get the used register validator.
     *
     *      @return The used register validator.
     */
    QSharedPointer<RegisterValidator> getRegisterValidator() const;

    /*!
     *  Get the used register file validator.
     *
     *      @return The used register validator.
     */
    QSharedPointer<RegisterFileValidator> getRegisterFileValidator() const;

    /*!
     *  Validates the given address block.
     *
     *      @param [in] addressBlock        The address block to validate.
     *      @param [in] addressUnitBits     The address unit bits of a memory map.
     *
     *      @return True, if the address block is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<AddressBlock> addressBlock, QString const& addressUnitBits) const;

    /*!
     *  Check if the address block contains a valid range.
     *
     *      @param [in] addressBlock    The selected address block.
     *
     *      @return True, if the range is valid, otherwise false.
     */
    bool hasValidRange(QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Check if the address block contains a valid width.
     *
     *      @param [in] addressBlock    The selected address block.
     *
     *      @return True, if the width is valid, otherwise false.
     */
    bool hasValidWidth(QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Check if the address block contains valid register data.
     *
     *      @param [in] addressBlock        The selected address block.
     *      @param [in] addressUnitBits     The address unit bits of a memory map.
     *
     *      @return True, if the register data is valid, otherwise false.
     */
    bool hasValidRegisterData(QSharedPointer<AddressBlock> addressBlock, QString const& addressUnitBits) const;

    /*!
     *  Check if the register size is not within address block width.
     *
     *      @param [in] targetRegister  The selected register.
     *      @param [in] addressBlock    The selected address block.
     *
     *      @return True, if the register size within address block width, otherwise false.
     */
    bool registerSizeIsNotWithinBlockWidth(QSharedPointer<Register> targetRegister,
        QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Check if the address block contains a valid volatile value compared to registers.
     *
     *      @param [in] addressBlock    The selected address block.
     *      @param [in] targetRegister  The selected register.
     *
     *      @return True, if the volatile is valid, otherwise false.
     */
    bool hasValidVolatileForRegister(QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<Register> targetRegister) const;

    /*!
     *  Check if the address block contains a valid usage.
     *
     *      @param [in] addressBlock    The selected address block.
     *
     *      @return True, if the usage is valid, otherwise false.
     */
    bool hasValidUsage(QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Locate errors within an address block.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] addressBlock        The selected address block.
     *      @param [in] addressUnitBits     The address unit bits of a memory map.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
        QString const& addressUnitBits, QString const& context) const;

private:

	// Disable copying.
	AddressBlockValidator(AddressBlockValidator const& rhs);
	AddressBlockValidator& operator=(AddressBlockValidator const& rhs);

    /*!
     *  Get the type of the memory block.
     *
     *      @return Identification type of the memory block.
     */
    virtual QString getBlockType() const override final;

    /*!
     *  Check if the contained registers have similar register definitions.
     *
     *      @param [in] targetRegister              The selected register.
     *      @param [in] addressBlock                The selected address block.
     *      @param [in] indexOfSimilarDefinition    The index of the register with similar register definition.
     *
     *      @return True, if the register definitions are similar, otherwise false.
     */
    bool registersHaveSimilarDefinitionGroups(QSharedPointer<Register> targetRegister,
        QSharedPointer<AddressBlock> addressBlock, int indexOfSimilarDefinition) const;

    /*!
     *  Check if a register contain a valid access.
     *
     *      @param [in] addressBlock    The selected address block.
     *      @param [in] targetRegister  The selected register.
     *
     *      @return True, if the register has a valid access, otherwise false.
     */
    bool hasValidAccessWithRegister(QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<Register> targetRegister) const;

    /*!
     *  Gets the register size in LAU.
     *
     *      @param [in] targetRegister      The selected register.
     *      @param [in] addressUnitBits     The address unit bits of a memory map.
     *
     *      @return The register size in LAU.
     */
    qint64 getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, int addressUnitBits) const;

    /*!
     *  Find errors within the address block range.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressBlock    The selected addressBlock.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInRange(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
        QString const& context) const;

    /*!
     *  Find errors within address block width.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressBlock    The selected addressBlock.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInWidth(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
        QString const& context) const;

    /*!
     *  Find errors within address block usage.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressBlock    The selected addressBlock.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInUsage(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
        QString const& context) const;

    /*!
     *  Find errors within register data.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] addressBlock        The selected addressBlock.
     *      @param [in] addressUnitBits     The address unit bits of a memory map.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInRegisterData(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
        QString const& addressUnitBits, QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The validator used for registers.
    QSharedPointer<RegisterValidator> registerValidator_;

    //! The validator used for register files.
    QSharedPointer<RegisterFileValidator> registerFileValidator_;
};

#endif // ADDRESSBLOCKVALIDATOR_H
