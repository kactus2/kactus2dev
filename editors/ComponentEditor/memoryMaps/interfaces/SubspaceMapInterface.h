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

#include <common/Global.h>

#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryBlockInterface.h>

class SubSpaceMap;
class AddressSpace;
class Segment;

class BusInterfaceInterface;
class ParametersInterface;

//-----------------------------------------------------------------------------
//! Interface for editing subspace maps.
//-----------------------------------------------------------------------------
class KACTUS2_API SubspaceMapInterface : public MemoryBlockInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] busInterfaceInterface   Interface for accessing bus interfaces.
     *      @param [in] parameterInterface      Interface for accessing parameters.
     */
    SubspaceMapInterface(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        BusInterfaceInterface* busInterfaceInterface,
        ParametersInterface* parameterInterface);

	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapInterface() = default;

    /*!
     *  Set the available address spaces. Replace this with the address space interface when done.
     *
     *      @param [in] newAddressSpaces    List of available address spaces.
     */
    void setAddressSpaces(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces);

    /*!
     *  Get the master bus interface reference of the selected subspace map.
     *
     *      @param [in] itemName    Name of the selected subspace map.
     *
     *      @return The master bus interface reference of the selected subspace map.
     */
    std::string getMasterReference(std::string const& itemName) const;

    /*!
     *  Set the master bus interface reference of the selected subspace map.
     *
     *      @param [in] itemName            Name of the selected subspace map.
     *      @param [in] newMasterReference  New master bus interface reference for the selected subspace map.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMasterReference(std::string const& itemName, std::string const& newMasterReference);

    /*!
     *  Get the segment reference of the selected subspace map.
     *
     *      @param [in] itemName    Name of the selected subspace map.
     *
     *      @return Segment reference of the selected subspace map.
     */
    std::string getSegmentReference(std::string const& itemName) const;

    /*!
     *  Set a new segment reference for the selected subspace map.
     *
     *      @param [in] itemName                Name of the selected subspace map.
     *      @param [in] newSegmentReference     The new segment reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setSegmentReference(std::string const& itemName, std::string const& newSegmentReference);

    /*!
     *  Get the calculated range value of the selected memory block.
     *
     *      @param [in] blockName   Name of the selected memory block.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated range value of the selected memory block.
     */
    virtual std::string getRangeValue(std::string const& blockName, int const& baseNumber = 0) const override
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
     *  Check if the selected subspace map has a valid base address.
     *
     *      @param [in] itemName    Name of the selected subspace map .
     *
     *      @return True, if the subspace map is valid, false otherwise.
     */
    virtual bool hasValidBaseAddress(std::string const& itemName) const override final;

    /*!
     *  Check if the selected subspace map has a valid is present value.
     *
     *      @param [in] itemName    Name of the selected subspace map.
     *
     *      @return True, if the is present value is valid, false otherwise.
     */
    virtual bool hasValidIsPresent(std::string const& itemName) const override final;

    /*!
     *  Add a new subspace map.
     *
     *      @param [in] row             Row of the new subspace map.
     *      @param [in] newBlockName    Name of the new subspace map.
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

    //! No copying. No assignment.
    SubspaceMapInterface(const SubspaceMapInterface& other) = delete;
    SubspaceMapInterface& operator=(const SubspaceMapInterface& other) = delete;

private:

    /*!
     *  Get the selected subspace map.
     *
     *      @param [in] itemName    Name of the selected subspace map.
     *
     *      @return The selected subspace map.
     */
    QSharedPointer<SubSpaceMap> getSubspaceMap(std::string const& itemName) const;

    /*!
     *  Get the default name for a subspace map.
     *
     *      @return Default name for a subspace map.
     */
    virtual std::string getDefaultName() const override final;

    /*!
     *  Get a list of the copied subspace maps.
     *
     *      @param [in] selectedRows    Rows of the copied subspace maps.
     *
     *      @return List of the copied subspace maps.
     */
    virtual QList<QSharedPointer<MemoryBlockBase> > getCopiedBlocks(std::vector<int> selectedRows) const override
        final;

    /*!
     *  Get the mime type of the subspace map.
     *
     *      @return The mime type of the subspace map.
     */
    virtual QString getMimeType() const override final;

    /*!
     *  Create a copy of the selected subspace map.
     *
     *      @param [in] copiedItem  The selected subspace map-
     *
     *      @return Copy of the selected subspace map.
     */
    virtual QSharedPointer<MemoryBlockBase> createCopyBlock(QSharedPointer<MemoryBlockBase> copiedItem) const
        override final;

    /*!
     *  Count the subspace map items in the selected list.
     *
     *      @param [in] itemList    List of subspace map items.
     *
     *      @return Number of subspace map items in the selected list.
     */
    virtual int countItems(QList<QSharedPointer<MemoryBlockBase> > itemList) const override final;

    /*!
     *  Get the address space within the bus interface referenced by the selected subspace map.
     *
     *      @param [in] subspaceMapName     Name of the selected subspace map.
     *
     *      @return Address space within the bus interface referenced by the selected subspace map.
     */
    QSharedPointer<AddressSpace> getReferencedAddressSpace(std::string const& subspaceMapName) const;

    /*!
     *  Get the segment referenced by the selected subspace map.
     *
     *      @param [in] referencedSpace     Address space within the master bus interface referenced by the
     *                                      selected subspace map.
     *      @param [in] subspaceMapName     Name of the selected subspace map.
     *
     *      @return Segment referenced by the selected subspace map.
     */
    QSharedPointer<Segment> getReferencedSegment(QSharedPointer<AddressSpace> referencedSpace,
        std::string const& subspaceMapName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterfaceInterface_;
};

#endif // SUBSPACEMAPINTERFACE_H
