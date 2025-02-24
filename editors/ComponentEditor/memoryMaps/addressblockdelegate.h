//-----------------------------------------------------------------------------
// File: addressblockdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The delegate that provides editors to add/remove/edit the details of address block.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKDELEGATE_H
#define ADDRESSBLOCKDELEGATE_H

#include "AddressBlockColumns.h"

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit the details of address block.
//-----------------------------------------------------------------------------
class AddressBlockDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:
	
	/*!
     *  The constructor
	 *
	 *    @param [in] completionModel     Model containing the completions used in expression editor.
     *    @param [in] parameterFinder     The parameter finder to use for for expression editor.
	 *    @param [in] parent              Pointer to the owner of this delegate.
	 */
	AddressBlockDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	//! The destructor.
	virtual ~AddressBlockDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *    @param [in] editor  The editor where the data is to be set.
	 *    @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *    @param [in] editor  The editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
	 *    @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

protected:
    
    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *    @param [in] column   The column to check.
     *
     *    @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    //! Gets the description column.
    virtual int descriptionColumn() const;

private:
	
	//! No copying
	AddressBlockDelegate(const AddressBlockDelegate& other);
	AddressBlockDelegate& operator=(const AddressBlockDelegate& other);
};

#endif // ADDRESSBLOCKDELEGATE_H
