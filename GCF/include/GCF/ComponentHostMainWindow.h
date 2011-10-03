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

#ifndef COMPONENT_HOST_MAIN_WINDOW_H
#define COMPONENT_HOST_MAIN_WINDOW_H

#include "IComponentHostWindow.h"
#include <QMainWindow>

namespace GCF
{

	class ComponentHostMainWindow : virtual public IComponentHostWindow
	{
		GCF_DECLARE_QUERYABLE

	public:
		ComponentHostMainWindow(QMainWindow* mainWindow) : m_mainWindow(mainWindow) { }
		virtual ~ComponentHostMainWindow() { }

		// Associate menu bars
		QMenuBar* windowMenuBar() const { return m_mainWindow->menuBar(); }
		void setWindowMenuBar(QMenuBar* menuBar) { m_mainWindow->setMenuBar(menuBar); }

		// Add toolbar
		void addWindowToolBar(QToolBar* toolbar) { m_mainWindow->addToolBar(toolbar); }

		// Custom menu widget
		void setWindowMenuWidget(QWidget* widget) { m_mainWindow->setMenuWidget(widget); }
		QWidget* windowMenuWidget() const { return m_mainWindow->menuWidget(); }

		// Pointer to a container widget. Dialog boxes will be modal with
		// respect to the pointer returned by this function
		QWidget* windowWidget() const { return m_mainWindow; }

	private:
		QMainWindow* m_mainWindow;
	};

}

GCF_BEGIN_QUERY_TABLE(GCF::ComponentHostMainWindow)
    GCF_IMPLEMENTS(IComponentHostWindow)
GCF_END_QUERY_TABLE(GCF::ComponentHostMainWindow)

#endif
