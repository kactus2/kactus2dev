//-----------------------------------------------------------------------------
// File: ScriptInputEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#ifndef SCRIPTINPUTEDITOR_H
#define SCRIPTINPUTEDITOR_H

#include <QPlainTextEdit>

#include "ScriptingTextEditor.h"

//-----------------------------------------------------------------------------
//! Text editor for script write and run.
//-----------------------------------------------------------------------------
class ScriptInputEditor : public ScriptingTextEditor
{
    Q_OBJECT
public:

    /*!
     * The constructor.
     *
     *     @param [in] parent  The parent widget.
     */
    explicit ScriptInputEditor(QWidget* parent = nullptr);

    //! The destructor.
    virtual ~ScriptInputEditor() = default;

    //! Apply the application specific settings e.g. text formatting.
    virtual void applySettings() override;

    /*!
     * Get the required width of the side area.
     *
     *     @return The width of the side area in pixels.
     */
    virtual int sideAreaWidth() const override;

    /*!
     * Handler for side area paint event.
     *
     *     @return
     */
    virtual void sideAreaPaintEvent() override;

    /*!
     * Get the text on selected lines.
     *
     *     @return The text on the currently selected lines.
     */
     QString getSelectedLines() const;

protected:

    //! Event handler for key press handling.
    virtual void keyPressEvent(QKeyEvent *e) override;  

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    bool useTabs_ = false;

};

#endif // SCRIPTINPUTEDITOR_H
