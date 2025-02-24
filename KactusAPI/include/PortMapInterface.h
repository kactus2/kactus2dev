//-----------------------------------------------------------------------------
// File: PortMapInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.10.2020
//
// Description:
// Interface for editing port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPINTERFACE_H
#define PORTMAPINTERFACE_H

#include <CommonInterface.h>
#include <ParameterizableInterface.h>

#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/generaldeclarations.h>

#include "KactusAPI/KactusAPIGlobal.h"

class PortMap;
class PortMapValidator;
class PortAbstraction;
class AbstractionDefinition;
class AbstractionType;
class Component;
class PortsInterface;
class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Interface for editing port maps.
//-----------------------------------------------------------------------------
class KACTUS2_API PortMapInterface : public ParameterizableInterface, public CommonInterface
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] validator               Validator for address blocks.
     *    @param [in] expressionParser        Parser for expressions.
     *    @param [in] expressionFormatter     Formatter for expressions.
     *    @param [in] physicalPortInterface   Interface for accessing physical ports.
     *    @param [in] logicalPortInterface    Interface for accessing logical ports.
     */
    PortMapInterface(QSharedPointer<PortMapValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        PortsInterface* physicalPortInterface,
        PortAbstractionInterface* logicalPortInterface);

	/*!
     *  The destructor.
     */
    virtual ~PortMapInterface() = default;

    /*!
     *  Setup the abstraction definition.
     *
     *    @param [in] absDef  The new abstraction definition.
     */
    void setupAbstractionDefinition(QSharedPointer<AbstractionDefinition const> absDef);

    /*!
     *  Setup the available physical ports.
     *
     *    @param [in] component   Component containing the selected ports.
     */
    void setupPhysicalPorts(Port::List ports);

    /*!
     *  Setup the available port maps.
     *
     *    @param [in] abstractionType     Abstraction type containing the available port maps.
     */
    virtual void setupPortMaps(QSharedPointer<AbstractionType> abstractionType);

    /*!
     *  Setup the bus interface mode.
     *
     *    @param [in] busMode     The new bus interface mode.
     */
    void setupBusMode(General::InterfaceMode busMode);

    /*!
     *  Setup the system group.
     *
     *    @param [in] systemGroup     The new system group.
     */
    void setupSystemGroup(QString const& systemGroup);

    /*!
     *  Get name of the indexed logical port.
     *
     *    @param [in] itemIndex   Index of the selected logical port.
     *
     *    @return Name of the selected logical port.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const;

    /*!
     *  Get the number of available port maps.
     *
     *    @return Number of available port maps.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available logical ports.
     *
     *    @return Names of the available logical ports.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Get the logical port name modified by the port map index.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Modified logical port name.
     */
    std::string getLogicalPortName(int const& portMapIndex) const;

    /*!
     *  Set a new name for the selected logical port.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] newName         New name for the item.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setLogicalPort(int const& portMapIndex, std::string const& newName);

    /*!
     *  Check if the selected port map has a connected physical port.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the selected port map is connected to a logical port, false otherwise.
     */
    bool hasPhysicalPort(int const& portMapIndex) const;

    /*!
     *  Get the name of the connected physical port in the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Name of the connected physical port in the selected port map.
     */
    std::string getPhysicalPortName(int const& portMapIndex) const;

    /*!
     *  Set a new name for the connected physical port in the selected port map.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *    @param [in] newPhysicalName     New name for the physical port.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPhysicalPort(int const& portMapIndex, std::string const& newPhysicalName);

    /*!
     *  Get the logical presence of the logical port in the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Logical presence.
     */
    std::string getLogicalPresence(int const& portMapIndex);

    /*!
     *  Get the logical presence of the selected logical port.
     *
     *    @param [in] logicalPortName     Name of the selected logical port.
     *
     *    @return Logical presence of the selected logical port.
     */
    std::string getLogicalPresenceWithLogicalPort(std::string const& logicalName);

    /*!
     *  Get the invert value of the logical port  of the selected port map in string form.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Invert value of the selected logical port in string form.
     */
    std::string getInvertString(int const& portMapIndex) const;

    /*!
     *  Get the invert value of the logical port  of the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Invert value.
     */
    BooleanValue getInvertValue(int const& portMapIndex) const;

    /*!
     *  Get the invert value of the logical port  of the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Invert value.
     */
    bool getInvertBool(int const& portMapIndex) const;

    /*!
     *  Set a new invert value for the selected logical port.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] newInvertValue  The new invert value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setInvertValue(int const& portMapIndex, bool newInvertValue) const;

    /*!
     *  Get the is informative value of the selected port map in string form.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Is informative value of the selected port map in string form.
     */
    std::string getIsInformativeString(int const& portMapIndex) const;

    /*!
     *  Get the is informative value of the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Is informative value of the selected port map.
     */
    bool getIsInformativeValue(int const& portMapIndex) const;

    /*!
     *  Set a new is informative value for the selected port map.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setIsInformativeValue(int const& portMapIndex, bool newIsInformativeValue) const;

    /*!
     *  Get the calculated logical tie off of the selected item.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *    @param [in] baseNumber          Base for displaying the value.
     *
     *    @return Calculated logical tie off value of the selected item.
     */
    std::string getLogicalTieOffValue(int const& portMapIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted logical tie off expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Formatted logical tie off expression of the selected item.
     */
    std::string getLogicalTieOffFormattedExpression(int const& portMapIndex) const;

    /*!
     *  Get the logical tie off expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Logical tie off expression of the selected item.
     */
    std::string getLogicalTieOffExpression(int const& portMapIndex) const;

    /*!
     *  Set a new logical tie off value for the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] newTieOff       New logical tie off value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setLogicalTieOff(int const& portMapIndex, std::string const& newTieOff);

    /*!
     *  Get the calculated logical left bound of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] baseNumber      Base for displaying the value.
     *
     *    @return Calculated logical left bound value of the selected item.
     */
    std::string getLogicalLeftBoundValue(int const& portMapIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted logical left bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Formatted logical left bound expression of the selected item.
     */
    std::string getLogicalLeftBoundFormattedExpression(int const& portMapIndex) const;

    /*!
     *  Get the logical left bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Logical left bound expression of the selected item.
     */
    std::string getLogicalLeftBoundExpression(int const& portMapIndex) const;

    /*!
     *  Set a new logical left value for the selected item.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *    @param [in] newLogicalLeft      New logical left value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setLogicalLeftBound(int const& portMapIndex, std::string const& newLogicalLeft);

    /*!
     *  Get the calculated logical right bound of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] baseNumber      Base for displaying the value.
     *
     *    @return Calculated logical right bound value of the selected item.
     */
    std::string getLogicalRightBoundValue(int const& portMapIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted logical right bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Formatted logical right bound expression of the selected item.
     */
    std::string getLogicalRightBoundFormattedExpression(int const& portMapIndex) const;

    /*!
     *  Get the logical right bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Logical right bound expression of the selected item.
     */
    std::string getLogicalRightBoundExpression(int const& portMapIndex) const;

    /*!
     *  Set a new logical right value for the selected item.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *    @param [in] newLogicalRight     New logical right value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setLogicalRightBound(int const& portMapIndex, std::string const& newLogicalRight);

    /*!
     *  Get the calculated physical left bound of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] baseNumber      Base for displaying the value.
     *
     *    @return Calculated physical left bound value of the selected item.
     */
    std::string getPhysicalLeftBoundValue(int const& portMapIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted physical left bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Formatted physical left bound expression of the selected item.
     */
    std::string getPhysicalLeftBoundFormattedExpression(int const& portMapIndex) const;

    /*!
     *  Get the physical left bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Physical left bound expression of the selected item.
     */
    std::string getPhysicalLeftBoundExpression(int const& portMapIndex) const;

    /*!
     *  Set a new physical left bound value for the selected item.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *    @param [in] newPhysicalLeft     New physical left value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPhysicalLeftBound(int const& portMapIndex, std::string const& newPhysicalLeft);

    /*!
     *  Get the calculated physical right bound of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *    @param [in] baseNumber      Base for displaying the value.
     *
     *    @return Calculated physical right bound value of the selected item.
     */
    std::string getPhysicalRightBoundValue(int const& portMapIndex, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted physical right bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Formatted physical right bound expression of the selected item.
     */
    std::string getPhysicalRightBoundFormattedExpression(int const& portMapIndex) const;

    /*!
     *  Get the physical right bound expression of the selected item.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Physical right bound expression of the selected item.
     */
    std::string getPhysicalRightBoundExpression(int const& portMapIndex) const;

    /*!
     *  Set a new physical right bound value for the selected item.
     *
     *    @param [in] portMapIndex        Index of the selected port map.
     *    @param [in] newPhysicalRight    New physical right value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setPhysicalRightBound(int const& portMapIndex, std::string const& newPhysicalRight);

    /*!
     *  Get the icon path for the logical port of the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Icon path.
     */
    std::string getLogicalPortIconPath(int const& portMapIndex) const;

    /*!
     *  Get the icon path for the physical port of the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Icon path for the physical port in the selected port map.
     */
    std::string getPhysicalPortIconPath(int const& portMapIndex) const;

    /*!
     *  Check if the logical port of the selected port map exists.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the logical port exists, false otherwise.
     */
    bool logicalPortExists(int const& portMapIndex) const;

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *    @param [in] itemName    Name of the selected item.
     *    @param [in] valueID     The selected ID.
     *
     *    @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const override
        final;

    /*!
     *  Get the expressions in the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Expressions in the selected component instantiations.
     */
    std::vector<std::string> getExpressionsInSelectedPortMap(int const& portMapIndex) const;

    /*!
     *  Validates the contained items.
     *
     *    @return True, if all the port maps are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the logical port of the selected port map is valid.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the logical port is valid, false otherwise.
     */
    bool logicalPortIsValid(int const& portMapIndex) const;

    /*!
     *  Check if the physical port of the selected port map is valid
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the physical port is valid, false otherwise.
     */
    bool physicalPortIsValid(int const& portMapIndex) const;

    /*!
     *  Check if the physical mapping of the selected port map is valid
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the physical mapping is valid, false otherwise.
     */
    bool physicalMappingIsValid(int const& portMapIndex) const;

    /*!
     *  Check if the mapped ports have valid port types.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the mapped ports have valid types, otherwise false.
     */
    bool connectedPortsHaveValidPortTypes(int const& portMapIndex) const;

    /*!
     *  Check if the connected ports of the selected port map have valid directions.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the directions are valid, otherwise false.
     */
    bool connectedPortsHaveValidDirections(int const& portMapIndex) const;

    /*!
     *  Check if the connected ports of the selected port map have valid initiatives.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the initiatives are valid, otherwise false.
     */
    bool connectedPortsHaveValidInitiatives(int const& portMapIndex) const;

    /*!
     *  Check if the connected ports of the selected port map have the same range.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the connected ports have the same range, otherwise false.
     */
    bool connectedPortsHaveSameRange(int const& portMapIndex) const;

    /*!
     *  Check if the tie off value of the selected port map is valid.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the tie off value of the selected port map is valid, otherwise false.
     */
    bool tieOffIsValid(int const& portMapIndex) const;

    /*!
     * Checks if the logical port of the selected port map has a valid range.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if the range is valid, otherwise false.
     */
    bool logicalPortHasValidRange(int const& portMapIndex) const;

    /*!
     *  Connect the selected logical port to the selected physical port.
     *
     *    @param [in] logicalPortName     Name of the selected logical port.
     *    @param [in] physicalPortName    Name of the selected physical port.
     *
     *    @return True, if successful, false otherwise.
     */
    bool connectPorts(std::string const& logicalPortName, std::string const& physicalPortName);

    /*!
     *  Add a new port map.
     *
     *    @param [in] row     Row of the new port map.
     */
    void addPortMap(int const& row);

    /*!
     *  Remove the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removePortMap(int const& portMapIndex);

    /*!
     *  Remove all the port maps from the selected logical port.
     *
     *    @param [in] logicalPortName     Name of the selected logical port.
     *
     *    @return List of connected physical port names.
     */
    std::vector<std::string> removeAllPortMapsFromLogicalPort(std::string const& logicalPortName);

    /*!
     *  Get a pointer to the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return Pointer to the selected port map.
     */
    PortMap* getPortMapPointer(int const& portMapIndex) const;

    /*!
     *  Get the interface for accessing physical ports.
     *
     *    @return Interface for accessing physical ports.
     */
    PortsInterface* getPhysicalPortInterface() const;

    /*!
     *  Get the interface for accessing logical ports.
     *
     *    @return Interface for accessing logical ports.
     */
    PortAbstractionInterface* getLogicalPortInterface() const;

    /*!
     *  Create port maps from signals with required presence.
     */
    void createRequiredSignals();

    /*!
     *  Create port maps from signals with optional presence.
     */
    void createOptionalSignals();

    /*!
     *  Create port maps from all signals.
     */
    void createAllSignals();

    /*!
     *  Get the currently active bus interface mode.
     *
     *    @return The active interface mode.
     */
    General::InterfaceMode getInterfaceMode() const;

    /*!
     *  Get the currently active system group.
     *
     *    @return The active system group.
     */
    std::string getSystemGroup() const;

private:

    /*!
     *  Get a port map with the selected logical signal without connected physical port or assigned tie off.
     *
     *    @param [in] logicalPortName     Name of the selected logical signal.
     *
     *    @return Port map with the connected logical port and unconnected physical.
     */
    QSharedPointer<PortMap> getUnconnectedPortMap(std::string const& logicalPortName) const;

    /*!
     *  Get the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return The selected port map.
     */
    QSharedPointer<PortMap> getPortMap(int const& portMapIndex) const;

    /*!
     *  Remove an empty logical port from the selected port map.
     *
     *    @param [in] editedPortMap   The selected port map.
     *
     *    @return True, if the logical port was removed, false otherwise.
     */
    bool removeEmptyLogicalPort(QSharedPointer<PortMap> editedPortMap);

    /*!
     *  Remove an empty range container from the selected logical port.
     *
     *    @param [in] editedPortLogical   The selected logical port.
     */
    bool removeEmptyRange(QSharedPointer<PortMap::LogicalPort> editedPortLogical);

    /*!
     *  Remove an empty physical port container from the selected port map.
     *
     *    @param [in] editedPortMap   The selected port map.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeEmptyPhysicalPort(QSharedPointer<PortMap> editedPortMap);

    /*!
     *  Remove an empty physical part select container from the selected physical port.
     *
     *    @param [in] editedPortMap   The selected port map.
     *
     *    @return True, if successful, false otherwise.
     */
    bool removeEmptyPhysicalPartSelect(QSharedPointer<PortMap::PhysicalPort> physicalPort);

    /*!
     *  Create a part select container for the physical port in the selected port map.
     *
     *    @param [in] portMapIndex    Index of the selected port map.
     *
     *    @return The created part select.
     */
    QSharedPointer<PartSelect> getPhysicalPartSelect(int const& portMapIndex);

    /*!
     *  Create port maps with the selected presence.
     *
     *    @param [in] presence    The selected presence.
     */
    void createPortMapsWithPresence(int presence);

    /*!
     *  Check if a port map exists for the selected signal.
     *
     *    @param [in] signalName  Name of the selected logical signal.
     *
     *    @return True, if a port map exists for the selected signal, false otherwise.
     */
    bool portMapExistsForLogicalSignal(std::string const& signalName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained port maps.
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps_{ nullptr };

    //! Specifies the currently active interface mode.
    General::InterfaceMode interfaceMode_;

    //! The system group name in case of system mode.
    std::string systemGroup_;

    //! The abstraction definition referenced by the abstraction type.
    QSharedPointer<AbstractionDefinition const> abstractionDef_{ nullptr };

    //! Validator for port maps.
    QSharedPointer<PortMapValidator> validator_{ nullptr };

    //! Interface for accessing physical ports.
    PortsInterface* physicalPortInterface_{ nullptr };

    //! Interface for accessing logical ports.
    PortAbstractionInterface* logicalPortInterface_{ nullptr };
};

#endif // PORTMAPINTERFACE_H
