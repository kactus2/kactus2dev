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

#ifndef ACTION_CONTAINER_WIDGET_H
#define ACTION_CONTAINER_WIDGET_H

#include "Common.h"
#include <QWidget>
#include <QMetaType>
#include <QGridLayout>

namespace GCF
{
    struct ActionContainerWidgetData;
    class GCF_EXPORT_CLASS ActionContainerWidget : public QWidget
    {
        Q_OBJECT

    public:
        ActionContainerWidget(int maxRows=2, QWidget* parent=0);
        ~ActionContainerWidget();

        void addAction(QAction* action, int rowSpan=1, int colSpan=1);

    protected:
        bool event(QEvent* e);
        QWidget* createWidget(QAction* action, int rowSpan, int colSpan);

    private:
        ActionContainerWidgetData* d;
    };

    struct ActionContainerWidgetLayoutData;
    class GCF_EXPORT_CLASS ActionContainerWidgetLayout : public QGridLayout
    {
    public:
        ActionContainerWidgetLayout(int maxRows=2, QWidget* parent=0);
        ~ActionContainerWidgetLayout();

        void findCell(int rowSpan, int colSpan, int* row, int* col, bool mark=true);
        void markCells(bool val, int row, int col, int rowSpan=1, int colSpan=1);
        bool isCellsMarked(int row, int col, int rowSpan=1, int colSpan=1);
        void markAllCells(bool val);
        int indexOf(int row, int col) const;

    private:
        ActionContainerWidgetLayoutData* d;
    };
}

Q_DECLARE_METATYPE(GCF::ActionContainerWidget*);

#endif
