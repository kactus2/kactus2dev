//-----------------------------------------------------------------------------
// File: ResetTypesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the reset types of a component.
//-----------------------------------------------------------------------------

#ifndef RESETTYPESDELEGATE_H
#define RESETTYPESDELEGATE_H

#include <QStyledItemDelegate>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The delegate to provide editors to add/remove/edit the reset types of a component.
//-----------------------------------------------------------------------------
class ResetTypesDelegate: public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] parent      The owner of the delegate.
	 */
    ResetTypesDelegate(QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~ResetTypesDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
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

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
     */
	void commitAndCloseEditor();

private:
	
	//! No copying.
	ResetTypesDelegate(const ResetTypesDelegate& other);
    ResetTypesDelegate& operator=(const ResetTypesDelegate& other);
};

#endif // RESETTYPESDELEGATE_H
