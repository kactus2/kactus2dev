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

#ifndef SCRIPT_EDITOR_CONFIGURATION_PAGE_H
#define SCRIPT_EDITOR_CONFIGURATION_PAGE_H

#include <QWidget>

namespace GCF
{
	namespace Components
	{
		class ScriptEditor;
		struct ScriptEditorConfigurationPageData;
		class ScriptEditorConfigurationPage : public QWidget
		{
			Q_OBJECT

		public:
			ScriptEditorConfigurationPage(QWidget* parent=0);
			~ScriptEditorConfigurationPage();

			void initFrom(ScriptEditor* scriptEditor, bool autoSave=false);
			void saveInto(ScriptEditor* scriptEditor);

        protected slots:
            void on_tabSize_valueChanged();
            void on_fontComboBox_currentIndexChanged();
            void on_fontSize_valueChanged();

        protected:
            void showEvent(QShowEvent* se);
            void hideEvent(QHideEvent* he);

		private:
			ScriptEditorConfigurationPageData* d;
		};
	}
}

#endif

