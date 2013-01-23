/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "MenuStrip.h"
#include <math.h>

#include <QIcon>
#include <QMenu>
#include <QColor>
#include <QBrush>
#include <QEvent>
#include <QLabel>
#include <QAction>
#include <QVector>
#include <QToolTip>
#include <QPalette>
#include <QPainter>
#include <QTimeLine>
#include <QWheelEvent>
#include <QToolButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QActionEvent>
#include <QWidgetAction>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QStyle>

#define MEAN_COLOR(c1, c2) QColor( (c1.red()+c2.red())/2, (c1.green()+c2.green())/2, (c1.blue()+c2.blue())/2 );

/*
Compile time hard-coded setting that defines the total number of rows a grid
layout within the menu group can have. This has to be hardcoded. If we give
too much flexibility, then we may land up messing up with the whole effect.
*/
const int MaxRows = 2;
const int ActionContainerMargin = 2;
const int GroupSpacing = 2;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::MenuStrip
/////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace GCF
{
	class ScrollArea : public QScrollArea
	{
	public:
		ScrollArea(QWidget* parent=0) : QScrollArea(parent) { }
		~ScrollArea() { }
		void scrollBy(int dx, int dy) {
			QWidget* w = this->widget();
			if(w->width() < width())
				return;

			QPoint pos = w->pos();
			pos.setX( pos.x() + dx );
			pos.setY( pos.y() + dy );
			if(pos.x() > 0)
				pos.setX(0);
			else if(pos.x() < -(w->width()-this->width()))
				pos.setX(-(w->width()-this->width()));
			w->move(pos);
		}
	};

	class GroupContainer : public QWidget
	{
	public:
		GroupContainer(QWidget* parent=0) : QWidget(parent), m_locked(false) { }
		~GroupContainer() { }

		void lockPos() { 
			if(m_locked)
				return;
			m_locked = true; 
			m_lockedPos = this->pos(); 
		}

		void unlockPos() { 
			startTimer(200);
		}

		void paintEvent(QPaintEvent* pe);
		bool event(QEvent* e);
		void timerEvent(QTimerEvent* te) {
			m_locked = false;
			killTimer(te->timerId());
		}

	private:
		QPoint m_lockedPos;
		bool m_locked;
	};
}

struct GCF::MenuStripData
{
    GCF::ScrollArea* scrollArea;
	GCF::GroupContainer* groupContainer;
    QList<GCF::MenuStripGroup*> groupList;
    bool mousePressed;
    QPoint mousePos;
    QHBoxLayout* groupContainerLayout;
};

/**
\class GCF::MenuStrip GCF::MenuStrip.h
\brief A Menu/Toolbar replacement widget.

Conventionally the main windows consisted of a menu bar and a bunch of toolbars which could be
docked to any corner of the main window. The menu bar consisted of a huge number of options, while
the toolbar would show off a sub-set of the menu commands. While this was (and still is) a great
user interface for applications, it had one problem: USERS WOULD NEVER BECOME AWARE OF ALL THE
COMMANDS SUPPORTED BY THE APPLICATION.

So we came up with this concept called menu strips. Menu strip is not a invention or an innovation.
Microsoft Office 2007 has this ribbon menu concept which is kind of nice, but to me it looks like
a tab widget shown as a menu widget. So in GCF we have a menu strip in which commands are
organized into groups that can be scrolled to the left or right.

Developers using menu strip need not learn about the menu strip API because it is all hidden within
the GCF framework.

Take a look at the simple menu strip code below.

\code
#include <GCF/MenuStrip.h>
#include <QAction>
#include <QStyle>
#include <stdlib.h>

double randomNumber(double min, double max)
{
    double rand_max = double(RAND_MAX);
    double r = double( rand() );
    double value = (max - min) / (rand_max) * r + min;
    return value;
}

QAction* createAction(QIcon icon, QString text, QObject* parent, int rowSpan=1, int colSpan=1)
{
    QAction* action = new QAction(icon, text, parent);
    if(rowSpan > 3)
        rowSpan = 3;
    action->setProperty("rowSpan", rowSpan);
    action->setProperty("colSpan", colSpan);
    return action;
}

QAction* createRandomAction(QObject* parent)
{
    static int count = 0;
    QStyle::StandardPixmap sp = (QStyle::StandardPixmap)((int)randomNumber(0, 57));
    QString text = QString("Action%1").arg(count++);
    int rowSpan = (int)(randomNumber(1, 2));
    int colSpan = (int)(randomNumber(1, 3));
    QAction* action = createAction(qApp->style()->standardPixmap(sp), text, parent, rowSpan, colSpan);
    return action;
}

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    GCF::MenuStrip* menuStrip = new GCF::MenuStrip;

    int nrGroups = 8; //(int)(randomNumber(3, 20));
    for(int g=0; g<nrGroups; g++)
    {
        GCF::MenuStripGroup* group = menuStrip->addGroup( QString("Group%1").arg(g) );
        int nrActions = (int)(randomNumber(6, 20));
        for(int i=0; i<nrActions; i++)
            group->addAction(createRandomAction(group));

        QStyle::StandardPixmap sp = (QStyle::StandardPixmap)((int)randomNumber(0, 57));
        group->setIcon( qApp->style()->standardPixmap(sp) );
    }

    menuStrip->setFixedHeight( menuStrip->heightForWidth(400) );
    menuStrip->setWindowTitle("MenuStripDemo");

    menuStrip->show();

    return a.exec();
}
\endcode

When executed, the output would look as shown in the image below.

\image html menustripdemo.jpg

The groups that are not visible can be scrolled by dragging the strip to the left or right.
Essentially a menu strip can be thought of as a set of menu strip groups, each group can
show one or more actions. The actions in each group are layed out in a grid that can have
at most two rows and any number of columns. Actions can span multiple rows / columns.
As actions are added to the group, its layout will be first filled across column and then
across rows.
*/

