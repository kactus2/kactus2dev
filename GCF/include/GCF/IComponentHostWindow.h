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

#ifndef I_COMPONENT_HOST_WINDOW_H
#define I_COMPONENT_HOST_WINDOW_H

#include "IQueryable.h"
class QMenuBar;
class QToolBar;
class QWidget;

class IComponentHostWindow : virtual public IQueryable
{
public:
    // Associate menu bars
    virtual QMenuBar* windowMenuBar() const = 0;
    virtual void setWindowMenuBar(QMenuBar* menuBar) = 0;

    // Add toolbar
    virtual void addWindowToolBar(QToolBar* toolbar) = 0;

    // Custom menu widget
    virtual void setWindowMenuWidget(QWidget* widget) = 0;
    virtual QWidget* windowMenuWidget() const = 0;

    // Pointer to a container widget. Dialog boxes will be modal with
    // respect to the pointer returned by this function
    virtual QWidget* windowWidget() const = 0;
};

#endif
