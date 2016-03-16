//-----------------------------------------------------------------------------
// File: filetypeeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileTypeEditor is used to specify a file type for a file.
//-----------------------------------------------------------------------------

#ifndef FILETYPEEDITOR_H
#define FILETYPEEDITOR_H

#include <common/widgets/listManager/listmanager.h>

#include <QSharedPointer>

class File;

//-----------------------------------------------------------------------------
//! FileTypeEditor is used to specify a file type for a file.
//-----------------------------------------------------------------------------
class FileTypeEditor : public ListManager
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget
	 *      @param [in] file    Pointer to the file being edited
	 */
	FileTypeEditor(QWidget *parent, QSharedPointer<File> file);

	//! The destructor.
	virtual ~FileTypeEditor();

	/*!
     *  Initialize the file type editor.
	 *
	 * This function must be called after creating the file type editor and before using it.
	 * 
	 *      @param [in] items   QStringList that contains the items to add to the widget.
	 */
	virtual void initialize(const QStringList& items = QStringList());

	/*!
     *  Apply the changes from the editor to the model.
	 */
	void apply();

	/*!
     *  Restore the changes from the model to the editor.
	 */
	void restore();

	/*!
     *  Check if the editor is in a valid state or not.
	 *
	 *      @return True if the editor is in valid state
	 */
	bool isValid() const;

private:

	//! No copying. No assignment. No quarter.
	FileTypeEditor(const FileTypeEditor& other);
	FileTypeEditor& operator=(const FileTypeEditor& other);

	//! Pointer to the file-model being edited.
	QSharedPointer<File> file_;
};

#endif // FILETYPEEDITOR_H
