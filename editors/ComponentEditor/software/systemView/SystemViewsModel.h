//-----------------------------------------------------------------------------
// File: SystemViewsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The model to manage system views for a table view.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWSMODEL_H
#define SYSTEMVIEWSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class SystemView;
//-----------------------------------------------------------------------------
//! The model to manage the system views for a table view
//-----------------------------------------------------------------------------
class SystemViewsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component The component being edited.
	 *      @param [in] parent The owner of the model
	*/
	SystemViewsModel(QSharedPointer<Component> component, QObject* parent);
	
	//! The destructor
	virtual ~SystemViewsModel();

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
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! Check if the software views model is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

    /*!
     *  Returns the supported actions of a drop.
     *
     *      @return The drop actions supported by the model.
     */
    Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns a list of supported MIME data types.
     *
     *      @return The supported MIME types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *      @param [in] data   The data associated to the drop.
     *      @param [in] action The drop action.  
     *      @param [in] row    The row beneath the drop position.
     *      @param [in] column The column beneath the drop position.
     *      @param [in] parent The parent index of the drop position.
     *
     *      @return True, if the model could handle the data, otherwise false.
     */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
        const QModelIndex &parent);

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

	//! Emitted when the contents of the model changes.
	void contentChanged();

	/*! Emitted when a new software view is added to the model.
	 *
	 *      @param [in] index The index of the added software view.
	 *
	*/
	void viewAdded(int index);

	/*! Emitted when a view is removed from the model.
	 *
	 *      @param [in] index The index of the software view to remove.
	 *
	*/
	void viewRemoved(int index);

private:
	
	//! No copying
	SystemViewsModel(const SystemViewsModel& other);

	//! No assignment
	SystemViewsModel& operator=(const SystemViewsModel& other);

	//! Contains the software views to edit.
	QList<QSharedPointer<SystemView> > views_;

	//! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // SYSTEMVIEWSMODEL_H
