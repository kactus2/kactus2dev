//-----------------------------------------------------------------------------
// File: RibbonGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.02.2013
//
// Description:
// Ribbon group class.
//-----------------------------------------------------------------------------

#include "RibbonGroup.h"
#include "Ribbon.h"
#include "RibbonTheme.h"

#include <QPainter>
#include <QToolButton>
#include <QEvent>
#include <QActionEvent>
#include <QAction>

//-----------------------------------------------------------------------------
// Function: RibbonGroup::RibbonGroup()
//-----------------------------------------------------------------------------
RibbonGroup::RibbonGroup(QString const& title, Ribbon* parent):
QToolBar(parent),
title_(title)
{
    const int BOTTOM_MARGIN = 10;
	const int TOP_MARGIN = 10;

	setIconSize(QSize (32, 32));
    setContentsMargins(0, TOP_MARGIN, 0, BOTTOM_MARGIN);
	layout()->setContentsMargins(0, TOP_MARGIN, 0, BOTTOM_MARGIN);

	QString style = 
		"QToolBar { margin-top: %1px; margin-left: %1px; margin-right: %1px; margin-bottom: %1px;}";
	setStyleSheet(style.arg(QString::number(HMARGIN)));

	QFontMetrics metrics(font());
	setMinimumWidth(metrics.width(title_) +  TITLE_MARGIN);
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::~RibbonGroup()
//-----------------------------------------------------------------------------
RibbonGroup::~RibbonGroup()
{
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::paintEvent()
//-----------------------------------------------------------------------------
void RibbonGroup::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), RibbonTheme::GRADIENTTOP);

    // Draw a nice frame around the group area.
	painter.setPen(QPen(RibbonTheme::GROUPTITLEGRADIENTTOP, 1));
    painter.drawRect(rect());

    // Draw the title background.
    QRect titleRect = rect();
    titleRect.setTop(rect().height() - TITLE_HEIGHT);

    painter.fillRect(titleRect, RibbonTheme::GROUPTITLEGRADIENTBOTTOM);

    // Draw the title text.
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);

	painter.setPen(QPen(RibbonTheme::GROUPTITLETEXT));
    painter.drawText(titleRect, title_, opt);
}
