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

#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include <GCF/Common.h>
#include <QSplashScreen>
#include <QPixmap>

class QPainter;

namespace GCF
{
	namespace Components
	{

		class GCF_EXPORT_CLASS2 SplashScreen : public QSplashScreen
		{
		public:
			SplashScreen(const QPixmap & pixmap);
			~SplashScreen();

		public slots:
			void showMessage(const QString& message);

		protected:
			void mousePressEvent(QMouseEvent* me);
			bool eventFilter(QObject*, QEvent* e);

		private:
		};
	}
}

#endif
