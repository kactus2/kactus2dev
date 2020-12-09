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

#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>
#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>

#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/generaldeclarations.h>

class PortMap;
class PortMapValidator;
class PortAbstraction;
class AbstractionDefinition;
class AbstractionType;
class PortsInterface;
class Component;

//-----------------------------------------------------------------------------
//! Interface for editing port maps.
//-----------------------------------------------------------------------------
class PortMapInterface : public ParameterizableInterface, public CommonInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] validator               Validator for address blocks.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] portInterface           Interface for accessing ports.
     */
    PortMapInterface(QSharedPointer<PortMapValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        PortsInterface* portInterface);

	/*!
     *  The destructor.
     */
    virtual ~PortMapInterface() = default;

    /*!
     *  Set available port maps.
     *
     *      @param [in] absDef          The used new abstraction definition.
     *      @param [in] abstraction     The selected abstraction type.
     *      @param [in] interfaceMode   The selected interface mode.
     *      @param [in] systemGroup     The selected system group.
     *      @param [in] component       Component containing the port maps.
     */
    void setPortMaps(QSharedPointer<AbstractionDefinition const> absDef,
        QSharedPointer<AbstractionType> abstraction, General::InterfaceMode interfaceMode, std::string systemGroup,
        QSharedPointer<Component> component);

    /*!
     *  Get name of the indexed logical port.
     *
     *      @param [in] itemIndex   Index of the selected logical port.
     *
     *      @return Name of the selected logical port.
     */
    std::string getIndexedItemName(int const& itemIndex) const;

    /*!
     *  Get the number of available port maps.
     *
     *      @return Number of available port maps.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the amount of port maps connected to the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return Number of port maps connected to the selected logical port.
     */
    int portMapCount(std::string const& logicalPortName) const;

    /*!
     *  Get the number of logical ports in the current abstraction definition.
     *
     *      @return Number of logical ports in the current abstraction definition.
     */
    int logicalPortCount() const;

    /*!
     *  Get the index of the selected logical port mapping.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return Index of the selected logical port mapping.
     */
    int mappingIndex(std::string const& logicalPortName) const;

    /*!
     *  Get the names of the available logical ports.
     *
     *      @return Names of the available logical ports.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Get the logical port name modified by the port map index.
     *
     *      @param [in] logicalPortName     The current logical port name.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Modified logical port name.
     */
    std::string getLogicalPortName(std::string const& logicalPortName, int const& portMapIndex = -1) const;

    /*!
     *  Check if the selected logical port exists.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return True, if the logical port exists, false otherwise.
     */
    bool hasLogicalPort(std::string const& logicalPortName) const;

    /*!
     *  Set a new name for the selected logical port.
     *
     *      @param [in] currentName     Name of the selected logical port.
     *      @param [in] portMapIndex    Index of the selected port map.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLogicalPort(std::string const& currentName, int const& portMapIndex, std::string const& newName);

    /*!
     *  Check if the selected port map has a connected physical port.
     *
     *      @param [in] logicalPortName     Name of the connected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the selected port map is connected to a logical port, false otherwise.
     */
    bool hasPhysicalPort(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Get the name of the connected physical port in the selected port map.
     *
     *      @param [in] logicalPortName     Name of the connected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Name of the connected physical port in the selected port map.
     */
    std::string getPhysicalPortName(std::string const& logicalPortName, int const& portMapIndex = -1) const;

    /*!
     *  Set a new name for the connected physical port in the selected port map.
     *
     *      @param [in] logicalPortName     Name of the connected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newPhysicalName     New name for the physical port.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPhysicalPort(std::string const& logicalName, int const& portMapIndex,
        std::string const& newPhysicalName);

    /*!
     *  Get the logical presence of the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return Logical presence of the selected logical port.
     */
    std::string getLogicalPresence(std::string const& logicalPortName);

    /*!
     *  Get the invert value of the selected logical port in string form.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return Invert value of the selected logical port in string form.
     */
    std::string getInvertString(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Get the invert value of the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Invert value of the selected logical port.
     */
    bool getInvertValue(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Set a new invert value for the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newInvertValue      The new invert value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setInvertValue(std::string const& logicalPortName, int const& portMapIndex, bool newInvertValue) const;

    /*!
     *  Get the is informative value of the selected port map in string form.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Is informative value of the selected port map in string form.
     */
    std::string getIsInformativeString(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Get the is informative value of the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Is informative value of the selected port map.
     */
    bool getIsInformativeValue(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Get the is informative value of the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Is informative value of the selected port map.
     */
    bool setIsInformativeValue(std::string const& logicalPortName, int const& portMapIndex,
        bool newIsInformativeValue) const;

    /*!
     *  Get the calculated logical tie off of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated logical tie off value of the selected item.
     */
    std::string getLogicalTieOffValue(std::string const& logicalPortName, int const& portMapIndex,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted logical tie off expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Formatted logical tie off expression of the selected item.
     */
    std::string getLogicalTieOffFormattedExpression(std::string const& logicalPortName, int const& portMapIndex)
        const;

    /*!
     *  Get the logical tie off expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Logical tie off expression of the selected item.
     */
    std::string getLogicalTieOffExpression(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Set a new logical tie off value for the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newTieOff           New logical tie off value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLogicalTieOff(std::string const& logicalPortName, int const& portMapIndex,
        std::string const& newTieOff);

    /*!
     *  Get the calculated logical left bound of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated logical left bound value of the selected item.
     */
    std::string getLogicalLeftBoundValue(std::string const& logicalPortName, int const& portMapIndex,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted logical left bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Formatted logical left bound expression of the selected item.
     */
    std::string getLogicalLeftBoundFormattedExpression(std::string const& logicalPortName, int const& portMapIndex)
        const;

    /*!
     *  Get the logical left bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Logical left bound expression of the selected item.
     */
    std::string getLogicalLeftBoundExpression(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Set a new logical left value for the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newLogicalLeft      New logical left value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLogicalLeftBound(std::string const& logicalPortName, int const& portMapIndex,
        std::string const& newLogicalLeft);

    /*!
     *  Get the calculated logical right bound of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated logical right bound value of the selected item.
     */
    std::string getLogicalRightBoundValue(std::string const& logicalPortName, int const& portMapIndex,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted logical right bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Formatted logical right bound expression of the selected item.
     */
    std::string getLogicalRightBoundFormattedExpression(std::string const& logicalPortName,
        int const& portMapIndex) const;

    /*!
     *  Get the logical right bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Logical right bound expression of the selected item.
     */
    std::string getLogicalRightBoundExpression(std::string const& logicalPortName, int const& portMapIndex) const;
    
    /*!
     *  Set a new logical right value for the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newLogicalRight     New logical right value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLogicalRightBound(std::string const& logicalPortName, int const& portMapIndex,
        std::string const& newLogicalRight);

    /*!
     *  Get the calculated physical left bound of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated physical left bound value of the selected item.
     */
    std::string getPhysicalLeftBoundValue(std::string const& logicalPortName, int const& portMapIndex,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted physical left bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Formatted physical left bound expression of the selected item.
     */
    std::string getPhysicalLeftBoundFormattedExpression(std::string const& logicalPortName,
        int const& portMapIndex) const;

    /*!
     *  Get the physical left bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Physical left bound expression of the selected item.
     */
    std::string getPhysicalLeftBoundExpression(std::string const& logicalPortName, int const& portMapIndex) const;
    
    /*!
     *  Set a new physical left bound value for the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newPhysicalLeft     New physical left value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPhysicalLeftBound(std::string const& logicalPortName, int const& portMapIndex,
        std::string const& newPhysicalLeft);

    /*!
     *  Get the calculated physical right bound of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated physical right bound value of the selected item.
     */
    std::string getPhysicalRightBoundValue(std::string const& logicalPortName, int const& portMapIndex,
        int const& baseNumber = 0) const;

    /*!
     *  Get the formatted physical right bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Formatted physical right bound expression of the selected item.
     */
    std::string getPhysicalRightBoundFormattedExpression(std::string const& logicalPortName,
        int const& portMapIndex) const;

    /*!
     *  Get the physical right bound expression of the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Physical right bound expression of the selected item.
     */
    std::string getPhysicalRightBoundExpression(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Set a new physical right bound value for the selected item.
     *
     *      @param [in] logicalPortName     Name of the logical port of the port map.
     *      @param [in] portMapIndex        Index of the selected port map.
     *      @param [in] newPhysicalRight    New physical right value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setPhysicalRightBound(std::string const& logicalPortName, int const& portMapIndex,
        std::string const& newPhysicalRight);

    /*!
     *  Get the icon path for the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return Icon path for the selected logical port.
     */
    std::string getLogicalPortIconPath(std::string const& logicalPortName) const;

    /*!
     *  Get the icon path for the physical port in the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Icon path for the physical port in the selected port map.
     */
    std::string getPhysicalPortIconPath(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the selected logical port exists.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return True, if the logical port exists, false otherwise.
     */
    bool logicalPortExists(std::string const& logicalPortName) const;

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
     *  Get the expressions in the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Expressions in the selected component instantiations.
     */
    std::vector<std::string> getExpressionsInSelectedPortMap(std::string logicalPortName, int const& portMapIndex)
        const;

    /*!
     *  Change the abstraction definition for the port map validator.
     */
    void changeValidatorAbstractionDefinition();

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the port maps are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected logical port is valid
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the selected logical port is valid, false otherwise.
     */
    bool logicalPortIsValid(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the physical port of the selected port map is valid
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the physical port of the selected port map is valid, false otherwise.
     */
    bool physicalPortIsValid(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the physical port of the selected port map is valid
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the physical port of the selected port map is valid, false otherwise.
     */
    bool physicalMappingIsValid(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the port map has a valid physical port or a valid tie off.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the physical mapping is valid, otherwise false.
     */
    bool connectedPortsHaveValidPortTypes(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the connected ports of the selected port map have valid directions.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the directions are valid, otherwise false.
     */
    bool connectedPortsHaveValidDirections(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the connected ports of the selected port map have valid initiatives.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the initiatives are valid, otherwise false.
     */
    bool connectedPortsHaveValidInitiatives(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the connected ports of the selected port map have the same range.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the connected ports have the same range, otherwise false.
     */
    bool connectedPortsHaveSameRange(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Check if the tie off value of the selected port map is valid.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the tie off value of the selected port map is valid, otherwise false.
     */
    bool tieOffIsValid(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     * Checks if the logical port of the selected port map has a valid range.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if the range is valid, otherwise false.
     */
    bool logicalPortHasValidRange(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Connect the selected logical port to the selected physical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] physicalPortName    Name of the selected physical port.
     *
     *      @return True, if successful, false otherwise.
     */
    bool connectPorts(std::string const& logicalPortName, std::string const& physicalPortName);

    /*!
     *  Add a new port map.
     *
     *      @param [in] row                 Row of the new port map.
     *      @param [in] nweLogicalPortName  Name of the new logical port in the port map.
     */
    void addPortMap(int const& row, std::string const& newLogicalPortName = std::string(""));

    /*!
     *  Remove the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removePortMap(std::string const& logicalPortName, int const& portMapIndex);

    /*!
     *  Remove all the port maps from the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return List of connected physical port names.
     */
    std::vector<std::string> removeAllPortMapsFromLogicalPort(std::string const& logicalPortName);

    /*!
     *  Get a pointer to the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return Pointer to the selected port map.
     */
    PortMap* getPortMapPointer(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Get a pointer to the selected logical port.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *
     *      @return Pointer to the selected logical port.
     */
    PortAbstraction* getLogicalPortPointer(std::string const& logicalPortName) const;

    /*!
     *  Get the interface for accessing ports.
     *
     *      @return Interface for accessing ports.
     */
    PortsInterface* getPortInterface() const;

private:

    //! Container for a port mapping.
    struct PortMapping
    {
        //! The logical port of the port maps.
        QSharedPointer<PortAbstraction> logicalPort_;

        //! A list of port maps with the same logical port.
        QList<QSharedPointer<PortMap> > portMaps_;
    };

    /*!
     *  Get the selected port mapping.
     *
     *      @param [in] logicalPortName     Name of the selected port mapping.
     *
     *      @return The selected port mapping.
     */
    QSharedPointer<PortMapping> getPortMapping(std::string const& logicalPortName) const;

    /*!
     *  Get the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return The selected port map.
     */
    QSharedPointer<PortMap> getPortMap(std::string const& logicalPortName, int const& portMapIndex) const;

    /*!
     *  Get the combined tie off for display on the logical row.
     *
     *      @param [in] logicalPortName     Name of the logical port.
     *
     *      @return The combined tie off for display on the logical row.
     */
    QString getTieOffForLogicalRow(std::string const& logicalPortName) const;

    /*!
     *  Get the combined logical left bound value for display on the logical row.
     *
     *      @param [in] logicalPortName     Name of the logical port.
     *
     *      @return The combined logical left bound value for display on the logical row.
     */
    QString getLogicalLeftForLogicalPort(std::string const& logicalPortName) const;

    /*!
     *  Remove an empty range container from the selected logical port.
     *
     *      @param [in] editedPortLogical   The selected logical port.
     */
    void removeEmptyRange(QSharedPointer<PortMap::LogicalPort> editedPortLogical);

    /*!
     *  Create a new port map connecting the selected logical port.
     *
     *      @param [in] logicalPortName     The selected logical port.
     *
     *      @return The created port map.
     */
    QSharedPointer<PortMap> createNewPortMap(QString const& logicalPortName);

    /*!
     *  Remove an empty physical port container from the selected port map.
     *
     *      @param [in] editedPortMap   The selected port map.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeEmptyPhysicalPort(QSharedPointer<PortMap> editedPortMap);

    /*!
     *  Remove an empty physical part select container from the selected physical port.
     *
     *      @param [in] editedPortMap   The selected port map.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeEmptyPhysicalPartSelect(QSharedPointer<PortMap::PhysicalPort> physicalPort);

    /*!
     *  Create a part select container for the physical port in the selected port map.
     *
     *      @param [in] logicalPortName     Name of the selected logical port.
     *      @param [in] portMapIndex        Index of the selected port map.
     *
     *      @return The created part select.
     */
    QSharedPointer<PartSelect> createPhysicalPartSelect(std::string const& logicalPortName,
        int const& portMapIndex);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the contained port mappings.
    QList<QSharedPointer<PortMapping> > portMappings_;

    //! The current abstraction type.
    QSharedPointer<AbstractionType> abstraction_;

    //! Specifies the currently active interface mode.
    General::InterfaceMode interfaceMode_;

    //! The system group name in case of system mode.
    QString systemGroup_;

    //! The abstraction definition referenced by the abstraction type.
    QSharedPointer<AbstractionDefinition const> abstractionDef_;

    //! Validator for port maps.
    QSharedPointer<PortMapValidator> validator_;

    //! Interface for accessing ports.
    PortsInterface* portInterface_;
};

#endif // PORTMAPINTERFACE_H
