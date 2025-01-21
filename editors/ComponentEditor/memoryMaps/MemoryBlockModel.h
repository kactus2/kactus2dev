//-----------------------------------------------------------------------------
// File: MemoryBlockModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.02.2022
//
// Description:
// The parent model for editing memory blocks.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKMODEL_H
#define MEMORYBLOCKMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <KactusAPI/include/ParameterFinder.h>

#include <IPXACTmodels/common/Document.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class MemoryBlockInterface;

//-----------------------------------------------------------------------------
//! The parent model for editing memory blocks.
//-----------------------------------------------------------------------------
class MemoryBlockModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
     *    @param [in] blockInterface      Interface for memory blocks;
	 *    @param [in] expressionParser    Pointer to the expression parser.
	 *    @param [in] parameterFinder     Pointer to the parameter finder.
	 *    @param [in] docRevision         The IP-XACT standard revision in use.
	 *    @param [in] parent              Pointer to the owner of this model.
	 */
    MemoryBlockModel(MemoryBlockInterface* blockInterface,
        QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder,
        Document::Revision docRevision,
        QObject *parent);

	/*!
     *  The destructor.
     */
    virtual ~MemoryBlockModel() = default;

    //! No copying.	No assignment.
    MemoryBlockModel(const MemoryBlockModel& other) = delete;
    MemoryBlockModel& operator=(const MemoryBlockModel& other) = delete;


	/*!
     *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const override final;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const = 0;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index   Model index that identifies the item.
	 *
	 *    @return Qt::ItemFlags specify the possible operations for the item.
	 */
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *    @param [in] section         The section specifies the row/column number for the header.
	 *    @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *    @param [in] role            Specifies the type of the requested data.
	 *
	 *    @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *    @param [in] index   Specifies the item that's data is requested.
	 *    @param [in] role    The role that defines what kind of data is requested.
	 *
	 *    @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *    @param [in] index   The model index of the item that's data is to be saved.
	 *    @param [in] value   The data that is to be saved.
	 *    @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *    @return True if saving happened successfully.
	 */
	virtual bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Get the list of acceptable mime types.
     *
     *    @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const = 0;


public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *    @param [in] index   The index identifying the position for new item.
	 */
	void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *    @param [in] index   The index identifying the item to remove.
	 */
	void onRemoveItem(QModelIndex const& index);

    /*!
     *  Change the current address unit bits.
     *
     *    @param [in] newAddressUnitBits  The new address unit bits.
     */
    void addressUnitBitsUpdated(QString const& newAddressUnitBits);

    /*!
     *  Copy the items in the selected rows.
     *
     *    @param [in] indexList   List of indexes pointing to the selected rows.
     */
    void onCopyRows(QModelIndexList indexList);

    /*!
     *  Paste the copied items.
     */
    void onPasteRows();

protected:

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *    @param [in] index   The index of the target data.
     *
     *    @return Expression or plain value in the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  validates the data in the column.
     *
     *    @param [in] index   The index being validated.
     *
     *    @return True, if the data is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made on the selected row to the selected parameter.
     *
     *    @param [in] row         The row of the selected item.
     *    @param [in] valueID     The referenced parameter.
     *
     *    @return The number of references made to the parameter on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the expression of the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the value for the given index.
     *
     *    @param [in] index   The index of the target data.
     *
     *    @return The data in the given index.
     */
    virtual QVariant valueForIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The IP-XACT standard revision in use.
    Document::Revision docRevision_;

signals:

	/*!
     *  Emitted when the contents of the model change.
     */
	void contentChanged();

    /*!
     *  Informs of a need to redraw the visualization.
     *
     *    @param [in] index   Index of the memory block graphics item.
     */
    void graphicsChanged(int index);

	/*!
     *  Emitted when a new memory map item is added to the given index.
     *
     *    @param [in] index   Index of the new memory block item.
     */
	void itemAdded(int index);

	/*!
     *  Emitted when a memory map item is removed from the given index.
     *
     *    @param [in] index   Index of the removed memory block item.
     */
	void itemRemoved(int index);

    /*
     *  Informs of memory block name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void blockNameChanged(QString const& oldName, QString const& newName);

    /*!
     *  Invalidate the other memory block models.
     */
    void invalidateOtherFilter();

    /*!
     *  Informs of addressing changes in child items.
     *
     *    @param [in]     Index of the selected address block.
     */
    void childAddressingChanged(int);

private:

    /*!
     *  Get the index of the name column.
     *
     *    @return Index of the name column.
     */
    virtual int nameColumn() const = 0;

    /*!
     *  Get the index of the base address column.
     *
     *    @return Index of the base address column.
     */
    virtual int baseAddressColumn() const = 0;

    /*!
     *  Get the index of the is present column.
     *
     *    @return Index of the is present column.
     */
    virtual int isPresentColumn() const = 0;

    /*!
     *  Get the index of the description column.
     *
     *    @return Index of the description column.
     */
    virtual int descriptionColumn() const = 0;

    /*!
     *  Increase the number of references made in the selected expressions.
     *
     *    @param [in] expressionList  List of expressions.
     */
    void increaseReferencesInPastedExpressions(QStringList const& expressionList);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for address blocks.
    MemoryBlockInterface* blockInterface_;

};

#endif // MEMORYBLOCKMODEL_H
