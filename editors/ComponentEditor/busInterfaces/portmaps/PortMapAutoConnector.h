//-----------------------------------------------------------------------------
// File: PortMapAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.06.2016
//
// Description:
// Automatically forms port maps between logical and physical ports.
//-----------------------------------------------------------------------------

#ifndef PORTMAPAUTOCONNECTOR_H
#define PORTMAPAUTOCONNECTOR_H

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QObject>
#include <QSharedPointer>
#include <QMap>

class Component;
class BusInterface;
class AbstractionDefinition;
class VLNV;
class LibraryInterface;
class Port;
class PortAbstraction;
class ExpressionParser;
class PortMap;

//-----------------------------------------------------------------------------
//! Automatically forms port maps between logical and physical ports.
//-----------------------------------------------------------------------------
class PortMapAutoConnector : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component       Component containing the bus interface.
     *      @param [in] busInterface    The containing bus interface.
     *      @param [in] parser          The used expression parser.
     *      @param [in] libraryHandler  The library interface for locating required documents.
     *      @param [in] parent          The owner of this object.
     */
    PortMapAutoConnector(QSharedPointer<Component> component, QSharedPointer<BusInterface> busInterface,
        QSharedPointer<ExpressionParser> parser, LibraryInterface* libraryHandler, QObject* parent);

    /*!
     *  The destructor.
     */
    ~PortMapAutoConnector();

    /*!
     *  Set the abstraction definition.
     *
     *      @param [in] abstractionDefinitionVLNV   VLNV of the abstraction definition.
     *      @param [in] newMode                     The new interface mode.
     */
    void setAbstractionDefinition(VLNV const& abstractionDefinitionVLNV, General::InterfaceMode newMode);

public slots:

    /*!
     *  Handler for automatic connection.
     */
    void onAutoConnect();

    /*!
     *  Handler for selective automatic connection.
     *
     *      @param [in] logicalSignals  The selected logical signals to be connected.
     */
    void onAutoConnectLogicalSignals(QStringList const& logicalSignals);

signals:

    /*!
     *  Inform of a new port map.
     *
     *      @param [in] portMap     The new port map.
     */
    void portMapCreated(QSharedPointer<PortMap> portMap);

private:

    // Disable copying.
    PortMapAutoConnector(PortMapAutoConnector const& rhs);
    PortMapAutoConnector& operator=(PortMapAutoConnector const& rhs);

    //! Structure for possible logical-physical pairs.
    struct PossiblePortMaps
    {
        //! The logical signal.
        QSharedPointer<PortAbstraction> logicalPort_;

        //! Possible physical ports in the order of their weights.
        QMap<double, QString> possiblePhysicals_;
    };

    /*!
     *  Connect the selected logical signals.
     *
     *      @param [in] logicalPorts    List of the selected logical signals.
     */
    void connectSelectedLogicalPorts(QList<QSharedPointer<PortAbstraction> > logicalPorts);

    /*!
     *  Check if the logical port has been referenced in a port map.
     *
     *      @param [in] logicalName     Name of the logical port.
     *
     *      @return True, if the port has been referenced in a port map, otherwise false.
     */
    bool logicalPortHasReferencingPortMap(QString const& logicalName) const;

    /*!
     *  Get the possible physical ports for the selected logical port.
     *
     *      @param [in] logicalPort     The selected logical port.
     *
     *      @return The possible physical ports combined with weights.
     */
    QMap<double, QString> getWeightedPhysicalPorts(QSharedPointer<PortAbstraction> logicalPort) const;

    /*!
     *  Get the best matching physical port. If another logical signal has the same port with a better value, it
     *  is ignored.
     *
     *      @param [in] logicalPort         The selected logical port.
     *      @param [in] logicalIndex        Index of the logical port.
     *      @param [in] possiblePairings    A list of the possible logical-physical pairings.
     *
     *      @return Name of the best matching physical port.
     */
    QString getBestMatchingPhysicalPort(QSharedPointer<PortAbstraction> logicalPort, int logicalIndex,
        QList<PossiblePortMaps> const& possiblePairings) const;

    /*!
     *  Get a list of possible weighted ports by the direction of the given logical direction.
     *
     *      @param [in] direction     The selected logical port.
     *
     *      @return A list of weighted physical ports with a suitable direction to the logical port.
     */
    QMap<QSharedPointer<Port>, double> getPortsByDirection(DirectionTypes::Direction logicalDirection) const;

    /*!
     *  Get the width of the selected logical port.
     *
     *      @param [in] logicalPort     The selected logical port.
     *
     *      @return The width of the selected logical port.
     */
    QString getLogicalPortWidth(QSharedPointer<PortAbstraction> logicalPort) const;

    /*!
     *  Get a list of physical ports weighted by the width of the logical port.
     *
     *      @param [in] logicalWidth    The width of the selected logical port.
     *      @param [in] portList        The available weighted physical ports.
     *
     *      @return A list of physical ports weighted by suitability to logical width.
     */
    QMap<QString, double> getPortsByLogicalWidth(double logicalWidth, QMap<QSharedPointer<Port>, double> portList) const;

    /*!
     *  Get a list of physical ports weighted by the name of the logical port.
     *
     *      @param [in] logicalName     The name of the logical port.
     *      @param [in] portList        A list of available weighted physical ports.
     *
     *      @return A list of physical ports weighted by the name of the logical port.
     */
    QMap<QString, double> weightPortsByLogicalName(QString logicalName, QMap<QString, double> portList) const;

    /*!
     *  Get the port names.
     *
     *      @param [in] portList    A list of weighted ports.
     *
     *      @return A list of weighted port names.
     */
    QMap<QString, double> getPortNames(QMap<QSharedPointer<Port>, double> portList) const;

    /*!
     *  Reorder the ports according to their weights.
     *
     *      @param [in] portList    A list of weighter physical ports.
     *
     *      @return A list of port names reordered according to the weights.
     */
    QStringList reorderPortsToWeight(QMap<QString, double> portList) const;

    /*!
     *  Create a port map from the selected logical port and physical port.
     *
     *      @param [in] portAbstraction     The selected logical port.
     *      @param [in] componentPort       The selected physical port.
     */
    void connectPorts(QSharedPointer<PortAbstraction> portAbstraction, QSharedPointer<Port> componentPort);

    /*!
     *  Get the logical port with the given name.
     *
     *      @param [in] logicalName     The selected logical port name.
     *
     *      @return The selected logical port.
     */
    QSharedPointer<PortAbstraction> getLogicalPort(QString const& logicalName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! Component containing the bus interface.
    QSharedPointer<Component> component_;

    //! The containing bus interface.
    QSharedPointer<BusInterface> busInterface_;

    //! The abstraction definition referenced in the bus interface.
    QSharedPointer<AbstractionDefinition const> absDef_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> parser_;

    //! The used library handler.
    LibraryInterface* libraryHandler_;

    //! The used interface mode.
    General::InterfaceMode interfaceMode_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPAUTOCONNECTOR_H
