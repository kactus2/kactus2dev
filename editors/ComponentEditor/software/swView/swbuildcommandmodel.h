//-----------------------------------------------------------------------------
// File: swbuildcommandmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.04.2013
//
// Description:
// The model class to manage SW build commands in SW Build Command editor.
//-----------------------------------------------------------------------------

#ifndef SWBUILDCOMMANDMODEL_H
#define SWBUILDCOMMANDMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

class Component;
class SWFileBuilder;


//-----------------------------------------------------------------------------
//! The model class to manage SW build commands in SW Build Command editor.
//-----------------------------------------------------------------------------
class SWBuildCommandModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] component           The component being edited.
	 *      @param [in] swBuildCommands     the SW build commands being edited.
	 *      @param [in] parent              The owner of the model.
	*/
	SWBuildCommandModel(QSharedPointer<Component> component,
        QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > swBuildCommands,
		QObject* parent);
	
	//! The destructor.
	virtual ~SWBuildCommandModel();

	/*! Get the number of rows an item contains.
	 *
	 *      @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! Get the header data for specified header.
	 *
	 *      @param [in] section The section specifies the row/column number for the header.
	 *      @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *      @param [in] role Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is requested.
	 *      @param [in] role The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index The model index of the item that's data is to be saved.
	 *      @param [in] value The data that is to be saved.
	 *      @param [in] role The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! Check if the model is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

public slots:

	/*! Add a new item to the given index.
	 *
	 *      @param [in] index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

private:
	
	//! No copying
	SWBuildCommandModel(const SWBuildCommandModel& other);

	//! No assignment
	SWBuildCommandModel& operator=(const SWBuildCommandModel& other);

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the SW build commands being edited.
	QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > swBuildCommands_;
};

#endif // SWBUILDCOMMANDMODEL_H
