//-----------------------------------------------------------------------------
// File: AdHocVisibilityPolicy.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.05.2016
//
// Description:
// Policy handler for ad hoc port visibilities.
//-----------------------------------------------------------------------------

#ifndef ADHOCVISIBILITYPOLICY_H
#define ADHOCVISIBILITYPOLICY_H

#include <QSharedPointer>

class AdHocEnabled;
class AdHocConnection;
class Port;

//-----------------------------------------------------------------------------
//! Policy handler for ad hoc port visibilities.
//-----------------------------------------------------------------------------
class AdHocVisibilityPolicy
{

public:

	/*!
     *  The constructor.
	 */
	AdHocVisibilityPolicy();
	
	/*!
     *  The destructor.
     */
	~AdHocVisibilityPolicy();

    /*!
     *  Sets the ad-hoc port visibility data source being edited.
     *
     *      @param [in] dataSource      The data source.
     *      @param [in] connections     List of ad hoc connections within the design.
     */
    void setDataSource(AdHocEnabled* dataSource,
        QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections);

    /*!
     *  Check if the visibility can be changed for the selected port.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the visibility can be changed, false otherwise.
     */
    bool canChangeVisibility(QSharedPointer<Port> port) const;

    /*!
     *  Check if the selected port has connections.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the selected port has been connected, otherwise false.
     */
    bool portHasConnections(QSharedPointer<Port> port) const;

    /*!
     *  Check if the selected port has a tie off value.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the selected port has a tie off value, otherwise false.
     */
    bool portHasTieOffConnection(QSharedPointer<Port> port) const;

    /*!
     *  Check if the selected port is determined to be visible within the containing component.
     *
     *      @param [in] portName    The name of the port.
     *
     *      @return True, if the selected port is determined to be visible, otherwise false.
     */
    bool isPortAdhocVisible(QString const& portName) const;

    AdHocEnabled* getDataSource();

private:

    // Disable copying.
    AdHocVisibilityPolicy(AdHocVisibilityPolicy const& rhs);
    AdHocVisibilityPolicy& operator=(AdHocVisibilityPolicy const& rhs);

    /*!
     *  Get a connection containing the tie off value of the selected port.
     *
     *      @param [in] port    The selected port.
     *
     *      @return A connection containing the tie off value of the selected port. Empty pointer if no
     *              connection is found.
     */
    QSharedPointer<AdHocConnection> getTieOffConnection(QSharedPointer<Port> port) const;

    QSharedPointer<AdHocConnection> getInstanceTieOffConnection(QSharedPointer<Port> port) const;

    QSharedPointer<AdHocConnection> getTopComponentTieOffConnection(QSharedPointer<Port> port) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose ad-hoc port visibility is being edited.
    AdHocEnabled* dataSource_;

    //! A list of ad hoc connections within the design.
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > adhocConnections_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCVISIBILITYMODEL_H