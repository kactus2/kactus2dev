//-----------------------------------------------------------------------------
// File: PortListSortProxyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.08.2013
//
// Description:
// Sorting proxy model for port lists.
//-----------------------------------------------------------------------------

#ifndef PORTLISTSORTPROXYMODEL_H
#define PORTLISTSORTPROXYMODEL_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QSortFilterProxyModel>

#include <QSharedPointer>

class Component;
class AbstractionType;

//-----------------------------------------------------------------------------
//! Sorting proxy for port list view.
//-----------------------------------------------------------------------------
class PortListSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:

    /*!
	 *  Allowed direction filters.
	 */  
    enum class DirectionFilter
    {
        IN = DirectionTypes::IN,
        OUT = DirectionTypes::OUT,
        INOUT = DirectionTypes::INOUT,
        ANY
    };

	 /*!
	 *  Constructor.
	 *
	 *      @param [in] component  The owner component of the ports.
	 *      @param [in] parent     The parent object.
	 */
    PortListSortProxyModel(QSharedPointer<Component> component, QObject *parent = 0);

	 /*!
	 *  Destructor.
	 */
    virtual ~PortListSortProxyModel() = default;    

    /*!
	 *  Gets the filter for direction of the ports.
	 *
	 *      @return The accepted direction.
	 */
    DirectionFilter filterDirection() const;

    /*!
	 *  Gets the filter for hiding connected ports.
	 *
	 *      @return True, if ports are hidden, otherwise false.
	 */
    bool filterHideConnected() const;

    /*!
     *  Gets the filter for allowed port names.
     *
     *      @return The allowed port names.
     */
    QStringList filterPortNames() const;

    /*!
     *  Setup a new abstraction type for sorting.
     *
     *      @param [in] newAbstraction  The selected abstraction type.
     */
    void setNewAbstractionType(QSharedPointer<AbstractionType> newAbstraction);

public slots:
	 
    /*!
     *  Sets the filter for port name(s).
     *
     *      @param [in] portName   The allowed port name.
     */
    virtual void setFilterPortName(QString const& portName);

    /*!
     *  Sets the filter for port names.
     *
     *      @param [in] ports   Allowed port names.
     */
    virtual void setFilterPorts( QStringList const& ports );

    /*!
     *  Sets the filter for in-direction.
     *
     *      @param [in] enabled   If true, filter using in-direction.
     */
    virtual void setFilterInDirection(bool enabled);

    /*!
     *  Sets the filter for out-direction.
     *
     *      @param [in] filterOutDirection   If true, filter using out-direction.
     */
    virtual void setFilterOutDirection(bool filterOutDirection);

    /*!
	 *  Sets to show or hide already connected ports.
	 *
	 *      @param [in] hide  True, if ports are hidden, otherwise false.
	 */
    virtual void setFilterHideConnected(bool hide = true);

    /*!
	 *  Called when port mappings have been reset.
	 */    
    virtual void onConnectionsReset();

    /*!
     *  Called to update filtering when a port has been connected in a port map.
     *
     *      @param [in] portName   The name of the connected port.
     */
    virtual void onPortConnected(QString const& portName);

    /*!
     *  Called to update filtering when a port has been disconnected from all port maps.
     *
     *      @param [in] portName   The name of the disconnected port.
     */
    virtual void onPortDisconnected(QString const& portName);

protected:

    /*!
	 *  Implementation of the row filtering.
	 *
	 *      @param [in] source_row      The row to check for filtering.
     *      @param [in] source_parent   The parent index of the row.
     *
     *      @return True, if the row passes the filters, otherwise false.
	 */
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

private:
    //! No copying
    PortListSortProxyModel(const PortListSortProxyModel& other);

    //! No assignment
    PortListSortProxyModel& operator=(const PortListSortProxyModel& other);  
    
    /*!
     *  Add connected physical ports from the selected abstraction type port mappings.
     *
     *      @param [in] abstraction     The selected abstraction type.
     */
    void addConnectedPortsFromAbstraction(QSharedPointer<AbstractionType> abstraction);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The owner component of the ports.
    QSharedPointer<Component> component_;

    //! Filter for allowed direction.
    DirectionFilter filterDirection_;

    //! Filter for connected ports.
    bool hideConnected_; 

    //! Already connected ports.
    QStringList connectedPorts_;

    //! Filter for allowed port names. If set to empty, all ports are allowed by the filter.
    QStringList filterPorts_;

    //! The connected abstraction type.
    QSharedPointer<AbstractionType> abstraction_;
};

#endif // PORTLISTSORTPROXYMODEL_H