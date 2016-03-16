//-----------------------------------------------------------------------------
// File: viewsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 12.6.2012
//
// Description:
// The model to manage the views summary.
//-----------------------------------------------------------------------------

#ifndef VIEWSMODEL_H
#define VIEWSMODEL_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class ViewValidator;
//-----------------------------------------------------------------------------
//! The model to manage the views summary.
//-----------------------------------------------------------------------------
class ViewsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component       Pointer to the component being edited.
     *      @param [in] viewValidator   The validator used for views.
	 *      @param [in] parent          Pointer to the owner of the model.
	 */
	ViewsModel(QSharedPointer<Component> component, QSharedPointer<ViewValidator> viewValidator, QObject* parent);
	
	//! The destructor.
	virtual ~ViewsModel();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(QModelIndex const& index) const;

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
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Check if the views model is in a valid state.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	 */
	bool isValid() const;

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 */
	virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 */
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	//! Emitted when a new view is added to the given index.
	void viewAdded(int index);

	//! Emitted when a view is removed from the given index.
	void viewRemoved(int index);

private:
	//! No copying.
	ViewsModel(const ViewsModel& other);
	ViewsModel& operator=(const ViewsModel& other);

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Contains the views to manage.
    QSharedPointer<QList<QSharedPointer<View> > > views_;

    //! The validator used for views.
    QSharedPointer<ViewValidator> viewValidator_;
};

#endif // VIEWSMODEL_H
