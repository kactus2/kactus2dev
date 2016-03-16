//-----------------------------------------------------------------------------
// File: logicallistmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.5.2011
//
// Description:
// Model to display the Logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef LOGICALLISTMODEL_H
#define LOGICALLISTMODEL_H

#include "portlistmodel.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/VLNV.h>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Model to display the Logical ports of an abstraction definition.
//-----------------------------------------------------------------------------
class LogicalListModel : public PortListModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] libHandler      The instance that handles library.
	 *      @param [in] parent          The owner of this model.
	*/
	LogicalListModel(LibraryInterface* libHandler, QObject *parent);
	
	//! The destructor
	virtual ~LogicalListModel();

	/*! Set the abstraction type that defines logical signals.
	 *
	 *      @param [in] vlnv The vlnv identifying the abstraction definition.
	 */
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

	//! Refresh the list of ports.
	virtual void refresh();

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in] index Identifies the item that's data is wanted.
	 *      @param [in] role Specifies what kind of data is wanted
	 *
	 *      @return The requested data.
	*/
	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

private:

	//! No copying
	LogicalListModel(const LogicalListModel& other);
	LogicalListModel& operator=(const LogicalListModel& other);

	//! The instance that manages the library
	LibraryInterface* libHandler_;

	//! The vlnv of the currently used abstraction type.
	VLNV absTypeVLNV_;

	//! Specifies the interface mode of this bus interface.
	General::InterfaceMode mode_;
};

#endif // LOGICALLISTMODEL_H
