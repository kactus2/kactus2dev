//-----------------------------------------------------------------------------
// File: ScriptingConsole.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for showing info besides scripting text editor.
//-----------------------------------------------------------------------------

#ifndef SCRIPTING_SIDE_AREA_H
#define SCRIPTING_SIDE_AREA_H

#include <QWidget>


class ScriptInputEditor;

//-----------------------------------------------------------------------------
//! Widget for showing info besides scripting text editor.
//-----------------------------------------------------------------------------
class ScriptingSideArea : public QWidget
{
public:

    //! The constructor.
    explicit ScriptingSideArea(ScriptInputEditor* editor);

    virtual ~ScriptingSideArea() = default;

    //! Size hint for preferred size in layout.
    QSize sizeHint() const override;

protected:

    //! The handler for paint events.
    virtual void paintEvent(QPaintEvent *event) override;

private:

    //! The scripting editor for which to show side information.
    ScriptInputEditor* editor_;
};

#endif // SCRIPTING_SIDE_AREA_H
