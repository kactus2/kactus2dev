//-----------------------------------------------------------------------------
// File: portlistmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2011
//
// Description:
// The base class for logical list model and physical list model.
//-----------------------------------------------------------------------------

#ifndef PORTLISTMODEL_H
#define PORTLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

//-----------------------------------------------------------------------------
//! The base class for logical list model and physical list model.
//-----------------------------------------------------------------------------
class PortListModel : public QAbstractListModel
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] portMapsModel The port maps model to ask if connection is possible.
	 *      @param [in] parent The owner of this model
	*/
	PortListModel(QObject *parent);
	
	//! The destructor
	virtual ~PortListModel();

	/*! Get the number of rows in the model.
	 *
	 *      @param [in] parent  Index of the parent of the item.
	 *
	 *      @return Number of rows currently in the model.
	*/
	int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
	*/
	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	//! Refresh the list of ports.
	virtual void refresh() = 0;

	/*! Remove the given ports from the model
	 *
	 *      @param [in] portList List of port names to remove from the model
	 */
	virtual void removePorts(const QStringList& portList);

	/*! Remove the given port from the model.
	 *
	 *      @param [in] port The name of the port
	 */
	virtual void removePort(QString const& port);

public slots:

	//! Add a new port to the list of ports.
	virtual void addPort(QString const& portName);

	//! Remove the port with given model index.
	virtual void removeItem(QModelIndex const& index);

	//! Remove the items with given model indexes from the list.
	virtual void removeItems(const QModelIndexList& indexes);

	//! Move list of ports to a given index.
	virtual void onMoveItems(const QStringList& portNames, QModelIndex const& targetIndex);

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(QString const& msg) const;

	//! Prints a notification to user.
	void noticeMessage(QString const& msg) const;

	//! Emitted to inform that a mapping should be created between ports.
	//void createMap(QString const& logicalPort, QString const& physicalPort);

protected:

	//! Contains the items to be displayed in the view
	QStringList list_;

private:

	//! No copying
	PortListModel(const PortListModel& other);

	//! No assignment
	PortListModel& operator=(const PortListModel& other);
	
};

#endif // PORTLISTMODEL_H
