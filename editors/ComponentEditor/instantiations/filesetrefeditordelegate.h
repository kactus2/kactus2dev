//-----------------------------------------------------------------------------
// File: filesetrefeditordelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.12.2011
//
// Description:
// Delegate that manages the editors to select a file set in a component.
//-----------------------------------------------------------------------------

#ifndef FILESETREFEDITORDELEGATE_H
#define FILESETREFEDITORDELEGATE_H

#include <common/delegates/ComboDelegate/combodelegate.h>

#include <QSharedPointer>

class Component;
class FileSetInterface;

//-----------------------------------------------------------------------------
//! Delegate that manages the editors to select a file set in a component.
//-----------------------------------------------------------------------------
class FileSetRefEditorDelegate : public ComboDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] parent              The pointer to the owner of this delegate.
	 *    @param [in] fileSetInterface    Interface for accessing file sets.
	 */
    FileSetRefEditorDelegate(QObject *parent, FileSetInterface* fileSetInterface);

	//! The destructor.
	virtual ~FileSetRefEditorDelegate();

	/*!
     *  Create a new editor for the given item.
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
        const;

private:

    //! No copying. No assignment.
	FileSetRefEditorDelegate(const FileSetRefEditorDelegate& other);
	FileSetRefEditorDelegate& operator=(const FileSetRefEditorDelegate& other);

	//! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;
};

#endif // FILESETREFEDITORDELEGATE_H
