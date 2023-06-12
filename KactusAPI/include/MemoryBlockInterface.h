//-----------------------------------------------------------------------------
// File: MemoryBlockInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 31.01.2022
//
// Description:
// Parent interface for editing memory blocks.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKINTERFACE_H
#define MEMORYBLOCKINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <ParameterizableInterface.h>
#include <NameGroupInterface.h>

#include <QVector>
#include <QMap>

class MemoryBlockBase;
class AddressSpace;
class Segment;
class ParametersInterface;
class MemoryBlockValidator;
class BusInterfaceInterface;
class Component;

//-----------------------------------------------------------------------------
//! Parent interface for editing memory blocks.
//-----------------------------------------------------------------------------
class KACTUS2_API MemoryBlockInterface : public ParameterizableInterface, public NameGroupInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] busInterface            Interface for accessing bus interfaces.
     *      @param [in] parameterInterface      Interface for accessing parameters.
     */
    MemoryBlockInterface(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        BusInterfaceInterface* busInterface,
        ParametersInterface* parameterInterface);
	
	/*!
     *  The destructor.
     */
    virtual ~MemoryBlockInterface() = default;

    /*!
     *  Set available memory blocks.
     *
     *      @param [in] newMemoryBlocks     The new memory block data.
     */
    void setMemoryBlocks(QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > newMemoryBlocks);

    /*!
     *  Setup the sub interfaces.
     *
     *      @param [in] newComponent    Component containing the available items.
     */
    void setupSubInterfaces(QSharedPointer<Component> newComponent);

    /*!
     *  Get the interface for accessing bus interfaces.
     *
     *      @return Interface for accessing bus interfaces.
     */
    BusInterfaceInterface* getBusInterface() const;

    /*!
     *  Check if the selected memory block is accepted. 
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return True, if the memory block is accepted, false otherwise.
     */
    virtual bool acceptBlock(std::string const& blockName) const = 0;

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
     *  Get the calculated base address value of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated base address value of the selected memory block.
     */
    std::string getBaseAddressValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted base address expression of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return Formatted base address expression of the selected memory block.
     */
    std::string getBaseAddressFormattedExpression(std::string const& blockName) const;

    /*!
     *  Get the base address expression of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return Base address expression of the selected memory block.
     */
    std::string getBaseAddressExpression(std::string const& blockName) const;

    /*!
     *  Set a new base address value for the selected memory block.
     *
     *      @param [in] blockName       Name of the selected memory block.
     *      @param [in] newBaseAddress  New base memory value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBaseAddress(std::string const& blockName, std::string const& newBaseAdress) const;
    
    /*!
     *  Get the calculated is present value of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated is present value of the selected memory block.
     */
    std::string getIsPresentValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return Formatted is present expression of the selected memory block.
     */
    std::string getIsPresentFormattedExpression(std::string const& blockName) const;

    /*!
     *  Get the is present expression of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return Is present expression of the selected memory block.
     */
    std::string getIsPresentExpression(std::string const& blockName) const;

    /*!
     *  Set a new is present value for the selected memory block.
     *
     *      @param [in] blockName       Name of the selected memory block.
     *      @param [in] newIsPresent    New is present value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(std::string const& blockName, std::string const& newIsPresent) const;

    /*!
     *  Get the calculated range value of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated range value of the selected memory block.
     */
    std::string getRangeValue(std::string const& blockName, int const& baseNumber = 0) const;

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const override;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override = 0;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    bool itemHasValidName(std::string const& itemName) const;

    /*!
     *  Check if the selected memory block has a valid base address.
     *
     *      @param [in] itemName    Name of the selected memory block.
     *
     *      @return True, if the base address is valid, false otherwise.
     */
    bool hasValidBaseAddress(std::string const& itemName) const;

    /*!
     *  Check if the selected memory block has a valid is present value.
     *
     *      @param [in] itemName    Name of the selected memory block.
     *
     *      @return True, if the is present value is valid, false otherwise.
     */
    bool hasValidIsPresent(std::string const& itemName) const;

    /*!
     *  Add a new memory block.
     *
     *      @param [in] row             Row of the new memory block.
     *      @param [in] newBlockName    Name of the new memory block.
     */
    virtual void addBlock(int const& row, std::string const& newBlockName = std::string("")) = 0;

    /*!
     *  Remove the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
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
    virtual std::vector<std::string> getExpressionsInSelectedItems(std::vector<std::string> itemNames) const = 0;

    /*!
     *  Change the value for address unit bits.
     *
     *      @param [in] newAddressUnitbits  The new value for address unit bits.
     */
    void setAddressUnitBits(std::string const& newAddressUnitbits);

