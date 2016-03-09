//-----------------------------------------------------------------------------
// File: MemoryMapValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Validator for the ipxact:memoryMap.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPVALIDATOR_H
#define MEMORYMAPVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class MemoryMap;
class MemoryRemap;
class RemapState;
class AddressBlockValidator;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:memoryMap.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapValidator: public MemoryMapBaseValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser        The parser to use for solving expressions.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
     *      @param [in] remapStates             The currently available remap states.
	 */
    MemoryMapValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
        QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates);

	//! The destructor.
	virtual ~MemoryMapValidator();
    
    /*!
     *  Change the availavle remap states.
     *
     *      @param [in] newRemapStates  The new remap states.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<RemapState> > > newRemapStates);

    /*!
     *  Validates the given memory map.
     *
     *      @param [in] memoryMap   The memory map to validate.
     *
     *      @return True, if the memory map is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Check if the memory map contains valid address unit bits.
     *
     *      @param [in] memoryMap   The selected memory map.
     *
     *      @return True, if the address unit bits is valid, otherwise false.
     */
    bool hasValidAddressUnitBits(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Check if the memory map contains valid memory remaps.
     *
     *      @param [in] memoryMap   The selected memory map.
     *
     *      @return True, if the memory remaps are valid, otherwise false.
     */
    bool hasValidMemoryRemaps(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Check if the memory remap remap state is valid.
     *
     *      @param [in] memoryRemap     The selected memory remap.
     *
     *      @return True, if the remap state is valid, otherwise false.
     */
    bool remapStateIsNotValid(QSharedPointer<MemoryRemap> memoryRemap) const;

    /*!
     *  Locate errors within a memory map.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory map.
     *      @param [in] context     Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
        QString const& context) const;

private:

	// Disable copying.
	MemoryMapValidator(MemoryMapValidator const& rhs);
	MemoryMapValidator& operator=(MemoryMapValidator const& rhs);

    /*!
     *  Locate errors within memory map address unit bits.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInAddressUnitBits(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
        QString const& context) const;

    /*!
     *  Locate errors within memory map memory remaps.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInMemoryRemaps(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available remap states.
    QSharedPointer<QList<QSharedPointer<RemapState> > > availableRemapStates_;
};

#endif // MEMORYMAPVALIDATOR_H
