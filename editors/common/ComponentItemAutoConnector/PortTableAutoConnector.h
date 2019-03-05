//-----------------------------------------------------------------------------
// File: PortTableAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Automatically connects ports of two components.
//-----------------------------------------------------------------------------

#ifndef PORTTABLEAUTOCONNECTOR_H
#define PORTTABLEAUTOCONNECTOR_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <editors/common/ComponentItemAutoConnector/TableAutoConnector.h>

class Component;
class Port;

//-----------------------------------------------------------------------------
//! Automatically connects ports of two components.
//-----------------------------------------------------------------------------
class PortTableAutoConnector : public TableAutoConnector
{
public:

    /*!
     *  The constructor.
     */
    PortTableAutoConnector() = default;

    /*!
     *  Destructor.
     */
    virtual ~PortTableAutoConnector() = default;

    // No copying. No assignments.
    PortTableAutoConnector(PortTableAutoConnector const& rhs) = delete;
    PortTableAutoConnector& operator=(PortTableAutoConnector const& rhs) = delete;

protected:

    /*!
     *  Create a table item.
     *
     *      @param [in] itemName                Name for the table item.
     *      @param [in] containingComponent     Component containing the selected item.
     *
     *      @return The new table item.
     */
    virtual QTableWidgetItem* createTableWidgetItem(QString const& itemName,
        QSharedPointer<Component> containingComponent) const;

private:

    /*!
     *  Find the possible item connections from the selected components.
     *
     *      @param [in] firstComponent      The first component.
     *      @param [in] secondComponent     The second component.
     *
     *      @return List of item / possible connected items pairs.
     */
    virtual QVector<QPair<QString, QVector<QString> > > findPossibleCombinations(
        QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent) const;

    /*!
     *  Find the names of the ports possible to be connected to the selected port.
     *
     *      @param [in] protDirection       Direction of the selected port.
     *      @param [in] secondItemPorts     List of the ports contained within the second component.
     *
     *      @return List of connectible port names.
     */
    QVector<QString> getConnectablePortNames(DirectionTypes::Direction portDirection,
        QSharedPointer<QList<QSharedPointer<Port> > > secondItemPorts) const;

    /*!
     *  Get connectible port directions for the selected port direction.
     *
     *      @param [in] portDirection   The selected port direction.
     *
     *      @return List of connectible port directions.
     */
    virtual QVector<DirectionTypes::Direction> getConnectableDirections(DirectionTypes::Direction portDirection)
        const;
};

//-----------------------------------------------------------------------------

#endif // PORTTABLEAUTOCONNECTOR_H
