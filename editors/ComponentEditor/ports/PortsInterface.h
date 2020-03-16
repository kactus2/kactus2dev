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

#include <QSharedPointer>
#include <QList>

#include <vector>
#include <string>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <editors/ComponentEditor/common/ParameterizableInterface.h>

class Component;
class Port;
class PortValidator;

using namespace std;

//-----------------------------------------------------------------------------
//! Interface for editing component ports.
//-----------------------------------------------------------------------------
class PortsInterface : public ParameterizableInterface
{
public:

    /*!
     *  The constructor.
     */
    PortsInterface();

	/*!
     *  The destructor.
     */
    virtual ~PortsInterface() = default;

    /*!
     *  Set available ports.
     *
     *      @param [in] component   Component containing the selected ports.
     */
    void setPorts(QSharedPointer<Component> component);

    /*!
     *  Set port validator.
     *
     *      @param [in] validator   Validator for ports.
     */
    void setValidator(QSharedPointer<PortValidator> validator);

    /*!
     *  Get index of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Index of the selected port.
     */
    virtual int getItemIndex(string const& itemName) const override final;

    /*!
     *  Get name of the indexed port.
     *
     *      @param [in] portIndex   Index of the selected port.
     *
     *      @return Name of the selected port.
     */
    virtual string getIndexedItemName(int const& itemIndex) const override final;

    /*!
     *  Get the number of available ports.
     *
     *      @return Number of available ports.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the names of the available ports.
     *
     *      @return Names of the available ports.
     */
    virtual vector<string> getItemNames() const override final;

    /*!
     *  Set a new name for the selected port.
     *
     *      @param [in] currentPortName     Name of the selected port.
     *      @param [in] newPortName         New name for the port.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(string const& currentName, string const& newName) override final;

    /*!
     *  Get the description of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Description of the selected port.
     */
    virtual string getDescription(string const& itemName) const override final;

    /*!
     *  Set a new description for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(string const& itemName, string const& newDescription) override final;

    /*!
     *  Calculate all the references to the selected ID in the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected port.
     */
    virtual int getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const override final;

    /*!
     *  Validates the contained ports.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Check if the selected port has a valid name.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    bool itemHasValidName(string const& itemName) const override final;

    /*!
     *  Get the type name of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Type name of the selected port.
     */
    string getTypeName(string const& portName);

    /*!
     *  Set a new type name for the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] newType     New type name.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setTypeName(string const& portName, string const& newType);

    /*!
     *  Get the calculated array left value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated array left value of the selected port.
     */
    string getArrayLeftValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted array left expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted array left expression of the selected port.
     */
    string getArrayLeftFormattedExpression(string const& portName) const;

    /*!
     *  Get the array left expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Array left expression of the selected port.
     */
    string getArrayLeftExpression(string const& portName) const;

    /*!
     *  Set a new array left value for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newArrayLeft    New array left value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setArrayLeft(string const& portName, string const& newArrayLeft);

    /*!
     *  Get the calculated array right value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated array right value of the selected port.
     */
    string getArrayRightValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted array right expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted array right expression of the selected port.
     */
    string getArrayRightFormattedExpression(string const& portName) const;

    /*!
     *  Get the array right expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Array right expression of the selected port.
     */
    string getArrayRightExpression(string const& portName) const;

    /*!
     *  Set a new array right value for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newArrayRight   New array right value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setArrayRight(string const& portName, string const& newArrayRight);

    /*!
     *  Get the tags of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Tags of the selected port.
     */
    string getTags(string const& portName) const;

    /*!
     *  Set new tags for the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] tagList     List of the new tags separated by ','.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setTags(string const& portName, string const& tagList);

    /*!
     *  Check if the selected port is ad hoc.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port is ad hoc, false otherwise.
     */
    bool isAdHoc(string const& portName) const;

    /*!
     *  Set a new ad hoc visibility flag for the selected port.
     *
     *      @param [in] portName            Name of the selected port.
     *      @param [in] newAdHocVisibility  Flag for ad hoc visibility.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setAdHoc(string const& portName, bool newAdHocVisibility);

    /*!
     *  Get the direction of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Direction of the selected port.
     */
    string getDirection(string const& portName) const;

