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

#include <MasterPortInterface.h>
#include <KactusAPI/include/ParameterizableInterface.h>

#include "KactusAPI/KactusAPIGlobal.h"

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/Qualifier.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/generaldeclarations.h>

class PortAbstraction;
class TransactionalPort;
class WirePort;
class Protocol;
class AbstractionDefinition;
class PortAbstractionValidator;
class ExpressionParser;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Interface for editing abstraction definition port abstractions.
//-----------------------------------------------------------------------------
class KACTUS2_API PortAbstractionInterface : public MasterPortInterface, public ParameterizableInterface
{
public:

    /*!
     *	The constructor.
     *  
     *    @param [in] expressionParser        The expression parser to use.
     *    @param [in] expressionFormatter     The expression formatter to use.
     */
    PortAbstractionInterface(QSharedPointer<ExpressionParser> expressionParser, 
        QSharedPointer<ExpressionFormatter> expressionFormatter);

    /*!
     *  The destructor.
     */
    virtual ~PortAbstractionInterface() = default;

    /*!
     *  Setup the port abstractions.
     *
     *    @param [in] absDef  Abstraction definition containing the port abstractions.
     */
    void setAbsDef(QSharedPointer<AbstractionDefinition const> absDef);

    /*!
     *	Set the validator to use.
     *  
     *    @param [in] validator     The validator to use.
     */
    void setPortAbstractionValidator(QSharedPointer<PortAbstractionValidator> validator);

    /*!
     *  Get index of the first signal containing the selected port abstraction.
     *
     *    @param [in] itemName    Name of the selected port abstraction.
     *
     *    @return Index of the selected signal.
     */
    virtual int getItemIndex(std::string const& itemName) const override final;

    /*!
     *  Get index of the signal containing the selected port abstraction with the given mode.
     *
     *    @param [in] itemName    Name of the selected port abstraction.
     *    @param [in] mode        Mode of the selected port abstraction.
     *
     *    @return Index of the selected signal.
     */
    virtual int getItemIndex(std::string const& itemName, General::InterfaceMode mode,
        std::string const& systemGroup = std::string()) const;

    /*!
     *  Get name of the indexed port.
     *
     *    @param [in] itemIndex   Index of the selected port.
     *
     *    @return Name of the selected port.
     */
    std::string getIndexedItemName(int itemIndex) const final;

    /*!
     *  Get the number of available port abstractions.
     *
     *    @return Number of available port abstractions.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the number of available signals in the selected port abstraction.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *
     *    @return Number of available signals in the selected port abstraction.
     */
    int signalCount(std::string const& portName) const;

    /*!
     *  Check if the selected port abstraction has multiples of the selected interface mode.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *    @param [in] mode        The selected interface mode.
     *
     *    @return True, if the selected port abstraction has multiples of the selected mode, false if the
     *              selected port abstraction has only a single signal of the selected mode or the interface
     *              mode is system.
     */
    bool portHasMultiplesOfMasterOrSlave(std::string const& portName, std::string const& mode);

    /*!
     *  Set a new name for the selected port abstraction.
     *
     *    @param [in] currentName     Name of the selected port abstraction.
     *    @param [in] newName         New name for the port abstraction.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setName(std::string const& currentName, std::string const& newName) final;

    /*!
     *  Validates the contained port abstractions.
     *
     *    @return True, if all the port abstractions are valid, false otherwise.
     */
    bool validateItems() const final;

    /*!
     *  Check if the selected port abstraction has a valid name.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *
     *    @return True, if the name is valid, false otherwise.
     */
    bool itemHasValidName(std::string const& itemName) const final;

    /*!
     *	Check if the width of the wire port is valid.
     *  
     *    @param [in] portIndex     The index of the selected signal.
     *	    
     * 	    @return True, if the widht is valid, otherwise false.
     */
    bool wireHasValidWidth(int portIndex) const;

    /*!
     *	Check if the default value of the wire port is valid.
     *  
     *    @param [in] portIndex     The index of the selected signal.
     *	    
     * 	    @return True, if the default value is valid, otherwise false.
     */
    bool wireHasValidDefaultValue(int portIndex) const;

    /*!
     *	Get the match status of the selected signal.
     *  
     *    @param [in] portIndex	Index of the selected signal.
     *		
     * 		@return True, if signal port has match, otherwise false.
     */
    bool getMatch(int const& portIndex) const;

