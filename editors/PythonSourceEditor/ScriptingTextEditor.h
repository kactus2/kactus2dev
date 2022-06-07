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

#include "ScriptingSideArea.h"

//-----------------------------------------------------------------------------
//! Text editor for script write and run.
//-----------------------------------------------------------------------------
class ScriptingTextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:

    //! The constructor.
    explicit ScriptingTextEditor(QWidget* parent = nullptr);


    //! The destructor.
    virtual ~ScriptingTextEditor() = default;

    void applySettings();

    void insertInput(QString const& input);

    int sideAreaWidth() const;

    void sideAreaPaintEvent();

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

protected:

    //! Event handler for key press handling.
    virtual void keyPressEvent(QKeyEvent *e) override;  

    void resizeEvent(QResizeEvent *event) override;

private slots:
    
    //! Clears the editor of all text.
    void onClear();

    void updateSideArea(const QRect &rect, int dy);

private:

    //! Check if text can be pasted at currently selected position.
    bool canPaste() const;


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Side area for the widget.
    ScriptingSideArea* promtSideArea_;

    //! The current prompt text.
    QString promptText_;


    //! Use tabs or spaces for indentation.
    bool useTabs_;

};

#endif // SCRIPTINGTEXTEDITOR_H