GCF::MenuStrip::MenuStrip(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::MenuStripData;
    d->scrollArea = new GCF::ScrollArea(this);
    d->groupContainer = new GCF::GroupContainer(d->scrollArea);
    d->scrollArea->setWidget(d->groupContainer);
    d->scrollArea->setWidgetResizable(true);
    d->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->scrollArea->setFrameStyle(QFrame::NoFrame);

    QHBoxLayout* layout = new QHBoxLayout(d->groupContainer);
    layout->setSpacing(1+GroupSpacing);
    layout->setMargin(GroupSpacing/2);
    layout->addItem(new QSpacerItem(20, 10, QSizePolicy::Expanding));
    d->groupContainerLayout = layout;

    d->scrollArea->installEventFilter(this);
    d->groupContainer->installEventFilter(this);
}

GCF::MenuStrip::~MenuStrip()
{
    delete d;
    d = 0;
}

GCF::MenuStripGroup* GCF::MenuStrip::addGroup(const QString& title)
{
    GCF::MenuStripGroup* newGroup = new GCF::MenuStripGroup(this);
    newGroup->setTitle(title);
    newGroup->show();
    return newGroup;
}

GCF::MenuStripGroup* GCF::MenuStrip::insertGroup(const QString& title, GCF::MenuStripGroup* before)
{
    GCF::MenuStripGroup* newGroup = new GCF::MenuStripGroup(this, before);
    newGroup->setTitle(title);
    newGroup->show();
    return newGroup;
}

int GCF::MenuStrip::groupCount() const
{
    return d->groupList.count();
}

GCF::MenuStripGroup* GCF::MenuStrip::group(int index) const
{
    if(index < 0 || index >= d->groupList.count())
        return 0;

    return d->groupList[index];
}

int GCF::MenuStrip::heightForWidth(int) const
{
    int ret = (fontMetrics().height()+fontMetrics().ascent())*(MaxRows+2)+2*GroupSpacing;
    return ret;
}

QWidget* GCF::MenuStrip::groupContainer() const
{
    return d->groupContainer;
}

QRect GCF::MenuStrip::groupContainerRect() const
{
    return d->scrollArea->geometry();
}

void GCF::MenuStrip::registerGroup(GCF::MenuStripGroup* group, GCF::MenuStripGroup* before)
{
    if(!d || d->groupList.contains(group))
        return;

    if(before)
    {
        int index = d->groupContainerLayout->indexOf(before);
        if(index < 0)
            d->groupContainerLayout->insertWidget(d->groupContainerLayout->count()-1, group);
        else
            d->groupContainerLayout->insertWidget(index, group);
    }
    else
    {
        d->groupContainerLayout->insertWidget(d->groupContainerLayout->count()-1, group);
    }
    d->groupList.append(group);
    group->installEventFilter(this);
}

void GCF::MenuStrip::unregisterGroup(GCF::MenuStripGroup* group)
{
    if(!d || !d->groupList.contains(group))
        return;

    d->groupList.removeAll(group);
}

void GCF::MenuStrip::updateLayout()
{
    d->groupContainerLayout->invalidate();
    d->groupContainerLayout->setGeometry(d->groupContainer->rect());
}

void GCF::MenuStrip::lockGroupContainer()
{
	d->groupContainer->lockPos();
}

void GCF::MenuStrip::unlockGroupContainer()
{
	d->groupContainer->unlockPos();
}

void GCF::MenuStrip::ensureVisible(GCF::MenuStripGroup* group, int requiredWidth)
{
	QRect groupRect = group->geometry();
	QRect gcRect = d->groupContainer->geometry();
	QRect saRect = d->scrollArea->rect();

	if(requiredWidth > groupRect.width())
		groupRect.setWidth(requiredWidth);

	int left = -gcRect.left();
	int right = left + saRect.width();

	if(groupRect.left() < left)
		d->scrollArea->scrollBy( left-groupRect.left(), 0 );
	else if(groupRect.right() > right)
		d->scrollArea->scrollBy( right-groupRect.right(), 0 );
}

void GCF::MenuStrip::paintEvent(QPaintEvent* pe)
{
    QWidget::paintEvent(pe);
}

void GCF::MenuStrip::resizeEvent(QResizeEvent* re)
{
    QWidget::resizeEvent(re);
    d->scrollArea->setGeometry(this->rect());
    for(int i=0; i<d->groupList.count(); i++)
        d->groupList[i]->recalculateSize();
}

bool GCF::MenuStrip::eventFilter(QObject* obj, QEvent* event)
{
    GCF::MenuStripGroup* group = qobject_cast<GCF::MenuStripGroup*>(obj);
    if(group && d->groupList.contains(group))
    {
        QPoint dp;
        switch(event->type())
        {
        case QEvent::MouseButtonPress:
            d->mousePressed = true;
            d->mousePos = QCursor::pos();
            return false;
        case QEvent::MouseMove:
            if(!d->mousePressed)
                break;
            dp = QCursor::pos() - d->mousePos;
            d->scrollArea->scrollBy(dp.x(), 0);
            d->mousePos = QCursor::pos();
            return true;
        case QEvent::MouseButtonRelease:
            d->mousePressed = false;
            return false;
        case QEvent::Wheel: {
            QWheelEvent* we = (QWheelEvent*)(event);
            int delta = 20 * int(we->delta()) / 120;
            d->scrollArea->scrollBy(delta, 0);
            we->accept();
            }
        default:
            break;
        }
    }
    else
    {
        if(event->type() == QEvent::Wheel)
            return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::GroupContainer
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void GCF::GroupContainer::paintEvent(QPaintEvent* pe)
{
    Q_UNUSED(pe);

    QColor bottomColor = MEAN_COLOR(palette().midlight().color(), palette().highlight().color());
    QColor topColor = MEAN_COLOR(palette().window().color(), bottomColor);

    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0.0, topColor);
    gradient.setColorAt(1.0, bottomColor);

    QPainter paint(this);
    paint.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);
    paint.fillRect(rect(), gradient);
}

