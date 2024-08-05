//-----------------------------------------------------------------------------
// File: ResetTypesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.01.2019
//
// Description:
//! Model for reset type elements within a component.
//-----------------------------------------------------------------------------

#ifndef RESETTYPESMODEL_H
#define RESETTYPESMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class Component;
class ComponentValidator;
class ResetType;

//-----------------------------------------------------------------------------
//! Model for reset type elements within a component.
//-----------------------------------------------------------------------------
class ResetTypesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component   The component being edited.
     *      @param [in] validator   The validator for cpus.
	 *      @param [in] parent      The owner of this model.
	 */
	ResetTypesModel(QSharedPointer<Component> component, QSharedPointer<ComponentValidator> validator, QObject* parent);
	
	/*!
     *  The destructor.
     */
    virtual ~ResetTypesModel() = default;

    //! No copying
    ResetTypesModel(const ResetTypesModel& other) = delete;
    ResetTypesModel& operator=(const ResetTypesModel& other) = delete;

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
     */
	int rowCount(const QModelIndex& parent = QModelIndex()) const final;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
     */
	int columnCount(const QModelIndex& parent = QModelIndex()) const final;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
     */
	Qt::ItemFlags flags(const QModelIndex& index) const final;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role Specifies  the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
     */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
     */
	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const final;

	/*!
     *  Save the data to the model for specified item
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving was successful.
     */
	bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole) final;

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index The index identifying the position for new item.
     */
	virtual void onAddItem(const QModelIndex& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
     */
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	/*!
     *  Emitted when the contents of the model change.
     */
	void contentChanged();

private:

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the reset types being edited.
    QSharedPointer<QList<QSharedPointer<ResetType> > > resetTypes_;

    //! Validator for component items.
    QSharedPointer<ComponentValidator> validator_;
};

#endif // RESETTYPESMODEL_H