    /*!
     *  Set direction for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newDirection    New direction.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDirection(string const& portName, string const& newDirection);

    /*!
     *  Get the calculated left bound value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated left bound value of the selected port.
     */
    string getLeftBoundValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted left bound expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted left bound expression of the selected port.
     */
    string getLeftBoundFormattedExpression(string const& portName) const;

    /*!
     *  Get the left bound expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Left bound expression of the selected port.
     */
    string getLeftBoundExpression(string const& portName) const;

    /*!
     *  Set a new left bound value for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newLeftBound    New left bound value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setLeftBound(string const& portName, string const& newLeftBound);

    /*!
     *  Get the calculated right bound value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated right bound value of the selected port.
     */
    string getRightBoundValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted right bound expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted right bound expression of the selected port.
     */
    string getRightBoundFormattedExpression(string const& portName) const;

    /*!
     *  Get the right bound expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Right bound expression of the selected port.
     */
    string getRightBoundExpression(string const& portName) const;
    
    /*!
     *  Set a new right bound value for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newRightBound   New right bound value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setRightBound(string const& portName, string const& newRightBound);

    /*!
     *  Check if the selected port has an expression in the left or right bound values.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the port has an expression in the left or right bound values, false otherwise.
     */
    bool hasExpressionInLeftOrRightBound(string const& portName) const;

    /*!
     *  Get the calculated width of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Calculated width of the selected port.
     */
    string getWidth(string const& portName) const;

    /*!
     *  Set new values for the left and right bounds of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] newWidth    New width.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setWidth(string const& portName, string const& newWidth);

    /*!
     *  Get the calculated default value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated default value of the selected port.
     */
    string getDefaultValue(string const& portName, int const& baseNumber = 0) const;
    
    /*!
     *  Get the formatted default value expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted default value expression of the selected port.
     */
    string getDefaultValueFormattedExpression(string const& portName) const;

    /*!
     *  Get the default value expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Default value expression of the selected port.
     */
    string getDefaultValueExpression(string const& portName) const;
    
    /*!
     *  Set a new default value for the selected port.
     *
     *      @param [in] portName            Name of the selected port.
     *      @param [in] newDefaultValue     New default value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setDefaultValue(string const& portName, string const& newDefaultValue);

    /*!
     *  Get the calculated bus width value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated bus width value of the selected port.
     */
    string getBusWidthValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted bus width expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted bus width expression of the selected port.
     */
    string getBusWidthFormattedExpression(string const& portName) const;

    /*!
     *  Get the bus width expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Bus width expression of the selected port.
     */
    string getBusWidthExpression(string const& portName) const;

    /*!
     *  Set a new bus width for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newBusWidth     New bus width.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setBusWidth(string const& portName, string const& newBusWidth);

    /*!
     *  Get the initiative of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Initiative of the selected port.
     */
    string getInitiative(string const& portName) const;

    /*!
     *  Set initiative for the selected port.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] newInitiative   New initiative.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setInitiative(string const& portName, string const& newInitiative);

    /*!
     *  Get the kind of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Kind of the selected port.
     */
    string getKind(string const& portName) const;

    /*!
     *  Set kind for the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] newKind     New kind.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setKind(string const& portName, string const& newKind);

    /*!
     *  Get the protocol type of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Protocol type of the selected port.
     */
    string getProtocolType(string const& portName) const;

    /*!
     *  Set protocol type for the selected port.
     *
     *      @param [in] portName            Name of the selected port.
     *      @param [in] newProtocolType     New protocol type.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setProtocolType(string const& portName, string const& newProtocolType);

    /*!
     *  Get the calculated max connections value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated max connections value of the selected port.
     */
    string getMaxConnectionsValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted max connections expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted max connections expression of the selected port.
     */
    string getMaxConnectionsFormattedExpression(string const& portName) const;

    /*!
     *  Get the max connections expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Max connections expression of the selected port.
     */
    string getMaxConnectionsExpression(string const& portName) const;

