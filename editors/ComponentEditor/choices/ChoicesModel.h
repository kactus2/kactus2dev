//-----------------------------------------------------------------------------
// File: ChoicesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing choices in a component.
//-----------------------------------------------------------------------------

#ifndef CHOICESMODEL_H
#define CHOICESMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>
#include <QObject>

class Choice;

//-----------------------------------------------------------------------------
//! Model for editing choices in a component.
//-----------------------------------------------------------------------------
class ChoicesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *     @param [in] choices    Pointer to the component being edited.
	 *     @param [in] parent       Pointer to the owner of this model.
	*/
	ChoicesModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject* parent);
	
	//! The destructor
	virtual ~ChoicesModel();

	/*! Get the number of rows an item contains.
	 *
	 *     @param [in] parent   Identifies the parent that's row count is requested.
	 *
	 *     @return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *     @param [in] parent   Identifies the parent that's column count is requested.
	 *
	 *     @return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *     @param [in] index    Model index that identifies the item.
	 *
	 *     @return The possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! Get the header data for specified header.
	 *
	 *     @param [in] section      The section specifies the row/column number for the header.
	 *     @param [in] orientation  Specified if horizontal or vertical header is wanted.
	 *     @param [in] role         Specifies the type of the requested data.
	 *
	 *     @return The requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! Get the data for specified item.
	 *
	 *     @param [in] index Specifies the item that's data is requested.
	 *     @param [in] role The role that defines what kind of data is requested.
	 *
	 *     @return The data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *     @param [in] index    The model index of the item that's data is to be saved.
	 *     @param [in] value    The data that is to be saved.
	 *     @param [in] role     The role specifies what kind of data should be saved.
	 *
	 *     @return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*! Check if the choices model is in a valid state.
	 *
	 *     @return True, if all the choices are valid, otherwise false.
	*/
	bool isValid() const;

public slots:

	/*! Add a new item to the given index.
	 *
	 *     @param [in] index The index identifying the position for new item.
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! Remove the item in the given index.
	 *
	 *     @param [in] index The index identifying the item to remove.
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

private:

	//! No copying
	ChoicesModel(const ChoicesModel& other);

	//! No assignment
	ChoicesModel& operator=(const ChoicesModel& other);

    bool isNotValidIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the choices being edited.
	QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
};

#endif // CHOICESMODEL_H
