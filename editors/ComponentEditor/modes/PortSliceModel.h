//-----------------------------------------------------------------------------
// File: PortSliceModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Model for Mode elements within a component.
//-----------------------------------------------------------------------------

#ifndef PORTSLICEMODEL_H
#define PORTSLICEMODEL_H

#include <editors/common/ExpressionSet.h>

#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <IPXACTmodels/Component/PortSlice.h>

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
class PortSliceModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *      @param [in] component   The component being edited.
     *      @param [in] expressions The collection of objects for expression handling.
	 *      @param [in] parent      The owner of this model.
	*/
	PortSliceModel(QSharedPointer<Mode> mode, 
		ExpressionSet expressions,
        QObject* parent);
	
	//! The destructor
	virtual ~PortSliceModel() = default;

    //! No copying
    PortSliceModel(const PortSliceModel& other) = delete;
    PortSliceModel& operator=(const PortSliceModel& other) = delete;

	/*! Get the number of rows an item contains.
	 *
	 *      @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	*/
	int rowCount(const QModelIndex& parent = QModelIndex()) const final;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	*/
	int columnCount(const QModelIndex& parent = QModelIndex()) const final;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const final;

	/*! Get the header data for specified header.
	 *
	 *      @param [in] section The section specifies the row/column number for the header.
	 *      @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *      @param [in] role Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is requested.
	 *      @param [in] role The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const final;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index The model index of the item that's data is to be saved.
	 *      @param [in] value The data that is to be saved.
	 *      @param [in] role The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving was successful.
	*/
	bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole) final;


public slots:

	/*! Add a new item to the given index.
	 *
	 *      @param [in] index The index identifying the position for new item.
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

protected:
	    
	/*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *     @param [in] row         The row of the selected item.
     *     @param [in] valueID     The id of the referenced parameter.
     *
     *     @return The amount of references made to the selected id on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const final;

	/*!
	 *  Check if the column index is valid for containing expressions.
	 *
	 *     @param [in] index   The index being evaluated.
	 *
	 *     @return True, if the column can accept expressions, false otherwise.
	 */
	virtual bool isValidExpressionColumn(QModelIndex const& index) const final;

	/*!
     *  Gets the expression for the index, or the plain value, if an expression is not available.
     *
     *     @param [in] index   The index of the item.
     *
     *     @return The expression for the index if available, otherwise the value for the given index.
     */
	virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const final;

	/*!
	 *  Validates the data for the column.
	 *
	 *     @param [in] index   The column of the item to validate.
	 *
	 *     @return True, if the data of the item is valid for the column, false otherwise.
	 */
	virtual bool validateIndex(QModelIndex const& index) const final;

private:

	//! Gets the value of the cell in index.
    QVariant valueForIndex(QModelIndex const& index) const;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the PortSlice being edited.
	QSharedPointer<QList<QSharedPointer<PortSlice> > > portSlices_;

	//! Formatter to use for expressions.
	QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // PORTSLICEMODEL_H
