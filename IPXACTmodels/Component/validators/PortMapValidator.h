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

	/*!
     *  The destructor.
     */
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
    void abstractionDefinitionChanged(QSharedPointer<AbstractionDefinition const> newDefinition,
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
     *  Check if the selected logical port is valid.
     *
     *      @param [in] logical     The selected logical port of a port map.
     *
     *      @return True, if the logical port is valid, otherwise false.
     */
    bool hasValidLogicalPort(QSharedPointer<PortMap::LogicalPort> const& logical) const;

    /*!
     *  Check if the port map has a valid physical port or a valid tie off.
     *
     *      @param [in] portMap     The selected port map.
     *
     *      @return True, if the physical mapping is valid, otherwise false.
     */
    bool hasValidPhysicalMapping(QSharedPointer<PortMap> const& portMap) const;

    /*!
     *  Check if the selected physical port is valid.
     *
	 *      @param [in] physical    The selected physical port of a port map.
     *
     *      @return True, if the physical port is valid, otherwise false.
     */
    bool hasValidPhysicalPort(QSharedPointer<PortMap::PhysicalPort> const& physical) const;

    /*!
     *  Check if the tie off value is valid.
     *
     *      @param [in] tieOff  The selected tie off value.
     *
     *      @return True, if the tie off value is valid, otherwise false.
     */
    bool hasValidTieOff(QString const& tieOff) const;

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
     *  Check if the connected ports have valid initiatives.
     *
     *      @param [in] logicalPort     The logical port to check.
     *      @param [in] physicalPort    The physical port to check.
     *
     *      @return True, if the initiatives are valid, otherwise false.
     */
    bool connectedPortsHaveValidInitiatives(QSharedPointer<PortAbstraction> const& logicalPort,
        QSharedPointer<Port> const& physicalPort) const;

    /*!
     * Checks if the logical port has a valid range.
     *
     *     @param [in] logicalRange     The logical port range to check.
     *     @param [in] referencedPort   The logical signal in the referenced abstraction.
     *
     *     @return True, if the range is valid, otherwise false.
     */
    bool logicalPortHasValidRange(QSharedPointer<Range> const & logicalRange,
        QSharedPointer<PortAbstraction> const & referencedPort) const;

    /*!
     *  Check if the physical port has a valid part select.
     *
     *      @param [in] physicalPart    The selected part select.
     *      @param [in] referencedPort  Port referenced by the physical port.
     *
     *      @return True, if the part select is valid, otherwise false.
     */
    bool physicalPortHasValidPartSelect(QSharedPointer<PartSelect> const& physicalPart,
        QSharedPointer<Port> const& referencedPort) const;

    /*!
     *  Check if the connected ports have the same range.
     *
     *      @param [in] logicalRange    The logical range of the port map.
     *      @param [in] physicalPart    The physical part selected of the port map.
     *
     *      @return True, if the connected ports have the same range, otherwise false.
     */
    bool connectedPortsHaveSameRange(QSharedPointer<Range> const& logicalRange,
        QSharedPointer<PartSelect> const& physicalPart) const;

    /*!
     *  Locate errors within a port map.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] portMap     The selected port map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<PortMap> portMap, QString const& context)
        const;

private:

    /*!
     *  Checks if both of the given range values exist.
     *
     *      @param [in] leftRange   The selected left range.
     *      @param [in] rightRange  The selected right range.
     *
     *      @return True, if both of the range values exist.
     */
    bool bothRangeValuesExist(QString const& leftRange, QString const& rightRange) const;

    /*!
     *  Get the bus width of a logical signal.
     *
     *      @param [in] logicalPort     The selected logical signal.
     *
     *      @return The bus width of the selected logical signal.
     */
    quint64 getLogicalPortWidth(QSharedPointer<PortAbstraction> const& logicalPort) const;

    /*!
     *  Check if the range is within the selected width.
     *
     *      @param [in] rangeLeft   Left range value.
     *      @param [in] rangeRight  Right range value.
     *      @param [in] width       The selected width.
     *
     *      @return True, if the range is within the selected width, false otherwise.
     */
    bool rangeIsWithinWidth(qint64 const& rangeLeft, qint64 const& rangeRight, quint64 const& width) const;

    /*!
     *  Check if the range is valid in a wire port.
     *
     *      @param [in] rangeLeft   Left range value.
     *      @param [in] rangeRight  Right range value.
     *      @param [in] wireLeft    Left limit of the wire.
     *      @param [in] wireRight   Right limit of the wire.
     *
     *      @return True, if the range is valid in the wire limits, false otherwise.
     */
    bool rangeIsValidInWire(quint64 const& rangeLeft, quint64 const& rangeRight, quint64 const& wireLeft,
        quint64 const& wireRight) const;

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
	 *      @param [in] referencedPort  The abstraction for the logical port.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInLogicalPort(QVector<QString>& errors, QSharedPointer<PortMap::LogicalPort> logicalPort,
        QSharedPointer<PortAbstraction> referencedPort, QString const& context) const;

    /*!
     *  Find errors within the logical tie off.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] tieOff      The selected tie off value.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInTieOff(QVector<QString>& errors, QString const& tieOff, QString const& context) const;

    /*!
     *  Find errors within the physical port.
     *
     *      @param [in] errors          List of found errors.
	 *      @param [in] physicalPort    The physical port to check.
     *      @param [in] referencedPort  The component port referenced by the physical port.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInPhysicalPort(QVector<QString>& errors, QSharedPointer<PortMap::PhysicalPort> physicalPort,
        QSharedPointer<Port> referencedPort, QString const& context) const;

    /*!
     *  Find errors within the mapping.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] logical         The mapped logical port.
     *      @param [in] physical        The mapped physical port.
	 *      @param [in] logicalPort     The logical port abstraction.
	 *      @param [in] physicalPort    The physical port to check.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInPortConnection(QVector<QString>& errors, QSharedPointer<PortMap::LogicalPort> logical,
        QSharedPointer<PortMap::PhysicalPort> physical, QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<Port> physicalPort, QString const& context) const;

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
