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
class SubspaceMapValidator;
class Mode;

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
     *      @param [in] subspaceValidator       Validator used for subspace maps.
     *      @param [in] component               The containing component.
	 */
    MemoryMapValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
        QSharedPointer<SubspaceMapValidator> subspaceValidator,
        QSharedPointer<Component> component);

	//! The destructor.
	virtual ~MemoryMapValidator() override final = default;

    // Disable copying.
    MemoryMapValidator(MemoryMapValidator const& rhs) = delete;
    MemoryMapValidator& operator=(MemoryMapValidator const& rhs) = delete;
    
    /*!
     *  Change the containing component.
     *
     *      @param [in] newComponent    The selected component.
     */
    void componentChange(QSharedPointer<Component> newComponent);

    /*!
     *  Validates the given memory map.
     *
     *      @param [in] memoryMap   The memory map to validate.
     *
     *      @return True, if the memory map is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<MemoryMap> memoryMap);

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
    bool hasValidMemoryRemaps(QSharedPointer<MemoryMap> memoryMap);

    /*!
     *  Check if the memory remap remap state is valid.
     *
     *      @param [in] memoryRemap     The selected memory remap.
     *
     *      @return True, if the remap state is valid, otherwise false.
     */
    bool remapStateIsNotValid(QSharedPointer<MemoryRemap> memoryRemap) const;

    bool hasValidRemapStateOrModeReferences(QSharedPointer<MemoryMap> parentMap, QSharedPointer<MemoryRemap> memoryRemap) const;

    /*!
     *	Check if the memory remap has a valid structure. It cannot simultanously have a 
     *  definition reference and defined memory blocks (std 2022).
     *  
     *      @param [in] memoryRemap     The selected memory remap.
     *	    
     * 	    @return True, if the structure is valid, otherwise false.
     */
    bool remapHasValidStructure(QSharedPointer<MemoryRemap> memoryRemap) const;

    /*!
     *	Check if the memory map has a valid structure. It cannot simultanously have a
     *  definition reference and elements defined in the memoryMapGroup (std 2022).
     *
     *      @param [in] memoryMap       The selected memory map.
     *
     * 	    @return True, if the structure is valid, otherwise false.
     */
    bool hasValidStructure(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Locate errors within a memory map.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory map.
     *      @param [in] context     Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
        QString const& context);

private:
    
    /*!
     *	Check if memory remaps contain valid mode references.
     *  
     *      @param [in] memoryMap     The memory map whose remaps are checked.
     *	    
     * 	    @return True, if the mode references of remaps are valid, otherwise false.
     */
    bool remapsHaveValidModeRefs(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *	Find errors within the mode references of memory remaps.
     *  
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInRemapModeRefs(QStringList& errors, QSharedPointer<MemoryMap> memoryMap, 
        QString const& context) const;

    /*!
     *	Find errors in the structure of the memory map.
     *  
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInStrucutre(QStringList& errors, QSharedPointer<MemoryMap> memoryMap,
        QString const& context) const;

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
        QString const& context);

    /*!
     *	Find errors in the structure of a memory remap.
     *  
     *      @param [in] errors      List of found errors.
     *      @param [in] memoryMap   The selected memory remap.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInRemapStructure(QStringList& errors, QSharedPointer<MemoryRemap> memoryRemap,
        QString const& context) const;


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available remap states.
    QSharedPointer<QList<QSharedPointer<RemapState> > > availableRemapStates_;

    //! The available component modes.
    QSharedPointer<QList<QSharedPointer<Mode> > > componentModes_;
};

#endif // MEMORYMAPVALIDATOR_H
