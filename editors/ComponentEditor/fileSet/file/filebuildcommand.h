//-----------------------------------------------------------------------------
// File: filebuildcommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDCOMMAND_H
#define FILEBUILDCOMMAND_H

#include "targetnameedit.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QFileInfo>
#include <QSharedPointer>

class LibraryInterface;
class BuildCommand;
class File;
class Component;

//-----------------------------------------------------------------------------
//! FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------
class FileBuildCommand : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent      Pointer to the owner of this widget.
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] file        Pointer to the file that is being edited.
	 */
	FileBuildCommand(QWidget *parent, LibraryInterface* handler, QSharedPointer<Component> component,
        QSharedPointer<File> file);

	//! The destructor.
	virtual ~FileBuildCommand();

	/*!
     *  Restore the changes from the model to the editor.
	 */
	void refresh();

signals:

	//! Emitted when contents of the widget change.
	void contentChanged();

private slots:

	//! Handler for command changes.
	void onCommandChanged();

	//! Handler for flag changes.
	void onFlagsChanged();

	//! Handler for target changes.
	void onTargetChanged();

    void onReplaceDefaultChanged();

private:

	//! No copying. No assignment.
	FileBuildCommand(const FileBuildCommand& other);
	FileBuildCommand& operator=(const FileBuildCommand& other);

	/*!
	 *  Set up the target editor.
	 */
	void setupTarget();

	/*!
	 *  Set up the command editor.
	 */
	void setupCommand();

	/*!
	 *  Set up the flags editor.
	 */
	void setupFlags();

    void setupReplaceDefaultFlags();

	//! Pointer to the file's buildCommand.
    QSharedPointer<BuildCommand> buildCommand_; 

	//! Editor to set file's build command.
	QLineEdit command_;

	//! Editor to set build command's flags.
	QLineEdit flags_;

	//! Editor to set build command's replaceDefaultFlags setting.
    QLineEdit replaceDefault_;

	//! Editor to set build command's target file.
	TargetNameEdit target_;

	//! The layout for the widget.
	QGridLayout layout_;
};

#endif // FILEBUILDCOMMAND_H
