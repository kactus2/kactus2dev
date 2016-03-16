//-----------------------------------------------------------------------------
// File: enumeratedvaluedelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The delegate that provides editors to add/remove/edit the enumerated values of field.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUEDELEGATE_H
#define ENUMERATEDVALUEDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit the enumerated values of field.
//-----------------------------------------------------------------------------
class EnumeratedValueDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of the delegate.
	 */
	EnumeratedValueDelegate(QObject *parent);	

	//! The destructor.
	virtual ~EnumeratedValueDelegate();

	/*!
     *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! No copying.
	EnumeratedValueDelegate(const EnumeratedValueDelegate& other);

	//! No assignment.
	EnumeratedValueDelegate& operator=(const EnumeratedValueDelegate& other);
};

#endif // ENUMERATEDVALUEDELEGATE_H
