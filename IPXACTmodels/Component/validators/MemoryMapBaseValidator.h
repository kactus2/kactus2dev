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

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/validators/HierarchicalValidator.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class MemoryMapBase;
class MemoryBlockBase;
class AddressBlockValidator;
class ResetType;
class SubspaceMapValidator;
class Component;
class AddressBlock;
class SubSpaceMap;
class BusInterface;
class AddressSpace;
class Segment;
class MemoryReserve;

//-----------------------------------------------------------------------------
//! Validator for the base ipxact:memoryMap.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapBaseValidator : public HierarchicalValidator
{
public:


	/*!
	 *  The constructor.
	 *
	 *    @param [in] expressionParser        The parser to use for solving expressions.
     *    @param [in] addressBlockValidator   Validator used for address blocks.
	 */
    MemoryMapBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
        QSharedPointer<SubspaceMapValidator> subspaceValidator,
        Document::Revision docRevision = Document::Revision::Unknown);

	/*!
     *  The destructor.
     */
	virtual ~MemoryMapBaseValidator() = default;

    // Disable copying.
    MemoryMapBaseValidator(MemoryMapBaseValidator const& rhs) = delete;
    MemoryMapBaseValidator& operator=(MemoryMapBaseValidator const& rhs) = delete;
    
    /*!
     *  Change the containing component.
     *
     *    @param [in] newComponent    The selected component.
     */
    void componentChange(QSharedPointer<Component> newComponent);

    /*!
     *  Get the validator used for address blocks.
     *
     *    @return The validator used for address blocks.
     */
    QSharedPointer<AddressBlockValidator> getAddressBlockValidator() const;

    /*!
     *  Get the validator used for subspace maps.
     *
     *    @return The validator used for subspace maps.
     */
    QSharedPointer<SubspaceMapValidator> getSubspaceValidator() const;

    /*!
     *  Validates the given memory map base.
     *
     *    @param [in] memoryMapBase       The memory map base to validate.
     *    @param [in] addressUnitBits     The address unit bits used by the memory map.
     *
     *    @return True, if the memory map base is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits);

    /*!
     *  Check if the memory map base contains a valid name.
     *
     *    @param [in] memoryMapBase   The selected memory map base.
     *
     *    @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<MemoryMapBase> memoryMapBase) const;

    /*!
     *  Check if the memory map base contains a valid isPresent value.
     *
     *    @param [in] memoryMapBase   The selected memory map base.
     *
     *    @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<MemoryMapBase> memoryMapBase) const;

    /*!
     *  Check if the memory map base contains valid memory blocks.
     *
     *    @param [in] memoryMapBase       The selected memory map base.
     *    @param [in] addressUnitBits     The address unit bits used by the memory map.
     *
     *    @return True, if the memory blocks are valid, otherwise false.
     */
    bool hasValidMemoryBlocks(QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits);

    /*!
     *  Locate errors within a memory map base.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] memoryMapBase       The selected memory map base.
     *    @param [in] addressUnitBits     The address unit bits used by the memory map.
     *    @param [in] context             Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& addressUnitBits, QString const& context);

protected:

    /*!
     *  Get the expression parser.
     *
     *    @return The used expression parser.
     */
    QSharedPointer<ExpressionParser> getExpressionParser() const;

    //! The IP-XACT standard revision to comply to.
    Document::Revision docRevision_;

private:

    /*!
     *  Get the range of the selected memory block
     *
     *    @param [in] block   The selected memory block.
     *
     *    @return Range of the selected memory block.
     */
    quint64 getBlockRange(QSharedPointer<MemoryBlockBase> block) const;

    /*!
     *  Get the address space referenced by the selected subspace map.
     *
     *    @param [in] subspace    The selected subspace map.
     *
     *    @return The referenced address space.
     */
    QSharedPointer<AddressSpace> getReferencedAddressSpace(QSharedPointer<SubSpaceMap> subspace) const;

    /*!
     *  Get the segment referenced by the selected subspace map.
     *
     *    @param [in] subspace            The selected subspace map.
     *    @param [in] containingSpace     Address space referenced by the subspace map.
     *
     *    @return Segment referened by the selected subspace map.
     */
    QSharedPointer<Segment> getReferencedSegment(QSharedPointer<SubSpaceMap> subspace,
        QSharedPointer<AddressSpace> containingSpace) const;

    /*!
     *  Check if the address block width is a multiple of address unit bits.
     *
     *    @param [in] addressUnitBits     The address unit bits.
     *    @param [in] addressBlock        The selected address block.
     *
     *    @return True, if the address block width is a multiple of address unit bits, otherwise false.
     */
    bool addressBlockWidthIsMultipleOfAUB(QString const& addressUnitBits,
        QSharedPointer<AddressBlock> addressBlock) const;
    
    /*!
     *  Find errors within a name.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] memoryMapBase   The selected memory map base.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& context) const;

    /*!
     *  Find errors within a isPresent.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] memoryMapBase   The selected memory map base.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& context) const;

    /*!
     *  Find errors within memory blocks.
     *
     *    @param [in] errors              List of found errors.
     *    @param [in] memoryMapBase       The selected memory map base.
     *    @param [in] addressUnitBits     The address unit bits used by the memory map.
     *    @param [in] context             Context to help locate the error.
     */
    void findErrorsInMemoryBlocks(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QString const& addressUnitBits, QString const& context);

    /*!
     *  Find errors within overlapping memory blocks.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] memoryMapBase   The selected memory map base.
     *    @param [in] memoryBlock     The selected memory block.
     *    @param [in] blockIndex      The index of the address block.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInOverlappingBlocks(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
        QSharedPointer<MemoryBlockBase> memoryBlock, int blockIndex, QString const& context) const;

    /*!
     *	Create memory areas for memory block overlap check.
     *
     *    @param [in] memMap              The memory map containing memory blocks of which memory areas are created.
     *    @param [in] reserve             The memory reserve to add created areas to.
     */
    void setupMemoryAreas(QSharedPointer<MemoryMapBase> memMap, MemoryReserve& memReserve);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used address block validator.
    QSharedPointer<AddressBlockValidator> addressBlockValidator_;

    //! Validator for subspace maps.
    QSharedPointer<SubspaceMapValidator> subspaceValidator_;

    //! Interface for accessomg bus interfaces.
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableBusInterfaces_;

    //! List of available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > availableSpaces_;
};

#endif // MEMORYMAPBASEVALIDATOR_H
