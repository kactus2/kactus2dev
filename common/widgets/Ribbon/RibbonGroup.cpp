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

#include <QPainter>
#include <QToolButton>
#include <QEvent>
#include <QActionEvent>
#include <QAction>

//-----------------------------------------------------------------------------
// Function: RibbonGroup::RibbonGroup()
//-----------------------------------------------------------------------------
RibbonGroup::RibbonGroup(QIcon const& icon, QString const& title, Ribbon* parent)
    : QWidget(parent),
      title_(title),
      parent_(parent),
      layout_(new QGridLayout(this)),
      dropDownAction_(new QAction(icon, title, this)),
      dropDownButton_(0),
      dropDownMenu_(new QMenu(this)),
      autoCollapse_(true),
      collapsed_(false),
      curRow_(0),
      curColumn_(2),
      maxWidth_(2 * HMARGIN),
      widgetMap_()
{
    QFontMetrics metrics(font());
    setMinimumWidth(metrics.width(title_) + 2 * TITLE_MARGIN);

    layout_->setContentsMargins(HMARGIN, 10, HMARGIN, TITLE_HEIGHT + 5);
    layout_->setSpacing(0);

    // Create the drop-down menu button and add it to the layout.
    dropDownAction_->setMenu(dropDownMenu_);
    dropDownButton_ = static_cast<QToolButton*>(createWidget(dropDownAction_, 2, 2));
    dropDownButton_->setVisible(false);
    connect(dropDownAction_, SIGNAL(triggered()), dropDownButton_, SLOT(showMenu()));

    layout_->addWidget(dropDownButton_, 0, 0, 2, 2, Qt::AlignCenter);  

    setMinimumWidth(sizeHint().width());
    parent_->updateSize();

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

    // Draw the base gradient.
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0.0, parent_->getTheme().gradientTop);
    gradient.setColorAt(0.1, Qt::white);
    gradient.setColorAt(0.3, parent_->getTheme().gradientTop);    
    gradient.setColorAt(1.0, parent_->getTheme().gradientBottom);

    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), gradient);

    // Draw a nice frame around the group area.
    painter.setPen(QPen(parent_->getTheme().groupTitleGradientTop, 1));
    painter.drawRect(rect());

    // Draw the title background.
    QRect titleRect = rect();
    titleRect.setTop(rect().height() - TITLE_HEIGHT);

    QLinearGradient titleGradient(titleRect.topLeft(), titleRect.bottomLeft());
    titleGradient.setColorAt(0.0, parent_->getTheme().groupTitleGradientTop);
    titleGradient.setColorAt(1.0, parent_->getTheme().groupTitleGradientBottom);

    painter.fillRect(titleRect, titleGradient);

    // Draw the title text.
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);

    painter.setPen(QPen(parent_->getTheme().groupTitleText));
    painter.drawText(titleRect, title_, opt);
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::event()
//-----------------------------------------------------------------------------
bool RibbonGroup::event(QEvent* e)
{
    switch(e->type())
    {
    case QEvent::ActionAdded:
        {
            QAction* action = static_cast<QActionEvent*>(e)->action();
            addWidgetForAction(action);
            return true;
        }

    case QEvent::ActionRemoved:
        {
            // Remove the widget corresponding to the action.
            QAction* action = static_cast<QActionEvent*>(e)->action();
            removeWidgetForAction(action);
            return true;
        }

    case QEvent::Show:
    case QEvent::Hide:
        {
            parent_->updateCollapse();
            break;
        }

    default:
        break;
    }

    return QWidget::event(e);
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::setGroupIcon()
//-----------------------------------------------------------------------------
void RibbonGroup::setGroupIcon(QIcon const& icon)
{
    dropDownAction_->setIcon(icon);
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::setCollapsed()
//-----------------------------------------------------------------------------
void RibbonGroup::setCollapsed(bool collapsed)
{
    dropDownButton_->setVisible(collapsed);

    for (int i = 1; i < layout_->count(); ++i)
    {
        QLayoutItem* item = layout_->itemAt(i);

        if (item->widget() != 0)
        {
            item->widget()->setVisible(!collapsed);
        }
    }

    layout_->activate();
    collapsed_ = collapsed;
}


//-----------------------------------------------------------------------------
// Function: RibbonGroup::setAutoCollapse()
//-----------------------------------------------------------------------------
void RibbonGroup::setAutoCollapse(bool autoCollapse)
{
    autoCollapse_ = autoCollapse;
    parent_->updateCollapse();
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::isCollapsed()
//-----------------------------------------------------------------------------
bool RibbonGroup::isCollapsed() const
{
    return collapsed_;
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::canAutoCollapse()
//-----------------------------------------------------------------------------
bool RibbonGroup::canAutoCollapse() const
{
    return autoCollapse_;
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::createWidget()
//-----------------------------------------------------------------------------
QWidget* RibbonGroup::createWidget(QAction* action, int rowSpan, int colSpan)
{
    QToolButton* button = new QToolButton(this);
    button->setDefaultAction(action);
    button->setIconSize(QSize(24, 24));
    button->setAutoRaise(true);

    if (rowSpan == 2 && colSpan == 2)
    {
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        QFontMetrics metrics(button->font());
        int requiredWidth = metrics.width(button->text()) + 10;

        if (action->menu() != 0)
        {
            requiredWidth += 20;
        }

        button->setFixedWidth(qMax<int>(requiredWidth, MIN_LARGE_BUTTON_WIDTH));
        button->setFixedHeight(button->sizeHint().height() + 10);

    }
    else if (rowSpan == 1 && colSpan >= 2)
    {
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else
    {
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setFixedSize(button->sizeHint());
    }

    return button;
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::addWidgetForAction()
//-----------------------------------------------------------------------------
void RibbonGroup::addWidgetForAction(QAction* action)
{
    int rowSpan = qMax(1, action->property("rowSpan").toInt());
    int colSpan = qMin(qMax(1, action->property("colSpan").toInt()), 2);

    // Create the widget for the action if not found already in the widget map.
    QWidget* widget = widgetMap_.value(action);

    if (widget == 0)
    {
        widget = createWidget(action, rowSpan, colSpan);
        widget->setVisible(!collapsed_);
    }

    layout_->addWidget(widget, curRow_, curColumn_, rowSpan, colSpan, Qt::AlignCenter);

    if (curRow_ == 0)
    {
        maxWidth_ += widget->width();
        Q_ASSERT(maxWidth_ == sizeHint().width());
        setMaximumWidth(maxWidth_);
    }

    curRow_ += rowSpan;

    if (curRow_ >= 2)
    {
        curRow_ = 0;
        curColumn_ += colSpan;
    }

    parent_->updateSize();

    // Add the action also to the drop-down menu.
    dropDownMenu_->addAction(action);

    widgetMap_.insert(action, widget);
}

//-----------------------------------------------------------------------------
// Function: RibbonGroup::removeWidgetForAction()
//-----------------------------------------------------------------------------
void RibbonGroup::removeWidgetForAction(QAction* action)
{
    // Find the corresponding widget from the widget map, remove it from there and delete it.
    QWidget* widget = widgetMap_.take(action);
    Q_ASSERT(widget != 0);
    delete widget;

    // Recreate the layout.
    while (layout_->count() > 1)
    {
        layout_->takeAt(1);
    }

    curRow_ = 0;
    curColumn_ = 2;
    maxWidth_ = 2 * HMARGIN;
    dropDownMenu_->clear();

    foreach (QAction* action, actions())
    {
        addWidgetForAction(action);
        dropDownMenu_->addAction(action);
    }
}

