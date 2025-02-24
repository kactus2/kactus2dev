//-----------------------------------------------------------------------------
// File: filegeneraleditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 6.2.2011
//
// Description:
// FileGeneralEditor is a widget to edit File's general settings.
//-----------------------------------------------------------------------------

#ifndef FILEGENERALEDITOR_H
#define FILEGENERALEDITOR_H

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QSharedPointer>

class File;
class FileInterface;

//-----------------------------------------------------------------------------
//! FileGeneralEditor is a widget to edit File's general settings.
//-----------------------------------------------------------------------------
class FileGeneralEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] parent          Pointer to the owner of this widget
	 *    @param [in] fileName        Name of the file that is being edited.
     *    @param [in] fileInterface   Interface for accessing files.
	 */
    FileGeneralEditor(QWidget *parent, std::string const& fileName, FileInterface* fileInterface);

	/*!
     *  The destructor.
     */
	virtual ~FileGeneralEditor() = default;

	/*!
     *  Restore the settings from the model.
	 */
	void refresh();

    /*!
     *  Handle the name change of the containing file.
     *
     *    @param [in] newName     The new name of the file.
     */
    void fileRenamed(std::string const& newName);

signals:

	//! Emitted when the contents on one of the elements change.
	void contentChanged();

private slots:

	//! Handler for changes in logical name.
	void onLogicalNameChanged();

    //! Handler for changes in structural file setting.
    void onStructuralFileChanged();

    //! Handler for changes in include file setting.
	void onIncludeFileChanged();

	//! Handler for external declarations changes.
	void onExternalDecChanged();

private:

	//! No copying. No assignment. No regret.
	FileGeneralEditor(const FileGeneralEditor& other);
	FileGeneralEditor& operator=(const FileGeneralEditor& other);
	
	//! Name of the file that is modified.
    std::string fileName_;

	//! The editor to set the logical name for the file.
	QLineEdit logicalName_;

	//! The check box to set the logical name as default or not.
	QCheckBox logicalDefault_;

    //! The check box to set the file as structural file or not.
    QCheckBox structuralFile_;

	//! The check box to set the file as include file or not.
	QCheckBox includeFile_;

	//! The check box to inform that file contains external declarations.
	QCheckBox externalDec_;

    //! Interface for accessing files.
    FileInterface* fileInterface_;
};

#endif // FILEGENERALEDITOR_H
