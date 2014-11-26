//-----------------------------------------------------------------------------
// File: Ribbon.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
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

//-----------------------------------------------------------------------------
// Function: Ribbon::Ribbon()
//-----------------------------------------------------------------------------
Ribbon::Ribbon(QWidget* parent /*= 0*/)
    : QWidget(parent),
      layout_(new QHBoxLayout(this)),
	  RIBBONHEIGHT_(78)
{
    layout_->setContentsMargins(2, 2, 2, 2);
    layout_->setSpacing(3);
    layout_->addStretch(1);

	setFixedHeight(RIBBONHEIGHT_);

    setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::~Ribbon()
//-----------------------------------------------------------------------------
Ribbon::~Ribbon()
{
}

//-----------------------------------------------------------------------------
// Function: Ribbon::addGroup()
//-----------------------------------------------------------------------------
RibbonGroup* Ribbon::addGroup(QString const& title)
{
	RibbonGroup* group = new RibbonGroup(title, this);
	layout_->insertWidget(layout_->count() - 1, group);
	return group;
}

//-----------------------------------------------------------------------------
// Function: Ribbon::paintEvent()
//-----------------------------------------------------------------------------
void Ribbon::paintEvent(QPaintEvent* /*event*/)
{
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
	gradient.setColorAt(0.0, RibbonTheme::GRADIENTTOP);
	gradient.setColorAt(1.0, RibbonTheme::GRADIENTBOTTOM);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), gradient);
}
