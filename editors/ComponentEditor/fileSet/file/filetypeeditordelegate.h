//-----------------------------------------------------------------------------
// File: filetypeeditordelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 07.12.2011
//
// Description:
// Provides editor to select a type for a file.
//-----------------------------------------------------------------------------

#ifndef FILETYPEEDITORDELEGATE_H
#define FILETYPEEDITORDELEGATE_H

#include <QStyledItemDelegate>

class BasicLineContentMatcher;

//-----------------------------------------------------------------------------
//! Provides editor to select a type for a file.
//-----------------------------------------------------------------------------
class FileTypeEditorDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent  The owner of this delegate.
	 *
	*/
	FileTypeEditorDelegate(QObject *parent);
	
	//! The destructor
	virtual ~FileTypeEditorDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
		QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private:

	//! No copying
	FileTypeEditorDelegate(const FileTypeEditorDelegate& other);
    FileTypeEditorDelegate& operator=(const FileTypeEditorDelegate& other);

    //! Updates the suggested file types.
    void updateSuggestedItems() const;

    //! Matcher for assisting editing with type suggestions.
    BasicLineContentMatcher* matcher_;
};

#endif // FILETYPEEDITORDELEGATE_H
