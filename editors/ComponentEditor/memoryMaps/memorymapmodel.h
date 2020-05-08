//-----------------------------------------------------------------------------
// File: memorymapmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The model to manage the address blocks of a single memory map.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPMODEL_H
#define MEMORYMAPMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class AddressBlockInterface;

//-----------------------------------------------------------------------------
//! The model to manage the details of a single memory map.
//-----------------------------------------------------------------------------
class MemoryMapModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
     *      @param [in] blockInterface      Interface for address blocks;
	 *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] parent              Pointer to the owner of this model.
	 */
    MemoryMapModel(AddressBlockInterface* blockInterface,
        QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder,
        QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~MemoryMapModel();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const;

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 */
	virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 */
	virtual void onRemoveItem(QModelIndex const& index);

    /*!
     *  Change the current address unit bits.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits.
     */
    void addressUnitBitsUpdated(QString const& newAddressUnitBits);

    /*!
     *  Copy the items in the selected rows.
     *
     *      @param [in] indexList   List of indexes pointing to the selected rows.
     */
    void onCopyRows(QModelIndexList indexList);

    /*!
     *  Paste the copied items.
     */
    void onPasteRows();

protected:

    /*!
     *  Checks if the column index is valid for expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return True, if the column can have expressions, otherwise false.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return Expression or plain value in the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  validates the data in the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      @return True, if the data is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made on the selected row to the selected parameter.
     *
     *      @param [in] row         The row of the selected item.
     *      @param [in] valueID     The referenced parameter.
     *
     *      @return The number of references made to the parameter on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

    /*!
     *  Informs of a need to redraw the visualization.
     */
    void graphicsChanged();

	//! Emitted when a new memory map item is added to the given index.
	void itemAdded(int index);

	//! Emitted when a memory map item is removed from the given index.
	void itemRemoved(int index);

private:
	
	//! No copying.
	MemoryMapModel(const MemoryMapModel& other);

	//! No assignment.
	MemoryMapModel& operator=(const MemoryMapModel& other);

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the expression of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Increase the number of references made in the selected expressions.
     *
     *      @param [in] expressionList  List of expressions.
     */
    void increaseReferencesInPastedExpressions(QStringList const& expressionList);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for address blocks.
    AddressBlockInterface* blockInterface_;
};

#endif // MEMORYMAPMODEL_H
