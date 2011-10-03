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

#ifndef SCRIPTABLE_OBJECT_EXPLORER_H
#define SCRIPTABLE_OBJECT_EXPLORER_H

#include <QTreeWidget>
#include "IScriptableObjectExplorer.h"

namespace GCF
{
	namespace Components
	{
		class ScriptEngineManager;

		struct ScriptableObjectExplorerData;
		class ScriptableObjectExplorer : public QTreeWidget,
										  virtual public IScriptableObjectExplorer
		{
			Q_OBJECT
			Q_INTERFACES(IScriptableObjectExplorer)

		public:
			ScriptableObjectExplorer(QWidget* parent=0);
			~ScriptableObjectExplorer();

			void setScriptEngineManager(ScriptEngineManager* manager);
			ScriptEngineManager* scriptEngineManager() const;

			// IContainer implementation
			QObject* containerObject();

			// IScriptableObjectExplorer implementation
			IScriptEngineManager* scriptEngine();

		public slots:
			void updateExplorer();
			void monitorEvents(QWidget* widget, bool value=true);
			void highlightScriptableObject(QObject* object);

		signals:
			void objectDoubleClicked(QObject* object, QString objectName);
			void signalDoubleClicked(QObject* object, QString objectName, QString event, QString signal);
			void propertyDoubleClicked(QObject* object, QString objectName, QString propertyName, QString propDataType);
			void methodDoubleClicked(QObject* object, QString objectName, QString method, QString methodSignature);

		protected slots:
			void on_itemDoubleClicked(QTreeWidgetItem* item);

		protected:
			bool eventFilter(QObject* object, QEvent* event);

		private:
			ScriptableObjectExplorerData* d;
		};
	}
}

#endif

