//-----------------------------------------------------------------------------
// File: ComPropertyDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 17.4.2012
//
// Description:
// Delegate for editing COM properties.
//-----------------------------------------------------------------------------

#ifndef COMPROPERTYDELEGATE_H
#define COMPROPERTYDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
// Delegate for editing COM properties.
//-----------------------------------------------------------------------------
class ComPropertyDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
     *
     *    @param [in] parent The parent object.
     */
	ComPropertyDelegate(QObject* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ComPropertyDelegate();

	/*!
     *  Creates a new editor for the given item.
	 *
	 *    @param [in] parent  The parent widget.
     *    @param [in] option  Contains options for the editor.
     *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
		                          QModelIndex const& index) const;

	/*!
     *  Sets the data for the editor.
	 *
	 *    @param [in] editor  The editor where the data is to be set.
	 *    @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Saves the data from the editor to the model.
	 *
	 *    @param [in] editor  The editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
	 *    @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		                      QModelIndex const& index) const;

private slots:
	/*! 
     *  Commits the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
    // Disable copying.
    ComPropertyDelegate(ComPropertyDelegate const& rhs);
    ComPropertyDelegate& operator=(ComPropertyDelegate const& rhs);
};

//-----------------------------------------------------------------------------

#endif // COMPROPERTYDELEGATE_H
