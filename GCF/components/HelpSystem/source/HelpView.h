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

#ifndef HELP_VIEW_H
#define HELP_VIEW_H

#include <QTextBrowser>

namespace GCF
{
	namespace Components
	{
		struct HelpViewData;
		class HelpView : public QTextBrowser
		{
			Q_OBJECT
			Q_PROPERTY(QString FontName READ fontName WRITE setFontName)
			Q_PROPERTY(int FontSize READ fontSize WRITE setFontSize)

		public:
			HelpView(QWidget* parent=0);
			~HelpView();

			void setSource(const QUrl &url);

			void setFontName(const QString& fontName);
			QString fontName() const;

			void setFontSize(int size);
			int fontSize() const;

		public slots:
			QVariant loadResource(int type, const QUrl &name);
			void showHelp(const QUrl& url);
			void showHelp( const QMap<QString, QUrl> & links, const QString & keyword);
			void zoomIn();
			void zoomOut();

		private:
			HelpViewData* d;
		};
	}
}

#endif
