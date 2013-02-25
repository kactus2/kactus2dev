/* 
 *	Created on: 25.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	globalheadersavemodel.h
 * 	Project:	Kactus 2
*/

#ifndef GLOBALHEADERSAVEMODEL_H
#define GLOBALHEADERSAVEMODEL_H

#include <LibraryManager/vlnv.h>
#include <models/component.h>
#include <models/design.h>

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QFileInfo>

class LibraryInterface;

/*! \brief The model class to display the header files to be created for CPU instances within a design.
 *
 */
class GlobalHeaderSaveModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Contains the options to identify the local memory map and its save file path.
	struct SaveFileOptions {

		//! \brief The VLNV identifier of the component.
		VLNV comp_;

		//! \brief The name of the instance containing the interface.
		QString instance_;

		//! \brief The name of the master interface to create the header for.
		QString interface_;

		//! \brief Contains the path for the header file to be saved.
		QFileInfo fileInfo_;
	};

	//! \brief The default directory name to save the headers to.
	static const QString DEFAULT_HEADER_DIR;

	//! \brief The column numbers for the table.
	enum Columns {
		INSTANCE_NAME = 0,
		INTERFACE_NAME,
		FILE_NAME,
		FILE_PATH,
		COLUMN_COUNT
	};

	/*! \brief The constructor
	 *
	 * Method: 		GlobalHeaderSaveModel
	 * Full name:	GlobalHeaderSaveModel::GlobalHeaderSaveModel
	 * Access:		private 
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of the model.
	 *
	*/
	GlobalHeaderSaveModel(LibraryInterface* handler, 
		QObject *parent);
	
	//! \brief The destructor
	virtual ~GlobalHeaderSaveModel();

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
	GlobalHeaderSaveModel(const GlobalHeaderSaveModel& other);

	//! \brief No assignment
	GlobalHeaderSaveModel& operator=(const GlobalHeaderSaveModel& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Contains the instance and interface names and the file paths.
	QList<SaveFileOptions*> table_;

	//! \brief Pointer to the top component.
	QSharedPointer<Component> comp_;

	//! \brief Pointer to the design containing the CPU instances.
	QSharedPointer<Design> design_;
};

#endif // GLOBALHEADERSAVEMODEL_H
