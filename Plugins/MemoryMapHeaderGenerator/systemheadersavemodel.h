/* 
 *	Created on:	11.4.2013
 *	Author:		Antti Kamppi
 *	File name:	systemheadersavemodel.h
 *	Project:		Kactus 2
*/

#ifndef SYSTEMHEADERSAVEMODEL_H
#define SYSTEMHEADERSAVEMODEL_H

#include <LibraryManager/vlnv.h>
#include <models/component.h>

#include <QAbstractTableModel>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief The model class to display the header files to be generated for CPU instances of system design.
 *
 */
class SystemHeaderSaveModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Specifies the columns supported by the model.
	enum Columns {
		INSTANCE_COLUMN = 0,
		FILENAME_COLUMN,
		FILEPATH_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * Method: 		SystemHeaderSaveModel
	 * Full name:	SystemHeaderSaveModel::SystemHeaderSaveModel
	 * Access:		private 
	 *
	 * \param handler Pointer to the instance which manages the library.
	 * \param parent Pointer to the owner of the model.
	 *
	*/
	SystemHeaderSaveModel(LibraryInterface* handler, QObject *parent);
	
	//! \brief The destructor.
	virtual ~SystemHeaderSaveModel();

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

	//! \brief Contains the settings needed to generate the system headers
	struct SysHeaderOptions {

		//! \brief Name of the CPU instance.
		QString instanceName_;

		//! \brief Identifies the component.
		VLNV compVLNV_;

		//! \brief Uuid identifying the instance.
		QString instanceId_;

		//! \brief Identifies the hierarchical component which contains the instance.
		VLNV containingComp_;

		//! \brief Name of the active view for the hierarchical component which contains the instance.
		QString containingView_;

		//! \brief Refers to the system header to generate.
		QFileInfo sysHeaderInfo_;

		//! \brief References to the files to include in the system header.
		QList<QFileInfo> includeFiles_;

		//! \brief Indicates whether the instance has been found or not.
		bool found_;

		/*! \brief The constructor.
		 *
		 * Method: 		SysHeaderOptions
		 * Full name:	MemoryMapHeaderGenerator::SysHeaderOptions::SysHeaderOptions
		 * Access:		public 
		 *
		 * \param instanceName The name of the CPU instance.
		 * \param compVLNV VLNV identifying the component.
		 *
		*/
		SysHeaderOptions(const QString& instanceName = QString(), const VLNV& compVLNV = VLNV());

		//! \brief Copy constructor.
		SysHeaderOptions(const SysHeaderOptions& other);

		//! \brief Assignment operator.
		SysHeaderOptions& operator=(const SysHeaderOptions& other);

		//! \brief The equality operator.
		bool operator==(const SysHeaderOptions& other);

		//! \brief The inequality operator.
		bool operator!=(const SysHeaderOptions& other);

		//! \brief Smaller than operator.
		bool operator<(const SysHeaderOptions& other);
	};

	/*! \brief Set the objects to display in the model.
	 *
	 * Method: 		setObjects
	 * Full name:	SystemHeaderSaveModel::setObjects
	 * Access:		public 
	 *
	 * \param options Contains the settings for the objects to create.
	 *
	*/
	void setObjects(QSharedPointer<Component> topComponent, 
		const QList<SystemHeaderSaveModel::SysHeaderOptions>& options);

	/*! \brief Get the objects displayed in the model.
	 *
	 * Method: 		getObjects
	 * Full name:	SystemHeaderSaveModel::getObjects
	 * Access:		public 
	 *
	 *
	 * \return QList containing the settings for the objects.
	*/
	const QList<SystemHeaderSaveModel::SysHeaderOptions>& getObjects() const;

private:
	
	//! \brief No copying
	SystemHeaderSaveModel(const SystemHeaderSaveModel& other);

	//! \brief No assignment
	SystemHeaderSaveModel& operator=(const SystemHeaderSaveModel& other);

	//! \brief Pointer to the instance which manages the library.
	LibraryInterface* handler_;

	//! \brief Contains the settings to display.
	QList<SysHeaderOptions> table_;

	//! \brief Pointer to the system-level component.
	QSharedPointer<Component> topComp_;
};

#endif // SYSTEMHEADERSAVEMODEL_H
