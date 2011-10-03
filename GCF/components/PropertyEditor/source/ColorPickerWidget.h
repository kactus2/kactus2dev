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

#ifndef COLOR_PICKER_WIDGET_H
#define COLOR_PICKER_WIDGET_H

#include <QWidget>
#include <QDialog>

namespace GCF
{
	namespace Components
	{
		struct ColorPickerWidgetData;
		class ColorPickerWidget : public QWidget
		{
			Q_OBJECT
			Q_PROPERTY(QColor CurrentColor READ currentColor WRITE setCurrentColor)

		public:
			ColorPickerWidget(QWidget *parent = 0);
			~ColorPickerWidget();

			void setCurrentColor(QColor color);
			QColor currentColor() const;

		public slots:
			void copyColorToClipboard();
			void copyColorFToClipboard();

		private slots:
			void slotCurrentColorChanged(QColor color);
			void slotColorValueChanged();
			void on_chkTransparent_toggled(bool val);

		signals:
			void currentColorChanged(QColor color);

		private:
			ColorPickerWidgetData *d;
		};

		struct ColorPaletteWidgetData;
		class ColorPaletteWidget : public QWidget
		{
			Q_OBJECT
			Q_PROPERTY(QColor CurrentColor READ currentColor WRITE setCurrentColor)

		public:
			ColorPaletteWidget(QWidget* parent=0);
			~ColorPaletteWidget();

			void setCurrentColor(QColor color);
			QColor currentColor() const;

		signals:
			void currentColorChanged(QColor color);

		protected:
			void paintEvent(QPaintEvent* pe);
			void resizeEvent(QResizeEvent* re);
			void mousePressEvent(QMouseEvent* me);
			void mouseMoveEvent(QMouseEvent* me);

		protected:
			QColor colorAt(QPoint p);
			QPoint pointOf(QColor c);

		private:
			ColorPaletteWidgetData* d;
		};

		struct ColorValueSliderData;
		class ColorValueSlider : public QWidget
		{
			Q_OBJECT
			Q_PROPERTY(QColor BaseColor READ baseColor WRITE setBaseColor)
			Q_PROPERTY(int Value READ value WRITE setValue)

		public:
			ColorValueSlider(QWidget* parent=0);
			~ColorValueSlider();

			void setBaseColor(QColor color);
			QColor baseColor() const;

			void setValue(int value);
			int value() const;

			QColor color() const;

		signals:
			void valueChanged(int value);

		protected:
			void paintEvent(QPaintEvent* pe);
			void resizeEvent(QResizeEvent* re);
			void mousePressEvent(QMouseEvent* me);
			void mouseMoveEvent(QMouseEvent* me);
			void preparePixmap();

		private:
			ColorValueSliderData* d;
		};

		class ColorLabel : public QWidget
		{
		public:
			ColorLabel(QWidget* parent=0);
			~ColorLabel();

			void setColor(QColor color);
			QColor color() const;

		protected:
			void paintEvent(QPaintEvent* pe);

		private:
			QColor m_color;
		};
	}
}

#endif

