//-----------------------------------------------------------------------------
// File: PortsInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2020
//
// Description:
// Interface for editing component ports.
//-----------------------------------------------------------------------------

#ifndef PORTSINTERFACE_H
#define PORTSINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <ParameterizableInterface.h>
#include <MasterPortInterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Port.h>


class PortValidator;

//-----------------------------------------------------------------------------
//! Interface for editing component ports.
//-----------------------------------------------------------------------------
class KACTUS2_API PortsInterface : public ParameterizableInterface, public MasterPortInterface
{
public:

    /*!
     *  The constructor.
     *
     *    @param [in] validator               Validator for ports.
     *    @param [in] expressionParser        Parser for expressions.
     *    @param [in] expressionFormatter     Formatter for expressions.
     */
    PortsInterface(QSharedPointer<PortValidator> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

	/*!
     *  The destructor.
     */
    virtual ~PortsInterface() = default;

    /*!
     *  Set available ports.
     *
     *    @param [in] ports   The ports to set.
     */
    void setPorts(Port::List ports);

    /*!
     *  Check if the there are any available ports.
     *
     *    @return True, if there are available ports, false otherwise.
     */
    bool hasPorts() const;

    /*!
     *  Get index of the selected port.
     *
     *    @param [in] itemName    Name of the selected port.
     *
     *    @return Index of the selected port.
     */
    int getItemIndex(std::string const& itemName) const final;

    /*!
     *  Get name of the indexed port.
     *
     *    @param [in] portIndex   Index of the selected port.
     *
     *    @return Name of the selected port.
     */
    std::string getIndexedItemName(int itemIndex) const final;

    /*!
     *  Get the number of available ports.
     *
     *    @return Number of available ports.
     */
    int itemCount() const final;

    /*!
     *  Get the names of the available ports.
     *
     *    @return Names of the available ports.
     */
    std::vector<std::string> getItemNames() const final;

    /*!
     *  Set a new name for the selected port.
     *
     *    @param [in] currentPortName     Name of the selected port.
     *    @param [in] newPortName         New name for the port.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setName(std::string const& currentName, std::string const& newName) final;

    /*!
     *  Calculate all the references to the selected ID in the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] valueID     The selected ID.
     *
     *    @return Number of references to the selected ID in the selected port.
     */
    int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const final;

    /*!
     *  Check if the selected port exists.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the port exists, false otherwise.
     */
    bool portExists(std::string const& portName) const;

    /*!
     *  Validates the contained ports.
     *
     *    @return True, if all the ports are valid, false otherwise.
     */
    bool validateItems() const final;

    /*!
     *  Check if the selected port has a valid name.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the name is valid, false otherwise.
     */
    bool itemHasValidName(std::string const& itemName) const final;

    /*!
     *  Get the type name of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Type name of the selected port.
     */
    std::string getTypeName(std::string const& portName) const;

    /*!
     *	Get the type name and type view references as pairs.
     *  
     *    @param [in] portName     Name of the selected port.
     *	    
     * 	    @return Vector of pairs of type names and vectors of view references.
     */
    std::vector<std::pair<std::string, std::vector<std::string> > > getTypeNameViews(std::string const& portName) const;

    /*!
     *	Get the type name and type definitions as a list of pairs.
     *  
     *    @param [in] portName     Name of the selected port.
     *	    
     * 	    @return Vector containing pairs of type names and type definition vectors.
     */
    std::vector<std::pair<std::string, std::vector<std::string> > > getTypeDefinitions(std::string const& portName) const;

    /*!
     *	Set the port type definition view refs. Should only be used in copy pasting, updates or overwrites existing type def view refs.
     *  
     *    @param [in] portName            Name of the selected port.
     *    @param [in] typeNameViews       The type def name-view reference pairs to set.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setTypeDefViewRefs(std::string const& portName, std::vector<std::pair<std::string, std::vector<std::string> > > const& typeNameViews);

    /*!
     *	Set the port type definition type definitions.
     *  
     *    @param [in] portName            Name of the selected port.
     *    @param [in] typeDefinitions     The type def name-typeDef pairs to set.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setTypeDefDefinitions(std::string const& portName, std::vector<std::pair<std::string, std::vector<std::string> > > const& typeDefinitions);

    /*!
     *	Clear the type definitions of a selected port.
     *  
     *    @param [in] portName     Name of the selected port.
     *	    
     * 	    @return True, if the selected port is found, otherwise false.
     */
    bool clearTypeDefinitions(std::string const& portName);

    /*!
     *  Set a new type name for the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] newType     New type name.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setTypeName(std::string const& portName, std::string const& newType) const;

    /*!
     *  Get the calculated array left value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated array left value of the selected port.
     */
    std::string getArrayLeftValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted array left expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted array left expression of the selected port.
     */
    std::string getArrayLeftFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the array left expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Array left expression of the selected port.
     */
    std::string getArrayLeftExpression(std::string const& portName) const;

    /*!
     *  Set a new array left value for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newArrayLeft    New array left value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setArrayLeft(std::string const& portName, std::string const& newArrayLeft) const;

    /*!
     *  Get the calculated array right value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated array right value of the selected port.
     */
    std::string getArrayRightValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted array right expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted array right expression of the selected port.
     */
    std::string getArrayRightFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the array right expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Array right expression of the selected port.
     */
    std::string getArrayRightExpression(std::string const& portName) const;

    /*!
     *  Set a new array right value for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newArrayRight   New array right value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setArrayRight(std::string const& portName, std::string const& newArrayRight) const;

    /*!
     *  Get the tags of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Tags of the selected port.
     */
    std::string getTags(std::string const& portName) const;

    /*!
     *  Set new tags for the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] tagList     List of the new tags separated by ','.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setTags(std::string const& portName, std::string const& tagList) const;

    /*!
     *  Check if the selected port is ad hoc.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the selected port is ad hoc, false otherwise.
     */
    bool isAdHoc(std::string const& portName) const;

    /*!
     *  Set a new ad hoc visibility flag for the selected port.
     *
     *    @param [in] portName            Name of the selected port.
     *    @param [in] newAdHocVisibility  Flag for ad hoc visibility.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setAdHoc(std::string const& portName, bool newAdHocVisibility) const;

    /*!
     *  Get the direction of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Direction of the selected port.
     */
    std::string getDirection(std::string const& portName) const;

    /*!
     *  Get the direction of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Direction of the selected port.
     */
    DirectionTypes::Direction getDirectionType(std::string const& portName) const;

    /*!
     *  Set direction for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newDirection    New direction.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setDirection(std::string const& portName, std::string const& newDirection) const;

    /*!
     *  Get the calculated left bound value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated left bound value of the selected port.
     */
    std::string getLeftBoundValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted left bound expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted left bound expression of the selected port.
     */
    std::string getLeftBoundFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the left bound expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Left bound expression of the selected port.
     */
    std::string getLeftBoundExpression(std::string const& portName) const;

    /*!
     *  Set a new left bound value for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newLeftBound    New left bound value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setLeftBound(std::string const& portName, std::string const& newLeftBound) const;

    /*!
     *  Get the calculated right bound value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated right bound value of the selected port.
     */
    std::string getRightBoundValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted right bound expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted right bound expression of the selected port.
     */
    std::string getRightBoundFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the right bound expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Right bound expression of the selected port.
     */
    std::string getRightBoundExpression(std::string const& portName) const;
    
    /*!
     *  Set a new right bound value for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newRightBound   New right bound value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setRightBound(std::string const& portName, std::string const& newRightBound) const;

    /*!
     *  Check if the selected port has an expression in the left or right bound values.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the port has an expression in the left or right bound values, false otherwise.
     */
    bool hasExpressionInLeftOrRightBound(std::string const& portName) const;

    /*!
     *  Get the calculated width of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Calculated width of the selected port.
     */
    std::string getWidth(std::string const& portName) const;

    /*!
     *  Set new values for the left and right bounds of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] newWidth    New width.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setWidth(std::string const& portName, std::string const& newWidth) const;

    /*!
     *  Get the calculated default value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated default value of the selected port.
     */
    std::string getDefaultValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted default value expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted default value expression of the selected port.
     */
    std::string getDefaultValueFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the default value expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Default value expression of the selected port.
     */
    std::string getDefaultValueExpression(std::string const& portName) const;
    
    /*!
     *  Set a new default value for the selected port.
     *
     *    @param [in] portName            Name of the selected port.
     *    @param [in] newDefaultValue     New default value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setDefaultValue(std::string const& portName, std::string const& newDefaultValue) const;

    /*!
     *  Get the calculated bus width value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated bus width value of the selected port.
     */
    std::string getBusWidthValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted bus width expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted bus width expression of the selected port.
     */
    std::string getBusWidthFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the bus width expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Bus width expression of the selected port.
     */
    std::string getBusWidthExpression(std::string const& portName) const;

    /*!
     *  Set a new bus width for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newBusWidth     New bus width.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setBusWidth(std::string const& portName, std::string const& newBusWidth) const;

    /*!
     *  Get the initiative of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Initiative of the selected port.
     */
    std::string getInitiative(std::string const& portName) const;

    /*!
     *  Set initiative for the selected port.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] newInitiative   New initiative.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setInitiative(std::string const& portName, std::string const& newInitiative) const;

    /*!
     *  Get the kind of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Kind of the selected port.
     */
    std::string getKind(std::string const& portName) const;

    /*!
     *  Set kind for the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] newKind     New kind.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setKind(std::string const& portName, std::string const& newKind) const;

    /*!
     *  Get the protocol type of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Protocol type of the selected port.
     */
    std::string getProtocolType(std::string const& portName) const;

    /*!
     *  Set protocol type for the selected port.
     *
     *    @param [in] portName            Name of the selected port.
     *    @param [in] newProtocolType     New protocol type.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setProtocolType(std::string const& portName, std::string const& newProtocolType) const;

    /*!
     *  Get the calculated max connections value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated max connections value of the selected port.
     */
    std::string getMaxConnectionsValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted max connections expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted max connections expression of the selected port.
     */
    std::string getMaxConnectionsFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the max connections expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Max connections expression of the selected port.
     */
    std::string getMaxConnectionsExpression(std::string const& portName) const;

    /*!
     *  Set a new max connections value for the selected port.
     *
     *    @param [in] portName            Name of the selected port.
     *    @param [in] newMaxConnections   New max connections value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setMaxConnections(std::string const& portName, std::string const& newMaxConnections) const;

    /*!
     *  Get the calculated min connections value of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *    @param [in] baseNumber  Base for displaying the value.
     *
     *    @return Calculated min connections value of the selected port.
     */
    std::string getMinConnectionsValue(std::string const& portName, unsigned int baseNumber = 0) const;

    /*!
     *  Get the formatted min connections expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Formatted min connections expression of the selected port.
     */
    std::string getMinConnectionsFormattedExpression(std::string const& portName) const;

    /*!
     *  Get the min connections expression of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Min connections expression of the selected port.
     */
    std::string getMinConnectionsExpression(std::string const& portName) const;

    /*!
     *  Set a new min connections value for the selected port.
     *
     *    @param [in] portName            Name of the selected port.
     *    @param [in] newMinConnections   New max connections value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setMinConnections(std::string const& portName, std::string const& newMinConnections) const;

	/*!
	 *  Add a wire port.
	 *
	 *    @param [in] newPortName     Name of the new port.
	 */
	void addWirePort(std::string const& newPortName = std::string()) final;

    /*!
     *  Add a transactional port.
     *
     *    @param [in] newPortName     Name of the new port.
     */
    void addTransactionalPort(std::string const& newPortName = std::string()) final;

    /*!
     *  Remove the selected port.
	 *
	 *    @param [in] portName    Name of the selected port.
     */
    bool removePort(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid left array value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the left array value is valid, false otherwise.
     */
    bool portLeftArrayValueIsValid(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid right array value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the right array value is valid, false otherwise.
     */
    bool portRightArrayValueIsValid(std::string const& portName) const;

    /*!
     *  Check if the selected port has valid types.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the types is valid, false otherwise.
     */
    bool portHasValidTypes(std::string const& portName) const;

    /*!
     *  Check if the selected port is a wire.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the selected port is a wire, false otherwise.
     */
    bool portIsWire(std::string const& portName) const final;

    /*!
     *  Check if the selected port is a wire.
     *
     *    @param [in] port    The selected port.
     *
     *    @return True, if the selected port is a wire, false otherwise.
     */
    bool portIsWire(QSharedPointer<Port> port) const;

    /*!
     *  Check if the selected port has a valid left bound value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the left bound value is valid, false otherwise.
     */
    bool portHasValidLeftBound(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid right bound value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the right bound value is valid, false otherwise.
     */
    bool portHasValidRightBound(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid default value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the default value is valid, false otherwise.
     */
    bool portHasValidDefaultValue(std::string const& portName) const;

    /*!
     *  Check if the selected port is transactional.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the selected port is transactional, false otherwise.
     */
    bool portIsTransactional(std::string const& portName) const final;

    /*!
     *  Check if the selected port is transactional.
     *
     *    @param [in] port    The selected port.
     *
     *    @return True, if the selected port is transactional, false otherwise.
     */
    bool portIsTransactional(QSharedPointer<Port> port) const;

    /*!
     *  Check if the selected port has a valid bus width.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the bus width is valid, false otherwise.
     */
    bool portHasValidBusWidth(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid initiative.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the initiative is valid, false otherwise.
     */
    bool portHasValidInitiative(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid kind.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the kind is valid, false otherwise.
     */
    bool portHasValidKind(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid protocol.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the protocol is valid, false otherwise.
     */
    bool portHasValidProtocol(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid max connections value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the max connections value is valid, false otherwise.
     */
    bool portHasValidMaxConnections(std::string const& portName) const;

    /*!
     *  Check if the selected port has a valid min connections value.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the min connections value is valid, false otherwise.
     */
    bool portHasValidMinConnections(std::string const& portName) const;

    /*!
     *  Get the icon path of the selected port.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return Icon path for the selected port.
     */
    std::string getIconPathForPort(std::string const& portName) const;

    /*!
     *  Get the port with the selected name.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return The port with the selected name.
     */
    QSharedPointer<Port> getPort(std::string const& portName) const;

    QSharedPointer<NameGroup> getItem(std::string const& portName) const final;

    //! No copying. No assignment.
    PortsInterface(const PortsInterface& other) = delete;
    PortsInterface& operator=(const PortsInterface& other) = delete;

private:

    /*!
     *  Set the type name and definition of a port.
     *
     *    @param [in] port    The selected port.
     */
    void setTypeNameAndDefinition(QSharedPointer<Port> port) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the available ports.
    Port::List ports_ = nullptr;

    //! The validator used for ports.
    QSharedPointer<PortValidator> portValidator_;
};

#endif // PORTSMODEL_H
