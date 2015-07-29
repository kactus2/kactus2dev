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

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/component.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The model to manage the views summary.
//-----------------------------------------------------------------------------
class ViewsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	//! \brief Contains the column numbers for columns.
	enum Column {
		NAME_COLUMN = 0,
		TYPE_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of the model.
	 *
	*/
	ViewsModel(QSharedPointer<Component> component,	QObject* parent);
	
	//! \brief The destructor
	virtual ~ViewsModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(QModelIndex const& index) const;

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
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

	/*! \brief Check if the views model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(QModelIndex const& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	//! \brief Emitted when a new view is added to the given index.
	void viewAdded(int index);

	//! \brief Emitted when a view is removed from the given index.
	void viewRemoved(int index);

private:
	//! \brief No copying
	ViewsModel(const ViewsModel& other);
	ViewsModel& operator=(const ViewsModel& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Contains the views to manage.
	QList<QSharedPointer<View> >& views_;
};

#endif // VIEWSMODEL_H
