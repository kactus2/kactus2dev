//-----------------------------------------------------------------------------
// File: addressspacesmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.06.2012
//
// Description:
// The model class to manage the objects for address spaces editor.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACESMODEL_H
#define ADDRESSSPACESMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

#include <QSharedPointer>

class ExpressionFormatter;
class Component;
class AddressSpace;
class AddressSpaceValidator;
class AddressSpaceExpressionGatherer;
class ReferenceCalculator;
//-----------------------------------------------------------------------------
//! The model class to manage the objects for address spaces editor.
//-----------------------------------------------------------------------------
class AddressSpacesModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

   /*!
    *   The constructor.
    *
    *       @param [in] component               The component being edited.
    *       @param [in] parameterFinder         The finder for available parameter names.
    *       @param [in] expressionFormatter     Formatter for expressions.
    *       @param [in] addressSpaceValidator   Validator used for address spaces.
    *       @param [in] parent                  The owner of the model.
	*/
	AddressSpacesModel(QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<AddressSpaceValidator> addressSpaceValidator,
		QObject *parent);
	
	//! The destructor.
	virtual ~AddressSpacesModel();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section The section specifies the row/column number for the header.
	 *      @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *      @param [in] role Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is requested.
	 *      @param [in] role The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item
	 *
	 *      @param [in] index The model index of the item that's data is to be saved.
	 *      @param [in] value The data that is to be saved.
	 *      @param [in] role The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

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
	 *      @param [in] index The index identifying the position for new item.
	 */
	virtual void onAddItem(const QModelIndex& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
	 */
	virtual void onRemoveItem(const QModelIndex& index);
    
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

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*!
     *  Emitted when a new file set is added to the model.
	 *
	 *      @param [in] index The index of the added file set.
	 */
	void addrSpaceAdded(int index);

	/*!
     *  Emitted when a file set is removed from the model.
	 *
	 *      @param [in] index The index of the remove file set.
	 */
	void addrSpaceRemoved(int index);

    /*!
     *  Emitted when address unit bits are changed in an address space.
     *
     *      @param [in] addressSpaceindex   Index of the target address space.
     */
    void aubChangedOnRow(int addressSpaceindex);

protected:
    
    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return     True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *      @param [in] index   The index whose expression to get.
     *
     *      @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;
    
    /*!
     *  Validates the data in an index.
     *
     *      @param [in] index   The index whose data to validate
     *
     *      @return True, if the data in the index is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *      @param [in] row         The row of the selected item.
     *      @param [in] valueID     The id of the referenced parameter.
     *
     *      @return The amount of references made to the selected id on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

private:
	
    /*!
     *  Decrease the number of references made from a removed address space.
     *
     *      @param [in] removedAddressSpace     The removed address space.
     */
    void decreaseReferencesWithRemovedAddressSpace(QSharedPointer<AddressSpace> removedAddressSpace);
    
    /*!
     *  Get the names of the contained address blocks.
     *
     *      @return The names of the contained address blocks.
     */
    QStringList getCurrentItemNames();

    /*!
     *  Increase the number of references made in the copied address space.
     *
     *      @param [in] pastedSpace             The copied address space.
     *      @param [in] gatherer                Address space expressions gatherer.
     *      @param [in] referenceCalculator     The reference calculator.
     */
    void increaseReferencesInPastedAddressSpace(QSharedPointer<AddressSpace> pastedSpace,
        AddressSpaceExpressionGatherer& gatherer, ReferenceCalculator& referenceCalculator);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! No copying
	AddressSpacesModel(const AddressSpacesModel& other);
	AddressSpacesModel& operator=(const AddressSpacesModel& other);

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Contains the address spaces to edit.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;

    //! Expression formatter.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The used address space validator.
    QSharedPointer<AddressSpaceValidator> addressSpaceValidator_;
};

#endif // ADDRESSSPACESMODEL_H
