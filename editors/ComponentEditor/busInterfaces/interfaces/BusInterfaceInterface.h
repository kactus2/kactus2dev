//-----------------------------------------------------------------------------
// File: BusInterfaceInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.02.2021
//
// Description:
// Interface for accessing bus interfaces.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEINTERFACE_H
#define BUSINTERFACEINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>
#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QSharedPointer>

class Component;
class MasterInterface;
class SlaveInterface;
class MirroredSlaveInterface;
class TransparentBridge;
class BusInterfaceValidator;
class TransparentBridgeInterface;
class FileSetInterface;
class MemoryMapInterface;
class AbstractionTypeInterface;
class ParametersInterface;

//-----------------------------------------------------------------------------
//! Interface for accessing bus interfaces.
//-----------------------------------------------------------------------------
class KACTUS2_API BusInterfaceInterface : public ParameterizableInterface, public NameGroupInterface
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] busValidator                Validator for bus interfaces.
     *      @param [in] expressionParser            Parser for expressions.
     *      @param [in] expressionFormatter         Formatter for expressions.
     *      @param [in] fileSetInterface            Interface for accessing file sets.
     *      @param [in] mapInterface                Interface for accessing memory maps.
     *      @param [in] abstractionTypeInterface    Interface for accessing abstraction types.
     *      @param [in] bridgeInterface             Interface for accessing bridges.
     *      @param [in] parameterInterface          Interface for accessing parameters.
     */
    BusInterfaceInterface(QSharedPointer<BusInterfaceValidator> busValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        FileSetInterface* fileSetInterface,
        MemoryMapInterface* mapInterface,
        AbstractionTypeInterface* abstractionTypeInterface,
        TransparentBridgeInterface* bridgeInterface,
        ParametersInterface* parameterInterface);

    /*!
     *  The destructor.
     */
    virtual ~BusInterfaceInterface() = default;

    /*!
     *  Setup the bus interfaces.
     *
     *      @param [in] newComponent    Component containing the bus interfaces.
     */
    void setBusInterfaces(QSharedPointer<Component> newComponent);

    /*!
     *  Setup the sub interfaces for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     */
    void setupSubInterfaces(std::string const& busName);

    /*!
     *  Get index of the selected bus interface.
     *
     *      @param [in] itemName    Name of the selected bus interface.
     *
     *      @return Index of the selected bus interface.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get name of the indexed bus interface.
     *
     *      @param [in] itemIndex   Index of the selected bus interface.
     *
     *      @return Name of the selected bus interface.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available bus interfaces.
     *
     *      @return Number of available bus interfaces.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available bus interfaces.
     *
     *      @return Names of the available bus interfaces.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected bus interface.
     *
     *      @param [in] currentName     Name of the selected bus interface.
     *      @param [in] newName         New name for the bus interface.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Get the description of the selected bus interface.
     *
     *      @param [in] itemName    Name of the selected bus interface.
     *
     *      @return Description of the selected bus interface.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Set a new description for the selected bus interface.
     *
     *      @param [in] itemName        Name of the selected bus interface.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Validates the contained bus interfaces.
     *
     *      @return True, if all the bus interfaces are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Validate the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if the bus interface is valid, false otherwise.
     */
    bool validateBusInterface(std::string const& busName) const;

    /*!
     *  Check if the selected bus interface has a valid name.
     *
     *      @param [in] itemName    Name of the selected bus interface.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Check if the selected bus interface has a valid bus type.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if the bus type is valid, false otherwise.
     */
    bool hasValidBusType(std::string const& busName) const;

    /*!
     *  Check if the selected bus interface has valid abstraction types.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if the abstraction types is valid, false otherwise.
     */
    bool hasValidAbstractionTypes(std::string const& busName) const;

    /*!
     *  Get the interface mode of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Interface mode of the selected bus interface.
     */
    General::InterfaceMode getMode(std::string const& busName) const;

    /*!
     *  Get the interface mode of the selected bus interface in string form.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Interface mode string of the selected bus interface.
     */
    std::string getModeString(std::string const& busName) const;

    /*!
     *  Set the interface mode for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newMode     The new interface mode.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMode(std::string const& busName, std::string const& newMode);

    /*!
     *  Get the system group of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return System group of the selected bus interface.
     */
    std::string getSystemGroup(std::string const& busName) const;

    /*!
     *  Set the system group for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newSystem   The new system group.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setSystemGroup(std::string const& busName, std::string const& newSystem);

    /*!
     *  Get the connection required value of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return The connection required value of the selected bus interface.
     */
    std::string connectionIsRequired(std::string const& busName) const;

    /*!
     *  Set the connection required value for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newValue    New connection required value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setConnectionIsRequired(std::string const& busName, bool newValue);

    /*!
     *  Get the bits in lau value of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Bits in lau value of the selected bus interface.
     */
    std::string getBitsInLau(std::string const& busName) const;

    /*!
     *  Set the bits in value for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newBits     New bits in lau value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBitsInLau(std::string const& busName, std::string const& newBits);

    /*!
     *  Get the endianness of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return BIG, LITTLE or UNSPECIFIED.
     */
    BusInterface::Endianness getEndianness(std::string const& busName) const;

    /*!
     *  Set the endianness of the selected bus interface.
     *
     *      @param [in] busName         Name of the selected bus interface.
     *      @param [in] newEndianness   New endianness value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setEndianness(std::string const& busName, std::string const& newEndianness);

    /*!
     *  Get the bit steering value of the selected bus interface.
	 *
     *      @param [in] busName     Name of the selected bus interface.
     *
	 *      @return The bit steering of the selected bus interface.
	 */
	BusInterface::BitSteering getBitSteering(std::string const& busName) const;

    /*!
     *  Set the bit steering value for the selected bus interface.
     *
     *      @param [in] busName         Name of the selected bus interface.
     *      @param [in] newBitSteering  New bit steering value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBitSteering(std::string const& busName, std::string const& newBitSteering);

    /*!
     *  Get the address space reference of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Address space reference of the selected bus interface.
     */
    std::string getAddressSpaceReference(std::string const& busName) const;
    
    /*!
     *  Set a new address space reference for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newSpace    New address space reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAddressSpaceReference(std::string const& busName, std::string const& newSpace);

    /*!
     *  Get the calculated base address value of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated base address value of the selected bus interface.
     */
    std::string getBaseAddressValue(std::string const& busName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted base address expression of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Formatted base address expression of the selected bus interface.
     */
    std::string getBaseAddressFormattedExpression(std::string const& busName) const;

    /*!
     *  Get the base address expression of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Base address expression of the selected bus interface.
     */
    std::string getBaseAddressExpression(std::string const& busName) const;

    /*!
     *  Set the base address for the selected bus interface.
     *
     *      @param [in] busName         Name of the selected bus interface.
     *      @param [in] newBaseAddress  New base address.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBaseAddress(std::string const& busName, std::string const& newBaseAddress);

    /*!
     *  Get the memory map reference of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Memory map reference of the selected bus interface.
     */
    std::string getMemoryMapReference(std::string const& busName) const;

    /*!
     *  Set a new memory map reference for the selected bus interface.
     *
     *      @param [in] busName             Name of the selected bus interface.
     *      @param [in] newMapReference     New memory map reference.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMemoryMapReference(std::string const& busName, std::string const& newMapReference);

    /*!
     *  Get the file set references of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return File set references of the selected bus interface.
     */
    std::vector<std::string> getFileSetReferences(std::string const& busName) const;

    /*!
     *  Set the file set references for the selected bus interface.
     *
     *      @param [in] busName                 Name of the selected bus interface.
     *      @param [in] newFileSetReferences    New file set references.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setFileSetReferences(std::string const& busName, std::vector<std::string>  const& newFileSetReferences);

    /*!
     *  Get the bus type of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Bus type of the selected bus interface.
     */
    ConfigurableVLNVReference getBusType(std::string const& busName) const;

    /*!
     *  Get the bus type of the selected bus interface as a string.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Bus type of the selected bus interface in string format.
     */
    std::string getBusTypeString(std::string const& busName) const;

    /*!
     *  Set the bus type of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newVLNV     New bus type VLNV.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBustype(std::string const& busName, ConfigurableVLNVReference const& newVLNV);

    /*!
     *  Set the bus type for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newVendor   Vendor of the new bus type.
     *      @param [in] newLibrary  Library of the new bus type.
     *      @param [in] newName     Name of the new bus type.
     *      @param [in] newVersion  Version of the new bus type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBustype(std::string const& busName, std::string const& newVendor, std::string const& newLibrary,
        std::string const& newName, std::string const& newVersion) const;

    /*!
     *  Get the abstraction reference string of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Abstraction reference if only a single abstraction type exists, otherwise [multiple].
     */
    std::string getAbstractionReferenceString(std::string const& busName) const;

    /*!
     *  Get the list of abstraction references of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return String list of the abstraction references.
     */
    std::vector<std::string> getAbstractionReferences(std::string const& busName) const;

    /*!
     *  Add a new abstraction type to the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newVendor   Vendor of the new abstraction type.
     *      @param [in] newLibrary  Library of the new abstraction type.
     *      @param [in] newName     Name of the new abstraction  type.
     *      @param [in] newVersion  Version of the new abstraction type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addAbstractionType(std::string const& busName, std::string const& newVendor,
        std::string const& newLibrary, std::string const& newName, std::string const& newVersion) const;

    /*!
     *  Remove all the abstraction types from the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if successfull, false otherwise.
     */
    bool removeAbstractionTypes(std::string const& busName);

    /*!
     *  Get the calculated range value of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated range value of the selected bus interface.
     */
    std::string getRangeValue(std::string const& busName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted range expression of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Formatted range expression of the selected bus interface.
     */
    std::string getRangeFormattedExpression(std::string const& busName) const;

    /*!
     *  Get the range expression of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Range expression of the selected bus interface.
     */
    std::string getRangeExpression(std::string const& busName) const;

    /*!
     *  Set the range for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newRange    New range value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRange(std::string const& busName, std::string const& newRange);

    /*!
     *  Get the calculated remap address value of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated remap address value of the selected bus interface.
     */
    std::string getRemapAddressValue(std::string const& busName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted remap address expression of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Formatted remap address expression of the selected bus interface.
     */
    std::string getRemapAddressFormattedExpression(std::string const& busName) const;

    /*!
     *  Get the remap address expression of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Remap address expression of the selected bus interface.
     */
    std::string getRemapAddressExpression(std::string const& busName) const;

    /*!
     *  Set the remap address for the selected bus interface.
     *
     *      @param [in] busName             Name of the selected bus interface.
     *      @param [in] newRemapAddress     New remap address value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRemapAddress(std::string const& busName, std::string const& newRemapAddress);

    /*!
     *  Get the monitor interface mode of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Monitor interface mode of the selected bus interface.
     */
    General::InterfaceMode getMonitorMode(std::string const& busName) const;

    /*!
     *  Check if the selected bus interface is a monitor interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if the bus interface contains monitor interface, false otherwise.
     */
    bool hasMonitorInterface(std::string const& busName) const;

    /*!
     *  Set the monitor interface mode for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newMode     New monitor interface mode.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMonitorMode(std::string const& busName, std::string const& newMode);

    /*!
     *  Get the monitor system group of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Monitor system group of the selected bus interface.
     */
    std::string getMonitorGroup(std::string const& busName) const;

    /*!
     *  Set the monitor system group for the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *      @param [in] newGroup    New monitor system group.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMonitorGroup(std::string const& busName, std::string const& newGroup);

    /*!
     *  Add a new bus interface.
     *
     *      @param [in] newBusIndex     Index of the new bus interface.
     *      @param [in] busName         Name of the new bus interface.
     */
    void addBusInterface(int const& newBusIndex, std::string const& busName = "");

    /*!
     *  Swap the selected bus interfaces.
     *
     *      @param [in] firstIndex      Index of the first bus interface.
     *      @param [in] secondIndex     Index of the second bus interface.
     */
    void swapBusInterfaces(int const& firstIndex, int const& secondIndex);

    /*!
     *  Remove the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeBusInterface(std::string const& busName);

    /*!
     *  Check if the selected bus interface exists.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return True, if the bus interface exists, false otherwise.
     */
    bool busInterfaceExists(std::string const& busName) const;

    /*!
     *  Get the interface for accessing transparent bridges.
     *
     *      @return Interface for accessing transparent bridges.
     */
    TransparentBridgeInterface* getBridgeInterface();

    /*!
     *  Get the interface for accessing file sets.
     *
     *      @return Interface for accessing file sets.
     */
    FileSetInterface* getFileSetInterface() const;

    /*!
     *  Get the interface for accessing memory maps.
     *
     *      @return Interface for accessing memory maps.
     */
    MemoryMapInterface* getMemoryMapInterface() const;

    /*!
     *  Get the interface for accessing abstraction types.
     *
     *      @return Interface for accessing abstraction types.
     */
    AbstractionTypeInterface* getAbstractionTypeInterface() const;

    /*!
     *  Get the validator for bus interfaces.
     *
     *      @return Validator for bus interfaces.
     */
    QSharedPointer<BusInterfaceValidator> getValidator() const;

    /*!
     *  Get the transparent bridges of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Transparent bridges of the selected bus interface.
     */
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > getBridges(std::string const& busName) const;

    /*!
     *  Create or get existing transparent bridges pointer of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Pointer to the transparent bridges of the selected bus interface.
     */
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > createBridges(std::string const& busName);

    /*!
     *  Get all the references made to the selected parameter in the selected bus interface.
     *
     *      @param [in] itemName    Name of the selected bus interface.
     *      @param [in] valueID     ID of the target parameter.
     *
     *      @return Number of references made to the given parameter in the selected bus interface.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
        override;

    /*!
     *  Get all the expressions in the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return List of the expressions in the selected bus interface.
     */
    std::vector<std::string> getAllExpressions(std::string const& busName) const;

    /*!
     *  Copy the selected bus interfaces.
     *
     *      @param [in] selectedRows    Indexes of the selected bus interfaces.
     */
    void copyRows(std::vector<int> selectedRows);

    /*!
     *  Paste the selected bus interfaces.
     *
     *      @return Names of the pasted bus interfaces.
     */
    std::vector<std::string> pasteRows();

    /*!
     *  Get the number of pasted bus interfaces.
     *
     *      @return Number of the pasted bus interfaces.
     */
    int getPasteRowCount() const;

    //! No copying. No assignment.
    BusInterfaceInterface(const BusInterfaceInterface& other) = delete;
    BusInterfaceInterface& operator=(const BusInterfaceInterface& other) = delete;

private:

    /*!
     *  Get the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return The selected bus interface.
     */
    QSharedPointer<BusInterface> getBusInterface(std::string const& busName) const;

    /*!
     *  Get the selected master bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected master bus interface.
     */
    QSharedPointer<MasterInterface> getMasterInterface(std::string const& busName) const;

    /*!
     *  Create or get an existing master for the selected bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected master bus interface.
     */
    QSharedPointer<MasterInterface> createMasterForBusInterface(std::string const& busName);

    /*!
     *  Get the selected slave bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected slave bus interface.
     */
    QSharedPointer<SlaveInterface> getSlaveInterface(std::string const& busName) const;

    /*!
     *  Create or get an existing slave for the selected bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected slave bus interface.
     */
    QSharedPointer<SlaveInterface> createSlaveForBusInterface(std::string const& busName);

    /*!
     *  Get the selected mirrored slave bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected mirrored slave bus interface.
     */
    QSharedPointer<MirroredSlaveInterface> getMirroredSlaveInterface(std::string const& busName) const;

    /*!
     *  Create or get an existing mirrored slave for the selected bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected mirrored slave bus interface.
     */
    QSharedPointer<MirroredSlaveInterface> createMirrorSlaveForBusInterface(std::string const& busName);

    /*!
     *  Get the selected monitor bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected monitor bus interface.
     */
    QSharedPointer<BusInterface::MonitorInterface> getMonitorInterface(std::string const& busName) const;

    /*!
     *  Create or get an existing monitor for the selected bus interface.
     *
     *      @param [in] busName     Name of the containing bus interface.
     *
     *      @return The selected monitor bus interface.
     */
    QSharedPointer<BusInterface::MonitorInterface> createMonitorForBusInterface(std::string const& busName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available bus interfaces.
    QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces_;

    //! Validator for bus interfaces.
    QSharedPointer<BusInterfaceValidator> busValidator_;

    //! Interface for accessing transparent bridges.
    TransparentBridgeInterface* bridgeInterface_;

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;

    //! Interface for accessing memory maps.
    MemoryMapInterface* mapInterface_;

    //! Interface for accessing abstraction types
    AbstractionTypeInterface* abstractionTypeInterface_;

    //! Interface for accessing parameters.
    ParametersInterface* parameterInterface_;
};

#endif // BUSINTERFACEINTERFACE_H
