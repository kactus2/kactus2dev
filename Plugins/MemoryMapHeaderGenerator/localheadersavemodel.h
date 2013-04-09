/* 
 *	Created on: 20.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	localheadersavemodel.h
 * 	Project:	Kactus 2
*/

#ifndef LOCALHEADERSAVEMODEL_H
#define LOCALHEADERSAVEMODEL_H

#include <models/component.h>

#include <QSharedPointer>
#include <QAbstractTableModel>
#include <QFileInfo>
#include <QList>

class LibraryInterface;

/*! \brief The model class to display the header files to be created from component's local memory maps.
 *
 */
class LocalHeaderSaveModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Contains the options to identify the local memory map and its save file path.
	struct SaveFileOptions {

		//! \brief Pointer to the local memory map.
		QSharedPointer<MemoryMap> localMemMap_;

		//! \brief Contains the path for the header file to be saved.
		QFileInfo fileInfo_;

		//! \brief The name of the sw view to create the header for.
		QString swView_;
	};

	//! \brief The default directory name to save the headers to.
	static const QString DEFAULT_HEADER_DIR;

	//! \brief The column numbers for the table.
	enum Columns {
		SW_VIEW_NAME = 0,
		MEM_MAP_NAME,
		FILE_NAME,
		FILE_PATH,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * Method: 		LocalHeaderSaveModel
	 * Full name:	LocalHeaderSaveModel::LocalHeaderSaveModel
	 * Access:		private 
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of the model.
	 *
	*/
	LocalHeaderSaveModel(LibraryInterface* handler,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~LocalHeaderSaveModel();

	/*! \brief Set the component which's local memory maps are displayed.
	 *
	 * Method: 		setComponent
	 * Full name:	LocalHeaderSaveModel::setComponent
	 * Access:		public 
	 *
	 * \param component The pointer to the component to create the local memory maps for.
	 *
	*/
	void setComponent(QSharedPointer<Component> component);

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get the header data for specified header.
	 *
	 * \param section The section specifies the row/column number for the header.
	 * \param orientation Specified if horizontal or vertical header is wanted.
	 * \param role Specifies the type of the requested data.
	 *
	 * \return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! \brief Get the options defining locations for headers to generate.
	 *
	 * Method: 		getHeaderOptions
	 * Full name:	LocalHeaderSaveModel::getHeaderOptions
	 * Access:		public 
	 *
	 *
	 * \return QList containing the save options.
	*/
	const QList<SaveFileOptions*>& getHeaderOptions() const;

private:
	
	//! \brief No copying
	LocalHeaderSaveModel(const LocalHeaderSaveModel& other);

	//! \brief No assignment
	LocalHeaderSaveModel& operator=(const LocalHeaderSaveModel& other);

	//! \brief Pointer to the component which contains the local memory maps.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Contains the local memory maps and paths to save the headers.
	QList<SaveFileOptions*> table_;
};

#endif // LOCALHEADERSAVEMODEL_H
