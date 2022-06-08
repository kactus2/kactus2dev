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

    /*!
     * The constructor.
     *
     *     @param [in] parent  The parent widget.
     */
     explicit ScriptingTextEditor(QWidget* parent = nullptr);


    //! The destructor.
    virtual ~ScriptingTextEditor() = default;

    //! Apply the application specific settings e.g. text formatting.
     virtual void applySettings();

    /*!
     * Get the required width of the side area.
     *
     *     @return The width of the side area in pixels.
     */
     virtual int sideAreaWidth() const = 0;

    /*!
     * Handler for side area paint event.
     *
     *     @param [in]  The paint event. 
     */
     virtual void sideAreaPaintEvent(QPaintEvent* event) = 0;

protected:

    //! Event handler for widget resize.
    void resizeEvent(QResizeEvent *event) override;

    //! Side area for the widget for holding line numbers or other information.
    ScriptingSideArea* editorSideArea_;

private slots:
    
    //! Clears the editor of all text.
    void onClear();

    /*!
     * Handler for side area update event.
     *
     *     @param [in] rect     The area being updated.
     *     @param [in] dy       The change of y coordinate in the update.
     */
     void updateSideArea(const QRect &rect, int dy);

};

#endif // SCRIPTINGTEXTEDITOR_H
