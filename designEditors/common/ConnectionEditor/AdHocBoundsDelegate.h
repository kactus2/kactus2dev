//-----------------------------------------------------------------------------
// File: AdHocBoundsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Delegate for the table visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCBOUNDSDELEGATE_H
#define ADHOCBOUNDSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit ad-hoc visibility of component ports.
//-----------------------------------------------------------------------------
class AdHocBoundsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  Constructor.
     *
	 *      @param [in] parent Pointer to the owner of this instance.
	 */
	AdHocBoundsDelegate(QObject *parent = 0);
	
	/*!
     *  Destructor.
     */
	virtual ~AdHocBoundsDelegate();

	/*!
     *  Creates a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*!
     *  Sets the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Saves the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 *
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private:
    // Disable copying.
    AdHocBoundsDelegate(AdHocBoundsDelegate const& rhs);
    AdHocBoundsDelegate& operator=(AdHocBoundsDelegate const& rhs);
};

//-----------------------------------------------------------------------------

#endif // ADHOCBOUNDSDELEGATE_H
