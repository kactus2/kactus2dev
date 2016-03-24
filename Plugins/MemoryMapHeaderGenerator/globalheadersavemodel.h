//-----------------------------------------------------------------------------
// File: globalheadersavemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.02.2013
//
// Description:
// The model class to display the header files to be created for CPU instances within a design.
//-----------------------------------------------------------------------------

#ifndef GLOBALHEADERSAVEMODEL_H
#define GLOBALHEADERSAVEMODEL_H

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/Design.h>

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QFileInfo>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The model class to display the header files to be created for CPU instances within a design.
//-----------------------------------------------------------------------------
class GlobalHeaderSaveModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	//! Contains the options to identify the local memory map and its save file path.
	struct SaveFileOptions
    {
		//! The VLNV identifier of the component.
		VLNV comp_;

		//! The name of the instance containing the interface.
		QString instance_;

		//! The id used to identify the instance through hierarchy levels.
		QString instanceId_;

		//! The name of the master interface to create the header for.
		QString interface_;

		//! Contains the path for the header file to be saved.
		QFileInfo fileInfo_;
	};

	//! The column numbers for the table.
	enum Columns
    {
		INSTANCE_NAME = 0,
		INTERFACE_NAME,
		FILE_NAME,
		FILE_PATH,
		COLUMN_COUNT
	};

	/*!
     *  The constructor
	 *
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] parent      Pointer to the owner of the model.
	 */
	GlobalHeaderSaveModel(LibraryInterface* handler, QObject *parent);
	
	//! The destructor.
	virtual ~GlobalHeaderSaveModel();

	/*!
     *  Set the design for which the global headers are generated.
	 *
	 *      @param [in] topComp     Pointer to the component which contains the design.
	 *      @param [in] design      Pointer to the design which instantiates the components.
	 */
	void setDesign(QSharedPointer<Component> topComp, QSharedPointer<Design> design);

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Get the options defining locations for headers to generate.
	 *
	 *      @return QList containing the save options.
	 */
	const QList<SaveFileOptions*>& getHeaderOptions() const;

private:
	
	//! No copying.
	GlobalHeaderSaveModel(const GlobalHeaderSaveModel& other);

	//! No assignment.
	GlobalHeaderSaveModel& operator=(const GlobalHeaderSaveModel& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Contains the instance and interface names and the file paths.
	QList<SaveFileOptions*> table_;

	//! Pointer to the top component.
	QSharedPointer<Component> comp_;

	//! Pointer to the design containing the CPU instances.
	QSharedPointer<Design> design_;
};

#endif // GLOBALHEADERSAVEMODEL_H
