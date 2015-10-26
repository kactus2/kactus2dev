//-----------------------------------------------------------------------------
// File: physlistmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.05.2011
//
// Description:
// Model to display the physical ports of a component.
//-----------------------------------------------------------------------------

#ifndef PHYSLISTMODEL_H
#define PHYSLISTMODEL_H

#include "portlistmodel.h"

#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Model to display the physical ports of a component.
//-----------------------------------------------------------------------------
class PhysListModel : public PortListModel {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component       The component that's ports are displayed.
	 *      @param [in] parent          The owner of this model.
	 */
	PhysListModel(QSharedPointer<Component> component, QObject *parent);
	
	//! The destructor
	virtual ~PhysListModel();

	/*! Refresh the list of physical ports.
	 *
	*/
	virtual void refresh();

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return The requested data.
	*/
	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

private:

	//! No copying
	PhysListModel(const PhysListModel& other);
	PhysListModel& operator=(const PhysListModel& other);
	
	//! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // PHYSLISTMODEL_H
