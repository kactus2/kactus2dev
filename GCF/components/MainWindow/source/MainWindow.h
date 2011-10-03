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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <GCF/AbstractComponent.h>

class QTabWidget;
class QDockWidget;
class QSplitter;

namespace GCF
{
	namespace Components
	{
		class MainWindowComponent;

		struct MainWindowData;
		class GCF_EXPORT_CLASS2 MainWindow : public QMainWindow
		{
			Q_OBJECT

		public:
			static MainWindow & instance();
			~MainWindow();

			QSplitter* centralWidget() const;
			QTabWidget* workspace();
			QDockWidget* dockWidget(Qt::DockWidgetArea area);

		signals:
			void workspaceWidgetActivated(QWidget* widget);

		protected:
			MainWindow();
			void addChildWidget(QWidget* parent, QWidget* child);
			void removeChildWidget(QWidget* parent, QWidget* child);
			void showChildWidget(QWidget* parent, QWidget* child);
			void hideChildWidget(QWidget* parent, QWidget* child);

		protected slots:
			void showConfigurationDialog();
			void on_workspace_currentChanged(int index);
			void about();

		protected:
			void closeEvent(QCloseEvent* ce);
			bool eventFilter(QObject* obj, QEvent* event);

		private:
			friend class MainWindowComponent;
			MainWindowData* d;
		};
	}
}

#endif
