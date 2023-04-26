//-----------------------------------------------------------------------------
// File: Ribbon.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 06.02.2013
//
// Description:
// Ribbon toolbar.
//-----------------------------------------------------------------------------

#include "Ribbon.h"
#include "RibbonTheme.h"
#include "RibbonGroup.h"

#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: Ribbon::Ribbon()
//-----------------------------------------------------------------------------
Ribbon::Ribbon(QWidget* parent /*= 0*/):
	QToolBar(parent)
{
    setObjectName("Ribbon");

	setFixedHeight(RIBBONHEIGHT);

	setMovable(false);
	setFloatable(false);

    setContentsMargins(0, 0, 0, 0);

    layout()->setSpacing(CONTAINER_SPACING);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::addGroup()
//-----------------------------------------------------------------------------
QAction* Ribbon::addGroup(RibbonGroup* group)
{
	return addWidget(group);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::paintEvent()
//-----------------------------------------------------------------------------
void Ribbon::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), RibbonTheme::GRADIENTBOTTOM);
}

