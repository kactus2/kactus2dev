//-----------------------------------------------------------------------------
// File: WirePortsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.05.2019
//
// Description:
// Table model for wire ports.
//-----------------------------------------------------------------------------

#ifndef WIREPORTSMODEL_H
#define WIREPORTSMODEL_H

#include <editors/ComponentEditor/ports/portsmodel.h>

//-----------------------------------------------------------------------------
//! Table model for wire ports.
//-----------------------------------------------------------------------------
class WirePortsModel : public PortsModel
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] model                   Pointer to the model being edited.
     *      @param [in] expressionParser        Pointer to the expression parser.
     *      @param [in] parameterFinder         Pointer to the parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] portValidator           Validator used for ports.
     *      @param [in] filter                  The model filter.
     *      @param [in] parent                  Pointer to the owner of this model.
     */
	WirePortsModel(QSharedPointer<Model> model, QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder, QSharedPointer <ExpressionFormatter> expressionFormatter,
        QSharedPointer<PortValidator> portValidator, QSortFilterProxyModel* filter, QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~WirePortsModel() = default;

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
    WirePortsModel(const WirePortsModel& other) = delete;
    WirePortsModel& operator=(const WirePortsModel& other) = delete;

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      return True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      return Expression in the given index, or plain value.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      return The data in the given index.
     */
    virtual QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in the cell given by the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      return True, if the data in the parameter is valid, false otherwise.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Get all the references made to the selected parameter from the selected row.
     *
     *      @param [in] row         The selected row.
     *      @param [in] valueID     The target parameter.
     *
     *      @return Number of references made to the selected id from the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const&  valueID) const;

private:

    /*!
     *  Get the column for row number.
     *
     *      @return Row number column.
     */
    virtual int rowNumberColumn() const;

    /*!
     *  Get the column for name.
     *
     *      @return Name column.
     */
    virtual int nameColumn() const;

    /*!
     *  Get the column for type definitions.
     *
     *      @return Type definitions column.
     */
    virtual int typeColumn() const;

    /*!
     *  Get the column for array left.
     *
     *      @return Array left column.
     */
    virtual int arrayLeftColumn() const;

    /*!
     *  Get the column for array right.
     *
     *      @return Array right column.
     */
    virtual int arrayRightColum() const;

    /*!
     *  Get the column for tags.
     *
     *      @return Tags column.
     */
    virtual int tagColumn() const;

    /*!
     *  Get the column for ad hoc visibility.
     *
     *      @return Ad hoc visibility column.
     */
    virtual int adHocColumn() const;

    /*!
     *  Get the column for description.
     *
     *      @return Description column.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Check if the selected item is disabled.
     *
     *      @param [in] index           Index of the selected item.
     *      @param [in] indexedPort     The indexed port.
     *
     *      @return True, if the indexed item is disabled, false otherwise.
     */
    virtual bool indexedItemIsDisabled(QModelIndex const& index, QSharedPointer<Port> indexedPort) const;

    /*!
     *  Check if the selected item is mandatory.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return True, if the indexed item is mandatory, false otherwise.
     */
    virtual bool indexedItemIsMandatory(QModelIndex const& index) const;

    /*!
     *  Check if the selected item is locked.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return True, if the indexed item is locked, false otherwise.
     */
    virtual bool indexedItemIsLocked(QModelIndex const& index) const;

    /*!
     *  Get a list of the locked port indexes from the selected index.
     *
     *      @param [in] portIndex   Index of the selected item.
     *
     *      @return List of the locked port indexes.
     */
    virtual QModelIndexList getLockedPortIndexes(QModelIndex const& portIndex) const;

    /*!
     *  Add the wire part to a new port.
     *
     *      @param [in] port    The selected port.
     */
    virtual void finalizePort(QSharedPointer<Port> port);

    /*!
     *  Changes the type name and definition on a row according to the port width.
     *
     *      @param [in] port    The port whose type name and definition to update.
     *      @param [in] row     The row number of the port.
     */
    void setTypeNameAndDefinitionOnRow(QSharedPointer<Port> port, int row);

    /*!
     *  Checks if a given port has an expression as a left or right bound.
     *
     *      @param [in] port   The port to check.
     *
     *      @return True, if left or right bound is an expression, otherwise false.
     */
    bool hasExpressionInLeftOrRightBound(QSharedPointer<Port> port) const;
};

#endif // WIREPORTSMODEL_H
