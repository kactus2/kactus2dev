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

    virtual void applySettings();


    virtual int sideAreaWidth() const = 0;

    virtual void sideAreaPaintEvent() = 0;

protected:


    void resizeEvent(QResizeEvent *event) override;

    //! Side area for the widget.
    ScriptingSideArea* promtSideArea_;

private slots:
    
    //! Clears the editor of all text.
    void onClear();

    void updateSideArea(const QRect &rect, int dy);

private:


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------


};

#endif // SCRIPTINGTEXTEDITOR_H
