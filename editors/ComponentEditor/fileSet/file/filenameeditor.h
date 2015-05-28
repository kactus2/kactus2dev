//-----------------------------------------------------------------------------
// File: filenameeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 5.2.2011
//
// Description:
// FileNameEditor is a widget to display the name element in FileSet.
//-----------------------------------------------------------------------------

#ifndef FILENAMEEDITOR_H
#define FILENAMEEDITOR_H

#include <QGroupBox>
#include <QString>
#include <QSharedPointer>
#include <QLabel>
#include <QPlainTextEdit>

class File;
//-----------------------------------------------------------------------------
// Class FileNameEditor.
//-----------------------------------------------------------------------------
class FileNameEditor : public QGroupBox 
{
    Q_OBJECT

public:
	
	/*! The constructor
	 *
	 *      @param [in] file    Pointer to the file being edited.
     *      @param [in] parent  Pointer to the owner of this widget     
	 *
	*/
	FileNameEditor(QSharedPointer<File> file, QWidget *parent);

	//! The destructor
	virtual ~FileNameEditor();

	/*! Checks that the widget has valid inputs.
	 *
	 * @return True if the widget is in valid state and changes can be applied.
	 */
	bool isValid() const;

	//! Refresh the editor.
	void refresh();

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

	//! No copying
	FileNameEditor(const FileNameEditor& other);

	//! No assignment
	FileNameEditor& operator=(const FileNameEditor& other);
	
	//! The label to display the file name.
    QLabel fileNameLabel_;

    //! The editor for description of a file.
    QPlainTextEdit descriptionEditor_;

	//! Pointer to the file being edited.
	QSharedPointer<File> file_;
};

#endif // FILENAMEEDITOR_H
