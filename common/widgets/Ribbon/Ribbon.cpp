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

#include "RibbonGroup.h"

#include <QPainter>
#include <QResizeEvent>

//-----------------------------------------------------------------------------
// Function: Ribbon::Ribbon()
//-----------------------------------------------------------------------------
Ribbon::Ribbon(QWidget* parent /*= 0*/)
    : QWidget(parent),
      parent_(parent),
      theme_(),
      layout_(new QHBoxLayout(this))
{
    layout_->setContentsMargins(2, 2, 2, 2);
    layout_->setSpacing(3);
    layout_->addStretch(1);

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
    return addGroup(QIcon(), title);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::addGroup()
//-----------------------------------------------------------------------------
RibbonGroup* Ribbon::addGroup(QIcon const& icon, QString const& title)
{
    RibbonGroup* group = new RibbonGroup(icon, title, this);
    layout_->insertWidget(layout_->count() - 1, group);
    return group;
}

//-----------------------------------------------------------------------------
// Function: Ribbon::setTheme()
//-----------------------------------------------------------------------------
void Ribbon::setTheme(RibbonTheme const& theme)
{
    theme_ = theme;
    repaint();
}

//-----------------------------------------------------------------------------
// Function: Ribbon::getTheme()
//-----------------------------------------------------------------------------
RibbonTheme const& Ribbon::getTheme() const
{
    return theme_;
}

//-----------------------------------------------------------------------------
// Function: Ribbon::paintEvent()
//-----------------------------------------------------------------------------
void Ribbon::paintEvent(QPaintEvent* /*event*/)
{
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0.0, theme_.gradientTop);
    gradient.setColorAt(1.0, theme_.gradientBottom);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), gradient);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::updateSize()
//-----------------------------------------------------------------------------
void Ribbon::updateSize()
{
    setFixedHeight(sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: Ribbon::resizeEvent()
//-----------------------------------------------------------------------------
void Ribbon::resizeEvent(QResizeEvent* /*event*/)
{
    updateCollapse();
}

//-----------------------------------------------------------------------------
// Function: Ribbon::updateCollapse()
//-----------------------------------------------------------------------------
void Ribbon::updateCollapse()
{
    int targetWidth = width();

    if (sizeHint().width() > targetWidth)
    {
        // Check which group must be set collapsed.
        for (int i = layout_->count() - 1; i >= 0; --i)
        {
            RibbonGroup* group = static_cast<RibbonGroup*>(layout_->itemAt(i)->widget());

            if (group != 0 && group->isVisible() && group->canAutoCollapse() && !group->isCollapsed())
            {
                group->setCollapsed(true);

                if (targetWidth >= sizeHint().width())
                {
                    return;
                }
            }
        }
    }
    else if (sizeHint().width() < targetWidth)
    {
        // Uncollapse as many groups as possible.
        for (int i = 0; i < layout_->count(); ++i)
        {
            RibbonGroup* group = static_cast<RibbonGroup*>(layout_->itemAt(i)->widget());

            if (group != 0 && group->isVisible() && group->canAutoCollapse() && group->isCollapsed())
            {
                int maxWidth = group->maximumWidth();
                
                if (sizeHint().width() - group->width() + maxWidth <= targetWidth)
                {
                    group->setCollapsed(false);
                }
                else
                {
                    return;
                }
            }
        }
    }
}
