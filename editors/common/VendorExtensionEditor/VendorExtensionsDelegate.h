//-----------------------------------------------------------------------------
// File: VendorExtensionsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the vendor extensions.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSIONSDELEGATE_H
#define VENDOREXTENSIONSDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The delegate to provide editors to add/remove/edit the vendor extensions.
//-----------------------------------------------------------------------------
class VendorExtensionsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] parent      The owner of the delegate.
	 */
    VendorExtensionsDelegate(QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~VendorExtensionsDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Paint the vendor extension rows.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] option      Available style options.
     *      @param [in] index       The current index.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    //! No copying. No assignment.
    VendorExtensionsDelegate(const VendorExtensionsDelegate& other) = delete;
    VendorExtensionsDelegate& operator=(const VendorExtensionsDelegate& other) = delete;

signals:

    /*!
     *  Informs a change in the edited document.
     */
    void contentChanged();

private:
	
    /*!
     *  Repositions the editor if there is not enough space under the default position. The editor is then resized
     *  to use the available space.
     *
     *      @param [in] editor  The editor to reposition.
     *      @param [in] option  The style options for the editor.
     *      @param [in] index   The current index.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const;
};

#endif // VENDOREXTENSIONSDELEGATE_H