bool GCF::GroupContainer::event(QEvent* e)
{
	if(m_locked)
	{
		switch(e->type())
		{
		case QEvent::Move:
			move(m_lockedPos);
			return true;

		default:
			break;
		}
	}

	return QWidget::event(e);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::MenuStripGroupGridLayout
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::MenuStripGroupData
{
    GCF::MenuStrip* parentMenuStrip;
    QWidget* actionContainer;
    QString title;
    QIcon icon;
    GCF::MenuStripGroupGridLayout* gridLayout;
    GCF::MenuStripGroupDrawer* groupDrawer;
    QMenu* groupMenu;
    QMap<QAction*, QWidget*> actionWidgetMap;
    bool drawGroupTitle;
};

GCF::MenuStripGroup::MenuStripGroup(GCF::MenuStrip* parent, GCF::MenuStripGroup* before)
:QWidget(parent->groupContainer())
{
    d = new GCF::MenuStripGroupData;
    d->parentMenuStrip = parent;
    d->actionContainer = new QWidget(this);
    d->groupMenu = new QMenu(this);
    d->groupDrawer = 0; //new GCF::MenuStripGroupDrawer(this); 
    // d->groupDrawer->setMaximumHeight(400);
    d->drawGroupTitle = true;

    d->gridLayout = new GCF::MenuStripGroupGridLayout;
    d->actionContainer->setLayout(d->gridLayout);
    d->actionContainer->installEventFilter(this);
    d->parentMenuStrip->registerGroup(this, before);

    int gSize=int((fontMetrics().height()+fontMetrics().ascent())*1.2);
    for(int i=0; i<MaxRows; i++)
        d->gridLayout->setRowMinimumHeight(i, gSize);

    recalculateSize();
}

GCF::MenuStripGroup::~MenuStripGroup()
{
    d->parentMenuStrip->unregisterGroup(this);
    delete d;
}

GCF::MenuStrip* GCF::MenuStripGroup::parentMenuStrip() const
{
    return d->parentMenuStrip;
}

void GCF::MenuStripGroup::setTitle(const QString& title)
{
    d->title = title;
    recalculateSize();
    update();
}

const QString& GCF::MenuStripGroup::title() const
{
    return d->title;
}

void GCF::MenuStripGroup::setIcon(const QIcon& icon)
{
    d->icon = icon;
}

const QIcon& GCF::MenuStripGroup::icon() const
{
    return d->icon;
}

void GCF::MenuStripGroup::addDrawerWidget(QWidget* widget)
{
    if(!d->groupDrawer)
        d->groupDrawer = new GCF::MenuStripGroupDrawer(this);

    d->groupDrawer->addWidget(widget);
}

void GCF::MenuStripGroup::removeDrawerWidget(QWidget* widget)
{
    if(!d->groupDrawer)
        return;

    d->groupDrawer->removeWidget(widget);
    if( !d->groupDrawer->widgetCount() )
    {
        d->groupDrawer->closeDrawer(); 
        delete d->groupDrawer;
        d->groupDrawer = 0;
    }
}

int GCF::MenuStripGroup::drawerWidgetCount() const
{
    if(!d->groupDrawer)
        return 0;

    return d->groupDrawer->widgetCount();
}

QWidget* GCF::MenuStripGroup::drawerWidget(int index) const
{
    if(!d->groupDrawer)
        return 0;

    return d->groupDrawer->widget(index);
}

static QSize iconSize(28, 28);
static int iconBorder = sqrt( double(iconSize.width()*iconSize.width()/4 + iconSize.height()*iconSize.height()/4) ) - iconSize.width()/2;

void GCF::MenuStripGroup::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);

    QColor baseColor = palette().midlight().color();
    QColor bottomColor = MEAN_COLOR(palette().midlight().color(), palette().highlight().color());
    QColor topColor = MEAN_COLOR(palette().window().color(), bottomColor);

    QRect rect = this->rect();
    QRect acRect = rect.adjusted(0, 0, 0, -(fontMetrics().height()+fontMetrics().ascent()));

    QLinearGradient gradient(acRect.topLeft(), acRect.bottomLeft());
    gradient.setColorAt(0.0, topColor);
    gradient.setColorAt(0.15, baseColor);
    gradient.setColorAt(0.2, topColor);
    gradient.setColorAt(1.0, bottomColor);
    paint.fillRect(acRect, gradient);

    if(d->drawGroupTitle)
    {
        paint.save();
        this->drawTextAndIcon(&paint, rect);
        paint.restore();
    }

    QColor borderColor(palette().highlight().color());
    borderColor.setAlphaF(0.7);
    paint.setPen( borderColor );
    paint.drawRect(0, 0, rect.width(), rect.height());
}

