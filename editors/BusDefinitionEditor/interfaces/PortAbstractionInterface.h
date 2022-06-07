//-----------------------------------------------------------------------------
// File: PortAbstractionInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.12.2020
//
// Description:
// Interface for editing abstraction definition port abstractions.
//-----------------------------------------------------------------------------

#ifndef PORTABSTRACTIONINTERFACE_H
#define PORTABSTRACTIONINTERFACE_H

#include <editors/ComponentEditor/ports/interfaces/MasterPortInterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/generaldeclarations.h>

class PortAbstraction;
class TransactionalPort;
class WirePort;
class Protocol;
class AbstractionDefinition;

//-----------------------------------------------------------------------------
//! Interface for editing abstraction definition port abstractions.
//-----------------------------------------------------------------------------
class PortAbstractionInterface : public MasterPortInterface
{
public:

    /*!
     *  The constructor.
     */
    PortAbstractionInterface();

    /*!
     *  The destructor.
     */
    virtual ~PortAbstractionInterface() = default;

    /*!
     *  Setup the port abstractions.
     *
     *      @param [in] absDef  Abstraction definition containing the port abstractions.
     */
    void setAbsDef(QSharedPointer<AbstractionDefinition const> absDef);

    /*!
     *  Get index of the first signal containing the selected port abstraction.
     *
     *      @param [in] itemName    Name of the selected port abstraction.
     *
     *      @return Index of the selected signal.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get index of the signal containing the selected port abstraction with the given mode.
     *
     *      @param [in] itemName    Name of the selected port abstraction.
     *      @param [in] mode        Mode of the selected port abstraction.
     *
     *      @return Index of the selected signal.
     */
    virtual int getItemIndex(std::string const& itemName, General::InterfaceMode mode,
        std::string const& systemGroup = std::string()) const;

    /*!
     *  Get name of the indexed port.
     *
     *      @param [in] itemIndex   Index of the selected port.
     *
     *      @return Name of the selected port.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available port abstractions.
     *
     *      @return Number of available port abstractions.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the number of available signals in the selected port abstraction.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *
     *      @return Number of available signals in the selected port abstraction.
     */
    int signalCount(std::string const& portName) const;

    /*!
     *  Check if the selected port abstraction has multiples of the selected interface mode.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *      @param [in] mode        The selected interface mode.
     *
     *      @return True, if the selected port abstraction has multiples of the selected mode, false if the
     *              selected port abstraction has only a single signal of the selected mode or the interface
     *              mode is system.
     */
    bool portHasMultiplesOfMasterOrSlave(std::string const& portName, std::string const& mode);

    /*!
     *  Set a new name for the selected port abstraction.
     *
     *      @param [in] currentName     Name of the selected port abstraction.
     *      @param [in] newName         New name for the port abstraction.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) override final;

    /*!
     *  Set a new description for the selected port abstraction.
     *
     *      @param [in] portName        Name of the selected port abstraction.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) override final;

    /*!
     *  Validates the contained port abstractions.
     *
     *      @return True, if all the port abstractions are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected port abstraction has a valid name.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const override final;

    /*!
     *  Get the description of the selected port abstraction.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *
     *      @return Description of the selected port abstraction.
     */
    virtual std::string getDescription(std::string const& itemName) const override final;

    /*!
     *  Get the names of the available port abstractions.
     *
     *      @return Names of the available ports abstractions.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Get the list of logical ports with the selected bus interface mode and system group.
     *
     *      @param [in] busModeString   The selected bus interface mode in string form.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return A list of logical ports names.
     */
    std::vector<std::string> getItemNamesWithModeAndGroup(std::string const& busModeString,
        std::string systemGroup) const;

    /*!
     *  Get the interface mode of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Interface mode of the selected signal.
     */
    General::InterfaceMode getMode(int const& portIndex) const;

    /*!
     *  Get the interface mode of the selected signal in string form.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Interface mode string of the selected signal.
     */
    std::string getModeString(int const& portIndex) const;

    /*!
     *  Set the interface mode for the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *      @param [in] newMode     The new interface mode.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMode(int const& portIndex, std::string const& newMode);

    /*!
     *  Get the system group of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return System group of the selected signal.
     */
    std::string getSystemGroup(int const& portIndex) const;

    /*!
     *  Get the system groups of the selected port abstraction.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *
     *      @return System groups of the selected port abstraction.
     */
    std::vector<std::string> getSystemGroupsForPort(std::string const& portName) const;

    /*!
     *  Set the system group for the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *      @param [in] newSystem   The new system group.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setSystemGroup(int const& portIndex, std::string const& newSystem);

    /*!
     *  Get the qualifier of the selected signal in string form.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Qualifier string of the selected signal.
     */
    std::string getQualifierString(int const& portIndex) const;