    /*!
     *  Set a new max connections value for the selected port.
     *
     *      @param [in] portName            Name of the selected port.
     *      @param [in] newMaxConnections   New max connections value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMaxConnections(string const& portName, string const& newMaxConnections) const;

    /*!
     *  Get the calculated min connections value of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *      @param [in] baseNumber  Base for displaying the value.
     *
     *      @return Calculated min connections value of the selected port.
     */
    string getMinConnectionsValue(string const& portName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted min connections expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Formatted min connections expression of the selected port.
     */
    string getMinConnectionsFormattedExpression(string const& portName) const;

    /*!
     *  Get the min connections expression of the selected port.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return Min connections expression of the selected port.
     */
    string getMinConnectionsExpression(string const& portName) const;

    /*!
     *  Set a new min connections value for the selected port.
     *
     *      @param [in] portName            Name of the selected port.
     *      @param [in] newMinConnections   New max connections value.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMinConnections(string const& portName, string const& newMinConnections) const;

	/*!
	 *  Add a wire port.
	 *
	 *      @param [in] newPortName     Name of the new port.
	 */
	void addWirePort(string const& newPortName = string(""));

    /*!
     *  Add a transactional port.
     *
     *      @param [in] newPortName     Name of the new port.
     */
    void addTransactionalPort(string const& newPortName = string(""));

    /*!
     *  Remove the selected port.
	 *
	 *      @param [in] portName    Name of the selected port.
     */
    bool removePort(string const& portName);

    /*!
     *  Check if the selected port has a valid left array value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the left array value is valid, false otherwise.
     */
    bool portLeftArrayValueIsValid(string const& portName) const;

    /*!
     *  Check if the selected port has a valid right array value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the right array value is valid, false otherwise.
     */
    bool portRightArrayValueIsValid(string const& portName) const;

    /*!
     *  Check if the selected port has valid types.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the types is valid, false otherwise.
     */
    bool portHasValidTypes(string const& portName) const;

    /*!
     *  Check if the selected port is a wire.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port is a wire, false otherwise.
     */
    bool portIsWire(string const& portName) const;

    /*!
     *  Check if the selected port has a valid left bound value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the left bound value is valid, false otherwise.
     */
    bool portHasValidLeftBound(string const& portName) const;

    /*!
     *  Check if the selected port has a valid right bound value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the right bound value is valid, false otherwise.
     */
    bool portHasValidRightBound(string const& portName) const;

    /*!
     *  Check if the selected port has a valid default value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the default value is valid, false otherwise.
     */
    bool portHasValidDefaultValue(string const& portName) const;

    /*!
     *  Check if the selected port is transactional.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port is transactional, false otherwise.
     */
    bool portIsTransactional(string const& portName) const;

    /*!
     *  Check if the selected port has a valid bus width.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the bus width is valid, false otherwise.
     */
    bool portHasValidBusWidth(string const& portName) const;

    /*!
     *  Check if the selected port has a valid initiative.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the initiative is valid, false otherwise.
     */
    bool portHasValidInitiative(string const& portName) const;

    /*!
     *  Check if the selected port has a valid kind.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the kind is valid, false otherwise.
     */
    bool portHasValidKind(string const& portName) const;

    /*!
     *  Check if the selected port has a valid protocol.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the protocol is valid, false otherwise.
     */
    bool portHasValidProtocol(string const& portName) const;

    /*!
     *  Check if the selected port has a valid max connections value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the max connections value is valid, false otherwise.
     */
    bool portHasValidMaxConnections(string const& portName) const;

    /*!
     *  Check if the selected port has a valid min connections value.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the min connections value is valid, false otherwise.
     */
    bool portHasValidMinConnections(string const& portName) const;

    //! No copying. No assignment.
    PortsInterface(const PortsInterface& other) = delete;
    PortsInterface& operator=(const PortsInterface& other) = delete;

private:

    /*!
     *  Get the port with the selected name.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return The port with the selected name.
     */
    QSharedPointer<Port> getPort(string const& portName) const;

    /*!
     *  Set the type name and definition of a port.
     *
     *      @param [in] port    The selected port.
     */
    void setTypeNameAndDefinition(QSharedPointer<Port> port);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the available ports.
    QSharedPointer<QList<QSharedPointer<Port> > > ports_;

    //! The validator used for ports.
    QSharedPointer<PortValidator> portValidator_;
};

#endif // PORTSMODEL_H
