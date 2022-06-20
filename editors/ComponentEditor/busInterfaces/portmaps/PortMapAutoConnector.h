//-----------------------------------------------------------------------------
// File: PortMapAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
class AbstractionDefinition;
class VLNV;
class LibraryInterface;
class Port;
class PortAbstraction;
class ExpressionParser;
class PortMap;
class AbstractionType;
class PortMapInterface;
class PortAbstractionInterface;

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
     *      @param [in] portMapInterface    Interface for accessing port maps.
     *      @param [in] parent              The owner of this object.
     */
    PortMapAutoConnector(PortMapInterface* portMapInterface,
        QObject* parent);

    /*!
     *  The destructor.
     */
    ~PortMapAutoConnector();

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

    /*!
     *  Sets a prefix for matching physical names to logical ports.
     *
     *      @param [in] prefix   The prefix to set.
     */
    void setPrefix(QString const& prefix);

signals:

    /*!
     *  Inform of a new port map.
     *
     *      @param [in] physicalPorts   List of the best matching physical ports.
     */
    void portMapsAutoConnected(QVector<QString> physicalPorts);

private:

    // Disable copying.
    PortMapAutoConnector(PortMapAutoConnector const& rhs);
    PortMapAutoConnector& operator=(PortMapAutoConnector const& rhs);

    //! Structure for possible logical-physical pairs.
    struct PossiblePortMaps
    {
        //! The logical signal name.
        QString logicalPort_;

        //! Possible physical ports in the order of their weights.
        QMap<double, QString> possiblePhysicals_;
    };

    /*!
     *  Connect the selected logical signals.
     *
     *      @param [in] logicalPorts    List of the selected logical signals.
     */
    void connectSelectedLogicalPorts(QStringList const& logicalPorts);

    /*!
     *  Get the possible physical ports for the selected logical port.
     *
     *      @param [in] logicalPort         The selected logical port.
     *      @param [in] logicalInterface    Interface for accessing logical ports.
     *
     *      @return The possible physical ports combined with weights.
     */
    QMap<double, QString> getWeightedPhysicalPorts(std::string const& logicalPort,
        PortAbstractionInterface* logicalInterface) const;

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
    QString getBestMatchingPhysicalPort(int logicalIndex,
        QList<PossiblePortMaps> const& possiblePairings) const;

    /*!
     *  Get a list of possible weighted ports by the direction of the given logical direction.
     *
     *      @param [in] direction     Direction of the selected logical port.
     *
     *      @return A list of weighted physical ports with a suitable direction to the logical port.
     */
    QMap<QString, double> getPortsByDirection(DirectionTypes::Direction logicalDirection) const;

    /*!
     *  Get a list of physical ports weighted by the width of the logical port.
     *
     *      @param [in] logicalWidth    The width of the selected logical port.
     *      @param [in] portList        Names of the available weighted physical ports.
     *
     *      @return A list of physical ports weighted by suitability to logical width.
     */
    QMap<QString, double> getPortsByLogicalWidth(double logicalWidth, QMap<QString, double> portList) const;

    /*!
     *  Get a list of physical ports weighted by the name of the logical port.
     *
     *      @param [in] logicalName     The name of the logical port.
     *      @param [in] portList        A list of available weighted physical ports.
     *
     *      @return A list of physical ports weighted by the name of the logical port.
     */
    QMap<QString, double> weightPortsByLogicalName(QString const& logicalName, QMap<QString, double> portList) const;

    /*!
     *  Reorder the ports according to their weights.
     *
     *      @param [in] portList    A list of weighter physical ports.
     *
     *      @return A list of port names reordered according to the weights.
     */
    QStringList reorderPortsToWeight(QMap<QString, double> portList) const;

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! Prefix for physical ports.
    QString physicalPrefix_;

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPAUTOCONNECTOR_H