void GCF::MenuStripGroup::drawTextAndIcon(QPainter* paint, const QRect& rect)
{
    QColor bottomColor = MEAN_COLOR(palette().midlight().color(), palette().highlight().color());
    QColor topColor = MEAN_COLOR(palette().window().color(), bottomColor);
    QColor textAreaColor = palette().highlight().color();
    QColor textColor = palette().highlightedText().color();
    QRect acRect = rect.adjusted(0, 0, 0, -groupTitleHeight());
    QRect textRect(rect.left(), acRect.bottom(), rect.width(), (rect.bottom()-acRect.bottom()));
    QRect iconRect(rect.left()+iconBorder, rect.bottom()-iconSize.height()-iconBorder, iconSize.width(), iconSize.height());

    QLinearGradient gradient;
    gradient.setStart(textRect.topLeft());
    gradient.setFinalStop(textRect.bottomLeft());
    gradient.setColorAt(0.0, textAreaColor);
    gradient.setColorAt(0.7, textAreaColor);
    gradient.setColorAt(1.0, topColor);
    paint->fillRect(textRect, gradient);

    if(!d->icon.isNull())
        textRect.setLeft(iconRect.right());
    paint->setPen(textColor);
    paint->drawText(textRect, Qt::AlignCenter, d->title);

    if(!d->icon.isNull())
    {
        QRect iconGradRect = iconRect.adjusted(-iconBorder, -iconBorder, iconBorder, iconBorder);
        QRadialGradient iconGrad(iconGradRect.center(), iconGradRect.width()>iconGradRect.height()?iconGradRect.height():iconGradRect.width());
        iconGrad.setColorAt(0.0, QColor(220, 230, 255, 32));
        iconGrad.setColorAt(0.8, QColor(255,255,255,200));
        iconGrad.setColorAt(1.0, QColor(240, 250, 255, 32));

        paint->setPen(QColor(240, 250, 255, 64));
        paint->setBrush(iconGrad);
        paint->drawEllipse(iconGradRect);

        iconRect = iconRect.adjusted(2,2,-2,-2);
        paint->drawPixmap(iconRect, d->icon.pixmap(iconSize));
    }

    // if there is a drawer sheet, then lets draw a down arrow to the right
    if(d->groupDrawer)
    {
        QIcon arrowIcon;
        if(d->groupDrawer->isVisible())
            arrowIcon = style()->standardIcon(QStyle::SP_ArrowUp);
        else
            arrowIcon = style()->standardIcon(QStyle::SP_ArrowDown);

        QRect aIconRect(rect.right()-iconBorder-iconSize.width(), 
                       rect.bottom()-iconSize.height()-iconBorder, 
                       iconSize.width(), iconSize.height());
        aIconRect = aIconRect.adjusted(3, 3, 3, 3);
        paint->drawPixmap(aIconRect, arrowIcon.pixmap(iconSize));
    }
}

void GCF::MenuStripGroup::resizeEvent(QResizeEvent*)
{
    QRect rect = this->rect();
    QSize size = d->gridLayout->minimumSize();
    QRect acRect = rect.adjusted(ActionContainerMargin, ActionContainerMargin, -ActionContainerMargin, -(fontMetrics().height()+fontMetrics().ascent()));
    int heightDiff = acRect.height()-size.height();
    if(heightDiff > 0)
        acRect = acRect.adjusted(0, heightDiff/2, 0, -heightDiff/2);
    d->actionContainer->setGeometry(acRect);
}

void GCF::MenuStripGroup::mousePressEvent(QMouseEvent* me)
{
    QRect rect = this->rect();
    QRect iconRect(rect.left()+iconBorder, rect.bottom()-iconSize.height()-iconBorder, iconSize.width(), iconSize.height());
    if( iconRect.contains(me->pos()) )
    {
        d->groupMenu->exec( mapToGlobal(rect.bottomLeft()) );
		me->accept();
        return;
    }
}

void GCF::MenuStripGroup::mouseDoubleClickEvent(QMouseEvent* me)
{
    if(!d->groupDrawer)
        return;

    if(me->y() >= height()-this->groupTitleHeight())
    {
        d->groupDrawer->toggleDrawer();
		me->accept();
        return;
    }
}

void GCF::MenuStripGroup::recalculateSize()
{
    int height = d->parentMenuStrip->groupContainerRect().height() - GroupSpacing;
    QSize size = d->gridLayout->minimumSize();
    QSize newSize(size.width()+ActionContainerMargin+ActionContainerMargin, height);
    if(newSize.width() < fontMetrics().width(d->title) + iconBorder + iconSize.width())
        newSize.setWidth(fontMetrics().width(d->title) + iconBorder + iconSize.width());
    if(newSize.width() < 100)
        newSize.setWidth(100);
    setFixedSize(newSize);
}

bool GCF::MenuStripGroup::event(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::ActionAdded:
        {
            QActionEvent* ae = (QActionEvent*)event;
            QAction* action = ae->action();
            QWidget* actionWidget = 0;

            int row=-1, col=-1;
            int rowSpan = action->property("rowSpan").toInt();
            int colSpan = action->property("colSpan").toInt();
            if(!rowSpan)
                rowSpan = 1;
            if(!colSpan)
                colSpan = 1;
            if(rowSpan > MaxRows)
                rowSpan = MaxRows;

            actionWidget = createWidget(action, rowSpan, colSpan);
            d->gridLayout->findCell(rowSpan, colSpan, &row, &col);
            d->gridLayout->addWidget(actionWidget, row, col, rowSpan, colSpan, Qt::AlignJustify);
            d->actionWidgetMap[action] = actionWidget;
            action->setProperty("row", row);
            action->setProperty("col", col);
            actionWidget->show();
            recalculateSize();
            d->parentMenuStrip->updateLayout();
            d->groupMenu->addAction(action);
        }
        return true;
    case QEvent::ActionRemoved:
        {
            QActionEvent* ae = (QActionEvent*)event;
            QAction* action = ae->action();
            QWidget* actionWidget = d->actionWidgetMap.contains(action) ? d->actionWidgetMap[action] : 0;
            int row = action->property("row").toInt();
            int col = action->property("col").toInt();
            int rowSpan = action->property("rowSpan").toInt();
            int colSpan = action->property("colSpan").toInt();

            if(actionWidget && qobject_cast<QToolButton*>(actionWidget))
                delete actionWidget;
            d->actionWidgetMap.remove(action);
            d->gridLayout->markCells(false, row, col, rowSpan, colSpan);
            recalculateSize();
            d->parentMenuStrip->updateLayout();
            d->groupMenu->removeAction(action);
        }
        return true;
	case QEvent::ToolTip:
		{
			if(d->groupDrawer)
			{
				QString toolTip = d->groupDrawer->toolTipText();
				QRect ttRect(0, height()-this->groupTitleHeight(), width(), this->groupTitleHeight());
				QToolTip::showText(QCursor::pos(), toolTip, this, ttRect);

				return true;
			}
		}

    default:
        break;
    }

    return QWidget::event(event);
}

