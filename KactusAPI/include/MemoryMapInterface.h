//-----------------------------------------------------------------------------
// File: MemoryMapInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.05.2020
//
// Description:
// Interface for editing memory maps.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPINTERFACE_H
#define MEMORYMAPINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <ParameterizableInterface.h>
#include <NameGroupInterface.h>
#include <MemoryMapBaseInterface.h>

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
class KACTUS2_API MemoryMapInterface : public MemoryMapBaseInterface
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
     *  Get name of the indexed memory map.
     *
     *      @param [in] itemIndex   Index of the selected memory map.
     *
     *      @return Name of the selected memory map.
     */
    std::string getIndexedItemName(int itemIndex) const final;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const override final;

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
     *  Get the address unit bits expression of the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Address unit bits expression of the selected memory map.
     */
    std::string getAddressUnitBitsExpression(std::string const& mapName) const;

    /*!
     *	Get the address unit bits value of the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     * 	    @return Address unit bits of the selected memory map.
     */
    std::string getAddressUnitBitsValue(std::string const& mapName) const;

    /*!
     *	Get the formatted address unit bits expression of the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     * 	    @return Formatted address unit bits expression of the selected memory map.
     */
    std::string getAddressUnitBitsFormattedExpression(std::string const& mapName) const;

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
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final; //where should this be?

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

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
     *  Remove the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeMap(std::string const& mapName);

    /*!
     *  Copy the selected items.
     *
     *      @param [in] selectedRows    Indexes of the selected items.
     */
    void copyRows(std::vector<std::string> const& selectedRows);//where this should be?

    /*!
     *  Paste the selected memory maps.
     *
     *      @return Names of the pasted items.
     */
    std::vector<std::string> pasteMemoryMaps();

    /*!
     *  Get a pointer to the selected memory map (for index construction).
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return Pointer to the selected memory map.
     */
    MemoryMap* getMapPointer(std::string const& mapName) const;

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
     *  Paste a single memory map.
     *
     *      @param [in] mapCopy     The selected memory map to copy.
     *
     *      @return Name of the pasted memory map.
     */
    QString pasteMemoryMap(QSharedPointer<MemoryMap> mapCopy);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained memory maps.
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > mapData_;

    //! Validator for memory maps.
    QSharedPointer<MemoryMapValidator> validator_;

};

#endif // MEMORYMAPINTERFACE_H

