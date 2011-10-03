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

#ifndef MENU_STRIP_H
#define MENU_STRIP_H

#include "Common.h"
#include <QWidget>
#include <QGridLayout>

namespace GCF
{
    class MenuStrip;
    class MenuStripGroup;
    class MenuStripGroupGridLayout;

    struct MenuStripData;
    class GCF_EXPORT_CLASS MenuStrip : public QWidget
    {
        Q_OBJECT

    public:
        MenuStrip(QWidget* parent=0);
        ~MenuStrip();

        MenuStripGroup* addGroup(const QString& title);
        MenuStripGroup* insertGroup(const QString& title, MenuStripGroup* before);
        int groupCount() const;
        MenuStripGroup* group(int index) const;

        int heightForWidth(int width) const;

    protected:
        void paintEvent(QPaintEvent* pe);
        void resizeEvent(QResizeEvent* re);
        bool eventFilter(QObject* obj, QEvent* event);

    private:
        QWidget* groupContainer() const;
        QRect groupContainerRect() const;
        void registerGroup(MenuStripGroup* group, MenuStripGroup* before=0);
        void unregisterGroup(MenuStripGroup* group);
        void updateLayout();
		void lockGroupContainer();
		void unlockGroupContainer();
		void ensureVisible(MenuStripGroup* group, int requiredWidth=-1);

    private:
        friend class MenuStripGroup;
		friend class MenuStripGroupDrawer;
        MenuStripData* d;
    };

    struct MenuStripGroupData;
    class GCF_EXPORT_CLASS MenuStripGroup : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY(QString Title READ title WRITE setTitle)

    public:
        MenuStripGroup(MenuStrip* parent, MenuStripGroup* before=0);
        ~MenuStripGroup();

        MenuStrip* parentMenuStrip() const;

        void setTitle(const QString& title);
        const QString& title() const;

        void setIcon(const QIcon& icon);
        const QIcon& icon() const;

        void addDrawerWidget(QWidget* widget);
        void removeDrawerWidget(QWidget* widget);
        int drawerWidgetCount() const;
        QWidget* drawerWidget(int index) const;

    protected:
        void paintEvent(QPaintEvent* pe);
        void drawTextAndIcon(QPainter* paint, const QRect& rect);
        void resizeEvent(QResizeEvent* re);
        void mousePressEvent(QMouseEvent* me);
        void mouseDoubleClickEvent(QMouseEvent* me);
        void recalculateSize();
        bool event(QEvent* event);
        bool eventFilter(QObject* obj, QEvent* event);
        QWidget* createWidget(QAction* action, int rowSpan, int colSpan);

    protected:
        void setDrawGroupTitle(bool val);
        bool isDrawGroupTitle() const;
        int  groupTitleHeight() const;
		void ensureVisible(int requiredWidth=-1);

    private:
        friend class MenuStrip;
        friend class MenuStripGroupDrawer;
        MenuStripGroupData* d;
    };

    struct MenuStripGroupGridLayoutData;
    class GCF_EXPORT_CLASS MenuStripGroupGridLayout : public QGridLayout
    {
    public:
        MenuStripGroupGridLayout(QWidget* parentWidget=0);
        ~MenuStripGroupGridLayout();

        void findCell(int rowSpan, int colSpan, int* row, int* col, bool mark=true);
        void markCells(bool val, int row, int col, int rowSpan=1, int colSpan=1);
        bool isCellsMarked(int row, int col, int rowSpan=1, int colSpan=1);
        void markAllCells(bool val);
        int indexOf(int row, int col) const;

    private:
        MenuStripGroupGridLayoutData* d;
    };

    struct MenuStripGroupDrawerData;
    class MenuStripGroupDrawer : public QWidget
    {
        Q_OBJECT

    public:
        enum DrawerState
        {
            Closing,
            Close,
            Opening,
            Open,
        };

        MenuStripGroupDrawer(MenuStripGroup* parent);
        ~MenuStripGroupDrawer();

        DrawerState drawerState() const;
        GCF::MenuStripGroup* parentGroup() const;
		QString toolTipText() const;

        void addWidget(QWidget* w);
        void removeWidget(QWidget* w);
        int widgetCount() const;
        QWidget* widget(int index) const;

    public slots:
        void openDrawer();
        void closeDrawer();
        void toggleDrawer();

    protected:
        void resizeEvent(QResizeEvent* re);
        void paintEvent(QPaintEvent* pe);
        void showEvent(QShowEvent*);
        void hideEvent(QHideEvent*);
        void mousePressEvent(QMouseEvent* me);

    private:
        void computeMaximumSize();
        QRect computeDrawerContainerRect(const QRect& rect);

    protected slots:
        void timelineFrameChanged(int frame);
        void timelineFinished();

    private:
        MenuStripGroupDrawerData* d;
    };
}

#endif