bool GCF::MenuStripGroup::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == d->actionContainer)
    {
        switch(event->type())
        {
        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
            return this->event(event);
        default:
            break;
        }
    }
    return false;
}

QWidget* GCF::MenuStripGroup::createWidget(QAction* action, int rowSpan, int colSpan)
{
    QWidget* ret = 0;
    int gSize=int((fontMetrics().height()+fontMetrics().ascent())*1.2);
    int minWidth = gSize*colSpan;

    QWidgetAction* wAction = qobject_cast<QWidgetAction*>(action);
    if(wAction)
        ret = wAction->requestWidget(d->actionContainer);
    else
    {
        QToolButton* tb = new QToolButton(d->actionContainer);
        tb->setDefaultAction(action);
        tb->setAutoRaise(true);
        /*if(action->menu())
            tb->setMenu(action->menu());*/
        tb->setIconSize(QSize(gSize-4,gSize-4));

        if(action->icon().isNull())
            tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
        else if(colSpan == rowSpan && colSpan >= 2)
            tb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        else if(colSpan == rowSpan && colSpan == 1)
            tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
        else if(colSpan >= 2 || rowSpan == 1)
            tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if(rowSpan >= 3)
            tb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        int mw = 0;
        if(tb->toolButtonStyle() != Qt::ToolButtonIconOnly)
            mw += fontMetrics().width(action->iconText())+2*fontMetrics().width("_");

        if(!action->icon().isNull())
        {
            switch(tb->toolButtonStyle())
            {
            case Qt::ToolButtonTextUnderIcon:
                if(mw < gSize-4)
                    mw = gSize-4;
                break;
            case Qt::ToolButtonTextBesideIcon:
                mw += gSize-4;
            break;
            }
        }
        if(minWidth < mw)
            minWidth = mw;

        if(action->menu())
        {
            // Connect the default action object to the tool button. This way
            // when the toolbutton with menu is clicked, the default action is
            // shown.
            QList<QAction*> actions = action->menu()->actions();
            QAction* defAction = 0;
            for(int i=0; i<actions.count(); i++)
            {
                QAction* action = actions[i];
                QList<QByteArray> propNames = action->dynamicPropertyNames();
                if(propNames.contains("_default_"))
                {
                    bool val = action->property("_default_").toBool();
                    if(val)
                    {
                        defAction = action;
                        break;
                    }
                }
            }

            if(defAction)
            {
                if(defAction->isCheckable())
                    connect(tb, SIGNAL(clicked()), defAction, SLOT(toggle()));
                else
                    connect(tb, SIGNAL(clicked()), defAction, SLOT(trigger()));

                QFont font = defAction->font();
                font.setBold(true);
                defAction->setFont(font);
            }
        }

        ret = tb;
    }

    ret->setMinimumSize(minWidth, gSize*rowSpan);

    if(!action->icon().isNull())
    {
        QFont font = ret->font();
        // font.setPointSize(font.pointSize()-1);
        ret->setFont(font);
    }

    /*static QPlastiqueStyle Style;
    ret->setStyle(&Style);*/

    return ret;
}

void GCF::MenuStripGroup::setDrawGroupTitle(bool val)
{
    if(d->drawGroupTitle == val)
        return;

    d->drawGroupTitle = val;
    update();
}

bool GCF::MenuStripGroup::isDrawGroupTitle() const
{
    return d->drawGroupTitle;
}

int GCF::MenuStripGroup::groupTitleHeight() const
{
    return fontMetrics().height()+fontMetrics().ascent();
}