protected:

    /*!
     *  Get the available memory blocks.
     *
     *      @return List of available memory blocks.
     */
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > getBlocksData();

    /*!
     *  Get the base address for a new memory block.
     *
     *      @return Base address for a new memory block.
     */
    QString getNewBlockBaseAddress() const;

    /*!
     *  Get the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return The selected memory block.
     */
    QSharedPointer<MemoryBlockBase> getBlock(std::string const& itemName) const;

    /*!
     *  Get the address unit bits.
     *
     *      @return The address unit bits.
     */
    QString getAddressUnitBits() const;

    /*!
     *  Get the address space within the selected bus interface.
     *
     *      @param [in] masterBusReference  Name of the selected bus interface.
     *
     *      @return Address space within the selected bus interface.
     */
    QSharedPointer<AddressSpace> getReferencedAddressSpace(QString const& masterBusReference) const;

private:

    /*!
     *  Get the default name for a memory block.
     *
     *      @return Default name for a memory block.
     */
    virtual std::string getDefaultName() const = 0;

    /*!
     *  Get a list of the copied memory blocks.
     *
     *      @param [in] selectedRows    Rows of the copied memory blocks.
     *
     *      @return List of the copied memory blocks.
     */
    virtual QList<QSharedPointer<MemoryBlockBase> > getCopiedBlocks(std::vector<int> selectedRows) const = 0;

    /*!
     *  Get the mime type of the memory block.
     *
     *      @return The mime type of the memory block.
     */
    virtual QString getMimeType() const = 0;

    /*!
     *  Create a copy of the selected memory block.
     *
     *      @param [in] copiedItem  The selected memory block-
     *
     *      @return Copy of the selected memory block.
     */
    virtual QSharedPointer<MemoryBlockBase> createCopyBlock(QSharedPointer<MemoryBlockBase> copiedItem) const = 0;

    /*!
     *  Count the memory block items in the selected list.
     *
     *      @param [in] itemList    List of memory block items.
     *
     *      @return Number of memory block items in the selected list.
     */
    virtual int countItems(QList<QSharedPointer<MemoryBlockBase> > itemList) const = 0;

    /*!
     *  Get the memory block validator.
     *
     *      @return The memory block validator.
     */
    virtual QSharedPointer<MemoryBlockValidator> getValidator() const = 0;

    /*!
     *  Get the segment referenced by the selected subspace map.
     *
     *      @param [in] referencedSpace     Address space within the master bus interface referenced by the
     *                                      selected subspace map.
     *      @param [in] segmentReference    Name of the selected segment.
     *
     *      @return Segment referenced by the selected subspace map.
     */
    QSharedPointer<Segment> getReferencedSegment(QSharedPointer<AddressSpace> referencedSpace,
        QString const& segmentReference) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > availableAddressSpaces_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterfaceInterface_;

    //! List of the contained memory blocks.
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > blockData_;

    //! The address unit bits of the containing memory map.
    QString addressUnitBits_;

    //! Interface for accessing parameters.
    ParametersInterface* parameterInterface_;
};

#endif // MEMORYBLOCKINTERFACE_H
