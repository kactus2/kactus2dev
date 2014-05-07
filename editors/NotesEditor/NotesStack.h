//-----------------------------------------------------------------------------
// File: NotesStack.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.5.2014
//
// Description:
// Stacks NotesEditors for different VLNVs and contexts in a single widget.
//-----------------------------------------------------------------------------

#ifndef NotesStack_H
#define NotesStack_H

#include <library\LibraryManager\vlnv.h>

#include <QSplitter>
#include <QTextEdit>
#include <QStackedWidget>

//-----------------------------------------------------------------------------
// class NotesStack.
//-----------------------------------------------------------------------------
class NotesStack : public QStackedWidget
{
	Q_OBJECT

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent widget.
	 */
	explicit NotesStack(QWidget* parent);

	/*!
	 *  The destructor.
	*/
	~NotesStack();

public slots:

    //! Called when the active VLNV is changed.
    void onVLNVChanged(VLNV const& vlnv, QString const& path);

    //! Called when the active context is changed.
    void onContextChanged(QString const& context);

private:
	// Disable copying.
	NotesStack(NotesStack const& rhs);

	// Disable assignment.
	NotesStack& operator=(NotesStack const& rhs);

    /*!
     *  Saves a path associated with a VLNV.
     *
     *      @param [in] vlnv   The VLVN whose path to save.
     *      @param [in] path   The path to save.
     */
    void saveVLNVpath(VLNV const& vlnv, QString const& path);

    /*!
     *  Formats the path for a VLNV.
     *
     *      @param [in] vlnvPath    The complete path of the VLNV.
     *      @param [in] vlnv        The VLNV whose path to format.
     *
     *      @return The formatted path.
     */
    QString formatPathForVlnv(QString const& vlnvPath, VLNV const& vlnv);

    /*!
     *  Changes the editor to current VLNV and context.
     */
    void changeActiveEditor();

    /*!
     *  Changes the currently active path according to the path of current VLNV and context.
     */
    void adjustPath();

    /*!
     *  Checks if the currently active VLNV and context has not an editor in the stack.
     *
     *      @return True, if no editor is in the stack, otherwise false.
     */
    bool noOpenEditor();

    /*!
     *  Creates a new editor for the current VLNV and context.
     */
    void createEditor();

    /*!
     *  Associates an editor with the currently active VLNV.
     */
    void addEditorForVlnv(int stackIndex);

    /*!
     *  Changes the editor for the current VLNV and context to the top of the stack.
     */
    void showEditor();

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The current context.
    QString context_;

    //! The current active path.
    QString path_;

    //! The current active VLNV.
    VLNV vlnv_;

    //! The paths of the editors.
    QList<QString> editors_;

    //! The indexes of the editors for VLNVs.
    QMap<VLNV, QList<int> > editorsForVlnv_;

    //! The paths for VLNVs.
    QMap<VLNV, QString> vlnvPaths_;
};
#endif // NotesStack_H