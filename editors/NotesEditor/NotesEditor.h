//-----------------------------------------------------------------------------
// File: NotesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.5.2014
//
// Description:
// Editor for writing and viewing notes.
//-----------------------------------------------------------------------------

#ifndef NOTESEDITOR_H
#define NOTESEDITOR_H

#include <IPXACTmodels/vlnv.h>

#include <QSplitter>
#include <QTextEdit>
#include <QWidget>

//-----------------------------------------------------------------------------
// class NotesEditor.
//-----------------------------------------------------------------------------
class NotesEditor : public QWidget
{
	Q_OBJECT

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent widget.
	 */
	explicit NotesEditor(QWidget* parent = 0);

	/*!
	 *  The destructor.
	*/
	~NotesEditor();

    void setPreviewHTML(QString const& html);

private:
	// Disable copying.
	NotesEditor(NotesEditor const& rhs);

	// Disable assignment.
	NotesEditor& operator=(NotesEditor const& rhs);

    void setupLayout();

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The splitter for controlling the space of the editor and preview.
    QSplitter splitter_;

    //! The editor for notes.
    QTextEdit editor_;

    //! The preview of older notes.
    QTextEdit preview_;
};
#endif // NOTESEDITOR_H
