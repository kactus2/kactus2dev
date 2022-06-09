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


private slots:
    
    //! Clears the editor of all text.
    void onClear();

  
};

#endif // SCRIPTINGTEXTEDITOR_H
