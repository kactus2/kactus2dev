//-----------------------------------------------------------------------------
// File: GraphicsItemLabel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2018
//
// Description:
// Text label for design items.
//-----------------------------------------------------------------------------

#include "GraphicsItemLabel.h"

//-----------------------------------------------------------------------------
// Function: GraphicsItemLabel::GraphicsItemLabel()
//-----------------------------------------------------------------------------
GraphicsItemLabel::GraphicsItemLabel(QString const& text, QGraphicsItem* parent /* = 0 */):
QGraphicsProxyWidget(parent)
{
    setText(text);
    setWidget(textLabel_);
    textLabel_->setStyleSheet(QStringLiteral("QLabel {border: 1px solid gray; padding: 0px 0px 1px 1px;}"));
}

//-----------------------------------------------------------------------------
// Function: GraphicsItemLabel::setText()
//-----------------------------------------------------------------------------
void GraphicsItemLabel::setText(QString const& newText)
{
    textLabel_->setText(newText);

    QFontMetrics fontMetric(textLabel_->font());
    qreal newWidth = fontMetric.horizontalAdvance(newText);
    setMaximumWidth(newWidth + 10);

    updateLabelGeometry();
}

//-----------------------------------------------------------------------------
// Function: GraphicsItemLabel::getText()
//-----------------------------------------------------------------------------
QString GraphicsItemLabel::getText() const
{
    return textLabel_->text();
}

//-----------------------------------------------------------------------------
// Function: GraphicsItemLabel::updateLabelGeometry()
//-----------------------------------------------------------------------------
void GraphicsItemLabel::updateLabelGeometry()
{
    updateGeometry();
}
