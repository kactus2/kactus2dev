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

    //! The constructor.
    explicit ScriptInputEditor(QWidget* parent = nullptr);

    //! The destructor.
    virtual ~ScriptInputEditor() = default;

    virtual void applySettings() override;

    virtual int sideAreaWidth() const override;

    virtual void sideAreaPaintEvent() override;

    void runSelection();

    QString getSelectedLines() const;
signals:

    //! Emitted when a line is completed in the editor.
    void write(QString const& line);


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
