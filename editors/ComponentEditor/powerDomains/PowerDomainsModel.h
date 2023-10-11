//-----------------------------------------------------------------------------
// File: PowerDomainsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.10.2023
//
// Description:
// Table model that contains the items to edit PowerDomains of a component.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINSMODEL_H
#define POWERDOMAINSMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

#include <editors/common/ExpressionSet.h>

#include <QList>
#include <QSharedPointer>

class Component;
class PowerDomain;
class PowerDomainValidator;
//-----------------------------------------------------------------------------
//! Table model that contains the items to edit PowerDomains of a component.
//-----------------------------------------------------------------------------
class PowerDomainsModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component       The component being edited.
     *      @param [in] validator		The validator used for power domains.
     *      @param [in] expressions		The collection of objects for expression handling.
	 *      @param [in] parent          The owner of this model
	 */
	PowerDomainsModel(QSharedPointer<Component> component, 
		QSharedPointer<PowerDomainValidator > validator, 
		ExpressionSet expressions,
		QObject *parent);
	
	//! The destructor.
	virtual ~PowerDomainsModel();

    //! No copying.
    PowerDomainsModel(const PowerDomainsModel& other) = delete;

    //! No assignment.
    PowerDomainsModel& operator=(const PowerDomainsModel& other) = delete;

	/*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item. Must be invalid because this is not 
     *                          hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	int rowCount(QModelIndex const&  parent = QModelIndex()) const final;

	/*!
     *  Get the number of columns in the model
	 *
	 *      @param [in] parent  Model index of the parent of the item. Must be invalid because this is not 
     *                          hierarchical model.
	 *
	 *      @return The number of columns in the model.
	 */
	int columnCount(QModelIndex const&  parent = QModelIndex()) const final;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index Identifies the item that's data is wanted.
	 *      @param [in] role Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
	 */
	QVariant data(QModelIndex const&  index, int role = Qt::DisplayRole) const final;

	/*!
     *  Get the data for the headers
	 *
	 *      @param [in] section The column that's header is wanted
	 *      @param [in] orientation Only Qt::Horizontal is supported
	 *      @param [in] role Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed
	 */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is modified
	 *      @param [in] value The value to be set.
	 *      @param [in] role The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	bool setData(QModelIndex const&  index, const QVariant& value, int role = Qt::EditRole) final;

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in] index Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	 */
	Qt::ItemFlags flags(QModelIndex const&  index) const final;


public slots:

	/*!
     *  Remove a row from the model
	 *
	 *      @param [in] row Specifies the row to remove
	 */
	void onRemoveRow(int row);

	/*!
     *  Add a new empty row to the model
	 */
	void onAddRow();

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in] index The position where new item should be added at.
	 */
	void onAddItem(QModelIndex const&  index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in] index Identifies the item that should be removed.
	 */
	void onRemoveItem(QModelIndex const&  index);

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

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
     *      @return True, if column can have expressions, false otherwise.
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


private:

    /*!
     *  Validate the data contained within a given index.
     *
     *      @param [in] index   The index of the data being validated.
     *
     *      @return True, if the data is valid, otherwise false.
     */
    bool validateIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the current items in the editor.
	QSharedPointer<QList<QSharedPointer<PowerDomain> > > table_;

	//! Formatter for expressions.
	QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The validator used for power domains.
	QSharedPointer<PowerDomainValidator > validator_;

};

#endif // POWERDOMAINSMODEL_H
