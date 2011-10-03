/* 
 *
 *  Created on: 12.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsmodel.h
 */

#ifndef PORTMAPSMODEL_H
#define PORTMAPSMODEL_H

#include <models/generaldeclarations.h>
#include <models/component.h>
#include <models/abstractiondefinition.h>
#include <common/utils.h>

#include <LibraryManager/libraryinterface.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>
#include <QModelIndexList>
#include <QSortFilterProxyModel>
#include <QStringList>

class LibraryInterface;
class BusIfGeneralTab;

/*! \brief Model to display port maps of a bus interface.
 *
 */
class PortMapsModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Contains the information for a signle port map.
	struct Mapping {
		
		//! \brief the name of the physical port
		QString physPort_;

		//! \brief The left bound of physical port
		int physLeft_;

		//! \brief The right bound of the physical port
		int physRight_;

		//! \brief The name of the logical port
		QString logicalPort_;

		//! \brief The left bound of the logical port
		int logicalLeft_;

		//! \brief The right bound of the logical port
		int logicalRight_;

		/*! \brief The struct constructor
		 *
		 * \param logicalPort Name of the logical port
		 * \param physicalPort Name of the physical port
		 *
		*/
		Mapping(const QString& logicalPort, const QString& physicalPort);
	};

	/*! \brief The constructor
	 *
	 * \param proxy Pointer to the proxy model that does the sorting of the items.
	 * \param component Pointer to the component that is being edited.
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this model
	 *
	*/
	PortMapsModel(QSortFilterProxyModel* proxy, 
		QSharedPointer<Component> component,
		LibraryInterface* handler,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~PortMapsModel();

	/*! \brief Set the port maps for this model.
	 *
	 * \param portMaps Pointer to the QList containing the port maps to set.
	 *
	*/
	void setPortMaps(QList<QSharedPointer<General::PortMap> >* portMaps);

	/*! \brief Get the number of rows in the model.
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the number of columns in the model
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Always returns 6
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get the data for the headers
	 *
	 * \param section The column that's header is wanted
	 * \param orientation Only Qt::Horizontal is supported
	 * \param role Specified the type of data that is wanted.
	 *
	 * \return QVariant containing the data to be displayed
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for specified item.
	 *
	 * \param index Specifies the item that's data is modified
	 * \param value The value to be set.
	 * \param role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * \return True if data was successfully set.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole );

	/*! \brief Get information on how specified item can be handled.
	 *
	 * \param index Specifies the item that's flags are wanted.
	 *
	 * \return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Check if the model is in valid state or not.
	 *
	 * \return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! \brief Write the changes to the original model.
	 *
	*/
	void apply();

	/*! \brief Restore the settings from the original model.
	 *
	*/
	void restore();

	/*! \brief Get list of logical ports in this model.
	 *
	 * \return QStringList containing the port names
	*/
	QStringList logicalPorts() const;

	/*! \brief Get list of physical ports in this model.
	 *
	 * \return QStringList containing the port names
	*/
	QStringList physicalPorts() const;

	/*! \brief Check if the two ports can be mapped together.
	 *
	 * \param physicalPort Identifies the physical port.
	 * \param logicalPort Identifies the logical port.
	 *
	 * \return bool True if port mapping can be done.
	*/
	bool canCreateMap(const QString& physicalPort, const QString& logicalPort) const;

public slots:

	//! \brief Remove the indexed item from the model
	void onRemoveItems(const QModelIndex& index);

	/*! \brief Create a port map for given ports
	 *
	 * \param physicalPort Name of the physical port
	 * \param logicalPort Name of the logical port
	 *
	*/
	void createMap(const QString& physicalPort, const QString& logicalPort);

	/*! \brief Restore the selected port back to port list.
	 *
	 * \param index Model index that identifies the selected port.
	 *
	*/
	void onRestoreItem(const QModelIndex& index);

	/*! \brief Set the abstraction definition that is used in this port map.
	 *
	 * \param vlnv Identifies the abstraction definition.
	 *
	*/
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

	//! \brief When port map is removed informs name of the logical port.
	void logicalRemoved(const QString& logicalName) const;

	//! \brief When port map is removed informs name of the physical port.
	void physicalRemoved(const QString& physicalName) const;

private:
	
	//! \brief No copying
	PortMapsModel(const PortMapsModel& other);

	//! No assignment
	PortMapsModel& operator=(const PortMapsModel& other);

	/*! \brief Check if the port map is in valid state.
	 *
	 * \param map Reference to the map that is checked.
	 *
	 * \return bool True if map is in valid state.
	*/
	bool isValid(const Mapping& map) const;

	//! \brief Pointer to the data structure within the model containing the port maps.
	QList<QSharedPointer<General::PortMap> >* portMaps_;

	//! \brief The list that contains the table to be displayed.
	QList<Mapping> table_;

	//! \brief Pointer to the proxy that does the sorting for views.
	QSortFilterProxyModel* proxy_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the abstraction definition that is used.
	QSharedPointer<AbstractionDefinition> absDef_;

	//! \brief Specifies the interface mode of this bus interface
	General::InterfaceMode interfaceMode_;
};

#endif // PORTMAPSMODEL_H

