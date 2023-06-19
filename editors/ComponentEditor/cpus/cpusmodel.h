//-----------------------------------------------------------------------------
// File: cpusmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
//! Model for cpu elements within a component.
//-----------------------------------------------------------------------------

#ifndef CPUSMODEL_H
#define CPUSMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

#include <KactusAPI/include/ExpressionFormatter.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class Component;
class Cpu;
class CPUValidator;

//-----------------------------------------------------------------------------
//! Model for cpu elements within a component.
//-----------------------------------------------------------------------------
class CpusModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
     *      @param [in] validator   The validator for cpus.
	 *      @param [in] parent      The owner of this model.
	*/
	CpusModel(QSharedPointer<Component> component, QSharedPointer<CPUValidator> validator,
		QSharedPointer<ExpressionParser> expressionParser,
		QSharedPointer<ParameterFinder> parameterFinder,
		QSharedPointer<ExpressionFormatter> expressionFormatter,
        QObject* parent);
	
	//! The destructor
	virtual ~CpusModel() = default;

    //! No copying
    CpusModel(const CpusModel& other) = delete;
    CpusModel& operator=(const CpusModel& other) = delete;

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

protected:

	/*!
	 *  Gets the number of all the references made to a selected id on the selected row.
	 *
	 *      @param [in] row         The row of the selected item.
	 *      @param [in] valueID     The id of the referenced parameter.
	 *
	 *      @return The amount of references made to the selected id on the selected row.
	 */
    int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const final;

	/*!
	*  Check if the column index is valid for containing expressions.
	*
	*      @param [in] index   The index being evaluated.
	*
	*      @return     True, if column can have expressions, false otherwise.
	*/
	bool isValidExpressionColumn(QModelIndex const& index) const final;

	/*!
	 *  Gets the expression for the given index or the plain value if expression is not available.
	 *
	 *      @param [in] index   The index whose expression to get.
	 *
	 *      @return The expression for the index if available, otherwise the value for the given index.
	 */
	QVariant expressionOrValueForIndex(QModelIndex const& index) const final;

	/*!
	 *  Validates the data in an index.
	 *
	 *      @param [in] index   The index whose data to validate
	 *
	 *      @return True, if the data in the index is valid, otherwise false.
	 */
	bool validateIndex(QModelIndex const& index) const final;

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

	/*! Emitted when a new cpu is added to the model.
	 *
	 *      @param [in] index The index of the added cpu.
	*/
	void cpuAdded(int index);

	/*! Emitted when a cpu is removed from the model.
	 *
	 *      @param [in] index The index of the removed cpu.
	*/
	void cpuRemoved(int index);

private:

	//! Gets the value of the cell in index.
    QVariant valueForIndex(QModelIndex const& index) const;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the cpus being edited.
	QSharedPointer<QList<QSharedPointer<Cpu> > > cpus_;

	//! Validator for the CPU element.
    QSharedPointer<CPUValidator> validator_;

	//! Formatter to use for expressions.
	QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // CPUSMODEL_H
