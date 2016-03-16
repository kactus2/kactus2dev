//-----------------------------------------------------------------------------
// File: filesetrefeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
	 *      @param [in] component   The component being edited.
	 *      @param [in] title       Title for the QGroupBox
	 *      @param [in] parent      The owner of this editor.
	 */
	FileSetRefEditor(QSharedPointer<Component> component, const QString title = tr("List"), QWidget *parent = 0);
	
	//! The destructor.
	virtual ~FileSetRefEditor();

	/*!
     *  Initialize the file set reference editor.
	 *
	 *      @param [in] items   QStringList that contains the items to add to the widget.
	 */
	virtual void initialize(QStringList const& items = QStringList());

private:

	//! No copying.
	FileSetRefEditor(const FileSetRefEditor& other);

	//! No assignment.
	FileSetRefEditor& operator=(const FileSetRefEditor& other);

	//! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // FILESETREFEDITOR_H
