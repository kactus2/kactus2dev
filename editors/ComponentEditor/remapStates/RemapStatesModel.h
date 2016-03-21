//-----------------------------------------------------------------------------
// File: RemapStatesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The model to manage the remap states summary.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATESMODEL_H
#define REMAPSTATESMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class RemapState;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! The model to manage the remap states summary.
//-----------------------------------------------------------------------------
class RemapStatesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	//! Contains the column numbers for columns.
	enum Column {
		NAME_COLUMN = 0,
        DISPLAY_NAME_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

    /*!
     *  The constructor.
     *
     *      @param [in] remapStates         The remap states of the component.
     *      @param [in] parameterFinder     Finder used to locate parameter ids.
     *      @param [in] parent              Pointer to the owner of the model.
     */
    RemapStatesModel(QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates,
        QSharedPointer<ParameterFinder> parameterFinder, QObject* parent);

    /*!
     *  The destructor.
     */
    virtual ~RemapStatesModel();

	/*!
	 *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
     *
     *      @return The number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of displayable columns the item has.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
     *
     *      @return The number of columns to be displayed
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
	 *  Get the item flags that define the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
     *
     *      @return The possible operations for the item as Qt::ItemFlags.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
	 *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section for the header.
	 *      @param [in] orientation     Specifies if the header is horizontal or vertical.
	 *      @param [in] role            Specifies the type of the requested data.
     *
     *      @return The requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
	 *  Get the data for the specified item.
	 *
	 *      @param [in] index   The index of the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return The data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The index of the item used to store the data.
	 *      @param [in] value   The data to be saved.
	 *      @param [in] role    What kind of data should be saved.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

public slots:

    /*!
	 *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position of the new item.
	 */
	virtual void onAddItem(const QModelIndex& index);

	/*!
	 *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the position of the item to be removed.
	 */
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	/*!
	 *  Emitted when the contents of the model change.
	 */
	void contentChanged();

    /*!
     *  Emitted when a new remap state is added.
     *
     *      @param [in] index   The index of the new remap state.
     */
    void remapStateAdded(int index);

    /*!
     *  Emitted when a remap state is removed.
     *
     *      @param [in] index   The index of the remap state being removed.
     */
    void remapStateRemoved(int index);

    /*!
     *  Decrease the number of references to the selected parameter.
     *
     *      @param [in] id  ID of the selected parameter.
     */
    void decreaseReferences(QString id);

private:
	//! No copying
    RemapStatesModel(const RemapStatesModel& other);

	//! No assignment
    RemapStatesModel& operator=(const RemapStatesModel& other);

    /*!
     *  Remove references from a selected remap state.
     *
     *      @param [in] remapState  The selected remap state.
     */
    void removeReferencesFromRemapState(QSharedPointer<RemapState> remapState);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the list of remap states of the component.
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates_;

    //! The used parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // REMAPSTATESMODEL_H
