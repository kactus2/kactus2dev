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

#ifndef SCRIPT_ENGINE_MANAGER_H
#define SCRIPT_ENGINE_MANAGER_H

#include <QObject>
#include "IScriptEngineManager.h"
#include "IScriptDataType.h"

class QScriptEngine;

namespace GCF
{
	namespace Components
	{

		class ScriptableObjectExplorer;
		class ScriptEditor;

		struct ScriptEngineManagerData;
		class ScriptEngineManager : public QObject,
									 virtual public IScriptEngineManager,
									 virtual public IScriptDataType
		{
			Q_OBJECT
			Q_INTERFACES(IScriptEngineManager IScriptDataType)

		public:
			ScriptEngineManager();
			~ScriptEngineManager();

			int scriptableObjectCount() const;
			QObject* scriptableObject(int index) const;
			QString scriptableObjectName(int index) const;
			bool scriptableObjectExcludeChildren(int index) const;

			void setScriptableObjectExplorer(ScriptableObjectExplorer* exp);
			ScriptableObjectExplorer* scriptableObjectExplorer() const;

			void setScriptEditor(ScriptEditor* scriptEditor);
			ScriptEditor* scriptEditor() const;

			// IContainer
			QObject* containerObject();

			// IScriptEngineManager implementation
			void addScriptableObject(QObject* object, QString name, bool excludeChildren=true);
			void removeScriptableObject(QObject* object);
			void removeScriptableObject(QString name);
			void changeScriptableObjectName(QObject* object, QString newName);
			QScriptEngine* engine();
			void resetEngine();
			void setScript(QString script);
			QString script();
			void executeScript();
			void addScriptDataType(IScriptDataType* type);
			void removeScriptDataType(IScriptDataType* type);
			int scriptDataTypeCount();
			IScriptDataType* scriptDataType(int index);

			// IScriptDataType implementation
			void registerTypes(QScriptEngine* engine);

		signals:
			void scriptError(QString errMsg, int lineNr);

		private slots:
			void objectNameChanged();
			void setUpdateEditor(bool val);

		private:
			friend class ScriptEditor;
			friend class ScriptEditorComponent;
			ScriptEngineManagerData* d;
		};
	}
}

#endif


