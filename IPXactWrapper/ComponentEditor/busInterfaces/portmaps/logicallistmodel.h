/* 
 *
 *  Created on: 11.5.2011
 *      Author: Antti Kamppi
 * 		filename: logicallistmodel.h
 */

#ifndef LOGICALLISTMODEL_H
#define LOGICALLISTMODEL_H

#include "portlistmodel.h"

#include <models/generaldeclarations.h>
#include <LibraryManager/vlnv.h>

class LibraryInterface;
class PortMapsTreeModel;

/*! \brief Model to display the Logical ports of an abstraction definition.
 *
 */
class LogicalListModel : public PortListModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that handles library.
	 * \param portMapsModel Pointer to the port maps model to ask if connection is possible. 
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	LogicalListModel(LibraryInterface* libHandler,
		PortMapsTreeModel* portMapsModel,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~LogicalListModel();

	/*! \brief Set the abstraction type that defines logical signals.
	 *
	 * \param vlnv The vlnv identifying the abstraction definition.
	 *
	*/
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

	/*! \brief Refresh the list of ports.
	 *
	*/
	virtual void refresh();

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

private:

	//! \brief No copying
	LogicalListModel(const LogicalListModel& other);

	//! No assignment
	LogicalListModel& operator=(const LogicalListModel& other);

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* libHandler_;

	//! \brief The vlnv of the currently used abstraction type.
	VLNV absTypeVLNV_;

	//! \brief Specifies the interface mode of this bus interface.
	General::InterfaceMode mode_;
};

#endif // LOGICALLISTMODEL_H
