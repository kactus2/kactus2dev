//-----------------------------------------------------------------------------
// File: filesetsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 29.5.2012
//
// Description:
// The delegate class to provide editors for file sets editor.
//-----------------------------------------------------------------------------

#ifndef FILESETSDELEGATE_H
#define FILESETSDELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate class to provide editors for file sets editor.
//-----------------------------------------------------------------------------
class FileSetsDelegate : public MultilineDescriptionDelegate
{
	Q_OBJECT

public:

	//! The column number for name field.
	static const int NAME_COLUMN = 0;

	//! The column number for group identifiers.
	static const int GROUP_COLUMN = 1;

    //! The column number for description field.
    static const int DESC_COLUMN = 2;

	//! The minimum height for the custom list editor.
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*!
     *  The constructor
	 *
	 *      @param [in] parent Pointer to the owner of the delegate.
	 */
	FileSetsDelegate(QObject *parent);
	
	//! The destructor
	~FileSetsDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
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

protected:

    //! Gets the description column.
    virtual int descriptionColumn() const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! No copying
	FileSetsDelegate(const FileSetsDelegate& other);

	//! No assignment
	FileSetsDelegate& operator=(const FileSetsDelegate& other);
};

#endif // FILESETSDELEGATE_H
