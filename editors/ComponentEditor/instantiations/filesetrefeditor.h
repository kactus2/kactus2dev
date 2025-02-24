//-----------------------------------------------------------------------------
// File: filesetrefeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.4.2011
//
// Description:
// Editor to set the file set references of a view.
//-----------------------------------------------------------------------------

#ifndef FILESETREFEDITOR_H
#define FILESETREFEDITOR_H

#include <common/widgets/listManager/listmanager.h>

#include <QSharedPointer>

class Component;
class FileSetInterface;

//-----------------------------------------------------------------------------
//! Editor to set the file set references of a view.
//-----------------------------------------------------------------------------
class FileSetRefEditor : public ListManager
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] fileSetInterface    Interface for accessing file sets.
	 *    @param [in] title               Title for the QGroupBox.
	 *    @param [in] parent              The owner of this editor.
	 */
    FileSetRefEditor(FileSetInterface* fileSetInterface, const QString title = tr("List"), QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~FileSetRefEditor();

	/*!
     *  Initialize the file set reference editor.
	 *
	 *    @param [in] items   QStringList that contains the items to add to the widget.
	 */
	virtual void initialize(QStringList const& items = QStringList());

private:

	//! No copying.
	FileSetRefEditor(const FileSetRefEditor& other);

	//! No assignment.
	FileSetRefEditor& operator=(const FileSetRefEditor& other);

	//! The component being edited.
    FileSetInterface* fileSetInterface_;
};

#endif // FILESETREFEDITOR_H
