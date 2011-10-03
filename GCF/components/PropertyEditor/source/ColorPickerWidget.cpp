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

#include "ColorPickerWidget.h"
#include "ui_ColorPickerWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QPainterPath>
#include <QApplication>
#include <QClipboard>

struct GCF::Components::ColorPickerWidgetData
{
    Ui::ColorPickerWidget ui;

    QColor currentColor;
    GCF::Components::ColorPaletteWidget* colorPaletteWidget;
    GCF::Components::ColorLabel* colorLabel;
    GCF::Components::ColorValueSlider* colorValueSlider;
};

GCF::Components::ColorPickerWidget::ColorPickerWidget(QWidget *parent):
QWidget(parent)
{
    d = new GCF::Components::ColorPickerWidgetData;
    d->ui.setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(d->ui.colorPickerFrame);
    d->colorPaletteWidget = new GCF::Components::ColorPaletteWidget(d->ui.colorPickerFrame);
    layout->setMargin(0);
    layout->addWidget(d->colorPaletteWidget);

    layout = new QVBoxLayout(d->ui.selectedColorFrame);
    d->colorLabel = new GCF::Components::ColorLabel(d->ui.selectedColorFrame);
    layout->setMargin(0);
    layout->addWidget(d->colorLabel);

    layout = new QVBoxLayout(d->ui.valueSliderFrame);
    d->colorValueSlider = new GCF::Components::ColorValueSlider(d->ui.valueSliderFrame);
    layout->setMargin(0);
    layout->addWidget(d->colorValueSlider);

    connect(d->colorPaletteWidget, SIGNAL(currentColorChanged(QColor)), this, SLOT(slotCurrentColorChanged(QColor)));
    connect(d->colorValueSlider, SIGNAL(valueChanged(int)), this, SLOT(slotColorValueChanged()));
}

GCF::Components::ColorPickerWidget::~ColorPickerWidget()
{
    delete d;
}

void GCF::Components::ColorPickerWidget::setCurrentColor(QColor color)
{
    if(d->currentColor == color)
        return;

    d->currentColor = color;
    d->colorPaletteWidget->setCurrentColor(color);
    d->colorLabel->setColor(color);
    d->colorValueSlider->setBaseColor(color);

    d->ui.redILE->setText( QString("%1").arg(color.red()) );
    d->ui.greenILE->setText( QString("%1").arg(color.green()) );
    d->ui.blueILE->setText( QString("%1").arg(color.blue()) );

    d->ui.redFLE->setText( QString("%1").arg(color.redF(), 0, 'g', 2) );
    d->ui.greenFLE->setText( QString("%1").arg(color.greenF(), 0, 'g', 2) );
    d->ui.blueFLE->setText( QString("%1").arg(color.blueF(), 0, 'g', 2) );

    emit currentColorChanged(color);
}

QColor GCF::Components::ColorPickerWidget::currentColor() const
{
    if(d->ui.chkTransparent->isChecked())
        return Qt::transparent;

    return d->currentColor;
}

void GCF::Components::ColorPickerWidget::copyColorToClipboard()
{
    int r, g, b;
    d->currentColor.getRgb(&r, &g, &b);
    QString text = QString("%1, %2, %3").arg(r).arg(g).arg(b);
    QApplication::clipboard()->setText(text);
}

void GCF::Components::ColorPickerWidget::copyColorFToClipboard()
{
    qreal r, g, b;
    d->currentColor.getRgbF(&r, &g, &b);
    QString text = QString("%1, %2, %3").arg(r,0,'g',3).arg(g,0,'g',3).arg(b,0,'g',3);
    QApplication::clipboard()->setText(text);
}

void GCF::Components::ColorPickerWidget::slotCurrentColorChanged(QColor color)
{
    setCurrentColor(color);
}

void GCF::Components::ColorPickerWidget::slotColorValueChanged()
{
    QColor color = d->colorValueSlider->color();
    setCurrentColor(color);
}

void GCF::Components::ColorPickerWidget::on_chkTransparent_toggled(bool val)
{
    d->ui.redILE->setEnabled(!val);
    d->ui.blueILE->setEnabled(!val);
    d->ui.greenILE->setEnabled(!val);
    d->ui.redFLE->setEnabled(!val);
    d->ui.blueFLE->setEnabled(!val);
    d->ui.greenFLE->setEnabled(!val);
    d->ui.colorPickerFrame->setEnabled(!val);
    d->ui.valueSliderFrame->setEnabled(!val);

    if(val)
        emit currentColorChanged(Qt::transparent);
    else
        emit currentColorChanged(d->currentColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::Components::ColorValueSlider
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::Components::ColorValueSliderData
{
    QColor baseColor;
    int value;
    QColor color;
    qreal vFactor;

    QPixmap pixmap;
};

GCF::Components::ColorValueSlider::ColorValueSlider(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::Components::ColorValueSliderData;
    setBaseColor(Qt::black);
}

GCF::Components::ColorValueSlider::~ColorValueSlider()
{
    delete d;
}

void GCF::Components::ColorValueSlider::setBaseColor(QColor color)
{
    int h, s;
    d->baseColor = color;
    d->color = color;
    color.getHsv(&h, &s, &d->value);
    preparePixmap();
    update();
}

QColor GCF::Components::ColorValueSlider::baseColor() const
{
    return d->baseColor;
}

void GCF::Components::ColorValueSlider::setValue(int value)
{
    if(d->value == value)
        return;

    int h, s, v;
    d->value = value;
    d->color.getHsv(&h, &s, &v);
    d->color.setHsv(h, s, d->value);
    update();

    emit valueChanged(d->value);
}

int GCF::Components::ColorValueSlider::value() const
{
    return d->value;
}

QColor GCF::Components::ColorValueSlider::color() const
{
    return d->color;
}

void GCF::Components::ColorValueSlider::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, d->pixmap);
    paint.drawRect(0, 0, d->pixmap.width()-1, d->pixmap.height()-1);

    qreal h, s, v;
    d->color.getHsvF(&h, &s, &v);
    int y = int(v / d->vFactor);

    QPainterPath path;
    path.moveTo(width()-4, y);
    path.lineTo(width(), y-2);
    path.lineTo(width(), y+2);
    path.lineTo(width()-4, y);

    paint.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    paint.fillPath(path, Qt::black);
}

