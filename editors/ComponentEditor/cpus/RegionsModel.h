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
class AddressSpace;
class ReferenceCalculator;

//-----------------------------------------------------------------------------
//! The model that can be used to display the Regions to be edited.
//-----------------------------------------------------------------------------
class RegionsModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] Regions                The Regions being edited.
	 *      @param [in] parameterFinder         The finder for available parameter names.
	 *      @param [in] expressionFormatter     Formatter for expressions.
	 *      @param [in] parent                  The owner of this model.
	 */
	RegionsModel(QSharedPointer<QList<QSharedPointer<Region> > > Regions,
		ExpressionSet expressions,
		QObject *parent);
	
	//! The destructor.
	virtual ~RegionsModel() = default;

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
	 *      @return The number of columns to be displayed (always 4).
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

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
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const;

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in] index The position where new item should be added at.
	 */
	void onAddItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in] index Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);
    
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
	//! No copying.
	RegionsModel(const RegionsModel& other);

	//! No assignment.
	RegionsModel& operator=(const RegionsModel& other);

	/*! Get the last address of the address space that has Region assigned to it.
	 * 
	 * If there are no Regions defined then 0 is returned.
	 * 
	 *      @return The last address contained in a Region.
	*/
	quint64 getLastRegionedAddress() const;

    /*!
     *  Get the names of the contained address blocks.
     *
     *      @return The names of the contained address blocks.
     */
    QStringList getCurrentItemNames();

    /*!
     *  Increase the number of references made in the copied address space Region.
     *
     *      @param [in] pastedRegion           The copied address space Region.
     *      @param [in] referenceCalculator     The reference calculator.
     */
    void increaseReferencesInPastedRegion(QSharedPointer<Region> pastedRegion,
        ReferenceCalculator& referenceCalculator);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the data structure that contains the real Regions.
    QSharedPointer<QList<QSharedPointer<Region> > > regions_;

    //! Pointer to the used expression formatter.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // REGIONSSMODEL_H
