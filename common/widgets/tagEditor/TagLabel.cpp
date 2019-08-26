//-----------------------------------------------------------------------------
// File: TagLabel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.08.2019
//
// Description:
// The tag label.
//-----------------------------------------------------------------------------

#include "TagLabel.h"

//-----------------------------------------------------------------------------
// Function: TagLabel::TagLabel()
//-----------------------------------------------------------------------------
TagLabel::TagLabel(QString const& tagName, QWidget* parent, QString const& labelColor):
QLabel(tagName, parent)
{
    QColor tagColor(labelColor);

    setContentsMargins(5, 3, 5, 3);

    setAutoFillBackground(true);
    QPalette labelPalette = palette();
    labelPalette.setColor(QPalette::Window, tagColor);
    setPalette(labelPalette);

    setCursor(Qt::PointingHandCursor);
}

//-----------------------------------------------------------------------------
// Function: TagLabel::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void TagLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    QLabel::mouseReleaseEvent(ev);

    emit clicked(this);
}
