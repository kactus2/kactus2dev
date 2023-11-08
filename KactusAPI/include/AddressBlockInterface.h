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

#include <KactusAPI/KactusAPIGlobal.h>

#include <MemoryBlockInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

class AddressBlock;
class MemoryBlockBase;
class AddressBlockValidator;
class RegisterInterface;
class BusInterfaceInterface;

#include <QVector>
#include <QMap>

//-----------------------------------------------------------------------------
//! Interface for editing address blocks.
//-----------------------------------------------------------------------------
class KACTUS2_API AddressBlockInterface : public MemoryBlockInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] blockValidator          Validator for address blocks.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] busInterface            Interface for accessing bus interfaces.
     *      @param [in] subInterface            Interface for accessing registers.
     *      @param [in] parameterInterface      Interface for accessing parameters.
     */
    AddressBlockInterface(QSharedPointer<AddressBlockValidator> blockValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        BusInterfaceInterface* busInterface,
        RegisterInterface* subInterface,
        ParametersInterface* parameterInterface);
	
	/*!
     *  The destructor.
     */
    virtual ~AddressBlockInterface() = default;

    /*!
     *  Check if the selected memory block is an address block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *
     *      @return True, if the memory block is an address block, false otherwise.
     */
    virtual bool acceptBlock(std::string const& blockName) const override final;

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
     *  Get the access string of the selected address block or one of its access policies specified by the
     *  accessPolicyIndex parameter. The default value of -1 returns the address block's access value. 0 and above
     *  returns an indexed access policy's access value.
     *
     *      @param [in] blockName           Name of the selected address block.
     *      @param [in] accessPolicyIndex   The access policy to get the access value of.
     *
     *      @return Access string of the selected address block or its access policy.
     */
    std::string getAccessString(std::string const& blockName, int accessPolicyIndex = -1) const;

    /*!
     *  Get the access of the selected address block or one of its access policies specified by the
     *  accessPolicyIndex parameter. The default value of -1 returns the address block's access value. 0 and above
     *  returns an indexed access policy's access value.
     *
     *      @param [in] blockName           Name of the selected address block.
     *      @param [in] accessPolicyIndex   The access policy to get the access value of.
     *
     *      @return Access of the selected address block or its access policy.
     */
    AccessTypes::Access getAccess(std::string const& blockName, int accessPolicyIndex = -1) const;

    /*!
     *  Set a new access for the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *      @param [in] newAccess   New access value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAccess(std::string const& blockName, std::string const& newAccess, int accessPolicyIndex = -1) const;

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
    virtual void addBlock(int const& row, std::string const& newBlockName = std::string("")) override final;

    /*!
     *  Get the expressions in the selected items.
     *
     *      @param [in] itemNames   Names of the selected items.
     *
     *      @return The expressions of the selected items.
     */
    virtual std::vector<std::string> getExpressionsInSelectedItems(std::vector<std::string> itemNames) const
        override final;

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

    /*!
     *	Get the number of access policies an address block has.
     *  
     *      @param [in] blockName     Name of the selected address block.
     *	    
     * 	    @return The number of access policies of the selected address block.
     */
    int getAccessPolicyCount(std::string const& blockName) const;

    /*!
     *	Add a new access policy to an address block.
     *  
     *      @param [in] blockName     Name of the selected address block.
     * 
     *      @return True, if the operation was successful, otherwise false.
     */
    bool addAccessPolicy(std::string const& blockName) const;


private:

    /*!
     *  Get the selected address block.
     *
     *      @param [in] blockName   Name of the selected address block.
     *
     *      @return The selected address block.
     */
    QSharedPointer<AddressBlock> getAddressBlock(std::string const& itemName) const;

    /*!
     *  Get the default name for an address block.
     *
     *      @return Default name for an address block.
     */
    virtual std::string getDefaultName() const override final;

    /*!
     *  Get a list of the copied address blocks.
     *
     *      @param [in] selectedRows    Rows of the copied address blocks.
     *
     *      @return List of the copied memory blocks.
     */
    virtual QList<QSharedPointer<MemoryBlockBase> > getCopiedBlocks(std::vector<int> selectedRows) const override
        final;

    /*!
     *  Get the mime type of the address block.
     *
     *      @return The mime type of the address block.
     */
    virtual QString getMimeType() const override final;

    /*!
     *  Create a copy of the selected address block.
     *
     *      @param [in] copiedItem  The selected address block-
     *
     *      @return Copy of the selected address block.
     */
    virtual QSharedPointer<MemoryBlockBase> createCopyBlock(QSharedPointer<MemoryBlockBase> copiedItem) const
        override final;

    /*!
     *  Count the address block items in the selected list.
     *
     *      @param [in] itemList    List of address block items.
     *
     *      @return Number of address block items in the selected list.
     */
    virtual int countItems(QList<QSharedPointer<MemoryBlockBase> > itemList) const override final;

    /*!
     *  Get the address block validator.
     *
     *      @return The address block validator.
     */
    virtual QSharedPointer<MemoryBlockValidator> getValidator() const override final;

    /*!
     *  Get the address block with the selected name, sliced to a NameGroup.
     *
     *      @param [in] itemName    Name of the selected address block.
     *
     *      @return The sliced address block with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& itemName) const override;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Validator for registers.
    QSharedPointer<AddressBlockValidator> validator_;

    //! Interface for accessing registers.
    RegisterInterface* subInterface_;
};

#endif // ADDRESSBLOCKINTERFACE_H