void GCF::MenuStripGroup::ensureVisible(int requiredWidth)
{
	d->parentMenuStrip->ensureVisible(this, requiredWidth);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::MenuStripGroupGridLayout
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::MenuStripGroupGridLayoutData
{
    QVector<bool> gridMatrix;
    int rowCount;
    int colCount;
};

GCF::MenuStripGroupGridLayout::MenuStripGroupGridLayout(QWidget* parentWidget)
:QGridLayout(parentWidget)
{
    d = new GCF::MenuStripGroupGridLayoutData;
    d->rowCount = MaxRows;
    d->colCount = 1;
    d->gridMatrix = QVector<bool>(MaxRows);
    for(int i=0; i<d->gridMatrix.count(); i++)
        d->gridMatrix[i] = false;

    setSpacing(0);
    setMargin(0);
}

GCF::MenuStripGroupGridLayout::~MenuStripGroupGridLayout()
{
    delete d;
}

void GCF::MenuStripGroupGridLayout::findCell(int rowSpan, int colSpan, int* row, int* col, bool mark)
{
    if(!row || !col)
        return;

    *row = 0;
    *col = 0;

    for(int j=0; j<d->colCount; j++)
    {
        if(j+colSpan > d->colCount)
            indexOf(0, d->colCount+colSpan);
        for(int i=0; i<d->rowCount; i++)
        {
            if(i+rowSpan > d->rowCount)
                break;
            bool marked = isCellsMarked(i, j);
            if(!marked)
            {
                bool success = isCellsMarked(i, j, rowSpan, colSpan);
                if(!success)
                {
                    *row = i;
                    *col = j;
                    if(mark)
                        markCells(true, i, j, rowSpan, colSpan);
                    return;
                }
            }
        }
    }

    *row = 0;
    *col = d->colCount;
    if(mark)
        markCells(true, 0, d->colCount, rowSpan, colSpan);
    return;
}

void GCF::MenuStripGroupGridLayout::markCells(bool val, int row, int col, int rowSpan, int colSpan)
{
    for(int i=row; i<row+rowSpan; i++)
    {
        for(int j=col; j<col+colSpan; j++)
        {
            int index = indexOf(i, j);
            if(index >= 0)
                d->gridMatrix[index] = val;
        }
    }
}

bool GCF::MenuStripGroupGridLayout::isCellsMarked(int row, int col, int rowSpan, int colSpan)
{
    bool ret = false;

    if(rowSpan == 1 && colSpan == 1)
    {
        int index = indexOf(row, col);
        return d->gridMatrix[index];
    }

    for(int i=row; i<row+rowSpan; i++)
    {
        for(int j=col; j<col+colSpan; j++)
        {
            int index = indexOf(i, j);
            ret &= d->gridMatrix[index];
            if(!ret)
                return false;
        }
    }
    return true;
}

void GCF::MenuStripGroupGridLayout::markAllCells(bool val)
{
    for(int i=0; i<d->gridMatrix.count(); i++)
        d->gridMatrix[i] = val;
}

int GCF::MenuStripGroupGridLayout::indexOf(int row, int col) const
{
    if(row < 0 || row >= d->rowCount)
        return -1;
    if(col < 0 || col >= d->colCount)
    {
        QVector<bool> newVec(d->rowCount*(col+1));
        for(int i=0; i<newVec.count(); i++)
            newVec[i] = false;
        for(int i=0; i<d->rowCount; i++)
        {
            for(int j=0; j<d->colCount; j++)
            {
                int oldIndex = i*d->colCount + j;
                int newIndex = i*(col+1) + j;
                newVec[newIndex] = d->gridMatrix[oldIndex];
            }
        }
        d->colCount = col+1;
        d->gridMatrix = newVec;
    }
    return row*d->colCount + col;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::MenuStripGroupDrawer
/////////////////////////////////////////////////////////////////////////////////////////////////////////

const int MaxDrawerFrames = 100;

namespace GCF
{
    struct MenuStripGroupDrawerData
    {
        MenuStripGroupDrawerData() : timeline(0), state(GCF::MenuStripGroupDrawer::Close),
            maxHeight(0), currentHeight(0), heightPerFrame(0), 
			parentGroupMinWidth(0), parentGroupMaxWidth(0), parentGroupWidthPerFrame(0),
			refreshToolTip(false) { }

        QTimeLine* timeline;
        GCF::MenuStripGroupDrawer::DrawerState state;
        GCF::MenuStripGroup* parentGroup;

        int minHeight;
        int maxHeight;
        int currentHeight;
        qreal heightPerFrame;

        int parentGroupMinWidth;
        int parentGroupMaxWidth;
        qreal parentGroupWidthPerFrame;

        QList<QWidget*> widgetList;
        QWidget* drawerContainer;
        QVBoxLayout* layout;
        QSize maxSize;
        QPixmap drawerContainerPm;

		bool refreshToolTip;
		QString toolTipText;

        void grabDrawerContainer();
    };
}

GCF::MenuStripGroupDrawer::MenuStripGroupDrawer(GCF::MenuStripGroup* parent)
:QWidget(parent, Qt::Popup|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::CustomizeWindowHint)
{
    d = new MenuStripGroupDrawerData;
    d->parentGroup = parent;
    d->timeline = new QTimeLine(300, this);
    d->timeline->setFrameRange(0, MaxDrawerFrames);
    connect(d->timeline, SIGNAL(frameChanged(int)), this, SLOT(timelineFrameChanged(int)));
    connect(d->timeline, SIGNAL(finished()), this, SLOT(timelineFinished()));

    d->drawerContainer = new QWidget(this);
    d->drawerContainer->setAutoFillBackground(false);
    d->drawerContainer->setAttribute(Qt::WA_NoSystemBackground);
    // d->drawerContainer->setAttribute(Qt::WA_OpaquePaintEvent);
    d->drawerContainer->hide();
    d->layout = new QVBoxLayout(d->drawerContainer);
}

GCF::MenuStripGroupDrawer::~MenuStripGroupDrawer()
{
    delete d;
}

GCF::MenuStripGroupDrawer::DrawerState GCF::MenuStripGroupDrawer::drawerState() const
{
    return d->state;
}

GCF::MenuStripGroup* GCF::MenuStripGroupDrawer::parentGroup() const
{
    return d->parentGroup;
}

QString GCF::MenuStripGroupDrawer::toolTipText() const
{
	if(d->refreshToolTip)
	{
		int lastNewLine = 0;
		d->toolTipText = "Double click to see ";

		for(int i=0; i<d->widgetList.count(); i++)
		{
			QWidget* w = d->widgetList[i];
			QLabel* label = (QLabel*)(w->property("QLabel*").value<QWidget*>());

			if(i+1 < d->widgetList.count())
				d->toolTipText += QString("\"%1\", ").arg(label->text());
			else
				d->toolTipText += QString("\"%1\"").arg(label->text());

			if(d->toolTipText.length() - lastNewLine > 50)
			{
				d->toolTipText += QString("\n");
				lastNewLine = d->toolTipText.length();
			}
		}
	}

	return d->toolTipText;
}

void GCF::MenuStripGroupDrawer::addWidget(QWidget* w)
{
    if( d->widgetList.contains(w) || !w)
        return;

    if(d->state == Open || d->state == Opening)
        closeDrawer(); 

    QColor color = palette().highlightedText().color();
    QColor bgColor = palette().highlight().color();
    QString ss = QString(
        "color: rgb(%1, %2, %3);\n"
        "background-color: rgb(%4, %5, %6);\n"
        ).arg(color.red()).arg(color.green()).arg(color.blue())
        .arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue());

    QLabel* label = new QLabel(this);
    label->setText(w->windowTitle());
    label->setStyleSheet(ss);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label->setMargin(2);
    d->layout->addWidget(label);
    d->layout->addWidget(w);
    d->widgetList.append(w);

    // Ensure that the widget is shown. Sometimes the caller might have
    // explicitly hidden the widget.
    w->setVisible(true);
	d->refreshToolTip = true;

    w->setProperty( "QLabel*", qVariantFromValue<QWidget*>( (QWidget*)(label) ) );
}

void GCF::MenuStripGroupDrawer::removeWidget(QWidget* w)
{
    if( !w || !d->widgetList.contains(w) )
        return;

    QLabel* label = (QLabel*)(w->property("QLabel*").value<QWidget*>());
    delete label;
    d->widgetList.removeAll(w);
    d->layout->removeWidget(w);
	d->refreshToolTip = true;
}

int GCF::MenuStripGroupDrawer::widgetCount() const
{
    return d->widgetList.count();
}

QWidget* GCF::MenuStripGroupDrawer::widget(int index) const
{
    if(index < 0 || index >= d->widgetList.count())
        return 0;

    return d->widgetList[index];
}

void GCF::MenuStripGroupDrawer::openDrawer()
{
	// Make sure that the menu strip doesnt scroll, and that the parent group is visible
    computeMaximumSize();
	d->parentGroup->ensureVisible(d->maxSize.width()+20);
	d->parentGroup->parentMenuStrip()->lockGroupContainer();

    // Compute the area where the drawer needs to be shown.
    QRect rect = d->parentGroup->rect();
    QPoint blPt = rect.bottomLeft() - QPoint(0, d->parentGroup->groupTitleHeight());
    blPt = d->parentGroup->mapToGlobal( blPt );

    // Compute the growth strategy of the drawer
    d->minHeight = d->parentGroup->groupTitleHeight();
    d->maxHeight = d->maxSize.height();
    d->currentHeight = d->minHeight;
    d->heightPerFrame = qreal(d->maxHeight - d->currentHeight)/qreal(MaxDrawerFrames);

    // Adjust the width if needed
    if(rect.width() < d->maxSize.width())
        rect.setWidth( d->maxSize.width() );

    // Find out if the parent group also has to be expanded....
    d->parentGroupMinWidth = d->parentGroup->minimumWidth();
    d->parentGroupMaxWidth = d->maxSize.width();
	if(d->parentGroupMaxWidth < d->parentGroupMinWidth)
		d->parentGroupMaxWidth = d->parentGroupMinWidth;
    if(d->parentGroupMinWidth < d->parentGroupMaxWidth)
        d->parentGroupWidthPerFrame = qreal(d->parentGroupMaxWidth-d->parentGroupMinWidth)/qreal(MaxDrawerFrames);
    else
        d->parentGroupWidthPerFrame = 0;

    // Capture the drawer container widget
	QRect dcRect = computeDrawerContainerRect(rect);
    d->drawerContainer->setGeometry(dcRect);
	if(d->drawerContainerPm.isNull())
		d->grabDrawerContainer();

    // Show the drawer for the first time.
    setGeometry(blPt.x(), blPt.y(), rect.width(), d->currentHeight);
    show();
    
    // The tray is opening now.
    d->state = Opening;
    d->drawerContainer->hide();

    // Start the timeline
    d->timeline->stop();
    d->timeline->setDirection(QTimeLine::Forward);
    d->timeline->start();
}

void GCF::MenuStripGroupDrawer::closeDrawer()
{
    // min, max and current height are already computed during opening of the drawer
    d->state = Closing;
    d->drawerContainer->hide();
    qApp->removeEventFilter(this);

    // Capture the drawer container widget
    d->grabDrawerContainer();

    // Restart the timeline
    d->timeline->stop();
    d->timeline->setDirection(QTimeLine::Backward);
    d->timeline->start();
}

void GCF::MenuStripGroupDrawer::toggleDrawer()
{
    if(d->state == Close)
        openDrawer();
    else if(d->state == Open)
        closeDrawer();  
}

void GCF::MenuStripGroupDrawer::resizeEvent(QResizeEvent* re)
{
    QRect dcRect = computeDrawerContainerRect(this->rect());
    d->drawerContainer->setGeometry(dcRect);
}

void GCF::MenuStripGroupDrawer::paintEvent(QPaintEvent* pe)
{
    QPainter paint(this);
    paint.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);

    // Draw the shadows and stuff..
    QPalette pal = d->parentGroup->palette();
    QColor bgColor = MEAN_COLOR(pal.light().color(), pal.highlight().color());
    QColor shadow = MEAN_COLOR(pal.shadow().color(), pal.highlight().color());
    paint.fillRect(this->rect(), bgColor);

    if(!d->drawerContainer->isVisible())
    {
        // If the drawer container is not visible, we render the widget on the screen
        // this state is true when the drawer is coming out

        // Figure out where the drawer rectangle needs to be drawn.
        QRect dcPlaceRect = computeDrawerContainerRect(this->rect());
        if(dcPlaceRect.height() < 0)
            return;

        // Figure out what part of the container should be drawn.
        QRect dcRect( QPoint(0, 0), d->layout->sizeHint() );
        dcRect.setTop( dcRect.bottom() - dcPlaceRect.height() );

        // Now lets perform the painting
        // d->drawerContainer->render(&paint, dcPlaceRect.topLeft(), dcRect);
        paint.drawPixmap(dcPlaceRect, d->drawerContainerPm, dcRect);
    }

    QRect shadowRect(0, 0, width(), 8);

    {
        QLinearGradient grad(shadowRect.topLeft(), shadowRect.bottomLeft());
        grad.setColorAt(0, shadow);
        grad.setColorAt(1, bgColor);
        paint.fillRect(shadowRect, grad);
    }

    shadowRect.moveTopLeft(shadowRect.topLeft() + QPoint(0, height()-d->parentGroup->groupTitleHeight()-9));

    {
        QLinearGradient grad(shadowRect.topLeft(), shadowRect.bottomLeft());
        grad.setColorAt(1, shadow);
        grad.setColorAt(0, bgColor);
        paint.fillRect(shadowRect, grad);
    }

    paint.setPen( shadow );
    paint.drawRect( this->rect().adjusted(0, 0, -1, -1) );

    d->parentGroup->drawTextAndIcon(&paint, this->rect());
}

