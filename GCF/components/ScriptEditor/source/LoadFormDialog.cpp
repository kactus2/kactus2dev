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

#include "LoadFormDialog.h"
#include "ui_LoadFormDialog.h"

#include <GCF/AbstractComponent.h>
#include <QMainWindow>
#include <QPixmap>
#include <QPainter>
#include <QVBoxLayout>
#include <QMatrix>
#include <QFileDialog>
#include <QCompleter>

namespace GCF
{
	namespace Components
	{
		class PixmapLabel : public QWidget
		{
		public:
			PixmapLabel(QWidget* parent=0);
			~PixmapLabel();

			void setPixmap(QPixmap pixmap);
			QPixmap pixmap() const;

			void markRectangle(QRect rect);

		protected:
			void paintEvent(QPaintEvent* pe);
			void resizeEvent(QResizeEvent* re);

		private:
			QPixmap mOrigPixmap;
			QPixmap mScaledPixmap;
			QRect mMarkRect;
			QMatrix mMatrix;
		};
	}
}

struct GCF::Components::LoadFormDialogData
{
    QPixmap mainWindowPixmap;
    GCF::Components::PixmapLabel* label;
    QCompleter* completer;

    Ui::LoadFormDialog ui;
};

GCF::Components::LoadFormDialog::LoadFormDialog(QWidget* parent)
:QDialog(parent)
{
    d = new GCF::Components::LoadFormDialogData;
    d->ui.setupUi(this);

    QVBoxLayout* layout = new QVBoxLayout(d->ui.widgetDisplayFrame);
    d->label = new GCF::Components::PixmapLabel(d->ui.widgetDisplayFrame);
    layout->addWidget(d->label);
    d->completer = 0;
}

GCF::Components::LoadFormDialog::~LoadFormDialog()
{
    delete d;
}

void GCF::Components::LoadFormDialog::setWidgetInfo(const QStringList* widgetNames, const QList<QWidget*>* widgetList)
{
    if(!widgetNames || !widgetList)
    {
        widgetNames = 0;
        widgetList = 0;
        return;
    }

    int currentRow = -1;
    d->ui.lstWidgetNames->clear();
    for(int i=0; i<widgetNames->count(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem(d->ui.lstWidgetNames);

		QString wName = widgetList->at(i)->windowTitle();
		if(wName.isEmpty())
			wName = widgetNames->at(i);

        item->setText(wName);
        item->setData(Qt::UserRole, qVariantFromValue<QWidget*>(widgetList->at(i)));
		item->setData(Qt::UserRole+1, widgetNames->at(i));

        if(currentRow < 0 && item->text() == d->ui.txtParentWidget->text())
            currentRow = i;
    }
    d->ui.lstWidgetNames->setSortingEnabled(true);

    QMainWindow* mainWindow = GCF::AbstractComponent::mainWindow();
    QPixmap pix = QPixmap::grabWidget(mainWindow);
    d->label->setPixmap(pix);
    if(currentRow >= 0)
        d->ui.lstWidgetNames->setCurrentRow(currentRow);

    if(d->completer)
    {
        d->ui.txtParentWidget->setCompleter(0);
        delete d->completer;
    }
    d->completer = new QCompleter(*widgetNames, this);
    d->ui.txtParentWidget->setCompleter(d->completer);
    d->completer->setCaseSensitivity(Qt::CaseInsensitive);
}

QString GCF::Components::LoadFormDialog::formFileName() const
{
    return d->ui.txtFormFile->text();
}

QString GCF::Components::LoadFormDialog::parentWidgetName() const
{
    return d->ui.txtParentWidget->text();
}

void GCF::Components::LoadFormDialog::on_lstWidgetNames_currentRowChanged(int index)
{
    QListWidgetItem* i = d->ui.lstWidgetNames->item(index);
    if(!i)
        return;

    QWidget* widget = i->data(Qt::UserRole).value<QWidget*>();
    if(!widget)
        return;

	QString wName = i->data(Qt::UserRole+1).toString();
	d->ui.txtParentWidget->setText(wName);

    QRect rect = widget->rect();
    QMainWindow* mainWindow = GCF::AbstractComponent::mainWindow();

    QPoint topLeft = rect.topLeft();
    topLeft = widget->mapToGlobal(topLeft);
    topLeft = mainWindow->mapFromGlobal(topLeft);
    rect.moveTopLeft(topLeft);
    d->label->markRectangle(rect);
}

void GCF::Components::LoadFormDialog::on_cmdSelectFormFile_clicked()
{
    QString formFile = QFileDialog::getOpenFileName(this, "Choose a form file to open",
                        QDir::homePath(), "*.ui");
    d->ui.txtFormFile->setText(formFile);
}

void GCF::Components::LoadFormDialog::on_txtParentWidget_textChanged(const QString& text)
{
    d->ui.lstWidgetNames->blockSignals(true);

    QList<QListWidgetItem*> items = d->ui.lstWidgetNames->findItems(text, Qt::MatchStartsWith);
    if(items.count())
    {
        int row = d->ui.lstWidgetNames->row(items.first());
        d->ui.lstWidgetNames->setCurrentRow(row);
        on_lstWidgetNames_currentRowChanged(row);
    }

    d->ui.lstWidgetNames->blockSignals(false);
}

// GCF::Components::PixmapLabel

GCF::Components::PixmapLabel::PixmapLabel(QWidget* parent)
:QWidget(parent)
{

}

GCF::Components::PixmapLabel::~PixmapLabel()
{

}

void GCF::Components::PixmapLabel::setPixmap(QPixmap pixmap)
{
    mOrigPixmap = pixmap;
    mScaledPixmap = mOrigPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    qreal sx = qreal(mScaledPixmap.width()) / qreal(mOrigPixmap.width());
    qreal sy = qreal(mScaledPixmap.height()) / qreal(mOrigPixmap.height());
    mMatrix = QMatrix();
    mMatrix.scale(sx, sy);
    update();
}

QPixmap GCF::Components::PixmapLabel::pixmap() const
{
    return mOrigPixmap;
}

void GCF::Components::PixmapLabel::markRectangle(QRect rect)
{
    mMarkRect = rect;
    update();
}

void GCF::Components::PixmapLabel::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    int left = (width() - mScaledPixmap.width())/2;
    int top = (height() - mScaledPixmap.height())/2;
    paint.drawPixmap(left, top, mScaledPixmap);

    if(mMarkRect.isNull())
        return;

    QRect rect = mMatrix.mapRect(mMarkRect);
    rect.moveTopLeft(rect.topLeft() + QPoint(left, top));
    QColor color = palette().highlight().color();
    color.setAlphaF(0.5f);
    paint.fillRect(rect, color);
    paint.drawRect(rect);
}

void GCF::Components::PixmapLabel::resizeEvent(QResizeEvent*)
{
    if(mOrigPixmap.isNull())
        return;

    mScaledPixmap = mOrigPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    qreal sx = qreal(mScaledPixmap.width()) / qreal(mOrigPixmap.width());
    qreal sy = qreal(mScaledPixmap.height()) / qreal(mOrigPixmap.height());
    mMatrix = QMatrix();
    mMatrix.scale(sx, sy);
}

