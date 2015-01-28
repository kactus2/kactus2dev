/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockdelegate.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKDELEGATE_H
#define ADDRESSBLOCKDELEGATE_H

#include "AddressBlockColumns.h"

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit the details of address block.
//-----------------------------------------------------------------------------
class AddressBlockDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	
	/*! The constructor
	 *
	 *      @param [in] parent Pointer to the owner of this delegate.
	 *
	*/
	AddressBlockDelegate(QObject *parent);
	
	//! The destructor
	virtual ~AddressBlockDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
	        QModelIndex const& index) const;

private:
	
	//! No copying
	AddressBlockDelegate(const AddressBlockDelegate& other);

	//! No assignment
	AddressBlockDelegate& operator=(const AddressBlockDelegate& other);
};

#endif // ADDRESSBLOCKDELEGATE_H
