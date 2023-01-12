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
QWidget(parent),
layout_(new QHBoxLayout(this)),
ribbonScrollArea_(new QScrollArea(this)),
ribbonGroupContainer_(),
containerLayout_(),
scrollLeft_(new QPushButton(QIcon(":/icons/common/graphics/arrowLeft.png"), QString(""), this)),
scrollRight_(new QPushButton(QIcon(":/icons/common/graphics/arrowRight.png"), QString(""), this))
{
	setupRibbonScrollArea();

	setFixedHeight(RIBBONHEIGHT);

	setupScrollButtons();

    scrollLeft_->setFlat(true);
    scrollRight_->setFlat(true);

	layout_->addWidget(scrollLeft_);
	layout_->addWidget(ribbonScrollArea_);
	layout_->addWidget(scrollRight_);
	scrollLeft_->setToolTip(tr("Scroll left"));
	scrollRight_->setToolTip(tr("Scroll right"));

	setContentsMargins(0, 0, 0, 0);
	layout_->setContentsMargins(0, 0, 0, 0);
	layout_->setSpacing(0);

	ribbonScrollArea_->setWidgetResizable(true);

	setScrollButtonVisibilities();

	connect(scrollLeft_, SIGNAL(clicked()), this, SLOT(onLeftScrollPressed()));
	connect(scrollRight_, SIGNAL(clicked()), this, SLOT(onRightScrollPressed()));
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
	containerLayout_->insertWidget(containerLayout_->count() - 1, group);

	return group;
}

//-----------------------------------------------------------------------------
// Function: Ribbon::onLeftScrollPressed()
//-----------------------------------------------------------------------------
void Ribbon::onLeftScrollPressed()
{
	ribbonScrollArea_->horizontalScrollBar()->
		setValue(ribbonScrollArea_->horizontalScrollBar()->value() + getLeftScrollMovement());
}

//-----------------------------------------------------------------------------
// Function: Ribbon::onRightScrollPressed()
//-----------------------------------------------------------------------------
void Ribbon::onRightScrollPressed()
{
	ribbonScrollArea_->horizontalScrollBar()->
		setValue(ribbonScrollArea_->horizontalScrollBar()->value() + getRightScrollMovement());
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

//-----------------------------------------------------------------------------
// Function: Ribbon::wheelEvent()
//-----------------------------------------------------------------------------
void Ribbon::wheelEvent(QWheelEvent* event)
{
	event->ignore();

	ribbonScrollArea_->horizontalScrollBar()->
		setValue(ribbonScrollArea_->horizontalScrollBar()->value() - event->angleDelta().y() / 6);

	setScrollButtonVisibilities();
}

//-----------------------------------------------------------------------------
// Function: Ribbon::resizeEvent()
//-----------------------------------------------------------------------------
void Ribbon::resizeEvent(QResizeEvent* event)
{
	setScrollButtonVisibilities();

	QWidget::resizeEvent(event);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::showEvent()
//-----------------------------------------------------------------------------
void Ribbon::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);

	setScrollButtonVisibilities();
}

//-----------------------------------------------------------------------------
// Function: Ribbon::eventFilter()
//-----------------------------------------------------------------------------
bool Ribbon::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::Wheel)
	{
		Ribbon::wheelEvent(static_cast<QWheelEvent*> (event));
		return true;
	}

	if (object == ribbonGroupContainer_ && QEvent::Resize)
	{
		setScrollButtonVisibilities();
	}

	return QObject::eventFilter(object, event);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::setRibbonScrollArea()
