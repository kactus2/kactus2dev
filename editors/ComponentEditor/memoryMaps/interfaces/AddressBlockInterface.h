//-----------------------------------------------------------------------------
// File: AddressBlockInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.05.2020
//
// Description:
// Interface for editing address blocks.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKINTERFACE_H
#define ADDRESSBLOCKINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

class AddressBlock;
class MemoryBlockBase;
class AddressBlockValidator;

class RegisterInterface;

#include <QVector>
#include <QMap>

//-----------------------------------------------------------------------------
//! Interface for editing address blocks.
//-----------------------------------------------------------------------------
class AddressBlockInterface : public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] blockValidator          Validator for address blocks.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] subInterface            Interface for accessing registers.
     */
    AddressBlockInterface(QSharedPointer<AddressBlockValidator> blockValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        RegisterInterface* subInterface);
	
	/*!
     *  The destructor.
     */
    virtual ~AddressBlockInterface() = default;

    /*!
     *  Set available memory blocks.
     *
     *      @param [in] newRegisterData     The new register data.
     */
    void setAddressBlocks(QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > newAddressBlocks);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

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
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Get the calculated base address value of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated base address value of the selected address block.
     */
    std::string getBaseAddressValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted base address expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Formatted base address expression of the selected address block.
     */
    std::string getBaseAddressFormattedExpression(std::string const& blockName) const;

    /*!
     *  Get the base address expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Base address expression of the selected address block.
     */
    std::string getBaseAddressExpression(std::string const& blockName) const;

    /*!
     *  Set a new base address value for the selected address block.
     *
     *      @param [in] blockName       Name of the selected address block.
     *      @param [in] newBaseAddress  New base address value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBaseAddress(std::string const& blockName, std::string const& newBaseAdress) const;
    
    /*!
     *  Get the calculated is present value of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated is present value of the selected address block.
     */
    std::string getIsPresentValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Formatted is present expression of the selected address block.
     */
    std::string getIsPresentFormattedExpression(std::string const& blockName) const;

    /*!
     *  Get the is present expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Is present expression of the selected address block.
     */
    std::string getIsPresentExpression(std::string const& blockName) const;

    /*!
     *  Set a new is present value for the selected address block.
     *
     *      @param [in] blockName       Name of the selected address block.
     *      @param [in] newIsPresent    New is present value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(std::string const& blockName, std::string const& newIsPresent) const;

    /*!
     *  Get the calculated range value of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated range value of the selected address block.
     */
    std::string getRangeValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted range expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Formatted range expression of the selected address block.
     */
    std::string getRangeFormattedExpression(std::string const& blockName) const;

    /*!
     *  Get the range expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Range expression of the selected address block.
     */
    std::string getRangeExpression(std::string const& blockName) const;

    /*!
     *  Set a new range value for the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] newRange    New range value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRange(std::string const& blockName, std::string const& newRange) const;

    /*!
     *  Get the calculated width value of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated width value of the selected address block.
     */
    std::string getWidthValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted width expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Formatted width expression of the selected address block.
     */
    std::string getWidthFormattedExpression(std::string const& blockName) const;

    /*!
     *  Get the width expression of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Width expression of the selected address block.
     */
    std::string getWidthExpression(std::string const& blockName) const;

    /*!
     *  Set a new width value for the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] newWidth    New width value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWidth(std::string const& blockName, std::string const& newWidth) const;
    
    /*!
     *  Get the usage string of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Usage string of the selected address block.
     */
    std::string getUsageString(std::string const& blockName) const;

    /*!
     *  Get the usage of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Usage of the selected address block.
     */
    General::Usage getUsage(std::string const& blockName) const;

    /*!
     *  Set a new usage value for the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] newUsage    New usage value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setUsage(std::string const& blockName, std::string const& newUsage) const;
    
    /*!
     *  Get the access string of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Access string of the selected address block.
     */
    std::string getAccessString(std::string const& blockName) const;

    /*!
     *  Get the access of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Access of the selected address block.
     */
    AccessTypes::Access getAccess(std::string const& blockName) const;

    /*!
     *  Set a new access for the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] newAccess   New access value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAccess(std::string const& blockName, std::string const& newAccess) const;

    /*!
     *  Get the volatile value of the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return Volatile value of the selected address block.
     */
    std::string getVolatile(std::string const& blockName) const;

    /*!
     *  Set a new volatile value for the selected address block.
     *
     *      @param [in] blockName       Name of the selected address block.
     *      @param [in] newVolatile     New volatile value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setVolatile(std::string const& blockName, bool newVolatile) const;

    /*!
     *  Set a new volatile value for the selected address block.
     *
     *      @param [in] blockName           Name of the selected address block.
     *      @param [in] newVolatileValue    The new volatile value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setVolatile(std::string const& blockName, std::string const& newVolatileValue);

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const override
        final;

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
     *  Check if the selected address block has a valid base address.
     *
     *      @param [in] itemName    Name of the selected address block.
     *
     *      @return True, if the base address is valid, false otherwise.
     */
    bool hasValidBaseAddress(std::string const& itemName) const;

    /*!
     *  Check if the selected address block has a valid range.
     *
     *      @param [in] itemName    Name of the selected address block.
     *
     *      @return True, if the range is valid, false otherwise.
     */
    bool hasValidRange(std::string const& itemName) const;

    /*!
     *  Check if the selected address block has a valid width.
     *
     *      @param [in] itemName    Name of the selected address block.
     *
     *      @return True, if the width is valid, false otherwise.
     */
    bool hasValidWidth(std::string const& itemName) const;

    /*!
     *  Check if the selected address block has a valid is present value.
     *
     *      @param [in] itemName    Name of the selected address block.
     *
     *      @return True, if the is present value is valid, false otherwise.
     */
    bool hasValidIsPresent(std::string const& itemName) const;

    /*!
     *  Check if the selected address block has a valid usage.
     *
     *      @param [in] itemName    Name of the selected address block.
     *
     *      @return True, if the usage is valid, false otherwise.
     */
    bool hasValidUsage(std::string const& itemName) const;

    /*!
     *  Add a new address block.
     *
     *      @param [in] row             Row of the new address block.
     *      @param [in] newBlockName    Name of the new address block.
     */
    void addBlock(int const& row, std::string const& newBlockName = std::string(""));

    /*!
     *  Remove the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeBlock(std::string const& blockName);

    /*!
     *  Copy the selected items.
     *
     *      @param [in] selectedRows    Indexes of the selected items.
     */
    void copyRows(std::vector<int> selectedRows);

    /*!
     *  Paste the selected items.
     *
     *      @return Names of the pasted items.
     */
    std::vector<std::string> pasteRows();

    /*!
     *  Get the number of pasted items.
     *
     *      @return Number of the pasted items.
     */
    int getPasteRowCount() const;

    /*!
     *  Get the expressions in the selected items.
     *
     *      @param [in] itemNames   Names of the selected items.
     *
     *      @return The expressions of the selected items.
     */
    std::vector<std::string> getExpressionsInSelectedItems(std::vector<std::string> itemNames) const;

    /*!
     *  Change the value for address unit bits.
     *
     *      @param [in] newAddressUnitbits  The new value for address unit bits.
     */
    void setAddressUnitBits(std::string const& newAddressUnitbits);

    /*!
     *  Get the sub interface.
     *
     *      @return Interface for accessing registers.
     */
    RegisterInterface* getSubInterface() const;

    /*!
     *  Check if the selected address block contains registers.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return True, if the address block contains registers, false otherwise.
     */
    bool hasRegisters(std::string const& blockName) const;

private:

    /*!
     *  Get the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return The selected address block.
     */
    QSharedPointer<AddressBlock> getBlock(std::string const& itemName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained registers.
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > blockData_;

    //! Validator for registers.
    QSharedPointer<AddressBlockValidator> validator_;

    //! Interface for accessing registers.
    RegisterInterface* subInterface_;

    //! The address unit bits of the memory map.
    QString addressUnitBits_;
};

#endif // ADDRESSBLOCKINTERFACE_H
