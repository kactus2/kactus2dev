//-----------------------------------------------------------------------------
// File: Regionsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.06.2023
//
// Description:
// The model that can be used to display the regions to be edited.
//-----------------------------------------------------------------------------

#ifndef REGIONSSMODEL_H
#define REGIONSSMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Region.h>

#include <QList>
#include <QSharedPointer>

class ExpressionFormatter;
class ReferenceCalculator;
class AddressSpace;
class RegionValidator;

//-----------------------------------------------------------------------------
//! The model that can be used to display the regions to be edited.
//-----------------------------------------------------------------------------
class RegionsModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] regions			The regions being edited.
     *    @param [in] validator		The validator to use to check regions.
     *    @param [in] expressions		The collection of objects for expression handling.
	 *    @param [in] parent          The owner of this model.
	 */
	RegionsModel(QSharedPointer<QList<QSharedPointer<Region> > > regions,
		QSharedPointer<RegionValidator> validator,
		ExpressionSet expressions,
		QObject *parent);
	
	//! The destructor.
	virtual ~RegionsModel() = default;

    //! No copying.
    RegionsModel(const RegionsModel& other) = delete;

    //! No assignment.
    RegionsModel& operator=(const RegionsModel& other) = delete;

	/*!
     *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
	 */
	int rowCount(const QModelIndex& parent = QModelIndex()) const final;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed (always 4).
	 */
	int columnCount(const QModelIndex& parent = QModelIndex()) const final;

	/*!
     *  Get the header data for specified header.
	 *
	 *    @param [in] section The section specifies the row/column number for the header.
	 *    @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *    @param [in] role Specifies the type of the requested data.
	 *
	 *    @return QVariant Contains the requested data.
	 */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

	/*!
     *  Get the data for specified item.
	 *
	 *    @param [in] index Specifies the item that's data is requested.
	 *    @param [in] role The role that defines what kind of data is requested.
	 *
	 *    @return QVariant Contains the data for the item.
	 */
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const final;

	/*!
     *  Save the data to the model for specified item
	 *
	 *    @param [in] index The model index of the item that's data is to be saved.
	 *    @param [in] value The data that is to be saved.
	 *    @param [in] role The role specifies what kind of data should be saved.
	 *
	 *    @return True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) final;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index Model index that identifies the item.
	 *
	 *    @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const final;

    /*!
     *  Get the list of acceptable mime types.
     *
     *    @return The list of acceptable mime types.
     */
    QStringList mimeTypes() const final;

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *    @param [in] index The position where new item should be added at.
	 */
	void onAddItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *    @param [in] index Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);
    
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
     *  Check if the column index is valid for containing expressions.
     *
     *    @param [in] index   The index being evaluated.
     *
     *    @return     True, if column can have expressions, false otherwise.
     */
    bool isValidExpressionColumn(QModelIndex const& index) const final;

    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *    @param [in] index   The index whose expression to get.
     *
     *    @return The expression for the index if available, otherwise the value for the given index.
     */
    QVariant expressionOrValueForIndex(QModelIndex const& index) const final;
    
    /*!
     *  Validates the data in an index.
     *
     *    @param [in] index   The index whose data to validate
     *
     *    @return True, if the data in the index is valid, otherwise false.
     */
    bool validateIndex(QModelIndex const& index) const final;

    /*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *    @param [in] row         The row of the selected item.
     *    @param [in] valueID     The id of the referenced parameter.
     *
     *    @return The amount of references made to the selected id on the selected row.
     */
    int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const final;

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

	//! Emitted when a new Region is added to the address space.
	void regionAdded(QSharedPointer<Region> Region);

	//! Emitted when a Region is removed from the address space.
	void regionRemoved(const QString& RegionName);

	//! Emitted when a Region is renamed.
	void regionRenamed(const QString& oldName, const QString& newName);

	//! Emitted when the range or offset of a Region has changed.
	void regionChanged(QSharedPointer<Region> Region);

private:

	/*! Get the last address of the address space that has Region assigned to it.
	 * 
	 * If there are no Regions defined then 0 is returned.
	 * 
	 *    @return The last address contained in a Region.
	*/
	quint64 getLastRegionedAddress() const;

    /*!
     *  Get the names of the contained address blocks.
     *
     *    @return The names of the contained address blocks.
     */
    QStringList getCurrentItemNames();

    /*!
     *  Increase the number of references made in the copied address space Region.
     *
     *    @param [in] pastedRegion           The copied address space Region.
     *    @param [in] referenceCalculator     The reference calculator.
     */
    void increaseReferencesInPastedRegion(QSharedPointer<Region> pastedRegion,
        ReferenceCalculator& referenceCalculator);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the data structure that contains the regions.
    QSharedPointer<QList<QSharedPointer<Region> > > regions_;

	//! Validator for regions.
	QSharedPointer<RegionValidator> validator_;

    //! Pointer to the used expression formatter.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // REGIONSSMODEL_H
