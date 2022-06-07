//-----------------------------------------------------------------------------
// File: ScriptViewEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#ifndef SCRIPTVIEWEDITOR_H
#define SCRIPTVIEWEDITOR_H

#include <QPlainTextEdit>

#include "ScriptingTextEditor.h"

//-----------------------------------------------------------------------------
//! Text editor for script write and run.
//-----------------------------------------------------------------------------
class ScriptViewEditor : public ScriptingTextEditor
{
    Q_OBJECT
public:

    //! The constructor.
    explicit ScriptViewEditor(QWidget* parent = nullptr);


    //! The destructor.
    virtual ~ScriptViewEditor() = default;

    virtual int sideAreaWidth() const;

    virtual void sideAreaPaintEvent();

signals:

    //! Emitted when a line is completed in the editor.
    void write(QString const& line);

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


private slots:
    
    //! Clears the editor of all text.
    void onClear();

private:


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The current prompt text.
    QString promptText_;


    //! Use tabs or spaces for indentation.
    bool useTabs_;

};

#endif // SCRIPTVIEWEDITOR_H
