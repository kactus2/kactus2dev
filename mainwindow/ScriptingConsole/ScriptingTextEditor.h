//-----------------------------------------------------------------------------
// File: ScriptingTextEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#ifndef SCRIPTINGTEXTEDITOR_H
#define SCRIPTINGTEXTEDITOR_H

#include <QPlainTextEdit>

#include <QAction>

class WriteChannel;

//-----------------------------------------------------------------------------
//! Text editor for script write and run.
//-----------------------------------------------------------------------------
class ScriptingTextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:

    //! The constructor.
    ScriptingTextEditor(WriteChannel* outputChannel, QWidget* parent = nullptr);
    
    //! The destructor.
    virtual ~ScriptingTextEditor() = default;

public slots:

    /*! Print the given text in the editor.
     *
     *      @param [in] input  The text to print.
     */
    void print(QString const& input);

    /*! Print the given error text in the editor.
     *
     *      @param [in] input  The error text to print.
     */
    void printError(QString const& input);

protected:

    //! Event handler for key press handling.
    virtual void keyPressEvent(QKeyEvent *e) override;

    //! Event handler for context menu requests.
    void contextMenuEvent(QContextMenuEvent* event);    

private slots:
    
    //! Clears the editor of all text.
    void onClear();

private:

    //! Check if text can be pasted at currently selected position.
    bool canPaste() const;

    //! Lock position in the text i.e. text before the position cannot be changed.
    int textLockPosition_;

    //! The current prompt text.
    QString promptText_;

    //! Write channel to write user input into.
    WriteChannel* outputChannel_;

    //! The used monospace font family.
    QString fontFamily_;

    //! Copy the selected text to the clip board.
    QAction copyAction_;

};

#endif // SCRIPTINGTEXTEDITOR_H