void GCF::Components::ColorValueSlider::resizeEvent(QResizeEvent*)
{
    preparePixmap();
}

void GCF::Components::ColorValueSlider::mousePressEvent(QMouseEvent* me)
{
    qreal value = ( qreal(me->pos().y()) * d->vFactor );
    setValue( int(255.0 * value) );
}

void GCF::Components::ColorValueSlider::mouseMoveEvent(QMouseEvent* me)
{
    qreal value = ( qreal(me->pos().y()) * d->vFactor );
    setValue( int(255.0 * value) );
}

void GCF::Components::ColorValueSlider::preparePixmap()
{
    d->pixmap = QPixmap(width()-4, height());
    d->pixmap.fill(Qt::white);

    d->vFactor = 1.0 / qreal(height());

    qreal h, s, v;
    d->baseColor.getHsvF(&h, &s, &v);
    QColor c;

    QPainter paint(&d->pixmap);
    for(int i=0; i<height(); i++)
    {
        v = d->vFactor * qreal(i);
        c.setHsvF(h, s, v);
        paint.setPen(c);
        paint.drawLine(0, i, width(), i);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::Components::ColorPaletteWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::Components::ColorPaletteWidgetData
{
    ColorPaletteWidgetData() : hFactor(0), sFactor(0) { }

    QColor currentColor;
    qreal hFactor;
    qreal sFactor;
    QPixmap pixmap;
};

GCF::Components::ColorPaletteWidget::ColorPaletteWidget(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::Components::ColorPaletteWidgetData;
}

GCF::Components::ColorPaletteWidget::~ColorPaletteWidget()
{
    delete d;
}

void GCF::Components::ColorPaletteWidget::setCurrentColor(QColor color)
{
    if(d->currentColor == color)
        return;

    d->currentColor = color;
    update();
    emit currentColorChanged(color);
}

QColor GCF::Components::ColorPaletteWidget::currentColor() const
{
    return d->currentColor;
}

void GCF::Components::ColorPaletteWidget::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, d->pixmap);

    QPoint p = pointOf(d->currentColor);
    paint.setPen(Qt::black);
    paint.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    paint.drawArc( QRect(p.x()-3, p.y()-3, 6, 6), 0, 5760 );
}

void GCF::Components::ColorPaletteWidget::resizeEvent(QResizeEvent*)
{
    d->hFactor = 1.0 / qreal(width()-1);
    d->sFactor = 1.0 / qreal(height()-1);

    d->pixmap = QPixmap(width(), height());
    d->pixmap.fill(Qt::white);

    QPainter paint(&d->pixmap);
    QRect rect = this->rect();
    for(int i=rect.left(); i<rect.right(); i++)
    {
        for(int j=rect.top(); j<rect.bottom(); j++)
        {
            QColor c = colorAt( QPoint(i, j) );
            paint.setPen(c);
            paint.drawPoint(i, j);
        }
    }
}

void GCF::Components::ColorPaletteWidget::mousePressEvent(QMouseEvent* me)
{
    QColor c = colorAt(me->pos());
    setCurrentColor(c);
}

void GCF::Components::ColorPaletteWidget::mouseMoveEvent(QMouseEvent* me)
{
    QColor c = colorAt(me->pos());
    setCurrentColor(c);
}

QColor GCF::Components::ColorPaletteWidget::colorAt(QPoint p)
{
    qreal h = d->hFactor * qreal(p.x());
    qreal s = d->sFactor * qreal(p.y());
    qreal v = 1.0;
    QColor color;
    color.setHsvF(h, s, v);
    return color;
}

QPoint GCF::Components::ColorPaletteWidget::pointOf(QColor c)
{
    qreal h, s, v;
    c.getHsvF(&h, &s, &v);

    qreal x = h / d->hFactor;
    qreal y = s / d->sFactor;

    return QPoint( int(x), int(y) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::Components::ColorLabel
/////////////////////////////////////////////////////////////////////////////////////////////////////////

GCF::Components::ColorLabel::ColorLabel(QWidget* parent)
:QWidget(parent)
{

}

GCF::Components::ColorLabel::~ColorLabel()
{

}

void GCF::Components::ColorLabel::setColor(QColor color)
{
    m_color = color;
    update();
}

QColor GCF::Components::ColorLabel::color() const
{
    return m_color;
}

void GCF::Components::ColorLabel::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.fillRect(this->rect(), m_color);
}


