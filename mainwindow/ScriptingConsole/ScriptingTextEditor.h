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

    virtual void keyPressEvent(QKeyEvent *e) override;

    void contextMenuEvent(QContextMenuEvent* event);    

private slots:
    
    void onClear();

private:

    bool canPaste() const;

    int textLockPosition_;

    QString promptText_;

    WriteChannel* outputChannel_;

    QString fontFamily_;

    //! Copy the selected text to the clip board.
    QAction copyAction_;

};

#endif // SCRIPTINGTEXTEDITOR_H
