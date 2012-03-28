//-----------------------------------------------------------------------------
// File: TabWidgetEx.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.3.2012
//
// Description:
// Extended tab widget which supports shortcuts.
//-----------------------------------------------------------------------------

#include "TabWidgetEx.h"

#include <QKeyEvent>

//-----------------------------------------------------------------------------
// Function: TabWidgetEx::TabWidgetEx()
//-----------------------------------------------------------------------------
TabWidgetEx::TabWidgetEx(QWidget* parent /*= 0*/) : QTabWidget(parent)
{
}

//-----------------------------------------------------------------------------
// Function: TabWidgetEx::~TabWidgetEx()
//-----------------------------------------------------------------------------
TabWidgetEx::~TabWidgetEx()
{
}

//-----------------------------------------------------------------------------
// Function: TabWidgetEx::eventFilter()
//-----------------------------------------------------------------------------
bool TabWidgetEx::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->modifiers() == Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_Tab)
            {
                int nextPage = (currentIndex() + 1) % count();
                setCurrentIndex(nextPage);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_W)
            {
                if (currentIndex() >= 0 && currentIndex() < count())
                {
                    emit tabCloseRequested(currentIndex());
                }

                return true;
            }
        }
    }

    return QTabWidget::event(event);
}
