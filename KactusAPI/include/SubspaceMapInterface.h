//-----------------------------------------------------------------------------
// File: SubspaceMapInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.01.2022
//
// Description:
// Interface for editing subspace maps.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPINTERFACE_H
#define SUBSPACEMAPINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <KactusAPI/include/MemoryBlockInterface.h>

class Component;
class SubSpaceMap;
class AddressSpace;
class Segment;

class BusInterfaceInterface;
class ParametersInterface;
class SubspaceMapValidator;

//-----------------------------------------------------------------------------
//! Interface for editing subspace maps.
//-----------------------------------------------------------------------------
class KACTUS2_API SubspaceMapInterface : public MemoryBlockInterface
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] subspaceValidator       Validator for subspace maps.
     *    @param [in] expressionParser        Parser for expressions.
     *    @param [in] expressionFormatter     Formatter for expressions.
     *    @param [in] busInterfaceInterface   Interface for accessing bus interfaces.
     *    @param [in] parameterInterface      Interface for accessing parameters.
     */
    SubspaceMapInterface(QSharedPointer<SubspaceMapValidator> subspaceValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        BusInterfaceInterface* busInterface,
        ParametersInterface* parameterInterface);

	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapInterface() = default;

    /*!
     *  Check if the selected memory block is accepted.
     *
     *    @param [in] blockName   Name of the selected memory block.
     *
     *    @return True, if the memory block is accepted, false otherwise.
     */
    virtual bool acceptBlock(std::string const& blockName) const override final;

    /*!
     *  Get the master bus interface reference of the selected subspace map.
     *
     *    @param [in] itemName    Name of the selected subspace map.
     *
     *    @return The master bus interface reference of the selected subspace map.
     */
    std::string getInitiatorReference(std::string const& itemName) const;

    /*!
     *  Set the master bus interface reference of the selected subspace map.
     *
     *    @param [in] itemName            Name of the selected subspace map.
     *    @param [in] newMasterReference  New master bus interface reference for the selected subspace map.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setMasterReference(std::string const& itemName, std::string const& newMasterReference);

    /*!
     *  Get the segment reference of the selected subspace map.
     *
     *    @param [in] itemName    Name of the selected subspace map.
     *
     *    @return Segment reference of the selected subspace map.
     */
    std::string getSegmentReference(std::string const& itemName) const;

    /*!
     *  Set a new segment reference for the selected subspace map.
     *
     *    @param [in] itemName                Name of the selected subspace map.
     *    @param [in] newSegmentReference     The new segment reference.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setSegmentReference(std::string const& itemName, std::string const& newSegmentReference);

    /*!
     *  Get the calculated width value of the selected address block.
     *
     *    @param [in] blockName   Name of the selected address block.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated width value of the selected address block.
     */
    std::string getWidthValue(std::string const& subspaceName, int const& baseNumber = 0) const;

    /*!
     *  Validates the contained items.
     *
     *    @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected subspace map has a valid master bus interface reference.
     *
     *    @param [in] itemName    Name of the selected subspace map.
     *
     *    @return True, if the master bus interface reference is valid, false otherwise.
     */
    bool hasValidMasterReference(std::string const& itemName) const;

    /*!
     *  Check if the selected subspace map has a valid segment reference.
     *
     *    @param [in] itemName    Name of the selected subspace map.
     *
     *    @return True, if the segment reference is valid, false otherwise.
     */
    bool hasValidSegmentReference(std::string const& itemName) const;

    /*!
     *  Add a new subspace map.
     *
     *    @param [in] row             Row of the new subspace map.
     *    @param [in] newBlockName    Name of the new subspace map.
     */
    virtual void addBlock(int const& row, std::string const& newBlockName = std::string("")) override final;

    /*!
     *  Get the expressions in the selected items.
     *
     *    @param [in] itemNames   Names of the selected items.
     *
     *    @return The expressions of the selected items.
     */
    virtual std::vector<std::string> getExpressionsInSelectedItems(std::vector<std::string> const& itemNames) const override final;

    //! No copying. No assignment.
    SubspaceMapInterface(const SubspaceMapInterface& other) = delete;
    SubspaceMapInterface& operator=(const SubspaceMapInterface& other) = delete;

private:

    /*!
     *  Get the selected subspace map.
     *
     *    @param [in] itemName    Name of the selected subspace map.
     *
     *    @return The selected subspace map.
     */
    QSharedPointer<SubSpaceMap> getSubspaceMap(std::string const& itemName) const;

    /*!
     *  Get the subspace map with the selected name, sliced to a NameGroup.
     *
     *    @param [in] itemName    Name of the selected subspace map.
     *
     *    @return The sliced subspace map with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& itemName) const override;

    /*!
     *  Get the default name for a subspace map.
     *
     *    @return Default name for a subspace map.
     */
    virtual std::string getDefaultName() const override final;

    /*!
     *  Get a list of the copied subspace maps.
     *
     *    @param [in] selectedRows    Rows of the copied subspace maps.
     *
     *    @return List of the copied subspace maps.
     */
    virtual QList<QSharedPointer<MemoryBlockBase> > getCopiedBlocks(std::vector<int> const& selectedRows) const override final;

    /*!
     *  Get the mime type of the subspace map.
     *
     *    @return The mime type of the subspace map.
     */
    virtual QString getMimeType() const override final;

    /*!
     *  Create a copy of the selected subspace map.
     *
     *    @param [in] copiedItem  The selected subspace map-
     *
     *    @return Copy of the selected subspace map.
     */
    virtual QSharedPointer<MemoryBlockBase> createCopyBlock(QSharedPointer<MemoryBlockBase> copiedItem) const
        override final;

    /*!
     *  Count the subspace map items in the selected list.
     *
     *    @param [in] itemList    List of subspace map items.
     *
     *    @return Number of subspace map items in the selected list.
     */
    virtual int countItems(QList<QSharedPointer<MemoryBlockBase> > itemList) const override final;

    /*!
     *  Get the subspace map validator.
     *
     *    @return The subspace map validator.
     */
    virtual QSharedPointer<MemoryBlockValidator> getValidator() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The subspace map validator.
    QSharedPointer<SubspaceMapValidator> validator_;
};

#endif // SUBSPACEMAPINTERFACE_H
