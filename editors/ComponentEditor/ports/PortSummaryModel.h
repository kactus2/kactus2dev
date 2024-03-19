//-----------------------------------------------------------------------------
// File: PortSummaryModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 27.11.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef PORT_SUMMARY_MODEL_H
#define PORT_SUMMARY_MODEL_H

#include <editors/ComponentEditor/ports/portsmodel.h>

//-----------------------------------------------------------------------------
//! Table model for wire ports.
//-----------------------------------------------------------------------------
class PortSummaryModel : public PortsModel
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterFinder     Pointer to the parameter finder.
     *      @param [in] portsInterface      Interface for accessing the component ports.
     *      @param [in] signalInterface     Interface for accessing abstraction signals.
     *      @param [in] parent              Pointer to the owner of this model.
     */
    PortSummaryModel(QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer<PortsInterface> portsInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        QObject *parent);

	/*!
     *  The destructor.
     */
    ~PortSummaryModel() = default;

	/*!
     *  Get the number of columns in the model.
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid
     *
	 *      @return Always returns 9.
     */
	int columnCount(const QModelIndex& parent = QModelIndex() ) const final;
    	
    /*!
     *  Get the data for the headers.
	 *
	 *      @param [in] section         The column that's header is wanted
	 *      @param [in] orientation     Only Qt::Horizontal is supported
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed.
	 */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

    /*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
     */
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const final;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole) final;

public slots:

    void onAddWire();

    void onAddTransactional();

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      return True, if column can have expressions, false otherwise.
     */
    bool isValidExpressionColumn(QModelIndex const& index) const final;

    bool isPortTypeColumn(QModelIndex const& index) const final;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      return Expression in the given index, or plain value.
     */
    QVariant expressionOrValueForIndex(QModelIndex const& index) const final;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    QVariant formattedExpressionForIndex(QModelIndex const& index) const final;

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const final;


private:

    /*!
     *  Get the column for row number.
     *
     *      @return Row number column.
     */
    int rowNumberColumn() const final;

    /*!
     *  Get the column for name.
     *
     *      @return Name column.
     */
    int nameColumn() const final;

    /*!
     *  Get the column for type definitions.
     *
     *      @return Type definitions column.
     */
    int typeColumn() const final;

    /*!
     *  Get the column for array left.
     *
     *      @return Array left column.
     */
    int arrayLeftColumn() const final;

    /*!
     *  Get the column for array right.
     *
     *      @return Array right column.
     */
    int arrayRightColum() const final;

    /*!
     *  Get the column for tags.
     *
     *      @return Tags column.
     */
    int tagColumn() const final;

    /*!
     *  Get the column for ad hoc visibility.
     *
     *      @return Ad hoc visibility column.
     */
    int adHocColumn() const final;

    /*!
     *  Get the column for description.
     *
     *      @return Description column.
     */
    int descriptionColumn() const final;

    /*!
     *  Check if the selected item is disabled.
     *
     *      @param [in] index           Index of the selected item.
     *
     *      @return True, if the indexed item is disabled, false otherwise.
     */
    bool indexedItemIsDisabled(QModelIndex const& index) const final;

    /*!
     *  Check if the selected item is mandatory.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return True, if the indexed item is mandatory, false otherwise.
     */
    bool indexedItemIsMandatory(QModelIndex const& index) const final;

    /*!
     *  Check if the selected item is locked.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return True, if the indexed item is locked, false otherwise.
     */
    bool indexedItemIsLocked(QModelIndex const& index) const final;

    /*!
     *  Get a list of the locked port indexes from the selected index.
     *
     *      @param [in] portIndex   Index of the selected item.
     *
     *      @return List of the locked port indexes.
     */
    virtual QModelIndexList getLockedPortIndexes(QModelIndex const& portIndex) const final;


    void addNewPort() override;

};

#endif // PORT_SUMMARY_MODEL_H
