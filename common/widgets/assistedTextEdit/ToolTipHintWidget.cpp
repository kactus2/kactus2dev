//-----------------------------------------------------------------------------
// File: ToolTipHintWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.2.2011
//
// Description:
// Tool tip hint widget.
//-----------------------------------------------------------------------------

#include "ToolTipHintWidget.h"

#include <QToolTip>
#include <QStyle>

//-----------------------------------------------------------------------------
// Function: ToolTipHintWidget()
//-----------------------------------------------------------------------------
ToolTipHintWidget::ToolTipHintWidget() : QLabel()
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFocusPolicy(Qt::NoFocus);
    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    setFrameStyle(QFrame::Box);
    ensurePolished();

    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setIndent(1);
}

//-----------------------------------------------------------------------------
// Function: ~ToolTipHintWidget()
//-----------------------------------------------------------------------------
ToolTipHintWidget::~ToolTipHintWidget()
{
}