//-----------------------------------------------------------------------------
// File: VendorExtensionsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2018
//
// Description:
// Model class to manage the vendor extension values being edited.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSIONSMODEL_H
#define VENDOREXTENSIONSMODEL_H

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <QSharedPointer>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Model class to manage the vendor extension values being edited.
//-----------------------------------------------------------------------------
class VendorExtensionsModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent  The owner of this model.
	 */
	VendorExtensionsModel(QObject *parent);
	
	/*!
	 *  The destructor.
	 */
	virtual ~VendorExtensionsModel() = default;

    //! No copying.
    VendorExtensionsModel(const VendorExtensionsModel& other) = delete;

    //! No assignment.
    VendorExtensionsModel& operator=(const VendorExtensionsModel& other) = delete;

	/*!
	 *  Set the vendor extensions to edit.
	 *
     *      @param [in] extensions The vendor extensions to view/edit.
	 */
    void setVendorExtensions(QSharedPointer<QList<QSharedPointer<VendorExtension> > > extensions);

	/*!
	 *  Get the number of rows to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent, must always be invalid index.
     *
     *      @return The number of rows to display.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of columns to display.
	 *
	 *      @param [in] parent  Identifies the parent, must always be invalid index.
     *
     *      @return The number of columns to display.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the data for specified index in given data role.
	 *
	 *      @param [in] index   Identifies the object that's data is wanted.
	 *      @param [in] role    Identifies the type of data being requested.
     *
     *      @return QVariant that contains the requested data.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Get the data for the given header in the given data role.
	 *
	 *      @param [in] section         Identifies the header section.
	 *      @param [in] orientation     Only horizontal header is supported.
	 *      @param [in] role            Identifies the type of the data being requested.
     *
     *      @return QVariant that contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
	 *  Set the data for the specified index.
	 *
	 *      @param [in] index   Identifies the object which data is to be saved.
	 *      @param [in] value   Contains the data to be saved.
	 *      @param [in] role    Identifies the type of the data to be saved.
     *
     *      @return True if data was saved successfully.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*!
	 *  Get info on what operations are possible for specified item.
	 *
	 *      @param [in] index   Identifies the item that's operations are requested.
     *
     *      @return Flags containing info on which operations are available for given index.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the model index of the parent of the object.
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelindex that identifies the parent of the given object.
     */
    QModelIndex parent(const QModelIndex& child) const;

public slots:

	/*!
	 *  Clear the model.
	 */
	void clear();

    /*!
     *  Handles the addition of a new item.
     *
     *      @param [in] index   The index to add the new item.
     */
    void onAddItem(QModelIndex const& index);

    /*!
     *  Handles item removal.
     *
     *      @param [in] index   The index to remove the item from.
     */
    void onRemoveItem(QModelIndex const& index);

    /*!
     *  Handles the addition of a new sub item.
     *
     *      @param [in] index   The parent index for the new sub item.
     */
    void onAddSubItem(QModelIndex const& index);

    /*!
     *  Remove all the sub items from the selected item.
     *
     *      @param [in] index   Index of the selected item.
     */
    void onRemoveAllSubItems(QModelIndex const& index);

signals:

	/*!
     *  Emitted when contents of the model changes.
     */
	void contentChanged();

private:

    /*!
    *  Check if the given index is valid.
    *
    *      @param [in] index   The index to be checked.
    *
    *      @return True, if the given index is valid, false otherwise.
    */
    bool isIndexValid(QModelIndex const& index) const;

    /*!
    * Check if the index content can be edited by the user.
    *
    *     @param [in] index  The index to check.
    *
    *     @return True, if the index content is editable, otherwise false.
    */
    bool indexIsEditable(QModelIndex const& index) const;

    /*!
     *  Get the value for item in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return Value of the selected index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Get the generic extension of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return Generic vendor extension of the selected index.
     */
    GenericVendorExtension* getExtensionForIndex(QModelIndex const& index) const;

    /*!
     *  Create a parent index for the selected child index.
     *
     *      @param [in] childIndex  The selected child index.
     *
     *      @return The created parent index.
     */
    QModelIndex createParentIndexForChildExtension(QModelIndex const& childIndex) const;

    /*!
     *  Get the row of the selected extension item.
     *
     *      @param [in] extensionItem   The selected extension item.
     *
     *      @return Row of the selected extension item.
     */
    int getRowForItem(GenericVendorExtension* extensionItem) const;

    /*!
     *  Get the row of the selected root extension item.
     *
     *      @param [in] parentItem  The selected root extension item.
     *
     *      @return Row of the selected rot extension item.
     */
    int getRowForRootItem(GenericVendorExtension* parentItem) const;

    /*!
     *  Check if the column of the selected index is mandatory.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the column is mandatory, false otherwise.
     */
    bool mandatoryColumn(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! A list of the vendor extensions of the model.
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions_;

    //! Empty generic vendor extension for comparisons.
    GenericVendorExtension emptyExtension_;
};

#endif // VENDOREXTENSIONSMODEL_H
