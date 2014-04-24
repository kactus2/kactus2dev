//-----------------------------------------------------------------------------
// File: EditorArea.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.3.2012
//
// Description:
// Extended tab widget which supports shortcuts.
//-----------------------------------------------------------------------------

#ifndef TABWIDGETEX_H
#define TABWIDGETEX_H

#include <QTabWidget>

//-----------------------------------------------------------------------------
//! Extended tab widget which supports shortcuts.
//-----------------------------------------------------------------------------
class EditorArea : public QTabWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget. Can be null.
     */
    EditorArea(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~EditorArea();

    /*!
     *  Filters and handles keyboard shortcut events.
     */
    bool event(QEvent* event);

};

//-----------------------------------------------------------------------------

#endif // TABWIDGETEX_H