//-----------------------------------------------------------------------------
void Ribbon::setupRibbonScrollArea()
{
    ribbonGroupContainer_ = new QWidget(this);
    containerLayout_ = new QHBoxLayout(ribbonGroupContainer_);
    containerLayout_->setContentsMargins(0,0,0,0);
    containerLayout_->setSpacing(CONTAINER_SPACING);
    containerLayout_->addStretch(1);
    ribbonGroupContainer_->setLayout(containerLayout_);

    ribbonScrollArea_->setWidget(ribbonGroupContainer_);
    ribbonScrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ribbonScrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ribbonScrollArea_->viewport()->setAutoFillBackground(false);

    ribbonScrollArea_->setObjectName("ribbonScrollArea_");
    ribbonGroupContainer_->setObjectName("ribbonGroupContainer_");

    QString style = "QScrollArea#ribbonScrollArea_, QWidget#ribbonGroupContainer_ {background-color:transparent;}";
    ribbonScrollArea_->setStyleSheet(style);

    ribbonGroupContainer_->installEventFilter(this);

    containerLayout_->setSizeConstraint(QLayout::SetFixedSize);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::setupScrollButtons()
//-----------------------------------------------------------------------------
void Ribbon::setupScrollButtons()
{
    scrollLeft_->setFixedWidth(15);
    scrollRight_->setFixedWidth(15);
    scrollLeft_->setFixedHeight(RIBBONHEIGHT);
    scrollRight_->setFixedHeight(RIBBONHEIGHT);

    scrollLeft_->setObjectName("scrollLeft_");
    scrollRight_->setObjectName("scrollRight_");

    QString gradientTopName = RibbonTheme::GRADIENTTOP.name();

    QString buttonStyle = "QPushButton#scrollLeft_, QPushButton#scrollRight_ "
        "{background-color: " + gradientTopName + ";}";

    scrollLeft_->setStyleSheet(buttonStyle);
    scrollRight_->setStyleSheet(buttonStyle);
}

//-----------------------------------------------------------------------------
// Function: Ribbon::getLeftScrollMovement()
//-----------------------------------------------------------------------------
int Ribbon::getLeftScrollMovement()
{
	QPoint leftCenter(ribbonScrollArea_->rect().left(), ribbonScrollArea_->rect().center().y());
	QPoint leftPointOnWidget = ribbonScrollArea_->widget()->mapFromParent(leftCenter);

	int totalRibbonLength = 0;
	for (int groupIndex = 0; groupIndex < ribbonScrollArea_->widget()->layout()->count() - 1; ++groupIndex)
	{
		if (ribbonScrollArea_->widget()->layout()->itemAt(groupIndex)->widget()->isVisible())
		{
			int groupWidth = ribbonScrollArea_->widget()->layout()->itemAt(groupIndex)->widget()->width(); 

			totalRibbonLength += groupWidth;

			if (totalRibbonLength >= leftPointOnWidget.x())
			{
				return (totalRibbonLength - groupWidth - CONTAINER_SPACING - leftPointOnWidget.x());
			}

			totalRibbonLength += CONTAINER_SPACING;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Function: Ribbon::getRightScrollMovement()
//-----------------------------------------------------------------------------
int Ribbon::getRightScrollMovement()
{
	QPoint rightCenter(ribbonScrollArea_->rect().right(), ribbonScrollArea_->rect().center().y());
	QPoint rightPointOnWidget = ribbonScrollArea_->widget()->mapFromParent(rightCenter);

	int totalRibbonLength = 0;
	for (int groupIndex = 0; groupIndex < ribbonScrollArea_->widget()->layout()->count() - 1; ++groupIndex)
	{
		if (ribbonScrollArea_->widget()->layout()->itemAt(groupIndex)->widget()->isVisible())
		{
			int groupWidth = ribbonScrollArea_->widget()->layout()->itemAt(groupIndex)->widget()->width();
			totalRibbonLength += groupWidth;

			if (totalRibbonLength > rightPointOnWidget.x())
			{
				return (totalRibbonLength - rightPointOnWidget.x() + CONTAINER_SPACING);
			}

			totalRibbonLength += CONTAINER_SPACING;
		}
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// Function: Ribbon::setScrollButtonVisibilities()
//-----------------------------------------------------------------------------
void Ribbon::setScrollButtonVisibilities()
{
	scrollLeft_->setDisabled(false);
	scrollRight_->setDisabled(false);

	if (ribbonScrollArea_->width() + scrollLeft_->width() + scrollRight_->width() <
		ribbonScrollArea_->widget()->sizeHint().width())
	{
		scrollLeft_->show();
		scrollRight_->show();
	}
	else
	{
		scrollLeft_->hide();
		scrollRight_->hide();
	}

	QPoint leftCenter(ribbonScrollArea_->rect().left(), ribbonScrollArea_->rect().center().y());
	QPoint leftOnContainer = ribbonScrollArea_->widget()->mapFromParent(leftCenter);

	if (leftOnContainer.x() == 0)
	{
		scrollLeft_->setDisabled(true);
	}
	if (ribbonScrollArea_->horizontalScrollBar()->value() == ribbonScrollArea_->horizontalScrollBar()->maximum() &&
		ribbonScrollArea_->horizontalScrollBar()->maximum() != 0)
	{
		scrollRight_->setDisabled(true);
	}
}