    /*!
     *	Set the match status of the selected signal.
     *
     *    @param [in] portIndex	Index of the selected signal.
     *    @param [in] match       Match flag to set.
     *
     * 		@return True, if match could be set, otherwise false.
     */
    bool setMatch(int const& portIndex, bool match) const;

    /*!
     *  Get the names of the available port abstractions.
     *
     *    @return Names of the available ports abstractions.
     */
    std::vector<std::string> getItemNames() const final;

    /*!
     *  Get the list of logical ports with the selected bus interface mode and system group.
     *
     *    @param [in] busModeString   The selected bus interface mode in string form.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return A list of logical ports names.
     */
    std::vector<std::string> getItemNamesWithModeAndGroup(std::string const& busModeString,
        std::string systemGroup) const;

    /*!
     *  Get the interface mode of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Interface mode of the selected signal.
     */
    General::InterfaceMode getMode(int const& portIndex) const;

    /*!
     *  Get the interface mode of the selected signal in string form.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Interface mode string of the selected signal.
     */
    std::string getModeString(int const& portIndex) const;

    /*!
     *  Set the interface mode for the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] newMode     The new interface mode.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setMode(int const& portIndex, std::string const& newMode);

    /*!
     *  Get the system group of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return System group of the selected signal.
     */
    std::string getSystemGroup(int const& portIndex) const;

    /*!
     *  Get the system groups of the selected port abstraction.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *
     *    @return System groups of the selected port abstraction.
     */
    std::vector<std::string> getSystemGroupsForPort(std::string const& portName) const;

    /*!
     *  Set the system group for the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] newSystem   The new system group.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setSystemGroup(int const& portIndex, std::string const& newSystem);

    /*!
     *  Get the list of qualifiers of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return List of qualifiers of the selected signal.
     */
    std::vector<std::string> getQualifierStringList(int const& portIndex) const;

    /*!
     *  Set the qualifier for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newQualifier    The new qualifier.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setQualifier(int const& portIndex, std::string const& newQualifier);

    /*!
     *  Set a new list of qualifiers for the selected signal.
     *
     *    @param [in] portIndex           Index of the selected signal.
     *    @param [in] newQualifierList    The new list of qualifiers.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setQualifierStringList(int const& portIndex, std::vector<std::string> const& newQualifierList);

    /*!
     *	Get a chosen attribute of a qualifier.
     *  
     *    @param [in] portIndex	            Index of the selected signal.
     *    @param [in] attributeName	        The name of the attribute to get.
     *    @param [in] qualifierTypeString	    The qualifier type.
     *		
     * 		@return The chosen attribute.
     */
    std::string getQualifierAttribute(int const& portIndex, std::string const& attributeName) const;

    /*!
     *	Set a qualifier attribute for a specific qualifier type.
     *  
     *    @param [in] portIndex	            Index of the selected signal.
     *    @param [in] attributeName	        The name of the attribute to set.
     *    @param [in] attributeValue	        The value of the attribute to set.
     *    @param [in] qualifierTypeString	    The qualifier type.
     *		
     * 		@return True, if attribute was successfully set, otherwise false.
     */
    bool setQualifierAttribute(int const& portIndex, std::string const& attributeName, 
        std::string const& attributeValue) const;

    /*!
     *	Set all qualifier attributes at once.
     *  
     *    @param [in] portIndex	Index of the selected signal.
     *    @param [in] attributes	Map of attributes to set as name-value pairs.
     *		
     * 		@return True, if attributes were set, otherwise false.
     */
    bool setQualifierAttributes(int const& portIndex, std::vector<std::string> const& attributes) const;

    /*!
     *	Get all of the qualifier's attributes.
     *  
     *    @param [in] portIndex	Index of the selected signal.
     *		
     * 		@return A map of attributes as name-value pairs.
     */
    std::vector<std::string> getQualifierAttributes(int const& portIndex) const;

    /*!
     *  Get the direction of the selected signal in string form.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Direction string of the selected signal.
     */
    std::string getDirectionString(int const& portIndex) const;

    /*!
     *  Get the direction of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Direction of the selected signal.
     */
    DirectionTypes::Direction getDirection(int const& portIndex) const;

