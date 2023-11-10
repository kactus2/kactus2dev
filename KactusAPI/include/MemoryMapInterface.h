//-----------------------------------------------------------------------------
// File: MemoryMapInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.05.2020
//
// Description:
// Interface for editing memory maps and remaps.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPINTERFACE_H
#define MEMORYMAPINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <ParameterizableInterface.h>
#include <NameGroupInterface.h>

class Component;
class MemoryMap;
class MemoryRemap;

class MemoryMapValidator;
class AddressBlockInterface;
class SubspaceMapInterface;
class ModeReferenceInterface;
class ModeReference;

//-----------------------------------------------------------------------------
//! Interface for editing memory maps and remaps.
//-----------------------------------------------------------------------------
class KACTUS2_API MemoryMapInterface : public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] mapValidator            Validator for memory maps.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     */
    MemoryMapInterface(QSharedPointer<MemoryMapValidator> mapValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

	/*!
     *  The destructor.
     */
    virtual ~MemoryMapInterface() = default;

    /*!
     *  Set a new address block interface.
     *
     *      @param [in] blockInterface  The new address block interface.
     */
    void setAddressBlockInterface(AddressBlockInterface* blockInterface);

    /*!
     *  Set a new subspace map interface.
     *
     *      @param [in] submapInterface     The new subspace map interface.
     */
    void setSubspaceMapInterface(SubspaceMapInterface* submapInterface);

    /*!
     *  Set available memory maps.
     *
     *      @param [in] component   Component containing the memory maps.
     */
    void setMemoryMaps(QSharedPointer<Component> component);

    /*!
     *  Get index of the selected memory map.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get index of the selected memory remap.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return Index of the selected memory remap.
     */
    int getMemoryRemapIndex(std::string const& mapName, std::string const& remapName) const;

    /*!
     *  Get name of the indexed memory map.
     *
     *      @param [in] itemIndex   Index of the selected memory map.
     *
     *      @return Name of the selected memory map.
     */
    std::string getIndexedItemName(int itemIndex) const final;
    
    /*
     *  Get name of the indexed memory remap.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] itemIndex   Index of the selected memory remap.
     *
     *      @return Name of the selected memory remap.
     */
    std::string getIndexedRemapName(std::string mapName, int const& itemIndex) const;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the number of available memory remaps in the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Number of available memory remaps in the selected memory map.
     */
    int remapCount(std::string const& mapName) const;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Get the names of available memory remaps in the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Names of the available memory remaps in the selected memory map.
     */
    std::vector<std::string> getRemapNames(std::string const& mapName) const;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Set a new name for the selected memory remap.
     *
     *      @param [in] mapName         Name of the memory map containing the selected memory remap.
     *      @param [in] currentName     Name of the selected memory remap.
     *      @param [in] newName         New name for the memory remap.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRemapName(std::string const& mapName, std::string const& currentName, std::string const& newName);

    /*!
     *  Get the description of the selected memory remap.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return Description of the selected memory remap.
     */
    std::string getRemapDescription(std::string const& mapName, std::string const& remapName) const;

    /*!
     *  Set a new description for the selected memory remap.
     *
     *      @param [in] mapName         Name of the memory map containing the selected memory remap.
     *      @param [in] itemName        Name of the selected memory remap.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRemapDescription(std::string const& mapName, std::string const& itemName,
        std::string const& newDescription);

    /*!
     *  Get the calculated is present value of the selected item.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *      @param [in] remapName   Name of the selected memory remap.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated is present value of the selected item.
     */
    std::string getIsPresentValue(std::string const& mapName, std::string const& remapName = "",
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected item.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return Formatted is present expression of the selected item.
     */
    std::string getIsPresentFormattedExpression(std::string const& mapName, std::string const& remapName = "") const;

    /*!
     *  Get the is present expression of the selected item.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return Is present expression of the selected item.
     */
    std::string getIsPresentExpression(std::string const& mapName, std::string const& remapName = "") const;

    /*!
     *  Set a new is present value for the selected item.
     *
     *      @param [in] mapName         Name of the selected memory map.
     *      @param [in] remapName       Name of the selected memory remap.
     *      @param [in] newIsPresent    New is present value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(std::string const& mapName, std::string const& newIsPresent,
        std::string const& remapName = "");

    /*!
     *  Get the remap state of the selected item.
     *
     *      @param [in] mapName         Name of the selected memory map.
     *      @param [in] remapName       Name of the selected memory remap.
     *
     *      @return Remap state of the selected item.
     */
    std::string getRemapState(std::string const& mapName, std::string const& remapName = "") const;

    /*!
     *  Set the remap state for the selected item.
     *
     *      @param [in] mapName         Name of the selected memory map.
     *      @param [in] remapName       Name of the selected memory remap.
     *      @param [in] newRemapState   New remap state.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRemapState(std::string const& mapName, std::string const& remapName, std::string const& newRemapState);

    /*!
     *  Get the address unit bits of the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Address unit bits of the selected memory map.
     */
    std::string getAddressUnitBits(std::string const& mapName) const;

    /*!
     *  Set the address unit bits for the selected memory map.
     *
     *      @param [in] mapName             Name of the selected memory map.
     *      @param [in] newAddressUnitBits  New address unit bits.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAddressUnitBits(std::string const& mapName, std::string const& newAddressUnitBits) const;

    /*!
     *  Get the slave interface binding of the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Slave interface binding of the selected memory map.
     */
    std::string getInterfaceBinding(std::string const& mapName) const;

    /*!
     *  Get the associated slave interface of the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return The associated slave interfaces of the selected memory map.
     */
    std::vector<std::string> getAssociatedSlaveInterfaces(std::string const& mapName) const;

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const override
        final;

    /*!
     *  Get the expressions in the selected memory item.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return List of expressions in the selected memory item.
     */
    std::vector<std::string> getMapExpressions(std::string const& mapName, std::string const& remapName) const;

    /*!
     *  Get the number of references made to the selected parameter in the selected memory remap.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *      @param [in] remapName   Name of the selected memory remap.
     *      @param [in] valueID     ID of the selected parameter.
     *
     *      @return The number of references made to the selected parameter in the selected memory remap.
     */
    int getAllReferencesToIdInRemapItem(std::string const& mapName, std::string& remapName,
        std::string const& valueID) const;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Check if the selected memory remap has a valid name.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] itemName    Name of the selected memory remap.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    bool remapHasValidName(std::string const& mapName, std::string const& itemName) const;

    /*!
     *  Check if the selected memory remap has a valid remap state.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] itemName    Name of the selected memory remap.
     *
     *      @return True, if the remap state is valid, false otherwise.
     */
    bool remapHasValidRemapState(std::string const& mapName, std::string const& itemName) const;

    /*!
     *  Check if the selected memory item has a valid is present.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return True, if the is present is valid, false otherwise.
     */
    bool itemHasValidIsPresent(std::string const& mapName, std::string const& remapName = "") const;

    /*!
     *  Check if the selected memory map has a valid address unit bits value.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return True, if the address unit bits value is valid, false otherwise.
     */
    bool memoryMapHasValidAddressUnitBits(std::string const& mapName) const;

    /*!
     *  Add a new memory map.
     *
     *      @param [in] row         Row of the new memory map.
     *      @param [in] newMapName  Name of the new memory map.
     */
    void addMemoryMap(int const& row, std::string const& newMapName = std::string(""));

    /*!
     *  Add a new memory remap.
     *
     *      @param [in] mapName         Name of the memory map containing the new memory remap.
     *      @param [in] newRemapName    Name of the new memory map.
     */
    void addMemoryRemap(std::string const& mapName, std::string const& newRemapName = std::string(""));

    /*!
     *  Remove the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeMap(std::string const& mapName);

    /*!
     *  Remove the selected memory remap.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeRemap(std::string const& mapName, std::string remapName);

    /*!
     *  Copy the selected items.
     *
     *      @param [in] selectedRows    Indexes of the selected items.
     */
    void copyRows(std::vector<std::string> selectedRows);

    /*!
     *  Paste the selected memory maps.
     *
     *      @return Names of the pasted items.
     */
    std::vector<std::string> pasteMemoryMaps();

    /*!
     *  Paste the selected memory remaps.
     *
     *      @param [in] memoryMapName   Name of the memory map containing the pasted memory remaps.
     *
     *      @return Names of the pasted items.
     */
    std::vector<std::string> pasteMemoryRemaps(std::string const& memoryMapName);

    /*!
     *  Get the address block interface.
     *
     *      @return Interface for accessing address blocks.
     */
    AddressBlockInterface* getAddressBlockInterface() const;

    /*!
     *  Get the subspace map interface.
     *
     *      @return Interface for accessing subspace maps.
     */
    SubspaceMapInterface* getSubspaceMapInterface() const;

    /*!
     *  Get a pointer to the selected memory map (for index construction).
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Pointer to the selected memory map.
     */
    MemoryMap* getMapPointer(std::string const& mapName) const;

    /*!
     *  Get a pointer to the selected memory remap (for index construction).
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] remapName   Name of the selected memory remap.
     *
     *      @return Pointer to the selected memory remap.
     */
    MemoryRemap* getRemapPointer(std::string const& mapName, std::string const& remapName) const;

    /*!
     *	Get the number of mode references for a selected remap.
     *  
     *      @param [in] mapName       Memory map containing the remap.
     *      @param [in] remapName     The name of the remap.
     *	    
     * 	    @return The number of mode references within the remap.
     */
    int getRemapModeReferenceCount(std::string const& mapName, std::string const& remapName) const;

    /*!
     *	Get the mode references of a selected remap.
     *  
     *      @param [in] mapName             The containing memory map.
     *      @param [in] remapName           The name of the remap.
     *	    
     * 	    @return The mode references of selected remap.
     */
    std::vector<std::pair<unsigned int, std::string> > getRemapModeReferences(std::string const& mapName, std::string const& remapName) const;

    /*!
     *	Set the mode references of given remap.
     *  
     *      @param [in] mapName             The containing memory map.
     *      @param [in] remapName           The name of the remap whose mode references are set.
     *      @param [in] newModeRefs         The mode references to set.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setRemapModeReferences(std::string const& mapName, std::string const& remapName, std::vector<std::pair<unsigned int, std::string> > const& newModeRefs);

    /*!
     *	Add a new mode reference to a selected remap.
     *  
     *      @param [in] mapName             The containing memory map.
     *      @param [in] remapName           The name of the remap to add a mode reference to.
     *
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool addRemapModeReference(std::string const& mapName, std::string const& remapName);

    /*!
     *	Remove a selected mode reference of a given remap.
     *  
     *      @param [in] mapName                 The containing memory map.
     *      @param [in] remapName               The name of the remap to remove a mode reference of.
     *      @param [in] modeReferenceIndex      The index of the mode reference to be removed.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool removeRemapModeReference(std::string const& mapName, std::string const& remapName, int modeReferenceIndex);

    /*!
     *	Get the mode reference interface used for accessing remap mode references.
     *  
     * 	    @return  The mode reference interface.
     */
    ModeReferenceInterface* getModeReferenceInterface() const;

    /*!
     *	Set the mode reference interface used for accessing remap mode references.
     *  
     *      @param [in] modeRefInterface     Description
     */
    void setModeReferenceInterface(ModeReferenceInterface* modeRefInterface);

    /*!
     *	Get the mode references of all remaps in memory map (excluding selected remap) as a list of priority, 
     *  reference pairs.
     *  
     *      @param [in] mapName             The containing memory map.
     *      @param [in] remapName           The name of the remap to exclude from the mode ref list.
     *
     * 	    @return  The mode references of remaps in memory map, excluding selected remap.
     */
    std::vector<std::pair<unsigned int, std::string> > getRemapModeReferencesExcludingRemap(
        std::string const& mapName, std::string const& remapName) const;

private:

    /*!
     *  Get the selected memory map.
     *
     *      @param [in] itemName    Name of the selected memory map.
     *
     *      @return The selected memory map.
     */
    QSharedPointer<MemoryMap> getMemoryMap(std::string const& itemName) const;

    /*!
     *  Get the memory map with the selected name, sliced to a NameGroup.
     *
     *      @param [in] itemName    Name of the selected memory map.
     *
     *      @return The sliced memory map with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& itemName) const override;

    /*!
     *  Get the selected memory remap.
     *
     *      @param [in] mapName     Name of the memory map containing the selected memory remap.
     *      @param [in] itemName    Name of the selected memory remap.
     *
     *      @return The selected memory remap.
     */
    QSharedPointer<MemoryRemap> getMemoryRemap(std::string const& mapName, std::string const& itemName) const;

    /*!
     *  Paste a single memory map.
     *
     *      @param [in] mapCopy     The selected memory map to copy.
     *
     *      @return Name of the pasted memory map.
     */
    QString pasteMemoryMap(QSharedPointer<MemoryMap> mapCopy);

    /*!
     *  Paste a single memory remap.
     *
     *      @param [in] remapCopy       The selected memory remap to copy.
     *      @param [in] parentMapName   Name of the memory map containing the copied memory remap.
     *
     *      @return Name of the pasted memory remap.
     */
    QString pasteMemoryRemap(QSharedPointer<MemoryRemap> remapCopy, std::string const& parentMapName);

    /*!
     *  Transform the selected name to a unique memory remap name.
     *
     *      @param [in] mapName         Name of the memory map containing the memory remaps.
     *      @param [in] newName         The selected name.
     *      @param [in] itemTypeName    The type of the item.
     *
     *      @return A unique name derived from the selected name.
     */
    QString getUniqueRemapName(std::string const& mapName, std::string const& newName,
        std::string const& itemTypeName) const;

    /*!
     *  Check if the selected memory remap name is unique.
     *
     *      @param [in] mapName     Name of the memory map containing the memory remaps.
     *      @param [in] remapName   The selected memory remap name.
     *
     *      @return True, if the selected name is unique, false otherwise.
     */
    bool remapNameIsUnique(std::string const& mapName, QString const& remapName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Component containing the memory maps.
    QSharedPointer<Component> component_;

    //! List of the contained memory maps.
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > mapData_;

    //! Validator for memory maps.
    QSharedPointer<MemoryMapValidator> validator_;

    //! Interface for accessing address blocks.
    AddressBlockInterface* addressBlockInterface_;

    //! Interface for accessing subspace maps.
    SubspaceMapInterface* subspaceInterface_;

    //! Interface for accessing remap mode references.
    ModeReferenceInterface* modeReferenceInterface_;
};

#endif // MEMORYMAPINTERFACE_H
