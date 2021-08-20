//-----------------------------------------------------------------------------
// File: TransactionalPortsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.05.2019
//
// Description:
// Table model for transactional ports.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALPORTSMODEL_H
#define TRANSACTIONALPORTSMODEL_H

#include <editors/ComponentEditor/ports/portsmodel.h>

//-----------------------------------------------------------------------------
//! Table model for transactional ports.
//-----------------------------------------------------------------------------
class TransactionalPortsModel : public PortsModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] parameterFinder     Pointer to the parameter finder.
     *      @param [in] portsInterface      Interface for accessing the component ports.
     *      @param [in] signalInterface     Interface for accessing abstraction signals.
     *      @param [in] filter              The model filter.
	 *      @param [in] parent              Pointer to the owner of this model.
     */
    TransactionalPortsModel(QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer<PortsInterface> portsInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        QSortFilterProxyModel* filter, QObject *parent);

	/*!
     *  The destructor.
     */
    virtual ~TransactionalPortsModel() = default;

	/*!
     *  Get the number of columns in the model.
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid
     *
	 *      @return Always returns 9.
     */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the headers.
	 *
	 *      @param [in] section         The column that's header is wanted
	 *      @param [in] orientation     Only Qt::Horizontal is supported
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
     */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

    //! No copying. No assignment.
    TransactionalPortsModel(const TransactionalPortsModel& other) = delete;
    TransactionalPortsModel& operator=(const TransactionalPortsModel& other) = delete;

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      return True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const override final;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      return Expression in the given index, or plain value.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const override final;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const override final;

    /*!
     *  Get the expression of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const override final;

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      return The data in the given index.
     */
    virtual QVariant valueForIndex(QModelIndex const& index) const override final;

    /*!
     *  Validates the data in the cell given by the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      return True, if the data in the parameter is valid, false otherwise.
     */
    virtual bool validateIndex(QModelIndex const& index) const override final;

private:

    /*!
     *  Get the column for row number.
     *
     *      @return Row number column.
     */
    virtual int rowNumberColumn() const override final;

    /*!
     *  Get the column for name.
     *
     *      @return Name column.
     */
    virtual int nameColumn() const override final;

    /*!
     *  Get the column for type definitions.
     *
     *      @return Type definitions column.
     */
    virtual int typeColumn() const override final;

    /*!
     *  Get the column for array left.
     *
     *      @return Array left column.
     */
    virtual int arrayLeftColumn() const override final;

    /*!
     *  Get the column for array right.
     *
     *      @return Array right column.
     */
    virtual int arrayRightColum() const override final;

    /*!
     *  Get the column for tags.
     *
     *      @return Tags column.
     */
    virtual int tagColumn() const override final;

    /*!
     *  Get the column for ad hoc visibility.
     *
     *      @return Ad hoc visibility column.
     */
    virtual int adHocColumn() const override final;

    /*!
     *  Get the column for description.
     *
     *      @return Description column.
     */
    virtual int descriptionColumn() const override final;

    /*!
     *  Check if the selected item is disabled.
     *
     *      @param [in] index           Index of the selected item.
     *
     *      @return True, if the indexed item is disabled, false otherwise.
     */
    virtual bool indexedItemIsDisabled(QModelIndex const& index) const override final;

    /*!
     *  Check if the selected item is mandatory.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return True, if the indexed item is mandatory, false otherwise.
     */
    virtual bool indexedItemIsMandatory(QModelIndex const& index) const override final;

    /*!
     *  Check if the selected item is locked.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return True, if the indexed item is locked, false otherwise.
     */
    virtual bool indexedItemIsLocked(QModelIndex const& index) const override final;

    /*!
     *  Get a list of the locked port indexes from the selected index.
     *
     *      @param [in] portIndex   Index of the selected item.
     *
     *      @return List of the locked port indexes.
     */
    virtual QModelIndexList getLockedPortIndexes(QModelIndex const& portIndex) const override final;

    /*!
     *  Add a new transactional port.
     */
    virtual void addNewPort() override final;
};

#endif // TRANSACTIONALPORTSMODEL_H