    /*!
     *  Get the direction of the selected signal.
     *
     *    @param [in] portName        The selected signal name.
     *    @param [in] mode            The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Direction of the selected signal.
     */
    DirectionTypes::Direction getDirection(std::string const& portName, General::InterfaceMode mode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new direction for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newDirection    The new direction.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setDirection(int const& portIndex, std::string const& newDirection);

    /*!
     *  Get the width expression of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Width of the selected signal.
     */
    std::string getWidthExpression(int const& portIndex) const;

    /*!
     *  Get the width expression of the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode in string form.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Width of the selected signal.
     */
    std::string getWidthExpression(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Get the width expression of the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Width of the selected signal.
     */
    std::string getWidthExpression(std::string const& portName, General::InterfaceMode interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *	Get the formatted expression for the width of the selected wire port.
     *  
     *    @param [in] portIndex     Index of the selected signal.
     *	    
     *    @return The formatted expression of the width.
     */
    std::string getWidthFormattedExpression(int portIndex) const;

    /*!
     *	Get the width value of the selected signal.
     *  
     *    @param [in] portIndex      Index of the selected signal.
     *    @param [in] baseNumber     Base number to format the value to.
     *	    
     *    @return The evaluated value of the wire width.
     */
    std::string getWidthValue(int portIndex, int const& baseNumber = 0) const;

    /*!
     *	Get the width value of the selected signal.
     *  
     *    @param [in] portName          Name of the selected port.
     *    @param [in] interfaceMode     The interface mode of the port.
     *    @param [in] systemGroup       The port system group.
     *    @param [in] baseNumber        Base number to format the value to.
     *
     *    @return The evaluated value of the wire width.
     */
    std::string getWidthValue(std::string const& portName,
        General::InterfaceMode interfaceMode,
        std::string const& systemGroup,
        int const& baseNumber = 0) const;

    /*!
     *  Set a new width for the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] newWidth    The new width.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setWidth(int const& portIndex, std::string const& newWidth);

    /*!
     *  Get the presence of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Presence of the selected signal.
     */
    PresenceTypes::Presence getPresence(int const& portIndex) const;

    /*!
     *  Get the presence of the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Presence of the selected signal.
     */
    PresenceTypes::Presence getPresence(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Get the presence of the selected signal in string form.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Presence string of the selected signal.
     */
    std::string getPresenceString(int const& portIndex) const;

    /*!
     *  Get the presence of the selected signal in string form.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Presence string of the selected signal.
     */
    std::string getPresenceString(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new presence for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newPresence     The new presence.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPresence(int const& portIndex, std::string const& newPresence);

    /*!
     *  Get the default value expression of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Default value of the selected signal.
     */
    std::string getDefaultValueExpression(int const& portIndex) const;
    
    /*!
     *  Get the formatted expression default value of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Default value of the selected signal.
     */
    std::string getDefaultValueFormattedExpression(int const& portIndex) const;

    /*!
     *  Get the evaluated default value of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] baseNumber  The base to format the value to.
     *
     *    @return Default value of the selected signal.
     */
    std::string getDefaultValueValue(int const& portIndex, int baseNumber = 0) const;

    /*!
     *  Set a new default value for the selected signal.
     *
     *    @param [in] portIndex           Index of the selected signal.
     *    @param [in] newDefaultValue     The new default value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setDefaultValue(int const& portIndex, std::string const& newDefaultValue);

    /*!
     *  Get the driver of the selected signal in string form.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Driver string of the selected signal.
     */
    std::string getDriverString(int const& portIndex) const;

    /*!
     *  Set a new driver type for the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] newDriver   The new driver type.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setDriverType(int const& portIndex, std::string const& newDriver);

    /*!
     *  Get the bus width expression of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Bus width of the selected signal.
     */
    std::string getBusWidthExpression(int const& portIndex) const;

    /*!
     *  Get the bus width expression of the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Bus width of the selected signal.
     */
    std::string getBusWidthExpression(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Get the formatted expression of the bus width of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Bus width of the selected signal.
     */
    std::string getBusWidthFormattedExpression(int signalIndex) const;
    
    /*!
     *  Get the evaluated value of the bus width of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] baseNumber  The base to format the value to.
     *
     *    @return Bus width of the selected signal.
     */
    std::string getBusWidthValue(int portIndex, int baseNumber = 0) const;

    /*!
     *  Set a new bus width for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newBusWidth     The new bus width.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setBusWidth(int const& portIndex, std::string const& newBusWidth);

    bool transactionalHasValidBusWidth(int portIndex) const;

    /*!
     *  Get the initiative of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Initiative of the selected signal.
     */
    std::string getInitiative(int const& portIndex) const;

    /*!
     *  Get the initiative of the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Initiative of the selected signal.
     */
    std::string getInitiative(std::string const& portName, std::string const& interfaceMode,
        std::string const& systemGroup) const;

    /*!
     *  Set a new initiative for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newInitiative   The new initiative.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setInitiative(int const& portIndex, std::string const& newInitiative);

    /*!
     *  Get the kind of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Kind of the selected signal.
     */
    std::string getKind(int const& portIndex) const;

    /*!
     *  Set a new kind for the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *    @param [in] newKind     The new kind.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setKind(int const& portIndex, std::string const& newKind);

    /*!
     *  Check if the selected signal contains protocol.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return True, if the selected signal has protocol, false otherwise.
     */
    bool hasProtocol(int const& portIndex) const;

    /*!
     *  Get the protocol type of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Protocol type of the selected signal.
     */
    std::string getProtocolType(int const& portIndex) const;

    /*!
     *  Set a new protocol type for the selected signal.
     *
     *    @param [in] portIndex           Index of the selected signal.
     *    @param [in] newProtocolType     The new protocol type.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setProtocolType(int const& portIndex, std::string const& newProtocolType);

    /*!
     *  Get the payload name of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Payload name of the selected signal.
     */
    std::string getPayloadName(int const& portIndex);

    /*!
     *  Set a new payload name for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newPayloadName  The new payload name.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPayloadName(int const& portIndex, std::string const& newPayloadName);

    /*!
     *  Get the payload type of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Payload type of the selected signal.
     */
    std::string getPayloadType(int const& portIndex);

    /*!
     *  Set a new payload type for the selected signal.
     *
     *    @param [in] portIndex       Index of the selected signal.
     *    @param [in] newPayloadType  The new payload type.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPayloadType(int const& portIndex, std::string const& newPayloadType);

    /*!
     *  Get the payload extension of the selected signal.
     *
     *    @param [in] portIndex   Index of the selected signal.
     *
     *    @return Payload extension of the selected signal.
     */
    std::string getPayloadExtension(int const& portIndex);

    /*!
     *  Set a new payload extension for the selected signal.
     *
     *    @param [in] portIndex               Index of the selected signal.
     *    @param [in] newPayloadExtension     The new payload extension.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPayloadExtension(int const& portIndex, std::string const& newPayloadExtension);

    /*!
     *  Add a port abstraction containing wire.
     *
     *    @param [in] newPortName     Name of the new port.
     */
    void addWirePort(std::string const& newPortName = std::string("")) final;

    /*!
     *  Add a port abstraction containing transactional.
     *
     *    @param [in] newPortName     Name of the new port.
     */
    void addTransactionalPort(std::string const& newPortName = std::string("")) final;

    /*!
     *  Add a wire signal with the selected interface mode to the selected port abstraction.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *    @param [in] newMode     The selected interface mode.
     */
    void addModeSpecificWireSignal(std::string const& portName, General::InterfaceMode const& newMode);

    /*!
     *  Add a new wire signal with the selected system group.
     *
     *    @param [in] portName        Name of port abstraction containing the signal.
     *    @param [in] systemGroup     The new system group.
     */
    void addWireSystemSignal(std::string const& portName, std::string const& systemGroup);

    /*!
     *  Add a transactional signal with the selected interface mode to the selected port abstraction.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *    @param [in] newMode     The selected interface mode.
     */
    void addModeSpecificTransactionalSignal(std::string const& portName, General::InterfaceMode const& newMode);

    /*!
     *  Add a new transactional signal with the selected system group.
     *
     *    @param [in] portName        Name of port abstraction containing the signal.
     *    @param [in] systemGroup     The new system group.
     */
    void addTransactionalSystemSignal(std::string const& portName, std::string const& systemGroup);

    /*!
     *  Remove the selected signal.
     *
     *    @param [in] signalIndex     Index of the selected signal.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeSignal(int const& signalIndex);

    /*!
     *  Check if the selected port abstraction contains a wire.
     *
     *    @param [in] portName    Name of the selected port abstraction.
     *
     *    @return True, if the selected port abstraction contains a wire, false otherwise.
     */
    bool portIsWire(std::string const& portName) const final;
    
    /*!
     *  Check if the selected port abstraction contains a transactional.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the selected port abstraction contains a transactional, false otherwise.
     */
    bool portIsTransactional(std::string const& portName) const final;
    
    /*!
     *  Get the icon path of the selected signal.
     *
     *    @param [in] signalIndex     Index of the selected signal.
     *
     *    @return Icon path for the selected signal.
     */
    std::string getIconPathForSignal(int const& signalIndex) const;

    /*!
     *  Get the icon path of the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return Icon path for the selected signal.
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
     *    @param [in] portName        Name of the selected port.
     *    @param [in] interfaceMode   The selected bus interface mode.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return True, if the port has the selected bus interface mode with the selected system group, false
     *              otherwise.
     */
    bool portHasMode(std::string const& portName, std::string const& interfaceMode, std::string const& systemGroup)
        const;

    /*!
     *  Get the port abstraction with the selected name.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return The port abstraction with the selected name.
     */
    QSharedPointer<PortAbstraction> getPort(std::string const& portName) const;

    /*!
     *	Get the standard revision of the abstraction definition.
     *  
     * 		@return The standard revision used.
     */
    Document::Revision getRevision() const;

    
    /*!
     *	Get the number of references made to itemID on a given signal row.
     *  
     *    @param [in] signalIndex     The index of the signal row.
     *    @param [in] itemID          The parameter itemID to look for.
     *	    
     * 	    @return Return the number of references made to itemID.
     */
    int getAllReferencesToIdInRow(int signalIndex, std::string const& itemID) const;

    //! No copying. No assignment.
    PortAbstractionInterface(const PortAbstractionInterface& other) = delete;
    PortAbstractionInterface& operator=(const PortAbstractionInterface& other) = delete;

private:

    /*!
     *  Get the port abstraction with the selected name, sliced to a NameGroup.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return The sliced port abstraction with the selected name.
     */
    QSharedPointer<NameGroup> getItem(std::string const& portName) const override;

    //! SignalRow represents a single row in the table by grouping the port abstraction and transactional elements.
    struct SignalRow
    {
        //! Defines the Port represented in the row.
        QSharedPointer<PortAbstraction> abstraction_{ QSharedPointer<PortAbstraction>(new PortAbstraction()) };

        //! Defines the mode of the signal (master, slave or system).
        General::InterfaceMode mode_{ General::INTERFACE_MODE_COUNT };

        //! Defines the wirePort represented on the row.
        QSharedPointer<WirePort> wire_{ nullptr };

        //! Defines the transactionalPort represented on the row.
        QSharedPointer<TransactionalPort> transactional_{ nullptr };

        /*!
         *  The default constructor.
         *
         *    @param [in] isWirePort              Flag for wire ports.
         *    @param [in] isTransactionalPort     Flag for transactional ports.
         */
        SignalRow(bool isWirePort, bool isTransactionalPort);

        /*!
         *  Copy constructor
         */
        SignalRow(SignalRow const& other) = default;

        /*!
         *  Comparison of two SignalRows.
         *
         *    @param [in] other   The SignalRow to compare this to.
         *
         *    @return True, if the SignalRows are equal, otherwise false.
         */
        bool operator==(SignalRow const& other) const;

        /*!
         *  Comparison of two SignalRows.
         *
         *    @param [in] other   The SignalRow to compare this to.
         *
         *    @return True, if the SignalRows are not equal, otherwise false.
         */
        bool operator!=(SignalRow const& other) const;

        /*!
         *  Less than comparison of two SignalRows for table ordering.
         *
         *    @param [in] other   The SignalRow to compare this to.
         *
         *    @return True, if this precedes other, otherwise false.
         */
        bool operator<(SignalRow const& other) const;
    };
 
    /*!
     *  Get the signal of the selected index.
     *
     *    @param [in] signalIndex     The selected index.
     *
     *    @return The selected signal.
     */
    QSharedPointer<SignalRow> getSignal(int const& signalIndex) const;

    /*!
     *  Get or create a transactional protocol for the selected signal.
     *
     *    @param [in] signalIndex     Index of the selected signal.
     *
     *    @return Protocol of the selected signal.
     */
    QSharedPointer<Protocol> getOrCreateProtocol(int const& signalIndex) const;

    /*!
     *  Check if the selected protocol is empty and remove it if it is.
     *
     *    @param [in] signalIndex     Index of the containing signal.
     *    @param [in] signalProtocol  The selected protocol.
     */
    void removeEmptyProtocol(int const& signalIndex, QSharedPointer<Protocol> signalProtocol);

    /*!
     *  Check if the protocol type of the selected protocol is empty.
     *
     *    @param [in] portProtocol    The selected protocol.
     *
     *    @return True, if the port protocol is empty, false otherwise.
     */
    bool portProcotolTypeIsEmpty(QSharedPointer<Protocol> portProtocol) const;

    /*!
     *  Get the protocol type of the selected protocol.
     *
     *    @param [in] portProtocol    The selected protocol.
     *
     *    @return The type of the selected protocol.
     */
    QString getProtocolTypeText(QSharedPointer<Protocol> portProtocol) const;
    
    /*!
     *  Create a wire signal.
     *
     *    @param [in] portAbs         Port abstraction containing the signal.
     *    @param [in] modeSpesific    Wire port data.
     *    @param [in] mode            The interface mode.
     */
    void createWireSignal(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
        General::InterfaceMode mode);

    /*!
     *  Create a transactional signal.
     *
     *    @param [in] portAbs         Port abstraction containing the signal.
     *    @param [in] modeSpesific    Transactional port data.
     *    @param [in] mode            The interface mode.
     */
    void createTransactionalSignal(QSharedPointer<PortAbstraction> portAbs,
        QSharedPointer<TransactionalPort> modeSpesific, General::InterfaceMode mode);

    /*!
     *  Write the selected signal to the selected port abstraction.
     *
     *    @param [in] portAbs     The selected port abstraction.
     *    @param [in] i           Index of the selected signal.
     */
    void savePort(QSharedPointer<PortAbstraction> portAbs, int i);

    /*!
     *  Check if the selected port already contains the selected signal.
     *
     *    @param [in] mode            The selected signal.
     *    @param [in] portName        Name of the selected port.
     *    @param [in] systemGroup     The selected system group.
     *
     *    @return True, if the selected port already contains the selected signal, false otherwise.
     */
    bool modeExistsForPort(General::InterfaceMode const& mode, QString const& portName,
        QString const& systemGroup = "") const;

    /*!
     *  Construct a copy signal of the selected signal.
     *
     *    @param [in] signal  The selected port signal.
     *
     *    @return The constructed copy of the signal.
     */
     QSharedPointer<PortAbstractionInterface::SignalRow> constructCopySignal(QSharedPointer<PortAbstraction> port,
         bool isWire, bool isTransactional) const;
    
    /*!
     *  Get the mirrored direction for the selected signal.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] opposingMode    The opposite interface mode of the selected signal.
     *
     *    @return The mirrored direction for the selected signal.
     */
    DirectionTypes::Direction getMirroredDirectionForSignal(QString const& portName,
        General::InterfaceMode const& opposingMode) const;

    /*!
     *	Get the corresponding qualifier attribute for a given string.
     *  
     *    @param [in] attributeName	The attribute name to convert to attribute type.
     *		
     * 		@return The matching qualifier attribute type, or attribute type COUNT if there was no match.
     */
    Qualifier::Attribute getQualifierAttributeType(std::string const& attributeName) const;

    /*!
     *  Calculate all the references to the selected ID in the selected item. Does nothing useful in this interface.
     *  Replaced by getAllReferencesToIdInRow().
     *
     *    @param [in] itemName    Name of the selected item.
     *    @param [in] valueID     The selected ID.
     *
     *    @return Number of references to the selected ID in the selected item.
     */
    int getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! Pointer to the available port abstractions.
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports_;

    //! List of signals.
    QList<QSharedPointer<SignalRow> > signals_;

    //! The standard revision of the abstraction definition.
    Document::Revision abstractionStandardRevision_ = Document::Revision::Unknown;

    //! The port valudator to use.
    QSharedPointer<PortAbstractionValidator> portValidator_ = nullptr;
};

#endif // PORTABSTRACTIONINTERFACE_H
