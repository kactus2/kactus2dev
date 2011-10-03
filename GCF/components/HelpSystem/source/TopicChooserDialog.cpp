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
#include "TopicChooserDialog.h"
#include "ui_TopicChooserDialog.h"

#include <QString>
#include <QUrl>
#include <QMap>


struct GCF::Components::TopicChooserDialogData
{
    TopicChooserDialogData(): topicLabel(), labels(0) { }

    Ui::TopicChooserDialog ui;

    QString topicLabel;
    QStringList labels;
    QList<QUrl> urls;
};

GCF::Components::TopicChooserDialog::TopicChooserDialog(QWidget * parent): QDialog(parent)
{
	d = new GCF::Components::TopicChooserDialogData;

    d->ui.setupUi(this);
    d->topicLabel = d->ui.topicLabel->text();

    // Make connections
    connect(d->ui.displayButton, SIGNAL(clicked()), this, SLOT(slotShowUrl()));
    connect(d->ui.listWidget, SIGNAL(itemActivated ( QListWidgetItem *)), this, SLOT(slotShowUrl()));
}

GCF::Components::TopicChooserDialog::~TopicChooserDialog()
{
	delete d;
}

void GCF::Components::TopicChooserDialog::showTopics(const QMap<QString, QUrl> & links, const QString & keyword)
{
    d->ui.topicLabel->setText(d->topicLabel + "<b>"+keyword+"</b>:");

    d->labels = links.keys();
    d->urls = links.values();

    d->ui.listWidget->addItems(d->labels);

    //Make the first item selected by default.
    d->ui.listWidget->setCurrentItem(d->ui.listWidget->item(0));
}

void GCF::Components::TopicChooserDialog::slotShowUrl()
{
    int currentRow = d->ui.listWidget->currentRow();
    if(currentRow < 0 || currentRow > d->urls.count())
        return;

    emit topicSelected(d->urls[currentRow]);
    close();
}
