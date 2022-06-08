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

    /*!
     * The constructor.
     *
     *     @param [in] parent  The parent widget.
     */
    explicit ScriptViewEditor(QWidget* parent = nullptr);

    //! The destructor.
    virtual ~ScriptViewEditor() = default;

    /*!
     * Get the required width of the side area.
     *
     *     @return The width of the side area in pixels.
     */
    virtual int sideAreaWidth() const;

    /*!
     * Handler for side area paint event.
     *
     *     @param [in] event    The paint event.
     */
    virtual void sideAreaPaintEvent(QPaintEvent* event);

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

private:


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The current prompt text.
    QString promptText_ = QString();


};

#endif // SCRIPTVIEWEDITOR_H
