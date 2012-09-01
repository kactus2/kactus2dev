//-----------------------------------------------------------------------------
// File: AddressDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.08.2012
//
// Description:
// The delegate that provides widgets for the address editor.
//-----------------------------------------------------------------------------

#ifndef ADDRESSDELEGATE_H
#define ADDRESSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Address editor columns.
//-----------------------------------------------------------------------------
enum AddressEditorColumn
{
    ADDRESS_COL_INTERFACE_NAME = 0,
    ADDRESS_COL_MAP_NAME,
    ADDRESS_COL_START_ADDRESS,
    ADDRESS_COL_LOCKED,
    ADDRESS_COL_RANGE,
    ADDRESS_COL_END_ADDRESS,
    ADDRESS_COL_COUNT
};

//-----------------------------------------------------------------------------
//! The delegate that provides widgets for the address editor.
//-----------------------------------------------------------------------------
class AddressDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	/*!
	 *  Constructor.
     *
	 *      @param [in] parent Pointer to the owner of this instance.
	 */
	AddressDelegate(QObject* parent = 0);
	
	/*!
     *  Destructor.
     */
	virtual ~AddressDelegate();

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
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*!
     *  Saves the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 *
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		                      QModelIndex const& index) const;

protected:
    void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
        QModelIndex const& index);

private:
    // Disable copying.
    AddressDelegate(AddressDelegate const& rhs);
    AddressDelegate& operator=(AddressDelegate const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Boolean for ad-hoc group modify.
    bool groupModify_;

    //! The new state for the group modify.
    bool groupState_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSDELEGATE_H
