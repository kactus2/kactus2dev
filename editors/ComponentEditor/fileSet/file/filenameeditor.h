//-----------------------------------------------------------------------------
// File: filenameeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 5.2.2011
//
// Description:
// FileNameEditor is a widget to display the name element in FileSet.
//-----------------------------------------------------------------------------

#ifndef FILENAMEEDITOR_H
#define FILENAMEEDITOR_H

#include <QGroupBox>
#include <QSharedPointer>
#include <QLabel>
#include <QPlainTextEdit>

class File;
class FileInterface;

//-----------------------------------------------------------------------------
//! FileNameEditor is a widget to display the name element in FileSet.
//-----------------------------------------------------------------------------
class FileNameEditor : public QGroupBox 
{
    Q_OBJECT

public:
	
	/*!
     *  The constructor
	 *
	 *    @param [in] fileName        Name of to the file being edited.
     *    @param [in] fileInterface   Interface for accessing files.
     *    @param [in] parent          Pointer to the owner of this widget     
	 */
    FileNameEditor(std::string const& fileName, FileInterface* fileInterface, QWidget *parent);

	/*!
     *  The destructor
     */
	virtual ~FileNameEditor() = default;

    //! No copying. No assignment.
    FileNameEditor(const FileNameEditor& other) = delete;
    FileNameEditor& operator=(const FileNameEditor& other) = delete;

	/*!
     *  Refresh the editor.
     */
	void refresh();

    /*!
     *  Handle the name change of the containing file.
     *
     *    @param [in] newName     The new name of the file.
     */
    void fileRenamed(std::string const& newName);

signals:

    /*!
     *  Informs of a change of content in the component.
     */
    void contentChanged();

private slots:

    /*!
     *  Handles the change of description of file.
     */
    void onDescriptionChanged();

private:

	
	//! The label to display the file name.
    QLabel fileNameLabel_;

    //! The editor for description of a file.
    QPlainTextEdit descriptionEditor_;

	//! Name of the file being edited.
    std::string fileName_;

    //! Interface for accessing files.
    FileInterface* fileInterface_;
};

#endif // FILENAMEEDITOR_H
