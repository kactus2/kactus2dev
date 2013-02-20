//-----------------------------------------------------------------------------
// File: FileTypesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.02.2013
//
// Description:
// Delegate which provides widgets for the file types settings view.
//-----------------------------------------------------------------------------

#ifndef FILETYPESDELEGATE_H
#define FILETYPESDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate which provides widgets for the file types settings view.
//-----------------------------------------------------------------------------
class FileTypesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	/*!
	 *  Constructor.
     *
	 *      @param [in] parent Pointer to the owner of this instance.
	 */
	FileTypesDelegate(QObject* parent = 0);
	
	/*!
     *  Destructor.
     */
	virtual ~FileTypesDelegate();

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

private:
    // Disable copying.
    FileTypesDelegate(FileTypesDelegate const& rhs);
    FileTypesDelegate& operator=(FileTypesDelegate const& rhs);
};

#endif // FILETYPESDELEGATE_H