void GCF::MenuStripGroupDrawer::showEvent(QShowEvent*)
{
    d->parentGroup->setDrawGroupTitle(false);
	d->parentGroup->parentMenuStrip()->lockGroupContainer();
}

void GCF::MenuStripGroupDrawer::hideEvent(QHideEvent*)
{
    d->parentGroup->setDrawGroupTitle(true);
	d->parentGroup->parentMenuStrip()->unlockGroupContainer();
    d->state = Close;
}

void GCF::MenuStripGroupDrawer::mousePressEvent(QMouseEvent* me)
{
    int h = d->parentGroup->groupTitleHeight();
    if(me->y() < h || me->y() > height()-h)
    {
        closeDrawer();
        me->accept();
    }
}

void GCF::MenuStripGroupDrawer::computeMaximumSize()
{
    int maxHeight = d->layout->sizeHint().height();
    maxHeight += 10 + // offset from the top edge
                d->parentGroup->groupTitleHeight() + // offset for the group title bar
                10; // Offset from the group title bar to the container

    int maxWidth = d->layout->sizeHint().width();
    maxWidth += 10 + // margin on left
                10; // margin on right.

    d->maxSize = QSize(maxWidth, maxHeight);
}

QRect GCF::MenuStripGroupDrawer::computeDrawerContainerRect(const QRect& rect)
{
    return rect.adjusted(10, // left margin
                         10, // top margin
                         -10, // right margin
                         -10 - d->parentGroup->groupTitleHeight());
}

