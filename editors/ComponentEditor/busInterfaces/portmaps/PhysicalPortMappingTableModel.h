//-----------------------------------------------------------------------------
// File: PhysicalPortMappingTableModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// Model to display the physical ports of a component in port mapping.
//-----------------------------------------------------------------------------

#ifndef PHYSICALPORTMAPPINGTABLEMODEL_H
#define PHYSICALPORTMAPPINGTABLEMODEL_H

#include "PortMappingTableModel.h"

#include <QAbstractTableModel>
#include <QSharedPointer>

class Component;
class Port;
class ExpressionParser;
class PortsInterface;

//-----------------------------------------------------------------------------
//! Model to display the physical ports of a component in port mapping.
//-----------------------------------------------------------------------------
class PhysicalPortMappingTableModel : public PortMappingTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] portInterface   Interface for accessing ports.
	 *      @param [in] parent          The owner of this model.
	 */
    PhysicalPortMappingTableModel(PortsInterface* portInterface, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~PhysicalPortMappingTableModel();;

	/*!
     *  Refresh the list of physical ports.
	 */
	virtual void refresh();

    /*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid because this is not
     *                              a hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return The requested data.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

private:

	//! No copying.
	PhysicalPortMappingTableModel(const PhysicalPortMappingTableModel& other);
	PhysicalPortMappingTableModel& operator=(const PhysicalPortMappingTableModel& other);
	
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing ports.
    PortsInterface* portInterface_;
};

#endif // PHYSICALPORTMAPPINGTABLEMODEL_H
