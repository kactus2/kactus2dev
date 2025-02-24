//-----------------------------------------------------------------------------
// File: Modesmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Model for Mode elements within a component.
//-----------------------------------------------------------------------------

#ifndef MODESMODEL_H
#define MODESMODEL_H


#include <editors/common/ExpressionSet.h>

#include <KactusAPI/include/ExpressionFormatter.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class Component;
class Mode;
class ModeValidator;

//-----------------------------------------------------------------------------
//! Model for Mode elements within a component.
//-----------------------------------------------------------------------------
class ModesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *    @param [in] component   The component being edited.
	 *    @param [in] parent      The owner of this model.
	*/
	ModesModel(QSharedPointer<Component> component, 
        QObject* parent);
	
	//! The destructor
	virtual ~ModesModel() = default;

    //! No copying
    ModesModel(const ModesModel& other) = delete;
    ModesModel& operator=(const ModesModel& other) = delete;

	/*! Get the number of rows an item contains.
	 *
	 *    @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
	*/
	int rowCount(const QModelIndex& parent = QModelIndex()) const final;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed.
	*/
	int columnCount(const QModelIndex& parent = QModelIndex()) const final;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index Model index that identifies the item.
	 *
	 *    @return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const final;

	/*! Get the header data for specified header.
	 *
	 *    @param [in] section The section specifies the row/column number for the header.
	 *    @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *    @param [in] role Specifies the type of the requested data.
	 *
	 *    @return QVariant Contains the requested data.
	*/
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

	/*! Get the data for specified item.
	 *
	 *    @param [in] index Specifies the item that's data is requested.
	 *    @param [in] role The role that defines what kind of data is requested.
	 *
	 *    @return QVariant Contains the data for the item.
	*/
	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const final;

	/*! Save the data to the model for specified item
	 *
	 *    @param [in] index The model index of the item that's data is to be saved.
	 *    @param [in] value The data that is to be saved.
	 *    @param [in] role The role specifies what kind of data should be saved.
	 *
	 *    @return True if saving was successful.
	*/
	bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole) final;


public slots:

	/*! Add a new item to the given index.
	 *
	 *    @param [in] index The index identifying the position for new item.
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! Remove the item in the given index.
	 *
	 *    @param [in] index The index identifying the item to remove.
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*! Emitted when a new Mode is added to the model.
	 *
	 *    @param [in] index The index of the added Mode.
	*/
	void modeAdded(int index);

	/*! Emitted when a Mode is removed from the model.
	 *
	 *    @param [in] index The index of the removed Mode.
	*/
	void modeRemoved(int index);

private:

	//! Gets the value of the cell in index.
    QVariant valueForIndex(QModelIndex const& index) const;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the Modes being edited.
	QSharedPointer<QList<QSharedPointer<Mode> > > modes_;

	//! Formatter to use for expressions.
	QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // ModeSMODEL_H
