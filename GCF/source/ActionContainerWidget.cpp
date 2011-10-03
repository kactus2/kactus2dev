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

#include <GCF/ActionContainerWidget.h>

#include <QMap>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QFontMetrics>
#include <QActionEvent>
#include <QWidgetAction>

namespace GCF
{
    struct ActionContainerWidgetData
    {
        ActionContainerWidgetLayout* gridLayout;
        int maxRows;

        QMap<QAction*, QWidget*> actionWidgetMap;
    };
}

GCF::ActionContainerWidget::ActionContainerWidget(int maxRows, QWidget* parent)
:QWidget(parent)
{
    d = new ActionContainerWidgetData;
    d->maxRows = maxRows;
    d->gridLayout = new ActionContainerWidgetLayout(maxRows, this);
}

GCF::ActionContainerWidget::~ActionContainerWidget()
{
    delete d;
}

void GCF::ActionContainerWidget::addAction(QAction* action, int rowSpan, int colSpan)
{
    action->setProperty("rowSpan", rowSpan);
    action->setProperty("colSpan", colSpan);

    QWidget::addAction(action);
}

bool GCF::ActionContainerWidget::event(QEvent* e)
{
    switch(e->type())
    {
    case QEvent::ActionAdded:
        {
            QActionEvent* ae = (QActionEvent*)e;
            QAction* action = ae->action();
            QWidget* actionWidget = 0;

            int row=-1, col=-1;
            int rowSpan = action->property("rowSpan").toInt();
            int colSpan = action->property("colSpan").toInt();
            if(!rowSpan)
                rowSpan = 1;
            if(!colSpan)
                colSpan = 1;
            if(rowSpan > d->maxRows)
                rowSpan = d->maxRows;

            actionWidget = createWidget(action, rowSpan, colSpan);

            d->gridLayout->findCell(rowSpan, colSpan, &row, &col);
            d->gridLayout->addWidget(actionWidget, row, col, rowSpan, colSpan, Qt::AlignJustify);

            d->actionWidgetMap[action] = actionWidget;
            action->setProperty("row", row);
            action->setProperty("col", col);

            actionWidget->show();

        }
        return true;

    case QEvent::ActionRemoved:
        {
            QActionEvent* ae = (QActionEvent*)e;
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
        }
        return true;

    default:
        break;
    }

    return QWidget::event(e);
}

QWidget* GCF::ActionContainerWidget::createWidget(QAction* action, int rowSpan, int colSpan)
{
    QWidget* ret = 0;
    int gSize=int((fontMetrics().height()+fontMetrics().ascent())*1.2);
    int minWidth = gSize*colSpan;

    QWidgetAction* wAction = qobject_cast<QWidgetAction*>(action);
    if(wAction)
        ret = wAction->requestWidget(this);
    else
    {
        QToolButton* tb = new QToolButton(this);
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

struct GCF::ActionContainerWidgetLayoutData
{
    QVector<bool> gridMatrix;
    int rowCount;
    int colCount;
};

GCF::ActionContainerWidgetLayout::ActionContainerWidgetLayout(int maxRows, QWidget* parent)
:QGridLayout(parent)
{
    d = new ActionContainerWidgetLayoutData;

    d->rowCount = maxRows;
    d->colCount = 1;
    d->gridMatrix = QVector<bool>(d->rowCount);
    for(int i=0; i<d->gridMatrix.count(); i++)
        d->gridMatrix[i] = false;

    setSpacing(0);
    setMargin(0);
}

GCF::ActionContainerWidgetLayout::~ActionContainerWidgetLayout()
{
    delete d;
}

void GCF::ActionContainerWidgetLayout::findCell(int rowSpan, int colSpan, int* row, int* col, bool mark)
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

void GCF::ActionContainerWidgetLayout::markCells(bool val, int row, int col, int rowSpan, int colSpan)
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

bool GCF::ActionContainerWidgetLayout::isCellsMarked(int row, int col, int rowSpan, int colSpan)
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

void GCF::ActionContainerWidgetLayout::markAllCells(bool val)
{
    for(int i=0; i<d->gridMatrix.count(); i++)
        d->gridMatrix[i] = val;
}

int GCF::ActionContainerWidgetLayout::indexOf(int row, int col) const
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


