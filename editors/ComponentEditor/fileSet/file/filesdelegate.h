//-----------------------------------------------------------------------------
// File: filesdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 6.6.2012
//
// Description:
// Delegate for creating editors for files.
//-----------------------------------------------------------------------------

#ifndef FILESDELEGATE_H
#define FILESDELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate for creating editors for files.
//-----------------------------------------------------------------------------
class FilesDelegate : public MultilineDescriptionDelegate
{
	Q_OBJECT

public:

	//! The minimum height for the editor.
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of the delegate.
	 */
	FilesDelegate(QObject *parent);
	
	//! The destructor.
	~FilesDelegate();

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

protected:

    //! Gets the descriptions column.
    virtual int descriptionColumn() const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:

	//! No copying. No assignment.
	FilesDelegate(const FilesDelegate& other);
	FilesDelegate& operator=(const FilesDelegate& other);
};

#endif // FILESDELEGATE_H