    /*!
     *  Set the qualifier for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newQualifier    The new qualifier.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setQualifier(int const& portIndex, std::string const& newQualifier);

    /*!
     *  Get the direction of the selected signal in string form.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Direction string of the selected signal.
     */
    std::string getDirectionString(int const& portIndex) const;

    /*!
     *  Get the direction of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Direction of the selected signal.
     */
    DirectionTypes::Direction getDirection(int const& portIndex) const;

    /*!
     *  Set a new direction for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newDirection    The new direction.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDirection(int const& portIndex, std::string const& newDirection);

    /*!
     *  Get the width of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Width of the selected signal.
     */
    std::string getWidth(int const& portIndex) const;

    /*!
     *  Get the width of the selected signal.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return Width of the selected signal.
     */
    std::string getWidth(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new width for the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *      @param [in] newWidth    The new width.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWidth(int const& portIndex, std::string const& newWidth);

    /*!
     *  Get the presence of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Presence of the selected signal.
     */
    PresenceTypes::Presence getPresence(int const& portIndex) const;

    /*!
     *  Get the presence of the selected signal.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return Presence of the selected signal.
     */
    PresenceTypes::Presence getPresence(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Get the presence of the selected signal in string form.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Presence string of the selected signal.
     */
    std::string getPresenceString(int const& portIndex) const;

    /*!
     *  Get the presence of the selected signal in string form.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return Presence string of the selected signal.
     */
    std::string getPresenceString(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new presence for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newPresence     The new presence.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPresence(int const& portIndex, std::string const& newPresence);

    /*!
     *  Get the default value of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Default value of the selected signal.
     */
    std::string getDefaultValue(int const& portIndex) const;
    
    /*!
     *  Set a new default value for the selected signal.
     *
     *      @param [in] portIndex           Index of the selected signal.
     *      @param [in] newDefaultValue     The new default value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDefaultValue(int const& portIndex, std::string const& newDefaultValue);

    /*!
     *  Get the driver of the selected signal in string form.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Driver string of the selected signal.
     */
    std::string getDriverString(int const& portIndex) const;

    /*!
     *  Set a new driver type for the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *      @param [in] newDriver   The new driver type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDriverType(int const& portIndex, std::string const& newDriver);

    /*!
     *  Get the bus width of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Bus width of the selected signal.
     */
    std::string getBusWidthValue(int const& portIndex) const;

    /*!
     *  Get the bus width of the selected signal.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return Bus width of the selected signal.
     */
    std::string getBusWidthValue(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new bus width for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newBusWidth     The new bus width.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBusWidth(int const& portIndex, std::string const& newBusWidth);

    /*!
     *  Get the initiative of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Initiative of the selected signal.
     */
    std::string getInitiative(int const& portIndex) const;

    /*!
     *  Get the initiative of the selected signal.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return Initiative of the selected signal.
     */
    std::string getInitiative(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new initiative for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newInitiative   The new initiative.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setInitiative(int const& portIndex, std::string const& newInitiative);

    /*!
     *  Get the kind of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Kind of the selected signal.
     */
    std::string getKind(int const& portIndex) const;

    /*!
     *  Set a new kind for the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *      @param [in] newKind     The new kind.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setKind(int const& portIndex, std::string const& newKind);

    /*!
     *  Check if the selected signal contains protocol.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return True, if the selected signal has protocol, false otherwise.
     */
    bool hasProtocol(int const& portIndex) const;

    /*!
     *  Get the protocol type of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Protocol type of the selected signal.
     */
    std::string getProtocolType(int const& portIndex) const;

    /*!
     *  Set a new protocol type for the selected signal.
     *
     *      @param [in] portIndex           Index of the selected signal.
     *      @param [in] newProtocolType     The new protocol type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setProtocolType(int const& portIndex, std::string const& newProtocolType);

    /*!
     *  Get the payload name of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Payload name of the selected signal.
     */
    std::string getPayloadName(int const& portIndex);

    /*!
     *  Set a new payload name for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newPayloadName  The new payload name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPayloadName(int const& portIndex, std::string const& newPayloadName);

    /*!
     *  Get the payload type of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Payload type of the selected signal.
     */
    std::string getPayloadType(int const& portIndex);

    /*!
     *  Set a new payload type for the selected signal.
     *
     *      @param [in] portIndex       Index of the selected signal.
     *      @param [in] newPayloadType  The new payload type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPayloadType(int const& portIndex, std::string const& newPayloadType);

    /*!
     *  Get the payload extension of the selected signal.
     *
     *      @param [in] portIndex   Index of the selected signal.
     *
     *      @return Payload extension of the selected signal.
     */
    std::string getPayloadExtension(int const& portIndex);

    /*!
     *  Set a new payload extension for the selected signal.
     *
     *      @param [in] portIndex               Index of the selected signal.
     *      @param [in] newPayloadExtension     The new payload extension.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPayloadExtension(int const& portIndex, std::string const& newPayloadExtension);

    /*!
     *  Add a port abstraction containing wire.
     *
     *      @param [in] newPortName     Name of the new port.
     */
    virtual void addWirePort(std::string const& newPortName = std::string("")) override final;

    /*!
     *  Add a port abstraction containing transactional.
     *
     *      @param [in] newPortName     Name of the new port.
     */
    virtual void addTransactionalPort(std::string const& newPortName = std::string("")) override final;
    
    /*!
     *  Add a wire signal with the selected interface mode to the selected port abstraction.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *      @param [in] newMode     The selected interface mode.
     */
    void addModeSpecificWireSignal(std::string const& portName, General::InterfaceMode const& newMode);

    /*!
     *  Add a new wire signal with the selected system group.
     *
     *      @param [in] portName        Name of port abstraction containing the signal.
     *      @param [in] systemGroup     The new system group.
     */
    void addWireSystemSignal(std::string const& portName, std::string const& systemGroup);

    /*!
     *  Add a transactional signal with the selected interface mode to the selected port abstraction.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *      @param [in] newMode     The selected interface mode.
     */
    void addModeSpecificTransactionalSignal(std::string const& portName, General::InterfaceMode const& newMode);

    /*!
     *  Add a new transactional signal with the selected system group.
     *
     *      @param [in] portName        Name of port abstraction containing the signal.
     *      @param [in] systemGroup     The new system group.
     */
    void addTransactionalSystemSignal(std::string const& portName, std::string const& systemGroup);

    /*!
     *  Remove the selected signal.
     *
     *      @param [in] signalIndex     Index of the selected signal.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeSignal(int const& signalIndex);

    /*!
     *  Check if the selected port abstraction contains a wire.
     *
     *      @param [in] portName    Name of the selected port abstraction.
     *
     *      @return True, if the selected port abstraction contains a wire, false otherwise.
     */
    virtual bool portIsWire(std::string const& portName) const override final;
    
    /*!
     *  Check if the selected port abstraction contains a transactional.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port abstraction contains a transactional, false otherwise.
     */
    virtual bool portIsTransactional(std::string const& portName) const override final;
    
    /*!
     *  Get the icon path of the selected signal.
     *
     *      @param [in] signalIndex     Index of the selected signal.
     *
     *      @return Icon path for the selected signal.
     */
    std::string getIconPathForSignal(int const& signalIndex) const;

    /*!
     *  Get the icon path of the selected signal.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return Icon path for the selected signal.
     */
    std::string getIconPathForSignal(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Write the ports from the table to the abstraction definition.
     */
    void save();

    /*!
     *  Check if the selected port has the selected bus interface mode and system group.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] interfaceMode   The selected bus interface mode.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return True, if the port has the selected bus interface mode with the selected system group, false
     *              otherwise.
     */
    bool portHasMode(std::string const& portName, std::string const& interfaceMode, std::string const& systemGroup)
        const;

    /*!
     *  Get the port abstraction with the selected name.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return The port abstraction with the selected name.
     */
    QSharedPointer<PortAbstraction> getPort(std::string const& portName) const;

    //! No copying. No assignment.
    PortAbstractionInterface(const PortAbstractionInterface& other) = delete;
    PortAbstractionInterface& operator=(const PortAbstractionInterface& other) = delete;

private:

    //! SignalRow represents a single row in the table by grouping the port abstraction and transactional elements.
    struct SignalRow
    {
        //! Defines the Port represented in the row.
        QSharedPointer<PortAbstraction> abstraction_;

        //! Defines the mode of the signal (master, slave or system).
        General::InterfaceMode mode_;

        //! Defines the wirePort represented on the row.
        QSharedPointer<WirePort> wire_;

        //! Defines the transactionalPort represented on the row.
        QSharedPointer<TransactionalPort> transactional_;

        /*!
         *  The default constructor.
         *
         *      @param [in] isWirePort              Flag for wire ports.
         *      @param [in] isTransactionalPort     Flag for transactional ports.
         */
        SignalRow(bool isWirePort, bool isTransactionalPort);

        /*!
         *  Copy constructor
         */
        SignalRow(SignalRow const& other) = default;

        /*!
         *  Comparison of two SignalRows.
         *
         *      @param [in] other   The SignalRow to compare this to.
         *
         *      @return True, if the SignalRows are equal, otherwise false.
         */
        bool operator==(SignalRow const& other) const;

        /*!
         *  Comparison of two SignalRows.
         *
         *      @param [in] other   The SignalRow to compare this to.
         *
         *      @return True, if the SignalRows are not equal, otherwise false.
         */
        bool operator!=(SignalRow const& other) const;

        /*!
         *  Less than comparison of two SignalRows for table ordering.
         *
         *      @param [in] other   The SignalRow to compare this to.
         *
         *      @return True, if this precedes other, otherwise false.
         */
        bool operator<(SignalRow const& other) const;
    };
 
    /*!
     *  Get the signal of the selected index.
     *
     *      @param [in] signalIndex     The selected index.
     *
     *      @return The selected signal.
     */
    QSharedPointer<SignalRow> getSignal(int const& signalIndex) const;

    /*!
     *  Get or create a transactional protocol for the selected signal.
     *
     *      @param [in] signalIndex     Index of the selected signal.
     *
     *      @return Protocol of the selected signal.
     */
    QSharedPointer<Protocol> getOrCreateProtocol(int const& signalIndex) const;

    /*!
     *  Check if the selected protocol is empty and remove it if it is.
     *
     *      @param [in] signalIndex     Index of the containing signal.
     *      @param [in] signalProtocol  The selected protocol.
     */
    void removeEmptyProtocol(int const& signalIndex, QSharedPointer<Protocol> signalProtocol);

    /*!
     *  Check if the protocol type of the selected protocol is empty.
     *
     *      @param [in] portProtocol    The selected protocol.
     *
     *      @return True, if the port protocol is empty, false otherwise.
     */
    bool portProcotolTypeIsEmpty(QSharedPointer<Protocol> portProtocol) const;

    /*!
     *  Get the protocol type of the selected protocol.
     *
     *      @param [in] portProtocol    The selected protocol.
     *
     *      @return The type of the selected protocol.
     */
    QString getProtocolTypeText(QSharedPointer<Protocol> portProtocol) const;

    /*!
     *  Convert qualifier to string.
     *
     *      @param [in]  qualifier The Qualifier to convert.
     *
     *      @return A string representation for the Qualifier.
     */
    QString qualifierToString(Qualifier const& qualifier) const;
    
    /*!
     *  Convert a string to PortQualifier.
	 *
	 *      @param [in] str The string to convert.
	 *
	 *      @return A qualifier that matches the string.
     */
	Qualifier::Type stringToQualifier(std::string const& str) const;

    /*!
     *  Create a wire signal.
     *
     *      @param [in] portAbs         Port abstraction containing the signal.
     *      @param [in] modeSpesific    Wire port data.
     *      @param [in] mode            The interface mode.
     */
    void createWireSignal(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
        General::InterfaceMode mode);

    /*!
     *  Create a transactional signal.
     *
     *      @param [in] portAbs         Port abstraction containing the signal.
     *      @param [in] modeSpesific    Transactional port data.
     *      @param [in] mode            The interface mode.
     */
    void createTransactionalSignal(QSharedPointer<PortAbstraction> portAbs,
        QSharedPointer<TransactionalPort> modeSpesific, General::InterfaceMode mode);

    /*!
     *  Write the selected signal to the selected port abstraction.
     *
     *      @param [in] portAbs     The selected port abstraction.
     *      @param [in] i           Index of the selected signal.
     */
    void savePort(QSharedPointer<PortAbstraction> portAbs, int i);

    /*!
     *  Check if the selected port already contains the selected signal.
     *
     *      @param [in] mode            The selected signal.
     *      @param [in] portName        Name of the selected port.
     *      @param [in] systemGroup     The selected system group.
     *
     *      @return True, if the selected port already contains the selected signal, false otherwise.
     */
    bool modeExistsForPort(General::InterfaceMode const& mode, QString const& portName,
        QString const& systemGroup = "") const;

    /*!
     *  Construct a copy signal of the selected signal.
     *
     *      @param [in] signal  The selected port signal.
     *
     *      @return The constructed copy of the signal.
     */
     QSharedPointer<PortAbstractionInterface::SignalRow> constructCopySignal(QSharedPointer<PortAbstraction> port,
         bool isWire, bool isTransactional) const;
    
    /*!
     *  Get the mirrored direction for the selected signal.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] opposingMode    The opposite interface mode of the selected signal.
     *
     *      @return The mirrored direction for the selected signal.
     */
    DirectionTypes::Direction getMirroredDirectionForSignal(QString const& portName,
        General::InterfaceMode const& opposingMode) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! Pointer to the available port abstractions.
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports_;

    //! List of signals.
    QList<QSharedPointer<SignalRow> > signals_;
};

#endif // PORTABSTRACTIONINTERFACE_H
