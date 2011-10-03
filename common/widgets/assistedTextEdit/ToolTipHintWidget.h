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

#ifndef TOOLTIPHINTWIDGET_H
#define TOOLTIPHINTWIDGET_H

#include <QLabel>

//-----------------------------------------------------------------------------
//! ToolTipHintWidget class.
//-----------------------------------------------------------------------------
class ToolTipHintWidget : public QLabel
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    ToolTipHintWidget();

    /*!
     *  Destructor.
     */
    ~ToolTipHintWidget();
};

//-----------------------------------------------------------------------------

#endif // TOOLTIPHINTWIDGET_H