void GCF::MenuStripGroupDrawer::timelineFrameChanged(int frame)
{
    d->currentHeight = d->minHeight + int(d->heightPerFrame*qreal(frame));
    if(d->state == Opening)
    {
        if(d->currentHeight >= d->maxHeight)
        {
            d->timeline->stop();
            d->drawerContainer->show();
            d->state = Open;
        }
    }
    else if(d->state == Closing)
    {
        if(d->currentHeight <= d->minHeight + d->heightPerFrame*3)
        {
            d->timeline->stop();
            d->state = Close;
            if(d->parentGroupWidthPerFrame)
                d->parentGroup->setFixedWidth(d->parentGroupMinWidth);
            hide();
        }
    }

    if( d->state != Close )
    {
        QRect geo = this->geometry();
        geo.setHeight( d->currentHeight );
        setGeometry(geo);
        show();
    }

    if(!d->parentGroupWidthPerFrame && d->state != Close)
        return;

    // Animate parent group width change if needed...
    int pgWidth = d->parentGroupMinWidth + int(d->parentGroupWidthPerFrame*qreal(frame));
    if(pgWidth < d->parentGroupMinWidth)
        pgWidth = d->parentGroupMinWidth;
    else if(pgWidth > d->parentGroupMaxWidth)
        pgWidth = d->parentGroupMaxWidth;

    d->parentGroup->setFixedWidth(pgWidth);
}

void GCF::MenuStripGroupDrawer::timelineFinished()
{
    if(d->state == Opening)
    {
        // Compute the final geometry
        QRect geo = this->geometry();
        geo.setHeight( d->maxHeight );
        setGeometry(geo);

        d->drawerContainer->show();
        d->state = Open;
    }
    else if(d->state == Closing)
    {
        d->state = Close;
        if(d->parentGroupWidthPerFrame)
            d->parentGroup->setFixedWidth(d->parentGroupMinWidth);
    }
}

static void sendResizeEvents(QWidget *target)
{
    QResizeEvent e(target->size(), QSize());
    QApplication::sendEvent(target, &e);

    const QObjectList children = target->children();
    for (int i = 0; i < children.size(); ++i) {
        QWidget *child = static_cast<QWidget*>(children.at(i));
        if (child->isWidgetType() && !child->isWindow() && child->testAttribute(Qt::WA_PendingResizeEvent))
            sendResizeEvents(child);
    }
}

void GCF::MenuStripGroupDrawerData::grabDrawerContainer()
{
    /*
    The following code has been adopted from QPixmap::grabWidget().
    We dont want grabWidget to draw window background.
    */
    if (!drawerContainer)
        return;

    if (drawerContainer->testAttribute(Qt::WA_PendingResizeEvent) || !drawerContainer->testAttribute(Qt::WA_WState_Created))
        sendResizeEvents(drawerContainer);

    QPixmap res(drawerContainer->size());
    res.fill(Qt::transparent);
    drawerContainer->render(&res, QPoint(), QRect(), QWidget::DrawChildren);

    this->drawerContainerPm = res;
}



