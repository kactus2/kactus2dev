//-----------------------------------------------------------------------------
// File: MemoryMapBaseValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Validator for the memoryMap base class.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPBASEVALIDATOR_H
#define MEMORYMAPBASEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class MemoryMapBase;
class AddressBlock;
class AddressBlockValidator;
//-----------------------------------------------------------------------------
//! Validator for the base ipxact:memoryMap.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapBaseValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser        The parser to use for solving expressions.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
	 */
    MemoryMapBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator);

	//! The destructor.
	virtual ~MemoryMapBaseValidator();
    
    /*!
     *  Get the validator used for address blocks.
     *
     *      @return The validator used for address blocks.
     */
    QSharedPointer<AddressBlockValidator> getAddressBlockValidator() const;

    /*!
     *  Validates the given memory map base.
     *
     *      @param [in] memoryMapBase       The memory map base to validate.
     *      @param [in] addressUnitBits     The address unit bits used by the memory map.
     *
     *      @return True, if the memory map base is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits) const;

    /*!
     *  Check if the memory map base contains a valid name.
     *
     *      @param [in] memoryMapBase   The selected memory map base.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<MemoryMapBase> memoryMapBase) const;

    /*!
     *  Check if the memory map base contains a valid isPresent value.
     *
     *      @param [in] memoryMapBase   The selected memory map base.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<MemoryMapBase> memoryMapBase) const;

    /*!
     *  Check if the memory map base contains valid memory blocks.
     *
     *      @param [in] memoryMapBase       The selected memory map base.
     *      @param [in] addressUnitBits     The address unit bits used by the memory map.
     *
     *      @return True, if the memory blocks are valid, otherwise false.
     */
    bool hasValidMemoryBlocks(QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits) const;

    /*!
     *  Locate errors within a memory map base.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] memoryMapBase       The selected memory map base.
     *      @param [in] addressUnitBits     The address unit bits used by the memory map.
     *      @param [in] context             Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& addressUnitBits, QString const& context) const;

protected:

    /*!
     *  Get the expression parser.
     *
     *      @return The used expression parser.
     */
    QSharedPointer<ExpressionParser> getExpressionParser() const;

private:

	// Disable copying.
	MemoryMapBaseValidator(MemoryMapBaseValidator const& rhs);
	MemoryMapBaseValidator& operator=(MemoryMapBaseValidator const& rhs);

    /*!
     *  Check if the address block overlaps with another address block.
     *
     *      @param [in] addressBlock        The selected address block.
     *      @param [in] memoryMapBase       The selected memory map base.
     *      @param [in] addressBlockIndex   The index of the address block.
     *
     *      @return True, if the address blocks overlap, otherwise false.
     */
    bool addressBlockOverlaps(QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<MemoryMapBase> memoryMapBase, int addressBlockIndex) const;

    /*!
     *  Check if two address blocks overlap.
     *
     *      @param [in] addressBlock    The selected address block.
     *      @param [in] comparedBlock   The compared address block.
     */
    bool twoAddressBlocksOverlap(QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<AddressBlock> comparedBlock) const;

    /*!
     *  Check if the address block width is a multiplication of address unit bits.
     *
     *      @param [in] addressUnitBits     The address unit bits.
     *      @param [in] addressBlock        The selected address block.
     *
     *      @return True, if the address block width is a multiplication of address unit bits, otherwise false.
     */
    bool addressBlockWidthIsMultiplicationOfAUB(QString const& addressUnitBits,
        QSharedPointer<AddressBlock> addressBlock) const;
    
    /*!
     *  Find errors within a name.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryMapBase   The selected memory map base.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& context) const;

    /*!
     *  Find errors within a isPresent.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryMapBase   The selected memory map base.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& context) const;

    /*!
     *  Find errors within address blocks.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] memoryMapBase       The selected memory map base.
     *      @param [in] addressUnitBits     The address unit bits used by the memory map.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInAddressBlocks(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& addressUnitBits, QString const& context) const;

    /*!
     *  Find errors within overlapping address blocks.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryMapBase   The selected memory map base.
     *      @param [in] addressBlock    The selected address block.
     *      @param [in] blockIndex      The index of the address block.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInOverlappingBlocks(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QSharedPointer<AddressBlock> addressBlock, int blockIndex, QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used address block validator.
    QSharedPointer<AddressBlockValidator> addressBlockValidator_;
};

#endif // MEMORYMAPBASEVALIDATOR_H
