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

#ifndef SCRIPT_EDITOR_H
#define SCRIPT_EDITOR_H

#include <QTextEdit>
#include <QSyntaxHighlighter>
#include "IProjectPart.h"

class QHelpEvent;

namespace GCF
{
	namespace Components
	{
		class ScriptEngineManager;

		struct ScriptEditorData;
		class ScriptEditor : public QTextEdit, virtual public IProjectPart
		{
			Q_OBJECT
			Q_INTERFACES(IProjectPart)
			Q_PROPERTY(int TabSpace READ tabSpace WRITE setTabSpace)
			Q_PROPERTY(QString FontName READ fontName WRITE setFontName)
			Q_PROPERTY(int FontSize READ fontSize WRITE setFontSize)
			Q_PROPERTY(QString Code READ code WRITE setCode)
			Q_PROPERTY(bool ShowLineNumbers READ isShowLineNumbers WRITE setShowLineNumbers)

		public:
			ScriptEditor(QWidget* parent=0);
			~ScriptEditor();

			void setScriptingEngineManager(ScriptEngineManager* manager);
			ScriptEngineManager* scriptEngineManager() const;

			int tabSpace() const;
			void setTabSpace(int space);

			void setFontName(const QString& fontName);
			QString fontName() const;

			void setFontSize(int size);
			int fontSize() const;

			void setCode(QString code);
			QString code() const;

			void setShowLineNumbers(bool val);
			bool isShowLineNumbers() const;

		public slots:
			void gotoLine(int linrNr);
			void markLine(int lineNr, QString toolTip=QString(), QColor color=Qt::red);
			void unmarkLine(int lineNr);
			void toggleMarkLine(int lineNr, QColor color=Qt::red);
			void unmarkAllLines();

		protected slots:
			void showGotoLineDialog();
			void showFindDialog();
			void findNext();
			void findPrevious();
			void showFindAndReplaceDialog();
			void replaceText();
			void replaceAll();
			void insertCompletion(const QString& text);

		protected:
			bool event(QEvent* e);
			void myPaintEvent(QPaintEvent* pe);
			void myMousePressEvent(QMouseEvent* me);
			void myHelpEvent(QHelpEvent* he);
			void keyPressEvent(QKeyEvent* ke);
			void timerEvent(QTimerEvent* te);
			QString textUnderCursor() const;
			void prepareCompleter();

		public:
			// IContainer implementation
			QObject* containerObject();

			// IProjectPart implemnetation
			QString projectPartID();
			bool newProjectPart();
			bool loadProjectPart(QDomDocument doc, QDomElement partE);
			bool initializeProjectPart();
			bool saveProjectPart(QDomDocument doc, QDomElement partParentE);
			bool closeProjectPart();

		signals:
			void projectPartModified();

		private:
			ScriptEditorData* d;
		};

		class ScriptHighlighter : public QSyntaxHighlighter
		{
		public:
			ScriptHighlighter(ScriptEditor* parent);
			virtual void highlightBlock(const QString &text);

		private:
			void highlightKeyword(int currentPos, const QString &buffer);

			QTextCharFormat m_numberFormat;
			QTextCharFormat m_stringFormat;
			QTextCharFormat m_typeFormat;
			QTextCharFormat m_keywordFormat;
			QTextCharFormat m_labelFormat;
			QTextCharFormat m_commentFormat;
			QTextCharFormat m_preProcessorFormat;
		};
	}
}

#endif
