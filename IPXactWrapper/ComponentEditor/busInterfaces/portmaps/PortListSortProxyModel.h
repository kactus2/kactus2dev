//-----------------------------------------------------------------------------
// File: PortListSortProxyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2013
//
// Description:
// Sorting proxy model for port lists.
//-----------------------------------------------------------------------------

#ifndef PORTLISTSORTPROXYMODEL_H
#define PORTLISTSORTPROXYMODEL_H

#include <models/generaldeclarations.h>
#include <models/component.h>
#include <QSortFilterProxyModel>

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
    enum DirectionFilter
    {
        IN = General::IN,
        OUT = General::OUT,
        INOUT = General::INOUT,
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
    ~PortListSortProxyModel();    

    /*!
	 *  Gets the filter for direction of the ports.
	 *
	 *      @return The accepted direction.
	 */
    DirectionFilter filterDirection() const;



	 /*!
	 *  Sets direction for filtering.
	 *
	 *      @param [in] direction  The accepted direction.
	 */
    void setFilterDirection(DirectionFilter direction);

    /*!
	 *  Gets the filter for hiding connected ports.
	 *
	 *      @return True, if ports are hidden, otherwise false.
	 */
    bool filterHideConnected() const;


public slots:

	 /*!
	 *  Sets direction for filtering.
	 *
	 *      @param [in] direction  The accepted direction.
	 */
    void setFilterDirection(QString const& direction);
	 
    /*!
	 *  Sets to show or hide already connected ports.
	 *
	 *      @param [in] hide  True, if ports are hidden, otherwise false.
	 */
    void setFilterHideConnected(bool hide = true);

    /*!
	 *  Called when port mappings have changed.
	 */    
    virtual void onConnectionsChanged();

protected:

    /*!
	 *  Implementation of the row filtering.
	 *
	 *      @param [in] source_row      The row to check for filtering.
     *      @param [in] source_parent   The parent index of the row.
     *
     *      @return True, if the row passes the filters, otherwise false.
	 */
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:
    //! No copying
    PortListSortProxyModel(const PortListSortProxyModel& other);

    //! No assignment
    PortListSortProxyModel& operator=(const PortListSortProxyModel& other);  

    //! The owner component of the ports.
    QSharedPointer<Component> component_;

    //! Filter for allowed direction.
    DirectionFilter filterDirection_;

    //! Filter for connected ports.
    bool hideConnected_; 

    //! Already connected ports.
    QStringList connectedPorts_;
};

#endif // PORTLISTSORTPROXYMODEL_H