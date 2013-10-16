/* 
 *
 *  Created on: 10.5.2011
 *      Author: Antti Kamppi
 * 		filename: portlistmodel.h
 */

#ifndef PORTLISTMODEL_H
#define PORTLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class PortMapsTreeModel;

/*! \brief The base class for logical list model and physical list model.
 *
 */
class PortListModel : public QAbstractListModel {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param portMapsModel Pointer to the port maps model to ask if connection is possible.
	 * \param parent Pointer to the owner of this model
	 *
	*/
	PortListModel(PortMapsTreeModel* portMapsModel, QObject *parent);
	
	//! \brief The destructor
	virtual ~PortListModel();

	/*! \brief Get the number of rows in the model.
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of rows currently in the model.
	*/
	int rowCount(const QModelIndex& parent = QModelIndex()) const ;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*! \brief Refresh the list of ports.
	 *
	*/
	virtual void refresh() = 0;

	/*! \brief Remove the given ports from the model
	 *
	 * \param portList List of port names to remove from the model
	 *
	*/
	virtual void removePorts(const QStringList& portList);

	/*! \brief Remove the given port from the model.
	 *
	 * \param port The name of the port
	*/
	virtual void removePort(const QString& port);

public slots:

	//! \brief Add a new port to the list of ports.
	virtual void addPort(const QString& portName);

	//! \brief Remove the port with given model index.
	virtual void removeItem(const QModelIndex& index);

	//! \brief Remove the items with given model indexes from the list.
	virtual void removeItems(const QModelIndexList& indexes);

	//! \brief Move list of ports to a given index.
	virtual void onMoveItems(const QStringList& portNames, const QModelIndex& targetIndex);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

	//! \brief Emitted to inform that a mapping should be created between ports.
	//void createMap(const QString& logicalPort, const QString& physicalPort);

protected:

	//! \brief Contains the items to be displayed in the view
	QStringList list_;

	//! \brief Pointer to the port maps model to ask if connection is possible.
	PortMapsTreeModel* portMapsModel_;

private:

	//! \brief No copying
	PortListModel(const PortListModel& other);

	//! No assignment
	PortListModel& operator=(const PortListModel& other);
	
};

#endif // PORTLISTMODEL_H
