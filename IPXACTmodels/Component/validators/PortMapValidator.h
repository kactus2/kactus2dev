//-----------------------------------------------------------------------------
// File: PortMapValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.12.2015
//
// Description:
// Validator for the ipxact:portMap.
//-----------------------------------------------------------------------------

#ifndef PORTMAPVALIDATOR_H
#define PORTMAPVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>
#include <QString>

class AbstractionDefinition;
class LibraryInterface;
class ExpressionParser;
class ConfigurableVLNVReference;
class Port;
class PortAbstraction;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:portMap.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortMapValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser                  The parse to use for solving expressions.
     *      @param [in] ports                   The available ports.
     *      @param [in] libraryHandler          The library interface.
     */
    PortMapValidator(QSharedPointer<ExpressionParser> parser,
        QSharedPointer<QList<QSharedPointer<Port> > > ports,
        LibraryInterface* libraryHandler);

	//! The destructor.
	~PortMapValidator() = default;
    
    // Disable copying.
    PortMapValidator(PortMapValidator const& rhs) = delete;
    PortMapValidator& operator=(PortMapValidator const& rhs) = delete;

    /*!
     *  Handle the change in bus interface.
     *
     *      @param [in] newAbstractionReference     Abstraction definition of the new bus interface.
     *      @param [in] newInterfaceMode            Interface mode of the new bus interface.
     *      @param [in] newSystemGroup              Used system group of the new bus interface.
     */
    void busInterfaceChanged(QSharedPointer<ConfigurableVLNVReference> newAbstractionReference,
        General::InterfaceMode newInterfaceMode,
        QString const& newSystemGroup);

    /*!
     *  Handle the change of abstraction definition.
     *
     *      @param [in] newDefinition       The new abstraction definition.
     *      @param [in] newInterfaceMode    The new interface mode.
     */
    void abstractionDefinitionChanged(QSharedPointer<AbstractionDefinition> newDefinition,
        General::InterfaceMode newInterfaceMode);

    /*!
     *  Handle the change in component.
     *
     *      @param [in] newPorts    List of new ports.
     */
    void componentChanged(QSharedPointer<QList<QSharedPointer<Port> > > newPorts);

    /*!
     *  Validates the given port map.
     *
     *      @param [in] portMap     The selected port map.
     *
     *      @return True, if the port map is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<PortMap> const& portMap) const;

    /*!
     *  Check if the is present value is valid.
     *
     *      @param [in] portMap     The selected port map.
     *
     *      @return True, if the is present is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<PortMap> const& portMap) const;

    /*!
     *  Check if the logical port is valid.
     *
     *      @param [in] portMap     The selected port map.
     *
     *      @return True, if the logical port is valid, otherwise false.
     */
    bool hasValidLogicalPort(QSharedPointer<PortMap> const& portMap) const;

    /*!
     *  Check if the physical port is valid.
     *
	 *      @param [in] portMap         The selected port map.
	 *      @param [in] physicalPort    The physical port to check.
     *
     *      @return True, if the physical port is valid, otherwise false.
     */
    bool hasValidPhysicalPort(QSharedPointer<PortMap> const& portMap,
		QSharedPointer<Port> const& physicalPort) const;

    /*!
     *  Check if the tie off value is valid in the selected port map.
     *
     *      @param [in] portMap     The selected port map.
     *
     *      @return True, if the tie off value is valid, otherwise false.
     */
    bool hasValidTieOff(QSharedPointer<PortMap> const& portMap) const;

    /*!
     *  Check if the connected ports have valid directions.
     *
	 *      @param [in] logicalPort     The logical port to check.
	 *      @param [in] physicalPort    The physical port to check.
     *
     *      @return True, if the directions are valid, otherwise false.
     */
	bool connectedPortsHaveValidDirections(QSharedPointer<PortAbstraction> const& logicalPort,
		QSharedPointer<Port> const& physicalPort) const;

    /*!
     *  Check if the connected ports have valid port types.
     *
	 *      @param [in] logicalPort     The logical port to check.
	 *      @param [in] physicalPort    The physical port to check.
     *
     *      @return True, if the port types are valid, otherwise false.
     */
	bool connectedPortsHaveValidPortTypes(QSharedPointer<PortAbstraction> const& logicalPort,
		QSharedPointer<Port> const& physicalPort) const;

    /*!
     *  Check if the connected ports have the same range.
     *
     *      @param [in] portMap     The selected port map.
     *
     *      @return True, if the connected ports have the same range, otherwise false.
     */
    bool connectedPortsHaveSameRange(QSharedPointer<PortMap> const& portMap) const;

    /*!
     *  Locate errors within a port map.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] portMap     The selected port map.
     *      @param [in] context     Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<PortMap> portMap, QString const& context)
        const;

private:

    /*!
     * Checks if the logical and physical port have valid boundaries.
     *
     *     @param [in] logicalPort  The logical signal in abstraction to check.
     *     @param [in] logical      The logical port to check.
     *     @param [in] physicalPort The physical port in the component to check.
     *     @param [in] physical     The physical port to check.
     *
     *     @return True, if the boundaries are valid, otherwise false.
     */
     bool connectedPortsHaveValidBoundaries(QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<PortMap::LogicalPort> logical,
        QSharedPointer<Port> physicalPort,
        QSharedPointer<PortMap::PhysicalPort> physical) const;

    /*!
     * Checks if the logical port range is valid.
     *
     *     @param [in] logicalPort      The logical port to check.
     *     @param [in] referencedPort   The logical signal in abstraction to check.
     *
     *     @return True, if the range is valid, otherwise false.
     */
     bool logicalPortHasValidRange(QSharedPointer<PortMap::LogicalPort> const & logicalPort, 
         QSharedPointer<PortAbstraction> const & referencedPort) const;

    /*!
     *  Check if the physical port has a valid part select.
     *
     *      @param [in] physicalPort    The selected physical port.
     *
     *      @return True, if the physical port part select is valid, otherwise false.
     */
    bool physicalPortHasValidPartSelect(QSharedPointer<PortMap::PhysicalPort> const& physicalPort) const;

    /*!
     *  Check if the physical port range is within the referenced port.
     *
     *      @param [in] referencedPort  The referenced component port.
     *      @param [in] physicalPort    The selected physical port.
     *
     *      @return True, if the physical port range is within the referenced port, otherwise false.
     */
    bool physicalPortRangeIsWithinReferencedPort(QSharedPointer<Port> const& referencedPort,
        QSharedPointer<PortMap::PhysicalPort>  const& physicalPort) const;

    /*!
     *  Find errors within port map isPresent.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] portMap     The selected port map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<PortMap> portMap, QString const& context)
        const;

    /*!
     *  Find errors within the logical port.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] logicalPort     The selected logical port.
	 *      @param [in] abstractPort    The abstraction for the logical port.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInLogicalPort(QVector<QString>& errors,
        QSharedPointer<PortMap::LogicalPort> logicalPort, QString const& context) const;

    /*!
     *  Find errors within the physical port.
     *
     *      @param [in] errors          List of found errors.
	 *      @param [in] portMap         The selected port map.
	 *      @param [in] physicalPort    The physical port to check.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInPhysicalPort(QVector<QString>& errors, QSharedPointer<PortMap> portMap,
		QSharedPointer<Port> physicalPort, QString const& context) const;

    /*!
     *  Find errors within the mapping.
     *
     *      @param [in] errors          List of found errors.
	 *      @param [in] portMap         The selected port map.
	 *      @param [in] logicalPort     The logical port abstraction.
	 *      @param [in] physicalPort    The physical port to check.
     *      @param [in] context         Context to help locate the error.
     */
	void findErrorsInPortConnection(QVector<QString>& errors, 
		QSharedPointer<PortMap> portMap, 
		QSharedPointer<PortAbstraction> logicalPort,
		QSharedPointer<Port> physicalPort,
        QString const& context) const;

    /*!
     * Parse expression and check if it is valid.
     *
     *     @param [in] expression  The expression to parse
     *
     *     @return Indication if the expression is valid and the parsed value.
     */
     std::pair<bool, quint64> checkAndParseExpression(QString const & expression) const;

    /*!
     *  Get the referenced logical port.
     *
     *      @param [in] portName    The name of the referenced logical port.
     *
     *      @return The referenced logical port.
     */
    QSharedPointer<PortAbstraction> findLogicalPort(QString const& portName) const;

    /*!
     *  Find the referenced physical port.
     *
     *      @param [in] portName    The name of the referenced physical port.
     *
     *      @return The referenced physical port.
     */
    QSharedPointer<Port> findPhysicalPort(QString const& portName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The currently available ports.
    QSharedPointer<QList<QSharedPointer<Port> > > availablePorts_;

	//! The used abstraction definition.
    QSharedPointer<AbstractionDefinition const> abstractionDefinition_;

    //! Interface mode of the containing bus interface.
    General::InterfaceMode interfaceMode_;

    //! The system group name in case of system mode.
    QString systemGroup_;

    //! The library interface.
    LibraryInterface* libraryHandler_;
};

#endif // PORTMAPVALIDATOR_H
